#ifndef TASKMODULE_H
#define TASKMODULE_H

#include <Arduino.h>
#include <queue>

struct GoogleSheetParams {
  int entryExit;
  int park_slot;
};


void createBuzzerTask(QueueHandle_t buzzerQueue, TaskHandle_t* buzzerTaskHandle);
void createGoogleSheetTask(QueueHandle_t googleSheetQueue, TaskHandle_t* googleSheetTaskHandle);
void createLEDTask(QueueHandle_t ledQueue, TaskHandle_t* ledTaskHandle);
void updateLCD(int availableSpots);

#endif // TASKMODULE_H