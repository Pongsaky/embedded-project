
#include "TaskModule.h"
#include "BuzzerModule.h"

#define COMMAND_PLAY_ENTER 1
#define COMMAND_PLAY_STABLE 2
#define COMMAND_PLAY_LEAVE 3

void buzzerTask(void *param) {
  QueueHandle_t buzzerQueue = (QueueHandle_t)param;
  int command;
  while (true) {
    if (xQueueReceive(buzzerQueue, &command, portMAX_DELAY)) {
      switch (command) {
        case COMMAND_PLAY_ENTER:
          playEntrySound();
          break;
        case COMMAND_PLAY_STABLE:
          playStableSound();
          break;
        case COMMAND_PLAY_LEAVE:
          playLeaveSound();
          break;
      }
    }
  }
}

void createBuzzerTask(QueueHandle_t buzzerQueue, TaskHandle_t* buzzerTaskHandle) {
  xTaskCreate(buzzerTask, "Buzzer Task", 2048, (void*)buzzerQueue, 1, buzzerTaskHandle);
}