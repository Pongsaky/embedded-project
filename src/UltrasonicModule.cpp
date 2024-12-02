// UltrasonicModule.cpp

#include "UltrasonicModule.h"

// Initialize Ultrasonic sensors
void initUltrasonicSensors() {
    // Set the sensor pins as output/input
    pinMode(TRIG_PIN_1, OUTPUT);
    pinMode(ECHO_PIN_1, INPUT);

    pinMode(TRIG_PIN_2, OUTPUT);
    pinMode(ECHO_PIN_2, INPUT);

    pinMode(TRIG_PIN_3, OUTPUT);
    pinMode(ECHO_PIN_3, INPUT);
}

const float sound_speed = 0.034; // cm/microseconds

// Generic function to get distance from a sensor
float getUltrasonicValue(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long long duration = pulseIn(echoPin, HIGH);
    float distance = duration * sound_speed / 2.0;
    return distance;
}

// Function to get distance from Sensor 1
float getUltrasonic1Value() {
    return getUltrasonicValue(TRIG_PIN_1, ECHO_PIN_1);
}

// Function to get distance from Sensor 2
float getUltrasonic2Value() {
    return getUltrasonicValue(TRIG_PIN_2, ECHO_PIN_2);
}

// Function to get distance from Sensor 3
float  getUltrasonic3Value() {
    return getUltrasonicValue(TRIG_PIN_3, ECHO_PIN_3);
}
