#ifndef ELECTRONIC_ORGAN_MAIN_H
#define ELECTRONIC_ORGAN_MAIN_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

extern QueueHandle_t xQueue;
extern SemaphoreHandle_t xMutex;
extern EventGroupHandle_t xEventGroup;
extern TaskHandle_t xLedTaskHandle;

#endif