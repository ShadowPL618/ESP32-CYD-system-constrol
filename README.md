# ESP32-CYD-system-constrol

Markdown
# ESP32 CYD PC Hardware Monitor

An interactive, real-time PC hardware monitoring dashboard that streams performance telemetry from a Windows host machine onto an ESP32 Cheap Yellow Display (CYD) via Serial USB connection.

---

## Features

* **Live Telemetry Stream:** Tracks real-time CPU Workload, RAM utilization, and system Battery levels.
* **Dynamic Disk Management:** Monitors system drive usage. Automatically switches its layout to show `D: SSD SPACE` if an external SSD or partition is plugged in, reverting back to `C: DRIVE SPACE` when disconnected.
* **NVIDIA GPU Integration:** Fetches dedicated GPU core load and current running temperatures using native system queries.
* **Network Throughput:** Displays live connection speeds calculated dynamically in Megabits per second (Mbps).
* **Connection Status Badge:** Includes an intelligent connection monitoring window (`SYS LINK`) that detects if the background desktop stream engine is active or disconnected.

---

## Hardware Requirements

1. **ESP32 Cheap Yellow Display (CYD):** ESP32-2432S028R development board (ILI9341 320x240 resistive touch screen).
2. **Micro-USB / USB-C Data Cable:** Ensure the cable handles data transmission, not just power charging.
3. **Windows Host PC:** To run the background tracking controller script.

---

## Software Dependencies

### 1. Windows Host (Python Configuration)
The system data extraction script is built for **Python 3.11+**. 

Install the required hardware tracking libraries using your command terminal:
```bash
pip install psutil pyserial
Note: Make sure your nvidia-smi utility is accessible in your system environment path if you want to stream dedicated GPU telemetry.

2. Microcontroller Firmware (Arduino IDE)
To compile the device layout sketch, you need the Arduino IDE configured with the ESP32 board manager collection installed.

Required Microcontroller Libraries:
LovyanGFX (v1.x.x): Used for low-level high-performance display grid rendering and frame layouts.

File Structure
PC_stats.py: The Python automation script running locally on the Windows host machine to pull telemetry data and output stream strings over Serial.

display_firmware.ino: The main C++ firmware running on the ESP32 display board to receive, split, and display data blocks onto the screen panel interface.

Setup & Installation
Step 1: Flash the ESP32 Display
Open display_firmware.ino inside your Arduino IDE.

Select your corresponding ESP32 Dev Module board configuration and target USB COM port.

Verify and upload the code to your display module. The screen will initialize and display AWAITING STREAM....

Step 2: Configure the Python Agent
Open PC_stats.py in a text editor.

Locate the COM_PORT assignment configuration parameter (near line 33):

Python
COM_PORT = 'COM3'  # Update this value to match your device port!
Change 'COM3' to match the exact COM port number your device was assigned by Windows Device Manager.

Step 3: Run the Monitor Engine
Open your terminal emulator inside the project directory and launch the telemetry connection tool:

Bash
python PC_stats.py
The screen link will instantly wake up, display your system configurations, and begin animating live updates every second!
