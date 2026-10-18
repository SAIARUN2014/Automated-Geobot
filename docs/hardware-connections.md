# Automated Geobot — Hardware Connections

This document describes the recommended wiring and power connections for the Automated Geobot using the components you specified:
- DHT11 temperature/humidity sensor
- 16×2 LCD (I2C)
- Two DC motors driven by a single L298N motor driver
- GPS module (e.g., NEO-6M)
- ESP32-CAM used for camera and for exchanging control/telemetry with the Arduino UNO

Important safety notes
- Use a common ground between all modules (Arduino, L298N, ESP32-CAM, GPS, power supplies).
- Motor power (battery or adapter) should be connected to the L298N motor Vcc (Vmotor) and NOT directly to the Arduino 5V rail.
- ESP32-CAM requires a stable 5V supply capable of providing at least 700–1000 mA peak. Use a dedicated regulator or USB power bank if possible.
- Arduino UNO GPIO are 5V. ESP32 GPIO are 3.3V — never drive ESP32 inputs directly from Arduino TX without level shifting or a voltage divider.

Note about 4-motor attempts
- You mentioned trying to run 4 motors from a single L298N and it failed. That is expected: a single L298N module is designed to drive 2 motors (2 H-bridges). If you ever add more motors, use two L298N modules or a dedicated 4-motor driver board. For this repo and documentation we assume a 2-motor setup to avoid confusion.

Component wiring (recommended for 2 motors)

1) L298N motor driver (two DC motors)
- L298N +Vmotor -> Motor battery positive (e.g., 12V)
- L298N GND -> Battery negative and common ground to Arduino GND
- L298N 5V (logic) -> If your L298N has an onboard 5V regulator and you need it, you can jumper to produce 5V logic (optional). Otherwise power Arduino 5V from a stable regulator.

Pin mapping (Arduino UNO -> L298N)
- IN1 -> D2  (left motor direction)
- IN2 -> D4  (left motor direction)
- ENA -> D3  (left motor PWM speed control)
- IN3 -> D5  (right motor direction)
- IN4 -> D7  (right motor direction)
- ENB -> D6  (right motor PWM speed control)
- Left motor -> OUT1/OUT2
- Right motor -> OUT3/OUT4

Notes
- Use PWM pins (D3, D5, D6, D9, D10, D11 on UNO) for ENA/ENB so you can control speed.
- Add a bulk electrolytic capacitor (e.g., 470 µF or higher) across the motor supply near the L298N to reduce voltage dips when motors start.

2) DHT11 sensor
- VCC -> 5V
- GND -> GND
- DATA -> D12 (digital pin)
- Pull-up: place a 4.7 kΩ–10 kΩ resistor between DATA and VCC (the DHT11 usually needs a pull-up)

3) 16×2 LCD (I2C backpack assumed)
- SDA -> A4
- SCL -> A5
- VCC -> 5V
- GND -> GND

If your LCD is parallel (no I2C), tell me and I'll add the parallel 4-bit wiring.

4) GPS module (NEO-6M or similar)
- VCC -> 5V (or 3.3V if module requires it; check module)
- GND -> GND
- TX (GPS TX) -> Arduino RX (SoftwareSerial RX pin)
- RX (GPS RX) -> Arduino TX (SoftwareSerial TX pin)

Suggested SoftwareSerial pins for GPS:
- GPS_TX -> D10 (connects to Arduino D10 as RX)
- GPS_RX -> D11 (connects to Arduino D11 as TX)

5) ESP32-CAM (camera + control)
Power
- 5V -> 5V (VIN or 5V pin depending on board)
- GND -> GND

Serial (recommended: SoftwareSerial on UNO)
- UNO D8 (RX for SoftwareSerial) <- ESP32 TX
- UNO D9 (TX for SoftwareSerial) -> ESP32 RX (use a voltage divider / level shifter to drop 5V -> 3.3V)

Level shifting
- Protect ESP32 RX from 5V Arduino TX with a voltage divider (e.g., 10kΩ and 22kΩ) or a proper logic-level shifter.
- ESP32 TX (3.3V) to Arduino RX is safe without shifting.

ESP32 notes
- ESP32-CAM modules often reset or brown-out if the 5V supply can't source enough current. Use a regulator that can supply at least 1A.
- Programming the ESP32-CAM is easiest with an FTDI adapter; if you're using the UNO as a USB-serial bridge, be careful with TX/RX connections and powering.

6) Powering summary
- Motor battery -> L298N Vmotor (e.g., 12V)
- Arduino 5V -> stable 5V regulator (or USB)
- ESP32-CAM -> stable 5V supply with sufficient current
- GPS and sensors -> 5V (unless they need 3.3V)
- All grounds tied together

7) Suggested Arduino pin summary
- D2  -> L298N IN1
- D3  -> L298N ENA (PWM)
- D4  -> L298N IN2
- D5  -> L298N IN3
- D6  -> L298N ENB (PWM)
- D7  -> L298N IN4
- D8  -> SoftwareSerial RX (connect to ESP32 TX)
- D9  -> SoftwareSerial TX (connect to ESP32 RX through level shifter)
- D10 -> GPS TX (SoftwareSerial RX)
- D11 -> GPS RX (SoftwareSerial TX)
- D12 -> DHT11 DATA
- A4  -> I2C SDA (LCD)
- A5  -> I2C SCL (LCD)

8) Troubleshooting tips
- If motors cause Arduino resets, move the motor power to a separate battery or add decoupling capacitors and ferrite beads. Use a separate regulator for logic.
- If ESP32-CAM repeatedly reboots, check the 5V regulator current rating and wiring.
- If GPS data is garbled, verify baud rate (commonly 9600) and confirm TX/RX pins are correct. Use SoftwareSerial on the UNO.

9) Next steps I can do for you
- Add a parallel-LCD wiring variant if your LCD isn't I2C.
- Add a labelled wiring diagram (PNG) for visual guidance.

---

If this matches your hardware, I will commit this file to docs/hardware-connections.md. If you want any pin changes or have the LCD in parallel mode, tell me now.
