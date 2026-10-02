# Balance the Ball — getting started

Ball and beam rig with an Arduino UNO, a Hitec HS-422 servo and a VL53L0X ToF sensor.
The PID runs on the Arduino; the PC is only an interface for changing parameters and watching the response.

## Contents

| File | What it is |
|---|---|
| `BallBeam_Expo/BallBeam_Expo.ino` | Arduino firmware: sensor reading, filtering, PID, servo and serial protocol |
| `balance_the_ball.html` | Interface for the public; a single file you open in Chrome or Edge |

If you close the browser the rig keeps controlling, because the loop runs on the Arduino.
That is also why it starts up in PID mode even with no PC attached.

---

## 1. Wiring

| Item | Pin |
|---|---|
| Servo, signal | D11 |
| Servo, power | 5–6 V from a **separate supply** (2 A or more); ground **common** with the Arduino |
| VL53L0X VIN / GND | 5 V / GND (the module has its own regulator) |
| VL53L0X SDA / SCL | A4 / A5 |

During a long public event it matters that the servo is not powered from USB.
When the servo pulls hard, USB can sag below what the UNO needs and the Arduino resets.
The interface counts those resets in the operator panel. If the count climbs, this is why.

## Beam geometry

The beam has been shortened from the original build. The values the program uses are:

| Quantity | Value |
|---|---|
| Ball against the near end stop | 42 mm |
| Ball in the middle of the beam | 140 mm |
| Ball against the far end stop | 238 mm |
| Target, minimum and maximum | 70 and 210 mm |
| Target at start-up | 140 mm |

The 42 mm minimum follows from the other two: if the middle reads 140 and the far stop reads 238, the near stop falls at 140 − (238 − 140) = 42 mm.

If you change the beam length again, only `POS_MIN`, `POS_MAX`, `SP_MIN`, `SP_MAX` and `SP_DEFAULT` at the top of the sketch need editing. The interface reads these from the Arduino as soon as it connects (the `#CFG` message) and adapts the drawing, the target slider and the charts on its own, so the HTML needs no changes.

## 2. Loading the firmware

1. In the Arduino IDE open *Tools → Manage Libraries* and search for **VL53L0X**. Install the one by **Pololu**, not the Adafruit one. The `Servo` library already ships with the IDE.
2. Open `BallBeam_GUI/BallBeam_GUI.ino`. The folder has to have the same name as the file.
3. Select the *Arduino Uno* board and the right port, and upload.
4. If you open the Serial Monitor at 115200 baud you should see `#READY`, `#CFG,...` and lines starting with `$`.
5. **Close the Serial Monitor** before using the interface: only one program at a time can hold the port.

The parameters you may need to touch are grouped at the top of the file: servo pin, position range, default target, angle limits and observer settings.
Everything that depends on your particular build (level angle, direction, limit and gains) is adjusted from the interface and stored in EEPROM.

## 3. Opening the interface

1. Open `balance_the_ball.html` with **Chrome or Edge**. Firefox and Safari do not support Web Serial.
2. Press **Connect Arduino** and pick the port. Next time you open the page it connects on its own.
3. It needs no internet, so you can copy the file to the event laptop and open it directly.

With no rig attached, the **Simulator** button gives you the same controller as the firmware running against your identified model. It is useful for rehearsing the explanation, or for keeping people busy while someone else is using the rig.

## 4. One-off setup (⚙ Operator settings)

1. **Level.** Tick *Manual mode* with the manual tilt at 0°. Adjust *Servo angle that levels the beam* until the ball barely moves. The starting value is 85°.
2. **Sensor.** Move the ball along the beam by hand. The *Sensor reading* should follow it smoothly between about 42 and 238 mm. With the ball in the middle of the beam it should read about 140 mm; if it does not, check the end stops or where the sensor sits.
3. **Direction.** Set a manual tilt of +5°. The ball must move **away from the sensor**. If it comes closer, press *Flip direction*. This build needs −1, which is the default.
4. **Limit.** 15° is a good compromise. Larger values make the rig quicker but also harsher.
5. Untick manual mode and press **Save to the Arduino**. From then on it starts up with these values.

If the ball consistently runs off to one end with the PID recipe, step 3 is wrong. With the direction inverted, the more you raise P the sooner the ball escapes.

---

## 5. How it works inside

### The plant
Tilting the beam accelerates the ball:

`ẍ = K · (θ + tilt error)`, with K ≈ 83.5 mm/s² per degree.

That is the value from the `fmincon` identification: K_b ≈ 4.79 m/s²/rad. The plant is a **double integrator**. Tilt does not set the position of the ball, nor even its speed, but its acceleration.

### Why D is essential
With P alone the beam acts like a spring: it pushes the ball towards the target, but nothing brakes it, so the ball oscillates for ever.
D opposes the speed and plays the part of a damper.
I removes the error that is left when the beam is not perfectly level or friction is in play.

### Units of the gains
- **Kp** [°/mm]: at 0.10, a 10 mm error tilts the beam by 1°.
- **Ki** [°/(mm·s)]: at 0.04, a 10 mm error held for 1 s adds 0.4°.
- **Kd** [°·s/mm]: at 0.06, a ball travelling at 100 mm/s produces 6° of braking.

Starting values: **Kp = 0.10, Ki = 0.04, Kd = 0.06**. These are the ones that already worked on this rig: beam length does not change the relation between tilt and acceleration, so the gains still hold. With the short beam the errors are smaller, the beam tilts less, and the ball settles slightly sooner, in about 3–5 s, with a final error of 5 mm or less.

What does change is the margin to the end stops: from the highest target (210 mm) there are only 28 mm left before the end of the beam. With the more aggressive recipes the ball will touch the stop, bounce, and the controller will recover it. This is harmless and turns out to be rather instructive to watch.

