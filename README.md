# Truti

**An experimental embedded operating system and hardware abstraction
framework for microcontrollers.**

Truti is an ongoing project to build a lightweight, portable and
modular operating system for embedded systems, starting with the
ESP32.

The goal is to develop an architecture where hardware, peripherals,
system services and applications interact through well-defined
abstractions, without tying the entire system to a particular
microcontroller or hardware configuration.

The long-term vision extends beyond a conventional RTOS: Truti aims
to explore distributed embedded systems where multiple devices can
expose capabilities, communicate and coordinate through a common
software architecture.

---

## Project Status

Truti is currently in its early development stage.

The initial implementation focuses on establishing the hardware
abstraction layer and building applications on top of it.

### Implemented

- Hardware abstraction layer for peripheral integration.
- SSD1306 OLED display abstraction.
- Direct I2C communication with the SSD1306 display, without relying
  on external display libraries.
- Initial WiFi-synchronised OLED clock application.
- ESP32 development using PlatformIO.

### In Progress

- MPU6050 IMU integration.
- Expansion of the peripheral abstraction layer.
- Development of foundational operating system components.
- Establishing a modular architecture for hardware services.

### Planned

- Portable hardware abstraction across different MCU families.
- Memory management and resource management.
- Task scheduling and execution management.
- Modular driver and peripheral management.
- Display rendering and animation framework.
- Inter-process communication and system services.
- Device-to-device communication and distributed services.
- An extensible architecture for integrating intelligent agents.

---

## Architecture

Truti is being designed around a modular architecture that separates
hardware-specific implementations from system-level functionality.

The intended architecture consists of the following layers:

```text
+---------------------------------------+
|             Applications              |
+---------------------------------------+
|          System Services               |
+---------------------------------------+
|       Kernel / OS Components           |
|                                       |
|  Scheduling | Memory | IPC | Resources |
+---------------------------------------+
|     Hardware Abstraction Layer        |
+---------------------------------------+
|       Peripheral Drivers              |
+---------------------------------------+
|              Hardware                 |
|                                       |
|  ESP32 | Displays | Sensors | Modules |
+---------------------------------------+
