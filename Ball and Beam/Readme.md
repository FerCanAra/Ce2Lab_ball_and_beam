
# Ce2Lab - Ball and Beam

The Ball and Beam system is a classic experiment widely used in control engineering to study PID control techniques. The objective of the system is to maintain a ball at a desired position along a beam by adjusting the beam's angle. This is achieved by continuously measuring the ball position and applying a PID control action to a servo motor that tilts the beam.

<img src="../docs/Model_1_1.gif" width="350"> <img src="../docs/Model_2_1.gif" width="350">

In this project, a different ATmega328P-based implementation of the Ball and Beam system is developed, using an ultrasonic and laser sensors to measure the ball's position and a servo motor to control the beam angle. A PID (Proportional–Integral–Derivative) controller is implemented to automatically adjust the beam inclination, keeping the ball close to the desired target position.

> **Note:** Process tested with Windows 11 with PlatformIO (VSC) on 8th April 2026.

## Ball and Beam GUI

A web interface is also available, allowing you to run simulations in HTML and connect to the actual model for direct interaction. Follow this [official site](Ball%20and%20Beam%20GUI/) to learn more.

<img src="/Ball_and_Beam_GUI/gui_ball_and_beam.gif" width="350"> 

<details>
<summary>Model 1</summary>

A “Ball and Beam” with its support in the center:

<img width="400" height="250" alt="image-removebg-preview" src="https://github.com/user-attachments/assets/bf39def6-bd84-40d3-ad63-c261d27a0a88" />
<img width="350" height="250" alt="image3" src="https://github.com/user-attachments/assets/433cabf5-895e-41f3-8665-33afc534b4ba" />

<details>
<summary>1. Materials and cost</summary>

### 3D printing of parts

A Prusa MK4 (0.4 mm nozzle) printer was used. The total printing time is: 7h 39min + 10h 28min + 14h 54min + 1h 34min = 34h 35 min. Parameters:
- Layer height: 0.15 mm
- First layer height: 0.2 mm
- Infill: 5 %
- Raft: No
- Supports: Yes
- Ironing: 15 mm/s

Printing material:

- PLA 1.75 mm
- Extruder temperature: 215 °C
- Bed temperature: 60 °C
- PLA consumption: 78.73 g + 102.63 g + 146.65 g + 13.52 g = 341.53 g (5%)

### Additional material

Mechanical components:
- 12 × M4 Allen screws
- 7 x Jumper Wire 
- 1 x Hot glue
- 1 x Ping-pong ball

Electronic components:
- 1 × ATMega328P (Microcontroller socket board): 10 €
- 1 × MG996r motor: 6 €
- 1 × 341.53 g of PLA roll: 6.50 € (19€/kg)
- 1 × VL53L0X: 1.0 €

### Cost per prototype

Total cost: 23.50 €

</details>

<details>
<summary>2. Assembly</summary>

If you're reusing the code, try connecting everything as shown in the image below:

<img width="1075" height="725" alt="image" src="https://github.com/user-attachments/assets/6c240c90-e37e-4bd2-9243-f2fb357a2827" />

</details>
</details>
<details>
<summary>Model 2</summary>

A “ball-and-beam” structure supported at one end (with the same model at the other end)

<img width="350" height="250" alt="image22" src="https://github.com/user-attachments/assets/c8edad74-4b39-4ef2-a2e2-66cdb578f460" />
<img width="350" height="250" alt="image" src="https://github.com/user-attachments/assets/0f6acd55-1337-4300-8132-cff81ce212e4" />

<details>
<summary>1. Materials and cost</summary>

### 3D printing of parts

A Prusa MK4 (0.4 mm nozzle) printer was used. The total printing time is: 12 h 27 min
- Layer height: 0.15 mm
- First layer height: 0.2 mm
- Infill: 5 %
- Raft: No
- Supports: Yes
- Ironing: 15 mm/s

Printing material

- PLA 1.75 mm
- Extruder temperature: 215 °C
- Bed temperature: 60 °C
- PLA consumption: 112.5 g (5%)

### Additional material

Mechanical components:
- 3 × M4 1 cm Allen screws
- 2 x M2 8 cm Allen screws
- 2 x M2 washers
- 1 x 7 mm hollow aluminum rod: 2 €

Electronic components:
- 1 × ATMega328P (Microcontroller SMD board): 3.3 €
- 1 × MG996r motor: 6 €
- 1 × 112.2 g of PLA roll: 2.20 € (19€/kg)
- 1 × HC-SR04: 2.5 €

### Cost per prototype

Total: 16 €

</details>

<details>
<summary>2. Assembly</summary>

If you're reusing the code, try connecting everything as shown in the image below:

<img width="1069" height="713" alt="image" src="https://github.com/user-attachments/assets/cacfbd85-adf3-474e-af8e-c5bcedb8a1e6" />

</details>
</details>

