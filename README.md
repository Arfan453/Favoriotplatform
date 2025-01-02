IoT Environment Monitoring System 
==========================

## Introduction

Hibiscus Sense is a versatile Internet of Things (IoT) development board, powered by the mighty and popular dual-core ESP32 microcontroller. It comes equipped with an array of sensors and basic actuators as listed below:
- 3 sensors:
  - **APDS9960**: an environment sensor, which sense *proximity, RGB* and *gesture*.
  - **BME280**: an environment sensor, which sense the *altitude, barometric pressure, humidity* and *temperature*.
  - **MPU6050**: 6-axis motion tracking sensor, which sense *3-axis gravitational acceleration, 3-axis rotational velocity* and *temperature*.
- 3 actuators:
  - **Buzzer**: short distance small buzzer.
  - **LED**: blue LED.
  - **RGB LED**: WS2812 RGB LED.

Hibiscus Sense comes with **USB Type-C** to power up the board and to program the ESP32. The onboard USB-to-Serial converter (**Silicon Labs CP2104**) with **automatic bootloader reset** circuit, eliminates the need to press the RESET button each time you upload a program.


## Hibiscus Sense Features

<p align="center"><img src="https://github.com/myduino/Hibiscus-Sense-Arduino/raw/main/references/hibiscus-sense-features.png" width="900"></a></p>


This project demonstrates an IoT Environment Monitoring System using the Hibiscus Sense ESP32 microcontroller. The system collects environmental data and sends it to Favoriot's IoT platform via MQTT. Additionally, the RGB LED, blue LED, and buzzer can be controlled through incoming MQTT messages.


Software Setup
--------------

1.  **Install Arduino Libraries**:
    
    *   Open the Arduino IDE.
        
    *   Go to **Sketch** > **Include Library** > **Manage Libraries**.
        
    *   Install the following libraries:
        
        *   WiFi
            
        *   MQTT
            
        *   NetworkClientSecure
            
        *   Adafruit APDS9960
            
        *   Adafruit BME280
            
        *   Adafruit MPU6050
            
        *   Adafruit NeoPixel
            
        *   Tones (if not available, include the tones.h file in your project)
            
        *   FavoriotCA (custom CA certificate)
            
        
