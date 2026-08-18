# Truti

A hobby project to build a free, fast, portable RTOS/OS for embedded systems; one that runs across a wide range of boards, peripherals, and displays, with a clean, modifiable architecture at the OS level.

## Current status

Early days. Right now this is a **WiFi-synced OLED clock** built from scratch on an ESP32, driving an SSD1306 display directly over I2C (no display libraries) via PlatformIO. This is the first real application built on top of what will eventually grow into the broader Truti platform.

## Goals

- **Portability** : run across a majority of embedded boards, not tied to one MCU family
- **Display abstraction** : start with SSD1306 (I2C OLED), expand to other displays/monitors over time
- **Peripheral abstraction** : a clean HAL so drivers plug in without rewriting application logic
- **Animation & rendering** : beyond static text, support animated and designed UI on constrained displays
- **RTOS core** : deterministic scheduling, small footprint, no unnecessary bloat
- **Modifiable at the OS level** : core subsystems (scheduler, memory, drivers) should be swappable/configurable, not hardcoded assumptions

This is a long-term hobby project, scope and direction may shift as it goes. The clock is the starting point, not the destination.

## Structure (evolving)

```
drivers/      # peripheral & display drivers (e.g. SSD1306)
hal/          # hardware abstraction layer
kernel/       # scheduler, core OS primitives (future)
apps/         # applications built on top (e.g. clock)
```

## Building

Built and managed with [PlatformIO](https://platformio.org/).

```bash
pio run
pio run --target upload
```
