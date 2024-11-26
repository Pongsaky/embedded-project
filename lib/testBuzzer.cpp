#include <Arduino.h>
#include "pitches.h"

const int buzzerPin = 25;

// notes in the melody:
// int melody[] = { NOTE_C6, NOTE_F3, NOTE_C5, NOTE_F4, NOTE_C5, NOTE_F4, NOTE_C5, NOTE_F3, NOTE_F4, NOTE_C5, NOTE_F4, NOTE_C5, NOTE_F4, NOTE_C5, NOTE_F4, NOTE_C5, NOTE_F4, NOTE_F5, NOTE_F4, NOTE_F5 };
// int melody[] = {NOTE_F4, NOTE_F5 };
// int melody[] = {NOTE_F5, NOTE_F4 };

int melody[] = {
    NOTE_E4, NOTE_E4, NOTE_E4,                   // Jingle Bells, Jingle Bells
    NOTE_E4, NOTE_E4, NOTE_E4,                   // Jingle all the way
    NOTE_E4, NOTE_G4, NOTE_C4, NOTE_D4, NOTE_E4, // Oh, what fun it is to ride
    NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4,          // In a one-horse open sleigh
    NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4,
    NOTE_E4, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_G4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
    8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 4, // First part
    8, 8, 8, 8,                      // Second part
    4, 4, 8, 8, 8, 8, 4, 8, 8, 8, 4  // Third part
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
// int noteDurations[] = {
//   5, 5
// };

void setup()
{
  // iterate over the notes of the melody:
  int idxNote = 0;
  while(true) {
      int noteLength = sizeof(noteDurations) / sizeof(noteDurations[0]);
      int thisNote = melody[idxNote % noteLength];
      int noteDuration = 1000 / noteDurations[idxNote % noteLength];
      // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      // int noteDuration = 1000 / noteDurations[thisNote];
      tone(buzzerPin, thisNote, noteDuration);
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(buzzerPin);
      idxNote++;
  }

}

void loop()
{
  // no need to repeat the melody.
}