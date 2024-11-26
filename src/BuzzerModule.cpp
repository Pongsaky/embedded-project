#include "BuzzerModule.h"

// Melody for entry sound (car detected)
int entryMelody[] = {
    NOTE_F4, NOTE_F5};
int entryNoteDurations[] = {
    5, 5};

// Melody for stable sound (distance stable)
int stableMelody[] = {
    NOTE_E4, NOTE_E4, NOTE_E4,                   // Jingle Bells, Jingle Bells
    NOTE_E4, NOTE_E4, NOTE_E4,                   // Jingle all the way
    NOTE_E4, NOTE_G4, NOTE_C4, NOTE_D4, NOTE_E4, // Oh, what fun it is to ride
    NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4,          // In a one-horse open sleigh
    NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4,
    NOTE_E4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_G4};
int stableNoteDurations[] = {
    8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 4, // First part
    8, 8, 8, 8,                      // Second part
    4, 4, 8, 8, 8, 8, 4, 8, 8, 8, 4  // Third part
};

// Buzzer pin variable
int buzzerPin;

void initBuzzer(int pin)
{
  buzzerPin = pin;
  pinMode(buzzerPin, OUTPUT);
}

// Play the entry sound
void playEntrySound()
{
  int noteCount = sizeof(entryMelody) / sizeof(entryMelody[0]);
  for (int i = 0; i < noteCount; i++)
  {
    int noteDuration = 1000 / entryNoteDurations[i];
    tone(buzzerPin, entryMelody[i], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }
}

// Play the stable sound
void playStableSound()
{
  int noteCount = sizeof(stableMelody) / sizeof(stableMelody[0]);
  for (int i = 0; i < noteCount; i++)
  {
    int noteDuration = 1000 / stableNoteDurations[i];
    tone(buzzerPin, stableMelody[i], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }
}

// Stop any sound
void stopBuzzer()
{
  noTone(buzzerPin);
}