# How Automated Geobot Works

Automated Geobot is a mobile environmental monitoring robot designed
to collect information about its surroundings while moving.

## Main Controller

The Arduino UNO acts as the main controller of EcoBot.

It receives information from the environmental sensors and
controls the LCD, GPS system and motor driver.

## Environmental Monitoring

Automated Geobot uses several sensors to monitor its environment:

- Soil moisture sensor — monitors soil moisture.
- DHT11 — measures temperature and humidity.
- Dust sensor — monitors dust in the surrounding air.
- Rain sensor/module — detects rain or water.

The sensor information can be processed by the Arduino UNO
and displayed on the LCD.

## Movement System

The Arduino UNO controls the motor driver.

The motor driver provides the required power and control for
the BO motors and allows EcoBot to move.

The basic movement system is:

Arduino UNO → Motor Driver → BO Motors

The motor driver can be used to control the movement and
direction of the motors.

## Camera System

The ESP32-CAM provides camera capabilities for EcoBot.

It can be used for visual monitoring and can capture images
or video of the robot's surroundings.

## LCD Display

The LCD displays information collected by EcoBot.

It can be used to show sensor readings and other useful
information while the robot is operating.

## GPS System

The GPS module provides location information for EcoBot.

The Arduino UNO can receive GPS data and use it to determine
the robot's location.

## Power System

The battery provides electrical power to the EcoBot system.

Power is distributed to the Arduino, sensors, motor driver,
ESP32-CAM and other components according to their required
voltage and current.

## Complete System

The main system can be represented as:

Environmental Sensors
        ↓
   Arduino UNO
        ↓
 ┌──────┼─────────┐
 ↓      ↓         ↓
 LCD   GPS   Motor Driver
                  ↓
               BO Motors

The ESP32-CAM provides the camera system for visual
monitoring.

## Project Status

The Automated Geobot prototype is completed and the project is being
documented and tested.

The final body covering and complete testing will be added
to the project documentation.
