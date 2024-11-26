#ifndef BUZZER_MODULE_H
#define BUZZER_MODULE_H

// Include Arduino and pitch definitions
#include <Arduino.h>
#include "pitches.h"

// Pin definition
extern int buzzerPin;

// Initialize Buzzer
void initBuzzer(int pin);

// Functions to play different sounds
void playEntrySound();
void playStableSound();
void stopBuzzer();

#endif