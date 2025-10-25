#ifndef TASK_TTP_H
#define TASK_TTP_H

#include <stdint.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "electronic_organ_main.h"

#define TTP229_PIN_SCL GPIO_NUM_27
#define TTP229_PIN_SDO GPIO_NUM_26
#define NUM_TOUCH 16

static SemaphoreHandle_t ttp_mutex = NULL;

void music_start(void);
void ttp_start(void);
void ttp_mutex_start(void);
void ttp_mutex_write(uint16_t new_dat);
uint16_t ttp_mutex_get(void);
void SDA_OUT(void);
void SDA_IN(void);
unsigned short vReadttp229Task(void);
void vReadTask(void *pvParameters);

void buzzer_set_tone(int freq_hz);
void musictask(int message, int continue_time, int stop_time);
void music_start(void);

#endif