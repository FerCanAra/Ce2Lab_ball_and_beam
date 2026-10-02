/*
 * =====================================================================
 *  BALL & BEAM  ·  Firmware for the "Balance the Ball" teaching interface
 * =====================================================================
 *
 *  The Arduino runs the WHOLE control loop (sensor -> PID -> servo).
 *  The PC only changes parameters and draws what it receives. If the PC
 *  is unplugged or freezes, the rig keeps controlling on its own.
 *
 *  Hardware
 *    - Arduino UNO
 *    - VL53L0X ToF sensor over I2C (SDA=A4, SCL=A5), powered at 5 V
 *    - Hitec HS-422 servo, signal on pin 11
 *
 *  Libraries
 *    - VL53L0X by Pololu  (Library Manager -> "VL53L0X", author Pololu)
 *    - Servo, Wire, EEPROM (bundled with the IDE)
 *
 *  Sign convention
 *    pos  = sensor-to-ball distance in mm (raw ToF reading)
 *    u    = beam angle in degrees from horizontal.
 *           u > 0  ==>  the ball accelerates AWAY from the sensor.
 *    If your rig behaves the other way round, press "Flip direction" in
 *    the operator settings and save (or change servoDir here).
 *
 *  Serial protocol (text, 115200 baud, one command per line)
 *    PC -> Arduino
 *      P<v>  Kp [degrees/mm]           I<v>  Ki [degrees/(mm*s)]
 *      D<v>  Kd [degrees*s/mm]         S<v>  setpoint [mm]
 *      M<n>  mode: 0=stopped (beam level) 1=PID 2=manual
 *      A<v>  manual angle [degrees]    L<v>  angle limit [degrees]
 *      N<v>  servo angle that leaves the beam level [degrees]
 *      X<n>  servo direction: 1 or -1
 *      R     clear the integral term
 *      W     store level, direction, limit and gains in EEPROM
 *      ?     resend configuration
 *    Arduino -> PC
 *      $t,pos,sp,u,p,i,d,mode,raw,ok,tilt   telemetry (~30 Hz)
 *      #CFG,spMin,spMax,posMin,posMax,uMax,kp,ki,kd,sp,level,mode,dir
 *      #SAVED   #ERR,SENSOR   #READY
 * =====================================================================
 */

#include <Wire.h>
#include <Servo.h>
#include <EEPROM.h>
#include <VL53L0X.h>

// ---------------------------------------------------------------------
//  RIG CONFIGURATION  (check these values once)
// ---------------------------------------------------------------------
const uint8_t SERVO_PIN        = 11;
int8_t        servoDir         = -1;     // +1 or -1 (can be flipped from the interface)
float         servoLevelDeg    = 85.0;   // servo angle that leaves the beam level
const float   U_MAX_HARD       = 20.0;   // absolute tilt limit [deg]

// Measurement and setpoint range (mm, as read by the sensor)
const float   POS_MIN          = 42.0;   // ball against the near end stop
const float   POS_MAX          = 238.0;  // ball against the far end stop
const float   SP_MIN           = 70.0;
const float   SP_MAX           = 210.0;
const float   SP_DEFAULT       = 140.0;  // middle of the beam
const uint16_t RAW_VALID_MIN   = 20;     // readings outside this are ignored
const uint16_t RAW_VALID_MAX   = 320;

// Sensor
const uint32_t TIMING_BUDGET_US = 33000; // 33 ms -> ~30 samples/s
const uint16_t SENSOR_TIMEOUT_MS = 300;  // no samples -> sensor failure

// Alpha-beta observer (estimates position and speed with far less noise)
const float   OBS_ALPHA        = 0.35;
const float   OBS_BETA         = 0.05;
const float   OBS_GAMMA        = 0.001;  // estimates how far the beam is off level (slow)
const float   K_MODEL          = 83.5;   // mm/s^2 per degree (identified K_b)
const float   OBS_JUMP_MM      = 60.0;   // sudden jump (someone picked up the ball)
const float   END_MARGIN_MM    = 10.0;   // close to an end stop = ball resting against it

// PID
const float   I_MAX_DEG        = 8.0;    // integral term limit