2.  **Open the Project**:
    
    *   Open the MQTTS_Slider.ino file in the Arduino IDE.

      ```bash
      #include <WiFi.h>
      #include <MQTT.h>
      #include <NetworkClientSecure.h>
      #include <Adafruit_APDS9960.h>
      #include <Adafruit_BME280.h>
      #include <Adafruit_MPU6050.h>
      #include <Adafruit_NeoPixel.h>
      #include "tones.h"
      #include "FavoriotCA.h"
      
      // Replace these with your Wi-Fi credentials and device information
      const char ssid[] = "your_wifi_ssid";
      const char password[] = "your_wifi_password";
      const char deviceDeveloperId[] = "your_device_Developer_Id";
      const char deviceAccessToken[] = "your_Access_Token";
      const char publishTopic[] = "/v2/streams";
      const char statusTopic[] = "/v2/streams/status";
      const char rpcTopic[] = "/v2/rpc";  // Topic to listen for RPC commands
      
      // Define pin for device
      const int buzzerPin = 13; // Use GPIO 13 for the buzzer
      const int blueLedPin = 2;  // GPIO2 for the blue LED
      
        int redValue =  0;
        int greenValue = 0;
        int blueValue = 0;
      
      
      
      // Instances for sensors and LED control
      Adafruit_NeoPixel rgb(1, 16); // 1 LED on GPIO16
      Adafruit_APDS9960 apds;
      Adafruit_BME280 bme;
      Adafruit_MPU6050 mpu;
      
      // Sensor data
      sensors_event_t a, g, temp;
      
      // Sea level pressure for altitude calculation
      float hPaSeaLevel = 1015.00;
      unsigned long lastMillis = 0;
      
      // MQTT and Network Clients
      NetworkClientSecure client;
      MQTTClient mqtt(4096);
      
      
      
      // Connect to Wi-Fi
      void connectToWiFi() {
        Serial.print("Connecting to Wi-Fi '" + String(ssid) + "' ...");
        WiFi.begin(ssid, password);
      
        while (WiFi.status() != WL_CONNECTED) {
          Serial.print(".");
          delay(500);
        }
      
        Serial.println(" connected!");
      }
      
      // Extract the value from the MQTT payload
      int extractValue(String payload, String key) {
        int startIndex = payload.indexOf(key) + key.length();
        int endIndex = payload.indexOf(",", startIndex);
        if (endIndex == -1) {
          endIndex = payload.indexOf("}", startIndex);
        }
        
        // Extract the value, trimming spaces and quotes
        String valueStr = payload.substring(startIndex, endIndex);
        valueStr.trim(); // Remove any extra spaces
        valueStr.replace("\"", ""); // Remove any quotes
        
        return valueStr.toInt(); // Convert to integer
      }
      
      
      
      // Handle incoming MQTT messages
      void messageReceived(String &topic, String &payload) {
        Serial.println("Incoming RPC Command: " + payload);
        
        // Extract the RGB values or default to 0 if not provided
         if (payload.indexOf("\"r\":") != -1) {
        redValue = extractValue(payload, "\"r\":");
        }
        if (payload.indexOf("\"g\":") != -1) { 
          greenValue = extractValue(payload, "\"g\":"); // corrected the key
        }
        if (payload.indexOf("\"b\":") != -1) { 
          blueValue = extractValue(payload, "\"b\":"); // corrected the key
        }
      
        Serial.println("Extracted Values:");
        Serial.println("Red: " + String(redValue));
        Serial.println("Green: " + String(greenValue));
        Serial.println("Blue: " + String(blueValue));
      
        // Handle RGB LED Control
        rgb.setPixelColor(0, rgb.Color(redValue, greenValue, blueValue)); // Set full RGB color
        rgb.show();  // Display the color
        Serial.println("RGB LED set to R:" + String(redValue) + ", G:" + String(greenValue) + ", B:" + String(blueValue));
        
       // Handle blue LED GPIO2 control
        if (payload.indexOf("\"LED\":\"on\"") != -1) {
          digitalWrite(blueLedPin, LOW);  // Turn on the blue LED
          Serial.println("Blue LED on GPIO2 turned ON");
        } else if (payload.indexOf("\"LED\":\"off\"") != -1) {
          digitalWrite(blueLedPin, HIGH);  // Turn off the blue LED
          Serial.println("Blue LED on GPIO2 turned OFF");
        }
      
        // Handle Buzzer On/Off
        if (payload.indexOf("\"BUZZER\":\"on\"") != -1) {
          digitalWrite(buzzerPin, HIGH);
          playMelody();  // Activate buzzer
          Serial.println("Buzzer turned ON");
        } else if (payload.indexOf("\"BUZZER\":\"off\"") != -1) {
          digitalWrite(buzzerPin, LOW);
          noTone(buzzerPin);  // Deactivate buzzer
          Serial.println("Buzzer turned OFF");
        }
      
        Serial.println();
      }
      
      // Play melody on the buzzer
      void playMelody() {
      
        int melody[] = {NOTE_G5, NOTE_E5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4, NOTE_C5};
        int noteDurations[] = {8, 8, 8, 8, 4, 4, 8, 8, 4, 4};
        
        for (int i = 0; i < 10; i++) {
          int noteDuration = 1000 / noteDurations[i];
          tone(buzzerPin, melody[i], noteDuration);
          delay(noteDuration * 1.3);
          noTone(buzzerPin);
      
        /*int melody[] = {NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4, NOTE_CS5, NOTE_B4, NOTE_D5, NOTE_E5};
        int noteDurations[] = {8, 8, 4, 4, 8, 8, 4, 4};
        
        for (int i = 0; i < 8; i++) {
          int noteDuration = 2000 / noteDurations[i];
          tone(buzzerPin, melody[i], noteDuration);
          delay(noteDuration * 1.30);
          noTone(buzzerPin); */
        }
      }
      
      // Connect to Favoriot MQTT
      void connectToFavoriotMQTT() {
        Serial.print("Connecting to Favoriot MQTT ...");
        client.setCACert(rootCACertificate);
        mqtt.begin("mqtt.favoriot.com", 8883, client);
        mqtt.onMessage(messageReceived);
      
        String uniqueString = String(ssid) + "-" + String(random(1, 98)) + String(random(99, 999));
        char uniqueClientID[uniqueString.length() + 1];
        uniqueString.toCharArray(uniqueClientID, uniqueString.length() + 1);
      
        while (!mqtt.connect(uniqueClientID, deviceAccessToken, deviceAccessToken)) {
          Serial.print(".");
          delay(500);
        }
      
        Serial.println(" connected!");
        Serial.println("Subscribed to: " + String(deviceAccessToken) + String(rpcTopic));
        mqtt.subscribe(String(deviceAccessToken) + String(rpcTopic));
        Serial.println();
      }
      
      void setup() {
        Serial.begin(115200);
      
        // Initialize the NeoPixel LED
        rgb.begin();
        rgb.show();  // Make sure the LED starts off
      
        // Initialize the buzzer pin
        pinMode(buzzerPin, OUTPUT);
        digitalWrite(buzzerPin, LOW);  // Make sure the buzzer starts off
      
        // Initialize the blue LED pin
          pinMode(blueLedPin, OUTPUT);
          digitalWrite(blueLedPin, HIGH);  // Make sure the blue LED starts off
      
        // Initialize sensors
        if (!apds.begin()) {
          Serial.println("Failed to find Hibiscus Sense APDS9960 chip");
        }
        apds.enableProximity(true);
      
        if (!bme.begin()) {
          Serial.println("Failed to find Hibiscus Sense BME280 chip");
        }
      
        if (!mpu.begin()) {
          Serial.println("Failed to find Hibiscus Sense MPU6050 chip");
        }
      
        // Connect to Wi-Fi
        connectToWiFi();
        
        // Connect to Favoriot MQTT
        connectToFavoriotMQTT();
      }
      
      void loop() {
        // Check Wi-Fi connection
        if (WiFi.status() != WL_CONNECTED) {
          connectToWiFi();
        }
      
        // Check MQTT connection
        if (!mqtt.connected()) {
          connectToFavoriotMQTT();
        }
      
        // Keep MQTT client alive
        mqtt.loop();
        delay(10);  // <- fixes some issues with WiFi stability
      
        // Read data from sensors
        int proximity = apds.readProximity();
        float humidity = bme.readHumidity();
        float temperature = bme.readTemperature();
        float barometer = bme.readPressure() / 100.00;
        float altitude = bme.readAltitude(hPaSeaLevel);
        mpu.getEvent(&a, &g, &temp);
        float accx = a.acceleration.x;
        float accy = a.acceleration.y;
        float accz = a.acceleration.z;
        float gyrx = g.gyro.x;
        float gyry = g.gyro.y;
        float gyrz = g.gyro.z;
      
        Serial.print("Proximity: ");
        Serial.println(proximity);
      
        Serial.print("Relative Humidity: ");
        Serial.print(humidity);
        Serial.println(" %RH");
      
        Serial.print("Approx. Altitude: ");
        Serial.print(altitude);
        Serial.println(" m");
      
        Serial.print("Barometric Pressure: ");
        Serial.print(barometer);
        Serial.println(" Pa");
      
        Serial.print("Ambient Temperature: ");
        Serial.print(temperature);
        Serial.println(" °C");
      
        Serial.print("Acceleration X:");
        Serial.print(accx);
        Serial.print(", Y:");
        Serial.print(accy);
        Serial.print(", Z:");
        Serial.print(accz);
        Serial.println(" m/s^2");
      
        Serial.print("Rotation X:");
        Serial.print(gyrx);
        Serial.print(", Y:");
        Serial.print(gyry);
        Serial.print(", Z:");
        Serial.print(gyrz);
        Serial.println(" rad/s");
      
        // Send data to Favoriot's data stream using secure MQTT connection
        // Interval 15 seconds
        if (millis() - lastMillis > 3000) {
          lastMillis = millis();
      
          String favoriotJson = "{\"device_developer_id\":\"" + String(deviceDeveloperId) + "\",\"data\":{";
          favoriotJson += "\"proximity\":\"" + String(proximity) + "\","; 
          favoriotJson += "\"humidity\":\"" + String(humidity) + "\","; 
          favoriotJson += "\"altitude\":\"" + String(altitude) + "\","; 
          favoriotJson += "\"barometer\":\"" + String(barometer) + "\","; 
          favoriotJson += "\"temperature\":\"" + String(temperature) + "\","; 
          favoriotJson += "\"accx\":\"" + String(accx) + "\","; 
          favoriotJson += "\"accy\":\"" + String(accy) + "\","; 
          favoriotJson += "\"accz\":\"" + String(accz) + "\","; 
          favoriotJson += "\"gyrox\":\"" + String(gyrx) + "\","; 
          favoriotJson += "\"gyroy\":\"" + String(gyry) + "\","; 
          favoriotJson += "\"gyroz\":\"" + String(gyrz) + "\""; 
          favoriotJson += "}}";
      
          Serial.println("\nSending data to Favoriot's Data Stream ...");
          Serial.println("Data to Publish: " + favoriotJson);
          Serial.println("Publish to: " + String(deviceAccessToken) + String(publishTopic));
          mqtt.publish(String(deviceAccessToken) + String(publishTopic), favoriotJson);
        }
      
        Serial.println("=============================================");
        delay(3000);
      }
      ```
        
