# **📡 ESP32 RC Transmitter & Receiver (iBUS \+ ESP-NOW)**

| Status | ✅ Stable | Protocol | iBUS | Wireless | ESP-NOW |
| :---- | :---- | :---- | :---- | :---- | :---- |
| **Board** | ESP32 | **Target** | Flight Controllers (FCs) | **Range** | Up to 1 km (with external antenna) |

This project transforms two **ESP32** development boards into a complete, low-latency Radio Control (RC) system. It uses **ESP-NOW** for robust wireless communication and outputs the **iBUS** protocol, making it compatible with popular flight controllers like Betaflight, INAV, and Ardupilot.

## **✨ Key Features**

This system is a pure ESP32 solution, requiring no extra RF modules.

* **⚡ Low Latency:** Utilizes the fast, peer-to-peer **ESP-NOW** protocol.  
* **🔌 FC Compatibility:** Outputs **iBUS** frames directly to your flight controller (FC).  
* **📐 Auto-Calibration:** Automatically centers joysticks on startup for easy setup.  
* **🛡️ Configurable Deadzone:** Applies a deadzone (default ±34 ADC raw value) to prevent stick jitter.  
* **⬆️ Long Range Mode:** **ESP-NOW Long-Range Mode** is enabled for better open-field coverage (up to 1 km with external antenna).  
* **🕹️ 6 Channels:** Supports 4 primary control axes (**Roll, Pitch, Yaw, Throttle**) and 2 3-position auxiliary switches (**AUX1, AUX2**).  
* **🐛 Debugging:** Full debug logging available on both TX and RX via USB Serial Monitor.

## **🛠️ Hardware Required**

| Component | Quantity | Notes |
| :---- | :---- | :---- |
| **ESP32 Boards** | 2 | Any devkit with sufficient **ADC pins**. |
| **Analog Joysticks** | 2 | Standard 2-axis joysticks (e.g., KY-023 or similar). |
| **3-Position Switches** | 2 | Toggle switches wired with the center pin to 3.3V. |
| **Flight Controller** | 1 | Must support **iBUS** input (e.g., Betaflight, INAV, Ardupilot). |

## **🏗️ System Breakdown**

### **TX (Transmitter \- Handheld Unit)**

The TX ESP32 reads all physical inputs and transmits the data wirelessly.

* Reads 2× analog joysticks (**Roll, Pitch, Yaw, Throttle**).  
* Reads 2× 3-position switches (**AUX1, AUX2**).  
* Automatically performs **joystick center calibration** on startup.  
* Sends control packets to the RX via ESP-NOW (Long-Range Mode enabled).

| Input | Pin on ESP32 |
| :---- | :---- |
| **Roll (J1X)** | 34 |
| **Pitch (J1Y)** | 35 |
| **Yaw (J2X)** | 32 |
| **Throttle (J2Y)** | 33 |
| **Switch 1 UP** | 25 |
| **Switch 1 DOWN** | 26 |
| **Switch 2 UP** | 27 |
| **Switch 2 DOWN** | 14 |
| *Note: Switch center pins should be wired to* 3.3V*.* |  |

### **RX (Receiver \- Aircraft/Vehicle Unit)**

The RX ESP32 receives the data and converts it to the iBUS protocol for the FC.

* Receives control data via **ESP-NOW**.  
* Maps joystick and switch data to the corresponding iBUS channels.  
* Outputs the iBUS frame over **UART** on Pin17.  
* **Connection:** Plug Pin17 directly into your FC’s **iBUS / UART RX pin**.

| Output | Pin on ESP32 |
| :---- | :---- |
| **iBUS Output** | 17 |

## **⬇️ Installation and Setup**

### **Prerequisites**

1. **Install Arduino IDE.**  
2. Install the **ESP32 board package** (Arduino Core v3.x or later is recommended).

### **Uploading the Code**

1. **Clone the repository:**  
   git clone \[https://github.com/yourusername/esp32-rc-ibus-espnow.git\](https://github.com/yourusername/esp32-rc-ibus-espnow.git)  
   cd esp32-rc-ibus-espnow

2. Open TX.ino and RX.ino in the Arduino IDE.  
3. **Get the RX MAC Address:**  
   * Upload RX.ino to the receiver ESP32.  
   * Open the Serial Monitor to get the printed **MAC address** of the RX board.  
4. **Configure the TX:**  
   * In TX.ino, replace the placeholder in rxMac\[\] with the MAC address you just retrieved.  
5. Upload TX.ino to the transmitter ESP32.  
6. **Wire up the hardware** as per the [System Breakdown](https://www.google.com/search?q=%23-system-breakdown) section.  
7. **Connect the RX:** Connect the RX ESP32's **Pin** 17 to your FC's UART RX pin.

## **⚙️ Usage and Configuration**

### **Flight Controller Setup**

1. In your FC configurator (e.g., Betaflight, INAV), navigate to the **Ports** tab.  
2. Set the UART connected to the RX's **Pin** 17 to use the **iBUS** protocol for the serial receiver.  
3. Verify the connection in the **Receiver** tab—moving the sticks and switches should register channel changes.

### **Channel Mapping (Default)**

| Channel | Control | Input | Notes |
| :---- | :---- | :---- | :---- |
| **CH1** | **Roll** | Joystick 1 X-Axis | Primary Aileron |
| **CH2** | **Pitch** | Joystick 1 Y-Axis | Primary Elevator |
| **CH3** | **Yaw** | Joystick 2 X-Axis | Primary Rudder |
| **CH4** | **Throttle** | Joystick 2 Y-Axis | Primary Throttle |
| **CH5** | **AUX1** | Switch 1 | 3-position switch |
| **CH6** | **AUX2** | Switch 2 | 3-position switch |

### **Switch Values**

The **3-position switches (AUX channels)** map to standard iBUS pulse-width modulation (PWM) values:

* **Middle:** 1500  
* **Up:** 1300  
* **Down:** 1700

### **Advanced Notes**

* **Range:** With standard PCB antenna ESP32 boards, expect a reliable range of ∼200−400m. Using boards with external antenna connectors (e.g., ESP32-WROOM-32U) can extend the range up to ∼1 km in open fields.  
* **Update Rate:** The transmission update rate is controlled by the delay(20) call in the TX.ino loop. You can decrease this value to speed up the update rate, though it may increase CPU load and power consumption.  
* **Extensibility:** The system is easily scalable—you can extend the code to include more AUX switches or even different analog inputs (e.g., rotary potentiometers).

## **📱 Use a Phone as a Controller**

If you want to use a phone as your controller instead of a physical joystick, you can use the **ESP32 WiFi Joystick to iBUS Translator**. This project allows your phone to send control data over WiFi, which is then translated to iBUS by an ESP32.

Find the project here: [https://github.com/Boyyt357/ESP32-WiFi-Joystick-to-iBUS-Translator](https://github.com/Boyyt357/ESP32-WiFi-Joystick-to-iBUS-Translator)
