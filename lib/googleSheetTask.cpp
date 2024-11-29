// main.cpp

#include <WiFi.h>
#include "FirebaseModule.h"
#include "UltrasonicModule.h"
#include "BuzzerModule.h"
#include "LEDModule.h"
#include "EnvLoader.h"
#include "WiFiModule.h" // Include the Wi-Fi module
#include "TaskModule.h" // Include the Task module
#include <queue>
#include <LiquidCrystal_I2C.h>
#include <HTTPClient.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Queue to handle buzzer commands
QueueHandle_t buzzerQueue;
QueueHandle_t dataQueue;

struct GoogleSheetData
{
  int entryExit;
  int park_slot;
};

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

void updateLCD(int availableSpots)
{
  lcd.setCursor(0, 1);
  lcd.print("Spots: ");
  lcd.print(availableSpots);
}

// void sendDataToGoogleSheet(int entryExit, int park_slot) {
//   HTTPClient http;
//   String url = "https://script.google.com/macros/s/AKfycbxZ2HxbHJ9qxSsLTtD_fj4tG73LPtJNoVSr4w35lhG6JBzxJRsZ_F_LD8mwbNynV4jXSQ/exec?entryExit=";
//   url.concat(String(entryExit));
//   url.concat("&park_slot=");
//   url.concat(String(park_slot));
//   http.begin(url.c_str());
//   http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
//   int httpCode = http.GET();
//   if (httpCode > 0) {
//     Serial.printf("[HTTP] GET... code: %d\n", httpCode);
//     if (httpCode == HTTP_CODE_OK) {
//       String payload = http.getString();
//       // Serial.println(payload);
//       Serial.print("Data sent to Google Sheet successfully! : entryExit = ");
//       Serial.print(entryExit);
//       Serial.print(", park_slot = ");
//       Serial.println(park_slot);
//     }
//   } else {
//     Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//   }
//   http.end();
// }

void googleSheetTask(void *pvParameters)
{
  GoogleSheetData data;
  while (true)
  {
    // Wait for data from the queue
    if (xQueueReceive(dataQueue, &data, portMAX_DELAY) == pdTRUE)
    {
      HTTPClient http;
      String url = "https://script.google.com/macros/s/AKfycbxZ2HxbHJ9qxSsLTtD_fj4tG73LPtJNoVSr4w35lhG6JBzxJRsZ_F_LD8mwbNynV4jXSQ/exec?entryExit=";
      url.concat(String(data.entryExit));
      url.concat("&park_slot=");
      url.concat(String(data.park_slot));

      http.begin(url.c_str());
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      int httpCode = http.GET();
      if (httpCode > 0)
      {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK)
        {
          String payload = http.getString();
          Serial.print("Data sent to Google Sheet successfully! : entryExit = ");
          Serial.print(String(data.entryExit));
          Serial.print(", park_slot = ");
          Serial.println(String(data.park_slot));
        }
      }
      else
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }
  }
}

void setup()
{
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
  dataQueue = xQueueCreate(10, sizeof(GoogleSheetData));

  // Create a task for the buzzer
  createBuzzerTask(buzzerQueue, &buzzerTaskHandle);
  xTaskCreate(googleSheetTask, "GoogleSheetTask", 4096, NULL, 1, NULL);
}

// Array to track car entry and exit for each parking slot
bool carEntered[3] = {false, false, false};
bool carLeft[3] = {false, false, false};

// Array to track the previous state of car entry and exit for each parking slot
bool prevCarEntered[3] = {false, false, false};
bool prevCarLeft[3] = {false, false, false};

void loop()
{
  if (millis() - lastTime > interval)
  {
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

    if (ultrasonic1Value >= 5)
    {
      turnOnLED(0);
      availableSpots++;
    }
    else
    {
      turnOffLED(0);
    }

    if (ultrasonic2Value >= 5)
    {
      turnOnLED(1);
      availableSpots++;
    }
    else
    {
      turnOffLED(1);
    }

    if (ultrasonic3Value >= 5)
    {
      turnOnLED(2);
      availableSpots++;
    }
    else
    {
      turnOffLED(2);
    }

    updateLCD(availableSpots);

    // Reset car entry and exit arrays
    for (int i = 0; i < 3; i++)
    {
      carEntered[i] = false;
      carLeft[i] = false;
    }

    // Check if a car has entered or left
    if (lastUltrasonicValues[0] - ultrasonic1Value > STABILITY_THRESHOLD)
    {
      carEntered[0] = true;
    }
    else if (ultrasonic1Value - lastUltrasonicValues[0] > STABILITY_THRESHOLD)
    {
      carLeft[0] = true;
    }

    if (lastUltrasonicValues[1] - ultrasonic2Value > STABILITY_THRESHOLD)
    {
      carEntered[1] = true;
    }
    else if (ultrasonic2Value - lastUltrasonicValues[1] > STABILITY_THRESHOLD)
    {
      carLeft[1] = true;
    }

    if (lastUltrasonicValues[2] - ultrasonic3Value > STABILITY_THRESHOLD)
    {
      carEntered[2] = true;
    }
    else if (ultrasonic3Value - lastUltrasonicValues[2] > STABILITY_THRESHOLD)
    {
      carLeft[2] = true;
    }

    // Handle car entry and exit
    for (int i = 0; i < 3; i++)
    {
      if (carEntered[i] && !prevCarEntered[i])
      {
        // Car entered - send play enter sound command
        int command = COMMAND_PLAY_ENTER;
        xQueueSendToFront(buzzerQueue, &command, portMAX_DELAY); // Higher priority
        Serial.printf("Car detected in slot %d, playing Enter Car melody...\n", i + 1);
      }
      else if (carLeft[i] && !prevCarLeft[i])
      {
        // Car left - send play leave sound command
        int command = COMMAND_PLAY_LEAVE;
        xQueueSendToFront(buzzerQueue, &command, portMAX_DELAY); // Higher priority
        Serial.printf("Car left from slot %d, playing Leave Car melody...\n", i + 1);
      }
    }

    // Update last values for stability check
    lastUltrasonicValues[0] = ultrasonic1Value;
    lastUltrasonicValues[1] = ultrasonic2Value;
    lastUltrasonicValues[2] = ultrasonic3Value;

    for (int i = 0; i < 3; i++)
    {
      if (carEntered[i] && !prevCarEntered[i])
      {
        GoogleSheetData data = {1, i + 1}; // Car entered
        xQueueSendToBack(dataQueue, &data, portMAX_DELAY);
      }
      else if (carLeft[i] && !prevCarLeft[i])
      {
        GoogleSheetData data = {0, i + 1}; // Car left
        xQueueSendToBack(dataQueue, &data, portMAX_DELAY);
      }

      prevCarEntered[i] = carEntered[i];
      prevCarLeft[i] = carLeft[i];
    }

  }
}