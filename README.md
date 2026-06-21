
# 🚀 Industrial Guardian: ESP32 IoT Environmental Monitor with n8n Automation

An intelligent Industrial IoT (IIoT) monitoring system that tracks environmental metrics using an ESP32 micro-controller, handles real-time edge computing safety cutoffs, and broadcasts data payloads to a cloud-based n8n automation engine.

---

## 🛠️ System Architecture

The system reads physical sensor data at the edge, evaluates emergency conditions to trigger localized hardware overrides (LEDs), and securely streams state data to a cloud network.

* **Microcontroller:** ESP32 (DevKit V1)
* **Sensors:** DHT22 (Temperature & Humidity), MQ-2 Analog Gas Sensor, Potentiometer (Vibration Simulator)
* **Edge Actions:** Automated physical isolation rules via status LEDs
* **Cloud Orchestration:** n8n Webhook Workflow Automation Engine

---

## 💾 Hardware Layout (`diagram.json`)

To run this circuit inside the [Wokwi Simulator](https://wokwi.com), use the configuration below inside your `diagram.json` tab:

```json
{
  "version": 1,
  "author": "Anonymous maker",
  "editor": "wokwi",
  "parts": [
    { "type": "wokwi-esp32-devkit-v1", "id": "esp", "top": 110.4, "left": 139.2, "attrs": {} },
    { "type": "wokwi-dht22", "id": "dht", "top": -48, "left": -115.2, "attrs": {} },
    { "type": "wokwi-led", "id": "led-red", "top": 192, "left": -48, "attrs": { "color": "red" } },
    { "type": "wokwi-led", "id": "led-yellow", "top": 192, "left": -19.2, "attrs": { "color": "yellow" } },
    { "type": "wokwi-led", "id": "led-blue", "top": 192, "left": 9.6, "attrs": { "color": "blue" } },
    { "type": "wokwi-led", "id": "led-green", "top": 192, "left": 38.4, "attrs": { "color": "green" } },
    { "type": "wokwi-resistor", "id": "r1", "top": 240, "left": -57.6, "attrs": { "value": "220" } },
    { "type": "wokwi-resistor", "id": "r2", "top": 240, "left": -28.8, "attrs": { "value": "220" } },
    { "type": "wokwi-resistor", "id": "r3", "top": 240, "left": 0, "attrs": { "value": "220" } },
    { "type": "wokwi-resistor", "id": "r4", "top": 240, "left": 28.8, "attrs": { "value": "220" } },
    { "type": "wokwi-potentiometer", "id": "pot", "top": -155.2, "left": -4.6, "attrs": {} },
    { "type": "gas-sensor", "id": "gas", "top": -260, "left": 60, "attrs": {} }
  ],
  "connections": [
    [ "esp:GND.1", "r1:1", "black", [ "v0" ] ],
    [ "esp:GND.1", "r2:1", "black", [ "v0" ] ],
    [ "esp:GND.1", "r3:1", "black", [ "v0" ] ],
    [ "esp:GND.1", "r4:1", "black", [ "v0" ] ],
    [ "r1:2", "led-red:C", "black", [ "v0" ] ],
    [ "r2:2", "led-yellow:C", "black", [ "v0" ] ],
    [ "r3:2", "led-blue:C", "black", [ "v0" ] ],
    [ "r4:2", "led-green:C", "black", [ "v0" ] ],
    [ "esp:D13", "led-red:A", "red", [ "v0" ] ],
    [ "esp:D12", "led-yellow:A", "yellow", [ "v0" ] ],
    [ "esp:D14", "led-blue:A", "blue", [ "v0" ] ],
    [ "esp:D27", "led-green:A", "green", [ "v0" ] ],
    [ "esp:D15", "dht:SDA", "blue", [ "v0" ] ],
    [ "esp:3V3", "dht:VCC", "red", [ "v0" ] ],
    [ "esp:GND.2", "dht:GND", "black", [ "v0" ] ],
    [ "esp:D34", "pot:SIG", "green", [ "v0" ] ],
    [ "esp:3V3", "pot:VCC", "red", [ "v0" ] ],
    [ "esp:GND.2", "pot:GND", "black", [ "v0" ] ],
    [ "esp:D32", "gas:AO", "orange", [ "v0" ] ],
    [ "esp:3V3", "gas:VCC", "red", [ "v0" ] ],
    [ "esp:GND.2", "gas:GND", "black", [ "v0" ] ]
  ]
}

```

---

## 🧠 Edge Matrix Rules & Actions

The firmware constantly runs structural diagnostic checks at the edge level to protect localized hardware instantly without waiting for network latencies:

| Priority | Condition Trigger | Assigned System State | Physical Hardware Action |
| --- | --- | --- | --- |
| **1 (Critical)** | Gas > Ambient + 300 | `EMERGENCY_GAS_SHUTDOWN` | **RED** + **GREEN** LEDs Active |
| **2 (High)** | Temp $\ge$ 80.0°C | `EMERGENCY_THERMAL_SHUTDOWN` | **RED** LED Active |
| **3 (Warning)** | Temp $\ge$ 68.0°C | `DUAL_COOLING_ACTIVE` | **YELLOW** + **BLUE** LEDs Active |
| **4 (Caution)** | Temp $\ge$ 60.0°C | `LIGHT_FAN_COOLING_ACTIVE` | **YELLOW** LED Active |
| **5 (Normal)** | Safe Baselines | `SYSTEM_NORMAL` | All indicators clear |

---

## 📡 Cloud JSON Data Payload Structure

Every 4 seconds, if connected to `Wokwi-GUEST` virtual network infrastructure, the ESP32 pushes a structured JSON telemetry object down the pipeline via an HTTP POST Request:

```json
{
  "temperature": 62.5,
  "vibration": 1420,
  "gas_raw": 128,
  "state": "LIGHT_FAN_COOLING_ACTIVE"
}

```

---

## 🚀 How to Set Up & Run

1. Clone this repository to your computer or copy the code files.
2. Open a workspace in **[Wokwi Simulator](https://wokwi.com)**.
3. Paste the contents of `sketch.ino` into your main file, and paste the hardware JSON config above into the `diagram.json` tab.
4. Update the `n8n_webhook` string constant at the top of your code with your live n8n instance link.
5. In n8n, turn your automation workflow switch to **Active**.
6. Fire up the Wokwi Simulation, tweak the interactive sensor sliders (e.g. to `62°C`), and watch the automation cascade down your pipeline!

```

---