3.  **Update Wi-Fi and Device Credentials**:
    
    *   Replace the placeholder values for ssid, password, deviceDeveloperId, and deviceAccessToken in the code with your actual credentials.
        
4.  **Upload the Code**:
    
    *   Connect your ESP32 to the computer.
        
    *   Select the correct board (**ESP32 Dev Module**) and port in the Arduino IDE.
        
    *   Click the **Upload** button to upload the code to the ESP32.
        

Running the Project
-------------------

1.  **Power the ESP32**:
    
    *   Ensure the ESP32 is powered and connected to the Wi-Fi network.
        
2.  **Monitor Serial Output**:
    
    *   Open the Serial Monitor in the Arduino IDE (set the baud rate to 115200).
        
    *   Observe the initialization messages and data readings from the sensors.
  
<p align="center"><img src="https://github.com/user-attachments/assets/4170bac9-22fa-41fc-8ecc-371875db1a12" width="1000"></a></p>
        

        
3.  **Data Publishing**:
    
    *   Sensor data will be published to Favoriot's data stream at regular intervals (every 3 seconds).
      
<p align="center"><img src="https://github.com/user-attachments/assets/8b20b3d4-3ff7-4d61-a491-26020b958d56" width="1000"></a></p>


4.  **MQTT Control**:
    
    *   In your Favoriot Dashboard, add a widget control switch or control slider to send MQTT messages to control the RGB LED, blue LED, and buzzer.
        
      
<div align="center"><video src="https://github.com/user-attachments/assets/895d5b52-09a5-492c-874e-1529963cd144" width="250"></a></div>



5. **How To Setup Favoriot Dashboard**        

    *   **Link:** `https://platform.favoriot.com/tutorial/v2/#dashboard`



Acknowledgments
---------------

*   Thanks to Favoriot for the IoT platform support.
    
*   Special thanks to the contributors and the community for their support and feedback.



  

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

