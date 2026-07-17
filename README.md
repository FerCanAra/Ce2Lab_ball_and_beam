# Ce2Lab - Ball and Beam <img width="60" height="30" alt="image-removebg-preview" src="https://github.com/user-attachments/assets/bf39def6-bd84-40d3-ad63-c261d27a0a88" />

The Ball and Beam system is a classic experiment widely used in control engineering to study PID control techniques. The objective of the system is to maintain a ball at a desired position along a beam by adjusting the beam's angle. This is achieved by continuously measuring the ball position and applying a PID control action to a servo motor that tilts the beam.

 <img src="docs/Model_2_1.gif" width="350">

In this project, a different ATmega328P-based implementation of the Ball and Beam system is developed, using an ultrasonic and laser sensors to measure the ball's position and a servo motor to control the beam angle. A PID (Proportional–Integral–Derivative) controller is implemented to automatically adjust the beam inclination, keeping the ball close to the desired target position.

This work was carried out within the [Automation, Robotics and Mechatronics Group](https://arm.ual.es/arm-group/) (ARM TEP 197) at the University of Almería (Spain).

<a href="https://arm.ual.es/arm-group/"> 
  <img src="docs/arm-logo2.jpg" width="150" alt="ARM Group Logo" /> 
</a>
<a href="https://www.linkedin.com/company/automatic-robotics-and-mechatronics-research-group/"> 
  <img src="docs/logo2.png" width="55" alt="LinkedIn Logo" /> 
</a>

> **Note:** Process tested with Windows 11 with PlatformIO (VSC) on 8th April 2026.

## Model 1

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

<img width="460" height="315" alt="Conexion_2 (1)" src="https://github.com/user-attachments/assets/bbdf47e4-707b-4e8d-b99d-7b35124cfb7c" />

</details>

## License

<details>
<summary>This project is licensed under the **BSD 3-Clause License**.</summary>

Copyright (c) 2026, Fernando Cañadas Aránega, Igor Mendes Lima Pataro, Enrique Rodríguez Miranda, José Luis Guzmán Sánchez and Juan Diego Gil Vergel.

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

[Fernando Cañadas Aránega](https://linktr.ee/fercanara), 
Igor Mendes Lima Pataro,
Enrique Rodríguez Miranda, 
[Jose Luis Guzman Sánchez](https://w3.ual.es/personal/joguzman/) and
[Juan Diego Gil Vergel](https://w3.ual.es/personal/jgv132/).


_University of Almería, Department of Computer Science, CIESOL, ceia3, Ctra. Sacramento s/n, 04120, Almería, Spain._

## Acknowledgements

This work has been funded by the Project "Unlocking the Development and Experiences of Control Education Labs (CE2Labs)".

## References

Code template: [AntonAshraf](https://github.com/AntonAshraf/Ball-Beam-PID-Control/tree/main)

stl template: [OlegKor25](https://www.thingiverse.com/thing:6387659)
