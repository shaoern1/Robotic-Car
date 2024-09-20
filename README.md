# Robotic-Car
## Embedded Systems P3G Robot Car

### Members and Task Allocations (TBAa)
**Team Members** <br>
MUHAMMAD AZREEN BIN MUHAMMAD <br>
CHNG SONG HENG ALOYSIUS <br>
SEBASTIAN NUGUID FERNANDEZ <br>
TOH SHAO ERN <br>
ASHSYAHID BIN HUSSIN <br>

### Project Description
---
Our project focuses on developing an autonomous two-wheeled robot car powered by the Raspberry Pi Pico W as the main microcontroller. The robot efficiently navigates a predefined map using advanced navigation algorithms and incorporates obstacle detection and avoidance systems. It can also decode barcodes following the Barcode-39 standard. A PID controller ensures smooth and precise movement, while sensor fusion improves the car's overall performance, decision-making, and accuracy.

### Program Design (TBA)
---
In this section, we will go over the block diagram and modular design of the car. Our project comprises of 8 components, each catered for a particular sensor/aspect of the car:
- Main (program entrypoint for car to start executions)
- Navigation and Mapping
- Wifi Module
- Motor
- Ultrasonic Sensor(for obstacle detection)
- IRLine Sensor (for barcode decoding)
- Wheel Encoder (for car movement)
- Magnetometer (for car direction tracking)

### Component List (TBA)


**Raspberry Pi**
- 1 x [Raspberry Pi Pico W](https://www.raspberrypi.com/products/raspberry-pi-pico/)

### Project Structure (TBA)
The pentrypoint for this project resides in the `main.c` file. References are made by this program to functions and structs stored in the individual components' driver code via libraries enabled by these codes' `CMakeLists.txt` file.

```
docs/ (documentation images)

encoder/ (driver code for wheel encoders)

irline/ (driver code for barcode decoding and wall detection)

magnetometer/ (driver code for magnetometer and orientation)

main/ (main program code)

motor/ (driver code for the motors and PID controller)

nav/ (code for navigation algorithms to traverse the map within the shortest amount of time)

ultrasonic/ (code for ultrasonic sensor obstacle detection)

wifi/ (code for SSI and CGI and web server)

README.md (this file)
```

### Getting Started (TBA)