// ---------------------------------------------------------------------
//  PARAMETERS THE INTERFACE CHANGES (start-up values)
// ---------------------------------------------------------------------
float kp = 0.10;         // [deg/mm]
float ki = 0.04;         // [deg/(mm*s)]
float kd = 0.06;         // [deg*s/mm]
float setpoint = SP_DEFAULT;
float uMax = 15.0;       // tilt limit [deg]
float manualDeg = 0.0;
uint8_t mode = 1;        // starts up controlling: the rig works with no PC

// ---------------------------------------------------------------------
//  INTERNAL STATE
// ---------------------------------------------------------------------
VL53L0X sensor;
Servo   servo;

bool     sensorOk = false;
uint16_t rawBuf[3];
uint8_t  rawCount = 0;
uint16_t lastRaw = 0;
float    xh = SP_DEFAULT, vh = 0.0;  // observer state
float    bh = 0.0;                   // estimated beam tilt error [deg]
bool     obsInit = false;
uint8_t  jumpCount = 0;
float    iTerm = 0.0;
float    pTerm = 0.0, dTerm = 0.0;
float    uApplied = 0.0;
unsigned long lastSampleUs = 0;
float    dtF = TIMING_BUDGET_US * 1e-6; // smoothed sampling period [s]
unsigned long lastPollUs = 0;
unsigned long lastOkMs = 0;
unsigned long lastInitTryMs = 0;

char    rxBuf[24];
uint8_t rxLen = 0;

// EEPROM
const uint16_t EE_MAGIC = 0xB5B2;
struct Stored { uint16_t magic; float level, uMax, kp, ki, kd; int8_t dir; };

// ---------------------------------------------------------------------
//  SERVO
// ---------------------------------------------------------------------
void setBeam(float u) {
  uApplied = u;
  float deg = servoLevelDeg + servoDir * u;
  deg = constrain(deg, 0.0, 180.0);
  // Same scale as Servo.write(): 0 deg = 544 us, 180 deg = 2400 us,
  // but with microsecond resolution (~0.1 deg) instead of 1 deg.
  int us = (int)(544.0 + deg * (2400.0 - 544.0) / 180.0 + 0.5);
  servo.writeMicroseconds(us);
}

// ---------------------------------------------------------------------
//  EEPROM
// ---------------------------------------------------------------------
void loadSettings() {
  Stored s;
  EEPROM.get(0, s);
  if (s.magic != EE_MAGIC) return;
  if (s.level > 30 && s.level < 150) servoLevelDeg = s.level;
  if (s.uMax > 1 && s.uMax <= U_MAX_HARD) uMax = s.uMax;
  if (s.kp >= 0 && s.kp < 5) kp = s.kp;
  if (s.ki >= 0 && s.ki < 5) ki = s.ki;
  if (s.kd >= 0 && s.kd < 5) kd = s.kd;
  if (s.dir == 1 || s.dir == -1) servoDir = s.dir;
}

void saveSettings() {
  Stored s = { EE_MAGIC, servoLevelDeg, uMax, kp, ki, kd, servoDir };
  EEPROM.put(0, s);
  Serial.println(F("#SAVED"));
}

// ---------------------------------------------------------------------
//  SENSOR
// ---------------------------------------------------------------------
bool initSensor() {
  sensor.setTimeout(100);
  if (!sensor.init()) return false;
  sensor.setMeasurementTimingBudget(TIMING_BUDGET_US);
  sensor.startContinuous();
  return true;
}

uint16_t median3(uint16_t a, uint16_t b, uint16_t c) {
  if (a > b) { uint16_t t = a; a = b; b = t; }
  if (b > c) { b = c; }
  return (a > b) ? a : b;
}

