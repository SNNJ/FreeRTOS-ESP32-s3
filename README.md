# FreeRTOS-ESP32-s3
This project implements the embedded firmware for a **simulated satellite system** built on the ESP32 platform using the CADSE v5 board. The firmware controls multiple sensors, user interfaces, and communication protocols while supporting real-time operations via **FreeRTOS**.

```markdown
# 🚀 CADSE Satellite Firmware (ESP32)

This project implements the embedded firmware for a **simulated satellite system** built on the ESP32 platform using the CADSE v5 board. The firmware controls multiple sensors, user interfaces, and communication protocols while supporting real-time operations via **FreeRTOS**.

---

## 📌 Features

- ✅ **Modular multi-mode architecture** (Mode 0–5)
- ✅ **Real-time FreeRTOS tasking**
- ✅ **Sensor support**:
  - IMU (LSM6DS): Acceleration, Gyro, Temperature
  - BME280: Pressure, Temperature, Humidity
  - Voltage Monitoring: Battery & USB voltages
- ✅ **User input** via capacitive touch & push buttons
- ✅ **OLED display with 3 visual modes**:
  - Tabular telemetry
  - Artificial horizon (attitude)
  - Rolling plot of analog signals
- ✅ **Audible buzzer alerts** (mode indication, alarms)
- ✅ **LED feedback system** via MCP23017 I/O expander
- ✅ **Persistent flash storage** for default mode & file data
- ✅ **MQTT communication**:
  - Telemetry data published every second
  - Telecommand reception and processing
- ✅ **Doxygen-powered source code documentation**
- ✅ **PlatformIO-based ESP32 development environment**

---

## 📂 Project Structure

```
📁 src/
├── modes/                # Mode-specific behavior
├── sensors/              # IMU, BME280, voltage sensing
├── hardware/             # Touch, LED, buzzer, storage
├── main.cpp              # System entry point and task setup
📁 include/
├── *.h                   # All public header files (Doxygen-annotated)
```

---

## 🔧 Requirements

- [PlatformIO](https://platformio.org/) + VS Code
- ESP32 (CADSE v5 board)
- MQTT broker (e.g., `heide.bastla.net`)
- Optional: LaTeX (for PDF docs), Doxygen (for docs)

---

## 📡 Operation Modes

| Mode | Function |
|------|----------|
| 0    | Idle / Wait for commands |
| 1    | Microgravity detection (buzzer alert) |
| 2    | Pressure drop monitoring (LED/buzzer alarm) |
| 3    | Artificial horizon (IMU-based roll indicator) |
| 4    | Rolling plot of analog data (selectable source) |
| 5    | Telecommand & data packet validation (via MQTT) |

---

## 📘 Documentation

This project uses **Doxygen** for full source code documentation.

To generate HTML docs:
```bash
doxygen Doxyfile
```

To generate a PDF (optional):
```bash
cd latex
pdflatex refman.tex
```

Find the output in `/docs/html/index.html` or `/latex/refman.pdf`.

---

## 📥 Telecommands (via MQTT)

- `SetMode` → switches current mode
- `SetDefaultMode` → saves boot mode to flash
- `PacketID` → handles file chunks for transmission and display in Mode 5

---

## 🧑‍💻 Author

**Siamak Najafinia**  
CADSE – Embedded Satellite Systems

---
