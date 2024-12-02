// main.cpp

#include <Arduino.h>
#include "FirebaseModule.h"
#include "UltrasonicModule.h"
#include "BuzzerModule.h"
#include "LEDModule.h"
#include "WiFiModule.h" // Include the Wi-Fi module
#include "TaskModule.h"       // Include the Task module
#include <queue>
#include <LiquidCrystal_I2C.h>
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>

// // Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// // Queue to handle buzzer commands
QueueHandle_t buzzerQueue;

// Queue to handle Google Sheet commands
QueueHandle_t googleSheetQueue;

// Queue to handle LED updates
QueueHandle_t ledQueue;

// // Define buzzer commands
#define COMMAND_PLAY_ENTER 1
#define COMMAND_PLAY_STABLE 2
#define COMMAND_PLAY_LEAVE 3

// // Task handle for buzzer control
TaskHandle_t buzzerTaskHandle = NULL;

// Task handle for Google Sheet control
TaskHandle_t googleSheetTaskHandle = NULL;

// Task handle for LED control
TaskHandle_t ledTaskHandle = NULL;

// // Buzzer Pin
#define BUZZER_PIN 13

// // LED Pins
#define LED_PIN_1 12
#define LED_PIN_2 14
#define LED_PIN_3 27

// // Sensor stability threshold (for stable detection)
const int STABILITY_THRESHOLD = 3;

// // Sensor reading history for stability check
int lastUltrasonicValues[3] = {0, 0, 0};

#define WIFI_SSID "Pongsaky"
#define WIFI_PASSWORD "Pongsakon123"
#define DATABASE_URL "https://embedded-project-aa0e1-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define API_KEY "AIzaSyDX8xSf39GStlRH5UQLTGSy_6Xo0sIqsRE"

void updateLCD(int availableSpots) {
  lcd.setCursor(0, 1);
  lcd.print("Spots: ");
  lcd.print(availableSpots);
}

const float sound_speed = 0.034; // cm/microseconds

float getUltrasonicValue_(int trigPin, int echoPin)
{
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * sound_speed / 2;
  return distance;
}

unsigned long previousMillis = 0;
const long interval = 300; // Interval for sensor readings (1 second)

const int NUM_READINGS = 3; // Number of readings for moving average
float ultrasonic1Readings[NUM_READINGS];
float ultrasonic2Readings[NUM_READINGS];
float ultrasonic3Readings[NUM_READINGS];
int readingIndex = 0;
bool readingsFilled = false;

void setup()
{
  Serial.begin(115200);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  // // Initialize the LCD
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Available Spots:");

  // // Connect to Wi-Fi
  connectToWiFi(WIFI_SSID, WIFI_PASSWORD);

  // // Initialize Firebase
  initFirebase(API_KEY, DATABASE_URL);

  // // Initialize Ultrasonic sensors
  initUltrasonicSensors();

  // // Initialize Buzzer
  initBuzzer(BUZZER_PIN);

  // // Initialize LEDs
  initLEDs(LED_PIN_1, LED_PIN_2, LED_PIN_3);

  // Create a queue for buzzer commands
  buzzerQueue = xQueueCreate(5, sizeof(int));

  // Create a task for the buzzer
  createBuzzerTask(buzzerQueue, &buzzerTaskHandle);

  // Create a queue for Google Sheet commands
  googleSheetQueue = xQueueCreate(10, sizeof(GoogleSheetParams));

  // Create a task for the Google Sheet
  createGoogleSheetTask(googleSheetQueue, &googleSheetTaskHandle);

  // Create a queue for LED updates
  ledQueue = xQueueCreate(5, sizeof(int));

  // Create a task for the LEDs
  createLEDTask(ledQueue, &ledTaskHandle);

  for (int i = 0; i < NUM_READINGS; i++) {
    ultrasonic1Readings[i] = 0;
    ultrasonic2Readings[i] = 0;
    ultrasonic3Readings[i] = 0;
  }
}

float getSmoothedValue(float* readings) {
  float sum = 0;
  for (int i = 0; i < NUM_READINGS; i++) {
    sum += readings[i];
  }
  return sum / NUM_READINGS;
}

// Array to track car entry and exit for each parking slot
bool carEntered[3] = {false, false, false};
bool carLeft[3] = {false, false, false};

// Array to track the previous state of car entry and exit for each parking slot
bool prevCarEntered[3] = {false, false, false};
bool prevCarLeft[3] = {false, false, false};
long duration;
float distanceCm;

float ultrasonic1Value;
float ultrasonic2Value;
float ultrasonic3Value;

void updateLEDs() {
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

  xQueueSendToBack(ledQueue, &availableSpots, portMAX_DELAY);
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Get sensor data from Ultrasonic sensors
    ultrasonic1Value = getUltrasonic1Value();
    ultrasonic2Value = getUltrasonic2Value();
    ultrasonic3Value = getUltrasonic3Value();

    // Store the readings in the arrays
    ultrasonic1Readings[readingIndex] = ultrasonic1Value;
    ultrasonic2Readings[readingIndex] = ultrasonic2Value;
    ultrasonic3Readings[readingIndex] = ultrasonic3Value;

    readingIndex++;
    if (readingIndex >= NUM_READINGS) {
      readingIndex = 0;
      readingsFilled = true;
    }

    // Use smoothed values if readings array is filled
    if (readingsFilled) {
      ultrasonic1Value = getSmoothedValue(ultrasonic1Readings);
      ultrasonic2Value = getSmoothedValue(ultrasonic2Readings);
      ultrasonic3Value = getSmoothedValue(ultrasonic3Readings);
    }

    // Send data to Firebase
    sendSensorData("/ultrasonic/sensor1", ultrasonic1Value);
    sendSensorData("/ultrasonic/sensor2", ultrasonic2Value);
    sendSensorData("/ultrasonic/sensor3", ultrasonic3Value);

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

        // Send data to Google Sheet
        GoogleSheetParams params = {1, i + 1};
        xQueueSendToBack(googleSheetQueue, &params, portMAX_DELAY);
      }
      else if (carLeft[i] && !prevCarLeft[i])
      {
        // Car left - send play leave sound command
        int command = COMMAND_PLAY_LEAVE;
        xQueueSendToFront(buzzerQueue, &command, portMAX_DELAY); // Higher priority
        Serial.printf("Car left from slot %d, playing Leave Car melody...\n", i + 1);

        // Send data to Google Sheet
        GoogleSheetParams params = {0, i + 1};
        xQueueSendToBack(googleSheetQueue, &params, portMAX_DELAY);
      }
    }

    // Update last values for stability check
    lastUltrasonicValues[0] = ultrasonic1Value;
    lastUltrasonicValues[1] = ultrasonic2Value;
    lastUltrasonicValues[2] = ultrasonic3Value;

    // Update previous state arrays 
    for (int i = 0; i < 3; i++)
    {
      prevCarEntered[i] = carEntered[i];
      prevCarLeft[i] = carLeft[i];
    }

    updateLEDs();
  }
}