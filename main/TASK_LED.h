#ifndef TASK_LED_H
#define TASK_LED_H

#include "driver/rmt_tx.h"
#include "led_strip.h"
#include "esp_log.h"

#define LED_STRIP_USE_DMA 0

#if LED_STRIP_USE_DMA
// Numbers of the LED in the strip
#define LED_STRIP_LED_COUNT 256
#define LED_STRIP_MEMORY_BLOCK_WORDS 1024 // this determines the DMA block size
#else
// Numbers of the LED in the strip
#define LED_STRIP_LED_COUNT 12
#define LED_STRIP_MEMORY_BLOCK_WORDS 0 // let the driver choose a proper memory block size automatically
#endif                                 // LED_STRIP_USE_DMA

// GPIO assignment
#define LED_STRIP_GPIO_PIN 2

// 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000)

#define MAX_CONCURRENT_ANIMATIONS 6

#define LED_TOGGLE_BIT (1 << 0)

typedef struct
{
    int button_bit;        // 触发动画的按键位 (例如 2048, 1024, 16, 32 等)
    TickType_t start_time; // 动画开始的时间戳
    bool is_active;        // 动画是否仍在进行
    int center_led;        // 动画的中心 LED 序号 (0-11)
} animation_instance_t;

extern animation_instance_t g_animations[MAX_CONCURRENT_ANIMATIONS];
static SemaphoreHandle_t led_mutex = NULL;

// LED互斥锁
void led_mutex_start(void);
void led_mutex_write(int tag, int value);
int led_mutex_get(int tag);
// 动画
void led_animation_task(void *pvParameters);
// rmt初始化
led_strip_handle_t configure_led(void);

#endif