// ---------------------------------------------------------------------
//  COMMUNICATION
// ---------------------------------------------------------------------
void sendConfig() {
  Serial.print(F("#CFG,"));
  Serial.print(SP_MIN, 0);  Serial.print(',');
  Serial.print(SP_MAX, 0);  Serial.print(',');
  Serial.print(POS_MIN, 0); Serial.print(',');
  Serial.print(POS_MAX, 0); Serial.print(',');
  Serial.print(uMax, 1);    Serial.print(',');
  Serial.print(kp, 4);      Serial.print(',');
  Serial.print(ki, 4);      Serial.print(',');
  Serial.print(kd, 4);      Serial.print(',');
  Serial.print(setpoint, 1); Serial.print(',');
  Serial.print(servoLevelDeg, 1); Serial.print(',');
  Serial.print(mode);       Serial.print(',');
  Serial.println(servoDir);
}

void sendTelemetry(float pos) {
  Serial.print('$');
  Serial.print(millis());      Serial.print(',');
  Serial.print(pos, 1);        Serial.print(',');
  Serial.print(setpoint, 1);   Serial.print(',');
  Serial.print(uApplied, 2);   Serial.print(',');
  Serial.print(pTerm, 2);      Serial.print(',');
  Serial.print(iTerm, 2);      Serial.print(',');
  Serial.print(dTerm, 2);      Serial.print(',');
  Serial.print(mode);          Serial.print(',');
  Serial.print(lastRaw);       Serial.print(',');
  Serial.print(sensorOk ? 1 : 0); Serial.print(',');
  Serial.println(bh, 2);
}

void resetController() {
  iTerm = 0.0;
  pTerm = 0.0;
  dTerm = 0.0;
}

void processCommand(char *line) {
  char c = line[0];
  float v = atof(line + 1);
  switch (c) {
    case 'P': kp = constrain(v, 0.0, 5.0); break;
    case 'I': ki = constrain(v, 0.0, 5.0); if (ki == 0.0) iTerm = 0.0; break;
    case 'D': kd = constrain(v, 0.0, 5.0); break;
    case 'S': setpoint = constrain(v, SP_MIN, SP_MAX); break;
    case 'A': manualDeg = constrain(v, -uMax, uMax); break;
    case 'L': uMax = constrain(v, 1.0, U_MAX_HARD); break;
    case 'N': servoLevelDeg = constrain(v, 30.0, 150.0); break;
    case 'X': servoDir = (v < 0) ? -1 : 1; resetController(); break;
    case 'R': iTerm = 0.0; break;
    case 'W': saveSettings(); break;
    case '?': sendConfig(); break;
    case 'M': {
      uint8_t m = (uint8_t)constrain((int)v, 0, 2);
      if (m != mode) resetController();
      if (m == 2) manualDeg = 0.0;
      mode = m;
      break;
    }
    default: break;
  }
}

void handleSerial() {
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r') {
      if (rxLen > 0) {
        rxBuf[rxLen] = '\0';
        processCommand(rxBuf);
        rxLen = 0;
      }
    } else if (rxLen < sizeof(rxBuf) - 1) {
      rxBuf[rxLen++] = ch;
    } else {
      rxLen = 0;  // line too long: discard it
    }
  }
}

