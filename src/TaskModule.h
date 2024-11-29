
#ifndef TASKMODULE_H
#define TASKMODULE_H

#include <Arduino.h>
#include <queue>

void createBuzzerTask(QueueHandle_t buzzerQueue, TaskHandle_t* buzzerTaskHandle);

#endif // TASKMODULE_H