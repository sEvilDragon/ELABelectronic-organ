#ifndef TASK_BUZZER_H
#define TASK_BUZZER_H

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "esp_err.h"
#include "electronic_organ_main.h"

#define BUZZER_GPIO 25
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_CHANNEL LEDC_CHANNEL_7
// #define LEDC_DUTY_RES LEDC_TIMER_10_BIT           // 10位分辨率
#define PWM_FREQ 5000 // 设置PWM频率5000hz（学长的）
// #define LEDC_DUTY_HALF (1 << (LEDC_DUTY_RES - 1)) // 50% 占空比

static SemaphoreHandle_t buzzer_mutex = NULL;

void buzzer_mutex_start(void);
void buzzer_mutex_write(int new_duty);
int buzzer_mutex_get(void);

uint16_t ttp_mutex_get(void);
void timer(void);
void led_mutex_write(int tag, int value);
int led_mutex_get(int tag);

void buzzer_set_tone(int freq_hz);
void vBuzzerTask(void *pvParameters);

void musictask(int message, int continue_time, int stop_time);
void music_start(void);

#endif