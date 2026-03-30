# Ce2Lab - Ball and Beam

The Ball and Beam system is a classic experiment widely used in control engineering to study PID control techniques. The objective of the system is to maintain a ball at a desired position along a beam by adjusting the beam's angle. This is achieved by continuously measuring the ball position and applying a control action to a servo motor that tilts the beam.

<img width="350" height="250" alt="image-removebg-preview" src="https://github.com/user-attachments/assets/bf39def6-bd84-40d3-ad63-c261d27a0a88" />
 <img width="350" height="200" alt="image22" src="https://github.com/user-attachments/assets/c8edad74-4b39-4ef2-a2e2-66cdb578f460" />

In this project, an Arduino-based implementation of the Ball and Beam system is developed using an ultrasonic sensor to measure the ball position and a servo motor to control the beam angle. A PID (Proportional–Integral–Derivative) controller is implemented to automatically adjust the beam inclination so that the ball remains close to the desired target position.

To improve measurement stability, a low-pass filter is applied to the distance readings obtained from the ultrasonic sensor. This helps reduce noise and produces smoother control actions. The system operates in a closed-loop configuration where the controller continuously compares the desired position (setpoint) with the measured position and corrects the error.

This project is designed as an educational platform for learning feedback control, PID tuning, and embedded system implementation using Arduino. It demonstrates fundamental control concepts such as system dynamics, sensor filtering, and real-time control.

## Model 1

A “Ball and Beam” with its support in the center:

<img src="docs/model_11.gif" width="300">

<details>
<summary>1. Materials</summary>

### Prusa MK4 parameters, 0.4 nozzle

The total printing time is: 7h 39min + 10h 28min + 14h 54min + 1h 34min = 34h 35 min. Parameters:
- Layer height: 0.15 mm
- First layer height: 0.2 mm
- Infill: 5 %
- Raft: No
- Supports: Yes
- Ironing: 15 mm/s

### Printing material

- PLA 1.75 mm
- Extruder temperature: 215 °C
- Bed temperature: 60 °C
- PLA consumption: 78.73 g + 102.63 g + 146.65 g + 13.52 g = 341.53 g (5%)

### Additional material

- 13 × M4 Allen screws
- Hot glue
- 1 × Arduino UNO: 16 €
- 1 × MG996r motor: 6 €
- 1 × 341.53 g of PLA roll: 6.50 € 
- 1 × VL53L0X: 1.0 €

Total cost: 29.50 €

</details>

<details>
<summary>2. Assembly</summary>

If you're reusing the code, try connecting everything as shown in the image below:

<img width="1075" height="725" alt="image" src="https://github.com/user-attachments/assets/6c240c90-e37e-4bd2-9243-f2fb357a2827" />

</details>

## Model 2

A “ball-and-beam” structure supported at one end (with the same model at the other end)

<details>
<summary>1. Materials</summary>

### Prusa MK4 parameters, 0.4 nozzle
The total printing time is: 12 h 27 min
- Layer height: 0.15 mm
- First layer height: 0.2 mm
- Infill: 5 %
- Raft: No
- Supports: Yes
- Ironing: 15 mm/s

### Printing material

- PLA 1.75 mm
- Extruder temperature: 215 °C
- Bed temperature: 60 °C
- PLA consumption: 112.5 g (5%)

### Additional material

- 4 × M4 1 cm Allen screws
- 2 x M4 15 cm Allen screws
- 5 x M4 washers
- 1 x 7 mm hollow aluminum rod: 2 € 
- 1 × Arduino UNO: 16 €
- 1 × MG996r motor: 6 €
- 1 × 112.2 g of PLA roll: 2.20 € 
- 1 × HC-SR04: 2.5 €

Total: 28.7 €

</details>

<details>
<summary>2. Assembly</summary>

If you're reusing the code, try connecting everything as shown in the image below:

<img width="1088" height="724" alt="image" src="https://github.com/user-attachments/assets/6422b65a-0913-4173-85a9-4804b4280de7" />

</details>

## License

This project is licensed under the **BSD 3-Clause License**.

<details>
<summary>Details</summary>

Copyright (c) 2026, Fernando Cañadas Aránega, Enrique Rodríguez Miranda and José Luis Guzmán Sánchez

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holders nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES ARE DISCLAIMED.

For the complete license text see the `LICENSE` file in this repository.
</details>

## Contributor

[Fernando Cañadas Aránega](https://linktr.ee/fercanara)

Enrique Rodríguez Miranda

[Jose Luis Guzman Sánchez](https://w3.ual.es/personal/joguzman/)

Juan Diego Gil Vergel

Jose González Hernandez

Igor Mendes Lima Pataro


## References

Code template: [AntonAshraf](https://github.com/AntonAshraf/Ball-Beam-PID-Control/tree/main)

stl template: [OlegKor25](https://www.thingiverse.com/thing:6387659)
