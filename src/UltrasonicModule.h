// UltrasonicModule.h

#ifndef ULTRASONIC_MODULE_H
#define ULTRASONIC_MODULE_H

#include <Arduino.h>

// Pin definitions for the Ultrasonic sensors
#define TRIG_PIN_1 19  // Example pin for Sensor 1
#define ECHO_PIN_1 18  // Example pin for Sensor 1

#define TRIG_PIN_2 5  // Example pin for Sensor 2
#define ECHO_PIN_2 4  // Example pin for Sensor 2

#define TRIG_PIN_3 2  // Example pin for Sensor 3
#define ECHO_PIN_3 15  // Example pin for Sensor 3

// Function to initialize Ultrasonic sensors
void initUltrasonicSensors();

// Functions to get distances from each Ultrasonic sensor
float getUltrasonic1Value();
float getUltrasonic2Value();
float getUltrasonic3Value();

#endif // ULTRASONIC_MODULE_H