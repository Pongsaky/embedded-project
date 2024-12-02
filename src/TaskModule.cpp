#include "TaskModule.h"
#include "BuzzerModule.h"
#include <HTTPClient.h>

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
  xTaskCreate(buzzerTask, "Buzzer Task", 2048, (void*)buzzerQueue, 2, buzzerTaskHandle);
}

void sendDataToGoogleSheet(int entryExit, int park_slot) {
  HTTPClient http;
  String url = "https://script.google.com/macros/s/AKfycbxZ2HxbHJ9qxSsLTtD_fj4tG73LPtJNoVSr4w35lhG6JBzxJRsZ_F_LD8mwbNynV4jXSQ/exec?entryExit=";
  url.concat(String(entryExit));
  url.concat("&park_slot=");
  url.concat(String(park_slot));
  http.begin(url.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      // Serial.println(payload);
      Serial.println(String("Data sent to Google Sheet successfully! : entryExit = ") + String(entryExit) + ", park_slot = " + String(park_slot));
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void googleSheetTask(void *param) {
  QueueHandle_t googleSheetQueue = (QueueHandle_t)param;
  GoogleSheetParams params;
  while (true) {
    if (xQueueReceive(googleSheetQueue, &params, portMAX_DELAY)) {
      sendDataToGoogleSheet(params.entryExit, params.park_slot);
    }
  }
}

void createGoogleSheetTask(QueueHandle_t googleSheetQueue, TaskHandle_t* googleSheetTaskHandle) {
  xTaskCreate(googleSheetTask, "GoogleSheet Task", 8192, (void*)googleSheetQueue, 1, googleSheetTaskHandle);
}

void ledTask(void *param) {
  QueueHandle_t ledQueue = (QueueHandle_t)param;
  int availableSpots;
  while (true) {
    if (xQueueReceive(ledQueue, &availableSpots, portMAX_DELAY)) {
      updateLCD(availableSpots);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // Adjust the delay as needed
  }
}

void createLEDTask(QueueHandle_t ledQueue, TaskHandle_t* ledTaskHandle) {
  xTaskCreate(ledTask, "LED Task", 2048, (void*)ledQueue, 3, ledTaskHandle); // Highest priority
}