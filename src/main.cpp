// main.cpp

#include <WiFi.h>
#include "FirebaseModule.h"     // Include the Firebase module
#include "UltrasonicModule.h"   // Include the Ultrasonic module
#include "BuzzerModule.h"       // Include the Buzzer module
#include "EnvLoader.h"

// Buzzer Pin
#define BUZZER_PIN 13

// Sensor stability threshold (for stable detection)
const int STABILITY_THRESHOLD = 3;

// Sensor reading history for stability check
int lastUltrasonicValues[3] = {0, 0, 0};

unsigned long lastTime = 0;
const long interval = 500; // 15 seconds interval for sending data

std::map<std::string, std::string> env = loadEnv("../.env");

#define WIFI_SSID env["WIFI_SSID"].c_str()
#define WIFI_PASSWORD env["WIFI_PASSWORD"].c_str()
#define DATABASE_URL env["DATABASE_URL"].c_str()
#define API_KEY env["API_KEY"].c_str()

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Initialize Firebase
  initFirebase(API_KEY, DATABASE_URL);

  // Initialize Ultrasonic sensors
  initUltrasonicSensors();

  // Initialize Buzzer
  initBuzzer(BUZZER_PIN);
}

void loop() {
  if (millis() - lastTime > interval) {
    lastTime = millis();

    // Get sensor data from Ultrasonic sensors
    int ultrasonic1Value = getUltrasonic1Value();
    int ultrasonic2Value = getUltrasonic2Value();
    int ultrasonic3Value = getUltrasonic3Value();

    // Send data to Firebase
    sendSensorData("/ultrasonic/sensor1", ultrasonic1Value);
    sendSensorData("/ultrasonic/sensor2", ultrasonic2Value);
    sendSensorData("/ultrasonic/sensor3", ultrasonic3Value);

    // Check if a car has entered or the distance is stable
    if (abs(ultrasonic1Value - lastUltrasonicValues[0]) > STABILITY_THRESHOLD ||
        abs(ultrasonic2Value - lastUltrasonicValues[1]) > STABILITY_THRESHOLD ||
        abs(ultrasonic3Value - lastUltrasonicValues[2]) > STABILITY_THRESHOLD) {
      // Car entered - play the first melody
      playEntrySound();
      Serial.println("Car detected, playing melody...");
    } else {
      // Distance is stable
      Serial.println("Distance is stable.");
      playStableSound();
      // stopBuzzer(); // Stop or change the melody
    }

    // Update last values for stability check
    lastUltrasonicValues[0] = ultrasonic1Value;
    lastUltrasonicValues[1] = ultrasonic2Value;
    lastUltrasonicValues[2] = ultrasonic3Value;

  }
}