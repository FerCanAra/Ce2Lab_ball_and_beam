# Ce2Lab Ball and Beam

The Ball and Beam system is a classic experiment widely used in control engineering to study feedback control techniques. The objective of the system is to maintain a ball at a desired position along a beam by adjusting the beam's angle. This is achieved by continuously measuring the ball position and applying a control action to a servo motor that tilts the beam.

In this project, an Arduino-based implementation of the Ball and Beam system is developed using an ultrasonic sensor to measure the ball position and a servo motor to control the beam angle. A PID (Proportional–Integral–Derivative) controller is implemented to automatically adjust the beam inclination so that the ball remains close to the desired target position.

To improve measurement stability, a low-pass filter is applied to the distance readings obtained from the ultrasonic sensor. This helps reduce noise and produces smoother control actions. The system operates in a closed-loop configuration where the controller continuously compares the desired position (setpoint) with the measured position and corrects the error.

This project is designed as an educational platform for learning feedback control, PID tuning, and embedded system implementation using Arduino. It demonstrates fundamental control concepts such as system dynamics, sensor filtering, and real-time control.


## License

This project is licensed under the **BSD 3-Clause License**.

Copyright (c) 2026, Fernando Cañadas Aránega and Enrique Rodriguez Miranda  
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







## Modelo 1

- El tiempo total de impresión es: 7h 39min + 10h 28 min + 14h 54min + 1h 34min = 34h 35 min

### Parámetros de prusa mk4, 04 nozzle
- Altura capa: 0.15 mm
- Altura 1º copa: 0.2 mm
- Relleno: 5 %
- Balsa: No
- Soportes: Sí

- Alisado: 15 mm/s

### Material de impresión
- PLA 1.75 mm
- 215º extrusor
- 60º cama
- Consumo de PLA: 78.73 g + 102.63 g + 146.65 g + 13.52 g = 341.53 g (5 %)

### Material adicional

- 13 tornillos m4 allen
- Silicona caliente

- 1 x Arduino UNO: 16€
- 1 x Motor sg90: 6 €
- 1 x Rollo PLA: 19 €
- 1 x HCSR04: 2.5 €

Total: 43.5 €

## References

Code template: [AntonAshraf](https://github.com/AntonAshraf/Ball-Beam-PID-Control/tree/main)
stl template: [OlegKor25](https://www.thingiverse.com/thing:6387659)