// ---------------------------------------------------------------------
//  ONE CONTROL STEP (runs on every new sensor sample)
// ---------------------------------------------------------------------
void controlStep(uint16_t raw, float dt) {
  // 1) Median of 3: removes isolated ToF spikes
  rawBuf[0] = rawBuf[1]; rawBuf[1] = rawBuf[2]; rawBuf[2] = raw;
  if (rawCount < 3) rawCount++;
  float z = (rawCount < 3) ? raw : median3(rawBuf[0], rawBuf[1], rawBuf[2]);

  // 2) Alpha-beta observer with a model: it predicts using the applied
  //    angle and corrects with the measurement. The resulting speed is far
  //    less noisy than differentiating the measurement directly.
  if (!obsInit) { xh = z; vh = 0.0; obsInit = true; }
  //    The model is  a = K*(u + tilt error). The tilt error estimates itself,
  //    so the speed estimate is not biased when the beam is not perfectly
  //    level. Next to an end stop the model does not hold (the wall pushes
  //    back): there we use the measurement only and freeze the tilt estimate.
  bool atFar  = z >= POS_MAX - END_MARGIN_MM;
  bool atNear = z <= POS_MIN + END_MARGIN_MM;
  float a = (atFar || atNear) ? 0.0 : K_MODEL * (uApplied + bh);
  xh += vh * dt + 0.5 * a * dt * dt;
  vh += a * dt;
  float r = z - xh;
  if (fabs(r) > OBS_JUMP_MM) {
    if (++jumpCount >= 2) { xh = z; vh = 0.0; jumpCount = 0; r = 0.0; }
  } else {
    jumpCount = 0;
  }
  xh += OBS_ALPHA * r;
  vh += (OBS_BETA / dt) * r;
  if (!atFar && !atNear) {
    bh += 2.0 * OBS_GAMMA * r / (K_MODEL * dt * dt);
    bh = constrain(bh, -5.0, 5.0);
  }
  if (atFar && vh > 0)  vh = 0.0;
  if (atNear && vh < 0) vh = 0.0;
  xh = constrain(xh, POS_MIN - 20.0, POS_MAX + 20.0);

  // 3) Control law
  float u = 0.0;
  if (mode == 1) {
    float e = setpoint - xh;
    pTerm = kp * e;
    dTerm = -kd * vh;               // derivative on measurement: no kick
    float uTry = pTerm + iTerm + dTerm;
    // Anti-windup: do not integrate if we are already saturated that way
    bool pushUp   = (uTry >  uMax) && (e > 0);
    bool pushDown = (uTry < -uMax) && (e < 0);
    if (!pushUp && !pushDown) iTerm += ki * e * dt;
    iTerm = constrain(iTerm, -I_MAX_DEG, I_MAX_DEG);
    u = constrain(pTerm + iTerm + dTerm, -uMax, uMax);
  } else if (mode == 2) {
    resetController();
    u = constrain(manualDeg, -uMax, uMax);
  } else {
    resetController();
    u = 0.0;
  }
  setBeam(u);
  sendTelemetry(xh);
}

// ---------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  loadSettings();

  servo.attach(SERVO_PIN);
  setBeam(0.0);                 // beam level at start-up

  Wire.begin();
  Wire.setClock(400000);
  Wire.setWireTimeout(3000, true);   // a stuck I2C bus cannot freeze the Arduino
  sensorOk = initSensor();
  if (!sensorOk) Serial.println(F("#ERR,SENSOR"));

  lastSampleUs = micros();
  lastOkMs = millis();
  sendConfig();
  Serial.println(F("#READY"));
}

void loop() {
  handleSerial();

  unsigned long nowMs = millis();

  // Sensor down: level the beam and retry once a second
  if (!sensorOk) {
    if (uApplied != 0.0) setBeam(0.0);
    resetController();
    if (nowMs - lastInitTryMs > 1000) {
      lastInitTryMs = nowMs;
      sensorOk = initSensor();
      if (sensorOk) { lastOkMs = nowMs; obsInit = false; rawCount = 0; lastSampleUs = micros(); }
      else { Serial.println(F("#ERR,SENSOR")); sendTelemetry(xh); }
    }
    return;
  }

  // Non-blocking sensor polling (every 2 ms) so the serial port never stalls
  unsigned long nowUs = micros();
  if (nowUs - lastPollUs < 2000) return;
  lastPollUs = nowUs;

  if ((sensor.readReg(VL53L0X::RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
    if (nowMs - lastOkMs > SENSOR_TIMEOUT_MS) {
      sensorOk = false;
      Serial.println(F("#ERR,SENSOR"));
    }
    return;
  }

  uint16_t raw = sensor.readRangeContinuousMillimeters();  // data is ready: does not block
  float dt = (nowUs - lastSampleUs) * 1e-6;
  lastSampleUs = nowUs;
  if (dt > 0.005 && dt < 0.2) dtF += 0.1 * (dt - dtF);  // removes the polling jitter

  if (sensor.timeoutOccurred()) return;
  lastOkMs = nowMs;
  lastRaw = raw;

  if (raw < RAW_VALID_MIN || raw > RAW_VALID_MAX) {
    // Out-of-range reading (8190 = "nothing in sight"): reuse the last good one
    raw = (rawCount > 0) ? rawBuf[2] : (uint16_t)SP_DEFAULT;
  }
  controlStep(raw, dtF);
}
