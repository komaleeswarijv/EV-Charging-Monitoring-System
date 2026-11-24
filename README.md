# EV Charging and Vehicle Monitoring System

## Project Overview

An ESP32-based EV Charging and Vehicle Monitoring System developed and tested using the Wokwi simulator, with Blynk IoT dashboard integration.

The project monitors simulated EV charging parameters, identifies abnormal charging conditions, provides driver load-anomaly indications, and detects simulated unauthorised vehicle movement through a security mechanism.

## Implemented Features

- ESP32 microcontroller simulation using Wokwi.
- Simulated voltage and current monitoring using potentiometers.
- Temperature monitoring using a DHT22 sensor.
- Real-time charging parameter display using a 16x2 I2C LCD.
- Overcurrent detection and alert.
- Low-voltage and slow-charging detection.
- Charging fault detection.
- Simulated driver load-anomaly detection based on sudden current changes.
- Anti-theft monitoring using a security switch.
- Buzzer alerts for overcurrent, theft, and low-voltage conditions.
- Serial Monitor output for real-time monitoring.
- Blynk IoT integration for remote monitoring of voltage, current, temperature, charging status, and security status.

## Technologies Used

- ESP32
- C/C++
- Wokwi Simulator
- Blynk IoT
- DHT22
- I2C LCD
- Wi-Fi

## Project Status

The EV Charging and Vehicle Monitoring System has been implemented and tested successfully using Wokwi simulation and Blynk IoT dashboard integration.

## Simulation

**Wokwi Simulation:**  
https://wokwi.com/projects/481502054164321281

## Dashboard

A screenshot of the Blynk IoT dashboard is included in the project repository.

## Note

This project is a virtual prototype developed using simulated sensors and Wokwi. Voltage, current, temperature, and vehicle conditions are simulated values. Physical EV charger and vehicle hardware integration has not been implemented.
