#include <WiFi.h>
#include <MQTT.h>
#include <WiFiClientSecure.h>
#include <Adafruit_APDS9960.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MPU6050.h>
#include "FavoriotCA.h"

// Initialize sensors
Adafruit_APDS9960 apds;
Adafruit_BME280 bme;
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

// Wi-Fi and MQTT details
const char ssid[] = "favoriot@unifi";
const char password[] = "fav0r10t2017";
const char deviceDeveloperId[] = "Hibiscus_gateway@mohdarfan453";
const char deviceAccessToken[] = "QvnTivs4SDrxGBUYZw19L7pEdJvudHcV";
const char publishTopic[] = "QvnTivs4SDrxGBUYZw19L7pEdJvudHcV/v2/gateway/streams";
const char statusTopic[] = "QvnTivs4SDrxGBUYZw19L7pEdJvudHcV/v2/gateway/streams/status";

WiFiClientSecure wifiClient;
MQTTClient mqttClient(4096);  // Increase buffer size for larger payload

unsigned long lastProximityMillis = 0;
unsigned long lastEnvironmentMillis = 0;
unsigned long lastMotionMillis = 0;

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected!");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming message on topic " + topic + ": " + payload);
}

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("ESP32Client", deviceAccessToken, deviceAccessToken)) {
      Serial.println(" connected");
      mqttClient.subscribe(String(statusTopic));
      Serial.println("Subscribed to status topic: " + String(statusTopic));
    } else {
      Serial.print(" failed, status code: ");
      Serial.println(mqttClient.lastError());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize sensors
  if (!apds.begin()) {
    Serial.println("Failed to find APDS9960 chip");
  }
  apds.enableProximity(true);

  if (!bme.begin()) {
    Serial.println("Failed to find BME280 chip");
  }

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
  }

  // Connect to Wi-Fi and MQTT
  connectToWiFi();
  wifiClient.setCACert(rootCACertificate);
  mqttClient.begin("mqtt.favoriot.com", 8883, wifiClient);
  mqttClient.onMessage(messageReceived);
  connectToMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  if (!mqttClient.connected()) {
    connectToMQTT();
  }

  mqttClient.loop();

  unsigned long currentMillis = millis();

  // Proximity sensor data every 5 seconds
  if (currentMillis - lastProximityMillis >= 5000) {
    lastProximityMillis = currentMillis;

    int proximity = apds.readProximity();
    String payload = "{\"uid\":\"" + String("apds1234") + "\",\"data\":{\"proximity\":\"" + String(proximity) + "\"}}";

    Serial.println("\nSending Proximity data...");
    Serial.println("Data to Publish: " + payload);

    if (mqttClient.publish(String(publishTopic), payload)) {
      Serial.println("Proximity data sent successfully");
    } else {
      Serial.println("Failed to send Proximity data");
    }
  }

  // Environmental sensor data every 5 seconds
  if (currentMillis - lastEnvironmentMillis >= 5000) {
    lastEnvironmentMillis = currentMillis;

    float humidity = bme.readHumidity();
    float temperature = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0;
    float altitude = bme.readAltitude(1013.25);

    String payload = "{\"uid\":\"" + String("bme1234") + "\",\"data\":{";
    payload += "\"humidity\":\"" + String(humidity) + "\",";
    payload += "\"temperature\":\"" + String(temperature) + "\",";
    payload += "\"pressure\":\"" + String(pressure) + "\",";
    payload += "\"altitude\":\"" + String(altitude) + "\"}}";

    Serial.println("\nSending Environmental data...");
    Serial.println("Data to Publish: " + payload);

    if (mqttClient.publish(String(publishTopic), payload)) {
      Serial.println("Environmental data sent successfully");
    } else {
      Serial.println("Failed to send Environmental data");
    }
  }

  // Motion sensor data every 5 seconds
  if (currentMillis - lastMotionMillis >= 5000) {
    lastMotionMillis = currentMillis;

    mpu.getEvent(&a, &g, &temp);

    String payload = "{\"uid\":\"" + String("mpu1234") + "\",\"data\":{";
    payload += "\"acceleration_x\":\"" + String(a.acceleration.x) + "\",";
    payload += "\"acceleration_y\":\"" + String(a.acceleration.y) + "\",";
    payload += "\"acceleration_z\":\"" + String(a.acceleration.z) + "\",";
    payload += "\"gyro_x\":\"" + String(g.gyro.x) + "\",";
    payload += "\"gyro_y\":\"" + String(g.gyro.y) + "\",";
    payload += "\"gyro_z\":\"" + String(g.gyro.z) + "\"}}";

    Serial.println("\nSending Motion data...");
    Serial.println("Data to Publish: " + payload);

    if (mqttClient.publish(String(publishTopic), payload)) {
      Serial.println("Motion data sent successfully");
    } else {
      Serial.println("Failed to send Motion data");
    }
  }

  delay(100);  // Small delay for loop stability
}
