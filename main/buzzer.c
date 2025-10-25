#include "TASK_BUZZER.h"

extern int check_led;
extern int check_oled;
static int duty = 2048;
static const char *TAG = "log_buzzer";

// 为占空比变量写一个互斥锁
void buzzer_mutex_start(void)
{
    buzzer_mutex = xSemaphoreCreateMutex();
    if (buzzer_mutex == NULL)
        ESP_LOGE(TAG, "FATAL: Failed to create led_mutex");
}

// 创建一个函数用于写入互斥锁数据
void buzzer_mutex_write(int new_duty)
{
    if (new_duty < 0 || new_duty >= 4095)
        return;
    if (xSemaphoreTake(buzzer_mutex, portMAX_DELAY) == pdTRUE)
    {
        duty = new_duty;
        xSemaphoreGive(buzzer_mutex);
    }
}

// 创建一个函数用于读取互斥锁数据
int buzzer_mutex_get(void)
{
    int value = 0;
    if (xSemaphoreTake(buzzer_mutex, portMAX_DELAY) == pdTRUE)
    {
        value = duty;
        xSemaphoreGive(buzzer_mutex);
    }
    return value;
}

void timer(void)
{
    ledc_timer_config_t pwm_timer = {
        .duty_resolution = LEDC_TIMER_12_BIT, // 占空比分辨率为12位二进制数，4095max
        .freq_hz = PWM_FREQ,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER};  // esp32有0~7高速通道，尽量选择0~7通道
    ledc_timer_config(&pwm_timer); // 调用配置函数

    ledc_channel_config_t pwm_config = {
        .channel = LEDC_CHANNEL,
        .duty = 0,
        .gpio_num = BUZZER_GPIO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_sel = LEDC_TIMER};
    ledc_channel_config(&pwm_config);
}

// 发声函数
void buzzer_set_tone(int freq_hz)
{
    if (freq_hz > 0)
    {
        // 设置新的频率
        ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER, freq_hz);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL, buzzer_mutex_get());
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL);
    }
    else
    {
        // 停止声音：将占空比设置为 0
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL, 0);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL);
    }
}

// 蜂鸣器任务
void vBuzzerTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(500));
    ESP_LOGI(TAG, "Start blinking BUZZER");
    int b = 0;

    while (1)
    {

        if ((check_oled > 0 && check_oled < 7) || check_oled > 7)
        {
            buzzer_set_tone(0);
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        uint16_t dat = ttp_mutex_get();
        // 待办：AI将buzzer_set_tone(0);放到了后面，但之前放后面出现过问题
        int buzzer = 0;
        int k = 0; // 计数用
        double j = 1;

        if (((~dat) & 16) == 16)
        {
            j = 2;
            led_mutex_write(check_led, led_mutex_get(check_led) + 10);
        }
        else if (((~dat) & 64) == 64)
        {
            j = 0.5;
            led_mutex_write(check_led, led_mutex_get(check_led) - 10);
        }

        led_mutex_write(1, 25);
        led_mutex_write(2, 25);
        led_mutex_write(3, 25);
        led_mutex_write(4, 25);
        led_mutex_write(check_led, 120);

        if (((~dat) & 2048) == 2048) // 如果是第12个按键被按下
        {
            buzzer += 493;
            k++;
        }
        if (((~dat) & 1024) == 1024)
        {
            buzzer += 466;
            k++;
        }
        if (((~dat) & 512) == 512)
        {
            buzzer += 440;
            k++;
        }
        if (((~dat) & 256) == 256)
        {
            buzzer += 415;
            k++;
        }
        if (((~dat) & 4096) == 4096)
        {
            buzzer += 392;
            k++;
        }
        if (((~dat) & 8192) == 8192)
        {
            buzzer += 370;
            k++;
        }
        if (((~dat) & 16384) == 16384)
        {
            buzzer += 349;
            k++;
        }
        if (((~dat) & 32768) == 32768)
        {
            buzzer += 329;
            k++;
        }
        if (((~dat) & 1) == 1)
        {
            buzzer += 311;
            k++;
        }
        if (((~dat) & 2) == 2)
        {
            buzzer += 293;
            k++;
        }
        if (((~dat) & 4) == 4)
        {
            buzzer += 277;
            k++;
        }
        if (((~dat) & 8) == 8)
        {
            buzzer += 261;
            k++;
        }
        if (k == 0)
        {
            buzzer_set_tone(0);
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        buzzer = buzzer / k;
        buzzer_set_tone(buzzer * j);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}