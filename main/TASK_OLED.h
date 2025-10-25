#ifndef TASK_OLED_H
#define TASK_OLED_H

#include "electronic_organ_main.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "u8g2.h"
#include "u8x8.h"

// 宏定义SPIOLED相关的内容
#define OLED_CS GPIO_NUM_19  // 片选（好像用在多个设备上，但我们就一个）
#define OLED_DC GPIO_NUM_18  // DC是数据命令线，低电平发送命令
#define OLED_RES GPIO_NUM_5  // 初始化
#define OLED_SDA GPIO_NUM_17 // 数据线
#define OLED_SCL GPIO_NUM_4  // 时针线

extern u8g2_t u8g2;
static SemaphoreHandle_t letter_mutex = NULL;
// extern spi_device_handle_t spi;

uint8_t gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
// void oled_spi_pre_transfer_callback(spi_transaction_t *t);
void oled_spi_start(void);
// void oled_send_cmd(uint8_t cmd);
// void oled_send_data(uint8_t data[], size_t length);
// void location(int page, int y);
// void OLED_clear(unsigned int page);
// void OLED_word(void);

uint8_t u8g2_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8g2_hw_spi_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void u8g2_app_init(void);

void letter_mutex_start(void);
void letter_mutex_write(int tag, int value);
int letter_mutex_get(int tag);

void OLED_menu(void);
void vWordTask(void *pvParameters);
void music_start(void);

void OLED_scroll_music(void);
void OLED_scroll_word(void);

#endif