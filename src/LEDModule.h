#ifndef LED_MODULE_H
#define LED_MODULE_H

#include <Arduino.h>

// Pin definitions for the LEDs
extern int ledPins[3];

// Initialize the LEDs
void initLEDs(int pin1, int pin2, int pin3);

// Turn on a specific LED
void turnOnLED(int index);

// Turn off a specific LED
void turnOffLED(int index);

// Turn off all LEDs
void turnOffAllLEDs();

#endif // LED_MODULE_H