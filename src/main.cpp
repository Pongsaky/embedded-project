// main.cpp

#include <WiFi.h>
#include "FirebaseModule.h"
#include "UltrasonicModule.h"
#include "BuzzerModule.h"
#include "LEDModule.h"
#include "EnvLoader.h"
#include "WiFiModule.h"       // Include the Wi-Fi module
#include "TaskModule.h"       // Include the Task module
#include <queue>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Queue to handle buzzer commands
QueueHandle_t buzzerQueue;

// Define buzzer commands
#define COMMAND_PLAY_ENTER 1
#define COMMAND_PLAY_STABLE 2
#define COMMAND_PLAY_LEAVE 3

// Task handle for buzzer control
TaskHandle_t buzzerTaskHandle = NULL;

// Buzzer Pin
#define BUZZER_PIN 13

// LED Pins
#define LED_PIN_1 12
#define LED_PIN_2 14
#define LED_PIN_3 27

// Sensor stability threshold (for stable detection)
const int STABILITY_THRESHOLD = 3;

// Sensor reading history for stability check
int lastUltrasonicValues[3] = {0, 0, 0};

unsigned long lastTime = 0;
const long interval = 500; // 15 seconds interval for sending data

#define WIFI_SSID "Pongsaky"
#define WIFI_PASSWORD "Pongsakon123"
#define DATABASE_URL "https://embedded-project-aa0e1-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define API_KEY "AIzaSyDX8xSf39GStlRH5UQLTGSy_6Xo0sIqsRE"

void updateLCD(int availableSpots) {
  lcd.setCursor(0, 1);
  lcd.print("Spots: ");
  lcd.print(availableSpots);
}

void setup() {
  Serial.begin(115200);

  // Initialize the LCD
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Available Spots:");

  // Connect to Wi-Fi
  connectToWiFi(WIFI_SSID, WIFI_PASSWORD);

  // Initialize Firebase
  initFirebase(API_KEY, DATABASE_URL);

  // Initialize Ultrasonic sensors
  initUltrasonicSensors();

  // Initialize Buzzer
  initBuzzer(BUZZER_PIN);

  // Initialize LEDs
  initLEDs(LED_PIN_1, LED_PIN_2, LED_PIN_3);

  // Create a queue for buzzer commands
  buzzerQueue = xQueueCreate(5, sizeof(int));

  // Create a task for the buzzer
  createBuzzerTask(buzzerQueue, &buzzerTaskHandle);
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

    int availableSpots = 0;

    if (ultrasonic1Value >= 5) {
      turnOnLED(0);
      availableSpots++;
    } else {
      turnOffLED(0);
    }

    if (ultrasonic2Value >= 5) {
      turnOnLED(1);
      availableSpots++;
    } else {
      turnOffLED(1);
    }

    if (ultrasonic3Value >= 5) {
      turnOnLED(2);
      availableSpots++;
    } else {
      turnOffLED(2);
    }

    updateLCD(availableSpots);

    // Check if a car has entered or left
    bool carEntered = false;
    bool carLeft = false;

    if (lastUltrasonicValues[0] - ultrasonic1Value > STABILITY_THRESHOLD ||
        lastUltrasonicValues[1] - ultrasonic2Value > STABILITY_THRESHOLD ||
        lastUltrasonicValues[2] - ultrasonic3Value > STABILITY_THRESHOLD) {

      carEntered = true;
    } else if ((ultrasonic1Value - lastUltrasonicValues[0]) > STABILITY_THRESHOLD ||
               (ultrasonic2Value - lastUltrasonicValues[1]) > STABILITY_THRESHOLD ||
               (ultrasonic3Value - lastUltrasonicValues[2]) > STABILITY_THRESHOLD) {

      carLeft = true;
    }

    if (carEntered) {
      // Car entered - send play enter sound command
      int command = COMMAND_PLAY_ENTER;
      xQueueSendToFront(buzzerQueue, &command, portMAX_DELAY); // Higher priority
      Serial.println("Car detected, playing Enter Car melody...");
    } else if (carLeft) {
      // Car left - send play leave sound command
      int command = COMMAND_PLAY_LEAVE;
      xQueueSendToFront(buzzerQueue, &command, portMAX_DELAY); // Higher priority
      Serial.println("Car left, playing Leave Car melody...");
    }

    // Update last values for stability check
    lastUltrasonicValues[0] = ultrasonic1Value;
    lastUltrasonicValues[1] = ultrasonic2Value;
    lastUltrasonicValues[2] = ultrasonic3Value;
  }
}