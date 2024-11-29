#include "LEDModule.h"

// Pin definitions for the LEDs
int ledPins[3];

void initLEDs(int pin1, int pin2, int pin3) {
  ledPins[0] = pin1;
  ledPins[1] = pin2;
  ledPins[2] = pin3;
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Ensure the LEDs are off initially
  }
}

void turnOnLED(int index) {
  if (index >= 0 && index < 3) {
    digitalWrite(ledPins[index], HIGH);
  }
}

void turnOffLED(int index) {
  if (index >= 0 && index < 3) {
    digitalWrite(ledPins[index], LOW);
  }
}

void turnOffAllLEDs() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}