### Filtering: alpha-beta observer with a model
Differentiating a measurement carrying 20–30 mm of noise makes the servo shake.
The firmware first applies a median of 3, which kills the spikes, and then an **observer** that predicts where the ball should be using the model and corrects that prediction with the measurement:
- `α = 0.35` corrects position and `β = 0.05` corrects speed.
- `γ = 0.001` estimates the real tilt error of the beam, bounded to ±5°. Without this term the P+D recipe settled about 20 mm short of the target.
- If the ball jumps more than 60 mm, for instance because someone picks it up, the observer resets.
- Near the end stops it stops using the model and freezes the tilt estimate. Otherwise it "believes" a ball resting against a wall is still moving.

In simulation the servo shake drops from about 1.1–2.6° with conventional filters to about 0.6°.

### PID details
- **Derivative on the measurement**, not on the error, so changing the target produces no sudden kick.
- **Anti-windup**: the integral stops accumulating once the beam is already at its limit in that direction, and it is bounded to ±8° anyway.
- The servo is driven with `writeMicroseconds`, giving about 0.1° of resolution instead of 1°.
- The sensor is polled without blocking. If it fails, the beam goes level and the Arduino retries once a second.

---

## 6. The interface for the public

- **P, I and D sliders**, each with a plain-language line and a warning about what happens if you overdo it.
- **"Who is tilting the beam right now?"**: shows in real time how many degrees each term contributes. This is the single most useful panel for explaining a PID.
- **Recipes**, worth trying in this order:
  - **P only**: the ball oscillates.
  - **P + D**: it settles, but about 18 mm short of the target.
  - **PID**: it reaches the target.
  - **Too much I**: it overshoots.
  - **Jittery**: it shakes.
  - **Sluggish**: it never gets there.
- **Coach**: suggests which slider to move based on what the ball is doing.
- **Challenge** (*Practice* tab): move the target by 3 cm or more and it times how long the ball takes to stay within ±1 cm for 1.5 s, and keeps the record. If someone touches the gains halfway through, that time does not count.
- **Target**: set by dragging on the drawing of the beam or with the slider below it.
- **Keyboard**: *Space* stops or starts, *Esc* closes panels. During a competition, *Space* stops control and voids the go.
- **Automatic demo** after 90 s with nobody touching anything: it goes back to the PID recipe and cycles through targets. It can be turned off in ⚙.

### Competition mode

On the **Competition** tab, the *Start* button deals out a random target and random gains, starts the clock and waits. You have to move P, I and D until the ball sits still on the target (±1 cm) for **3 seconds in a row**. The time runs from *Start* to the moment the ball entered the good zone and never left it again.

On success a dialog shows the time, the position obtained and a box for a name. The **Leaderboard** tab shows the standings.

Details worth knowing for the event:

- The starting gains come from four archetypes (no brakes, extremely jittery, too much memory, everything maxed out), with random values inside each. They are chosen so that **none of them settles on its own**: this was checked over 320 simulated goes, with the beam level and with a tilt error of ±1.5°. Nobody wins without touching anything.
- The target always lands 7 cm or more from wherever the ball is, so there is ground to cover.
- During a go the target slider, the recipes and the demo button are locked. Otherwise pressing "PID" would be enough to win.
- The coach stops giving hints during a go and only reports the time. If you would rather it kept helping, for instance with small children, turn it on in ⚙ → *Keep the coach hints during the competition*.
- If someone stops control, switches to manual mode, or the Arduino resets, the go is voided. The limit per go is 2 minutes.
- The leaderboard is stored in the browser on the laptop (`localStorage`), so it survives reloading the page and closing the browser. Clear it from ⚙ → *Clear the leaderboard*. It is a good idea to empty it right before opening to the public.
- If the browser has local storage blocked, the interface says so on the leaderboard tab and the standings last only while the page is open.

### Tips for the event
- Walk people through this sequence: *P only → raise the D → raise the I*. That is how each letter earns its place. Once someone has the three letters, move them to the competition tab: that is where it hooks.
- Leave the good values saved on the Arduino so a reset does not leave the rig out of tune.
- Go full screen (F11) and stop the laptop from sleeping.
- Bring a spare ball. Ping-pong balls get dented, and a dented ball rolls differently.

---

## 7. Serial protocol (115200 baud, one command per line)

Useful if you want to drive the firmware from MATLAB or Python.

**PC → Arduino**

| Command | Meaning |
|---|---|
| `P0.1` `I0.04` `D0.06` | Gains |
| `S180` | Target in mm (between 70 and 210) |
| `M0` / `M1` / `M2` | Stopped (beam level) / PID / manual |
| `A5` | Angle in manual mode (°) |
| `L15` | Tilt limit (°) |
| `N85` | Servo angle that levels the beam |
| `X-1` | Servo direction |
| `R` | Clear the integral |
| `W` | Store in EEPROM |
| `?` | Resend the configuration |

**Arduino → PC**

- `$t,pos,sp,u,p,i,d,mode,raw,ok,tilt` at about 30 Hz.
- `#CFG,...` with the current configuration.
- `#SAVED` once stored in EEPROM.
- `#ERR,SENSOR` if the sensor fails.
- `#READY` on start-up.

## 8. Common problems

| Symptom | Likely cause |
|---|---|
| "Port busy" | The IDE Serial Monitor is open |
| The reset counter climbs | The servo is powered from USB |
| The ball always escapes to one end | Direction inverted (step 3) |
| With P at zero the ball rolls on its own | Level angle not adjusted (step 1) |
| The reading jumps a lot | Dirty sensor, direct sunlight, or the ball outside its measuring range |
| No connect button appears | The browser is not Chrome or Edge |
