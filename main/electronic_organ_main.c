/*
 * @file electronic_organ_main.c
 * @brief 拯救已经变为植物琴的电子琴酱
 * @version 内测中，仅允许小恐龙大魔王大人使用
 * @date 2025-10-23
 *
 * @details 这个文件是用来控制电子琴的
 *          读取触摸芯片的值，控制LED灯的闪烁和蜂鸣器发声
 */

#include "TASK_LED.h"
#include "TASK_TTP.h"
#include "TASK_BUZZER.h"
#include "TASK_OLED.h"

QueueHandle_t xQueue;
SemaphoreHandle_t xMutex;
EventGroupHandle_t xEventGroup;
TaskHandle_t xLedTaskHandle;

int check_led = 0;
int check_oled = 0;
int set_voice = 1;
float set_led = 1;
static const char *TAG = "log_main";

void vQueueProcessTask(void *pvParameters)
{
    int received_bit; // 接收按键位掩码

    while (1)
    {
        // 阻塞等待直到接收到新的按键事件
        if (xQueueReceive(xQueue, &received_bit, portMAX_DELAY))
        {
            int center_led = -1;
            char letter = 0;
            switch (received_bit)
            {
            case 2048:
                center_led = 0;
                letter = '7';
                break;
            case 1024:
                center_led = 1;
                letter = '+';
                break;
            case 512:
                center_led = 2;
                letter = '6';
                break;
            case 256:
                center_led = 3;
                letter = '-';
                break;
            case 4096:
                center_led = 4;
                letter = '5';
                break;
            case 8192:
                center_led = 5;
                letter = '*';
                break;
            case 16384:
                center_led = 6;
                letter = '4';
                break;
            case 32768:
                center_led = 7;
                letter = '3';
                break;
            case 1:
                center_led = 8;
                letter = '/';
                break;
            case 2:
                center_led = 9;
                letter = '2';
                break;
            case 4:
                center_led = 10;
                letter = '&';
                break;
            case 8:
                center_led = 11;
                letter = '1';
                break;
            default:
                ESP_LOGW(TAG, "Received bit %d does not map to a standard LED.", received_bit);
                continue;
            }

            // 2. 创建新的动画实例
            animation_instance_t new_animation = {
                .button_bit = received_bit,
                .start_time = xTaskGetTickCount(),
                .is_active = true,
                .center_led = center_led};

            letter_mutex_write(0, letter_mutex_get(1));
            letter_mutex_write(1, letter_mutex_get(2));
            letter_mutex_write(2, letter);
            // 3. 将新动画实例放入管理数组中
            // 找到第一个空闲的槽位（或允许替换最老的槽位）
            for (int i = 0; i < MAX_CONCURRENT_ANIMATIONS; i++)
            {
                if (!g_animations[i].is_active) // 原来是0会变为1就会执行
                {
                    g_animations[i] = new_animation; // 启动新动画
                    break;
                }
            }
        }
        else
            continue;

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void vMonitorTask(void *pvParameters)
{
    while (1)
    {
        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            ESP_LOGI(TAG, "=== System Status ===");
            ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());
            ESP_LOGI(TAG, "Min stack free: %d words", uxHighWaterMark);
            ESP_LOGI(TAG, "Tasks running: %d", uxTaskGetNumberOfTasks());

            xSemaphoreGive(xMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_main(void)
{
    // 各种初始化
    timer();
    buzzer_set_tone(0);
    ttp_start();
    oled_spi_start();
    led_mutex_start();
    ttp_mutex_start();
    letter_mutex_start();
    buzzer_mutex_start();

    ESP_LOGE(TAG, "初始化完成，开始创建同步对象");

    // 创建同步对象
    xQueue = xQueueCreate(10, sizeof(int));
    xMutex = xSemaphoreCreateMutex();
    xEventGroup = xEventGroupCreate();

    if (xQueue == NULL || xMutex == NULL || xEventGroup == NULL)
    {
        ESP_LOGE(TAG, "Failed to create synchronization objects");
        return;
    }

    // 设置参数
    BaseType_t ret;
    bool success = true;

    // 初始化LED灯任务
    ret = xTaskCreatePinnedToCore(led_animation_task, "ledanimation_task", 2048, NULL, 6, NULL, 1);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create ledanimation_task");
        success = false;
    }

    // 初始化蜂鸣器任务
    ret = xTaskCreatePinnedToCore(vBuzzerTask, "buzzer_task", 2048, NULL, 7, NULL, 0);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create buzzer_task");
        success = false;
    }

    ret = xTaskCreatePinnedToCore(vWordTask, "oled_task", 2048, NULL, 5, NULL, 0);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create oled_task");
        success = false;
    }

    // 初始化触摸芯片读取任务
    ret = xTaskCreatePinnedToCore(vReadTask, "button_task", 2048, NULL, 8, NULL, 0);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create button task");
        success = false;
    }

    ret = xTaskCreatePinnedToCore(vQueueProcessTask, "queue_task", 2048, NULL, 3, NULL, 1);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create queue task");
        success = false;
    }

    ret = xTaskCreatePinnedToCore(vMonitorTask, "monitor_task", 2048, NULL, 2, NULL, 1);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create monitor task");
        success = false;
    }

    if (success)
    {
        ESP_LOGI(TAG, "All tasks created successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Some tasks failed to create");
        // 这里可以添加资源清理代码
    }
}