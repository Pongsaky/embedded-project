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

// Generic function to get distance from a sensor
int getUltrasonicValue(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH);
    int distance = duration * 0.034 / 2;
    return distance;
}

// Function to get distance from Sensor 1
int getUltrasonic1Value() {
    return getUltrasonicValue(TRIG_PIN_1, ECHO_PIN_1);
}

// Function to get distance from Sensor 2
int getUltrasonic2Value() {
    return getUltrasonicValue(TRIG_PIN_2, ECHO_PIN_2);
}

// Function to get distance from Sensor 3
int getUltrasonic3Value() {
    return getUltrasonicValue(TRIG_PIN_3, ECHO_PIN_3);
}
