# Favoriot Dashboard Project

This repository demonstrates how to create and run a dashboard on the Favoriot platform to visualize real-time IoT data.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Setup Guide](#setup-guide)
  - [Step 1: Configure Sensors/Devices](#step-1-configure-sensorsdevices)
  - [Step 2: Send Data to Favoriot](#step-2-send-data-to-favoriot)
  - [Step 3: Set Up the Dashboard](#step-3-set-up-the-dashboard)
- [Usage](#usage)
- [Troubleshooting](#troubleshooting)
- [License](#license)

## Introduction

This project utilizes the Favoriot platform to display IoT data from connected devices and sensors on a customizable dashboard. The dashboard is designed for real-time data visualization and analysis.

## Features

- **Real-time Data**: Visualize temperature, humidity, and other sensor data.
- **Custom Widgets**: Create charts, gauges, and indicators tailored to your project.
- **User-Friendly Interface**: Configure and view dashboards easily on Favoriot.

## Requirements

1. **Hardware**:
   - IoT sensors or devices (e.g., BME280 sensor, photosensor, etc.).
   - Microcontroller (e.g., Hibiscus Sense ESP32).
   - Internet connectivity.

2. **Software**:
   - Favoriot account.
   - Code editor (e.g., VS Code).
   - MQTT client or REST API tools (e.g., Postman).

3. **Libraries/Dependencies**:
   - MQTT library for microcontroller (if applicable).
   - Relevant sensor libraries.

## Setup Guide

### Step 1: Configure Sensors/Devices

1. Connect your sensors (e.g., BME280 or photosensor) to the microcontroller.
2. Write and upload the code to read data from sensors and send it to Favoriot.
   - Example code snippet for sending data via MQTT:
     ```cpp
     #include <WiFi.h>
     #include <PubSubClient.h>

     const char* ssid = "YourWiFiSSID";
     const char* password = "YourWiFiPassword";
     const char* mqtt_server = "mqtt.favoriot.com";
     const char* topic = "dcms";

     WiFiClient espClient;
     PubSubClient client(espClient);

     void setup() {
         WiFi.begin(ssid, password);
         while (WiFi.status() != WL_CONNECTED) {
             delay(1000);
         }
         client.setServer(mqtt_server, 1883);
     }

     void loop() {
         if (!client.connected()) {
             while (!client.connect("ESP32Client")) {
                 delay(1000);
             }
         }
         float temperature = readTemperature();  // Replace with your sensor reading function
         String payload = "{\"temperature\": " + String(temperature) + "}";
         client.publish(topic, payload.c_str());
         delay(5000);
     }
     ```

### Step 2: Send Data to Favoriot

1. Log in to your Favoriot account.
2. Navigate to the **Devices** section and add a new device.
3. Copy the device ID and API key.
4. Use MQTT or REST API to send data to Favoriot.

   Example MQTT payload:
   ```json
   {
       "temperature": 25.3,
       "humidity": 60
   }
   ```

### Step 3: Set Up the Dashboard

1. Go to the Favoriot **Dashboard** section.
2. Add a new dashboard and customize it with widgets:
   - **Line Chart** for trends.
   - **Gauge** for real-time readings.
3. Configure the data source for each widget using your device ID.
4. Save and view the dashboard.

## Usage

- Power up your IoT device and ensure it is connected to the internet.
- Data from the sensors will be sent to Favoriot.
- Open the Favoriot dashboard to monitor real-time data.

## Troubleshooting

- **No Data on Dashboard**:
  - Check if your device is connected to Wi-Fi.
  - Ensure the MQTT or REST API credentials (e.g., device ID, API key) are correct.
- **Dashboard Widgets Not Updating**:
  - Verify the data source configuration in Favoriot.
  - Check if the data format matches Favoriot's requirements.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
