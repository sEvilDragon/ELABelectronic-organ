#include "TASK_TTP.h"

extern int check_led;
extern int check_oled;
extern int check_speed;
extern int set_voice;
extern float set_led;
extern int y;
extern int speedy;
extern QueueHandle_t xQueue;
static uint16_t dat = 0;
static const char *TAG = "log_ttp";
void OLED_scroll_word(void);

void ttp_start(void)
{
    ESP_LOGE(TAG, "开始初始化");
    // 初始化GPIO
    gpio_config_t io_conf;
    // 触摸芯片的SCL
    io_conf.intr_type = GPIO_INTR_DISABLE;           // 禁止中断
    io_conf.mode = GPIO_MODE_OUTPUT;                 // 输出模式
    io_conf.pin_bit_mask = (1ULL << TTP229_PIN_SCL); // 选择引脚
    io_conf.pull_down_en = 0;                        // 禁止下拉
    io_conf.pull_up_en = 0;                          // 禁止上拉
    gpio_config(&io_conf);                           // 应用配置

    // 触摸芯片的SDO
    io_conf.intr_type = GPIO_INTR_DISABLE;           // 禁止中断
    io_conf.mode = GPIO_MODE_INPUT;                  // 输入模式
    io_conf.pin_bit_mask = (1ULL << TTP229_PIN_SDO); // 选择引脚
    /* 防止 SDO 悬空导致默认高电平，启用下拉（若你的模块空闲为高，请改为 pull_up_en = 1） */
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf); // 应用配置
}

// 创建一个TTP的互斥锁
void ttp_mutex_start(void)
{
    ttp_mutex = xSemaphoreCreateMutex();
    if (ttp_mutex == NULL)
        ESP_LOGE(TAG, "FATAL: Failed to create ttp_mutex");
}

// 创建一个函数用于写入互斥锁数据
void ttp_mutex_write(uint16_t new_dat)
{
    if (xSemaphoreTake(ttp_mutex, portMAX_DELAY) == pdTRUE)
    {
        dat = new_dat;
        xSemaphoreGive(ttp_mutex);
    }
}

// 创建一个函数用于读取互斥锁数据
uint16_t ttp_mutex_get(void)
{
    uint16_t value = 0;
    if (xSemaphoreTake(ttp_mutex, portMAX_DELAY) == pdTRUE)
    {
        value = dat;
        xSemaphoreGive(ttp_mutex);
    }
    return value;
}

// 设置SDA为输出模式（学长代码cope）
void SDA_OUT(void)
{
    gpio_set_direction(TTP229_PIN_SDO, GPIO_MODE_OUTPUT);
}
// 设置SDA为输入模式
void SDA_IN(void)
{
    gpio_set_direction(TTP229_PIN_SDO, GPIO_MODE_INPUT);
}

unsigned short vReadttp229Task(void)
{
    unsigned short i;
    uint16_t local_dat = 0;

    /* 发起读序列：把 SDA 作为输出并产生启动脉冲，然后切回输入读取 */
    SDA_OUT();
    gpio_set_level(TTP229_PIN_SDO, 1);
    esp_rom_delay_us(100);
    gpio_set_level(TTP229_PIN_SDO, 0);
    esp_rom_delay_us(100);
    SDA_IN();

    for (i = 0; i < NUM_TOUCH; i++)
    {
        gpio_set_level(TTP229_PIN_SCL, 1);
        esp_rom_delay_us(100);
        gpio_set_level(TTP229_PIN_SCL, 0);
        esp_rom_delay_us(120);
        if (gpio_get_level(TTP229_PIN_SDO))
        {
            local_dat |= (1 << i);
        }
        esp_rom_delay_us(2);
    }
    return local_dat;
}

void thank(void)
{
    vTaskDelay(pdMS_TO_TICKS(500));
    if (check_oled == 0)
        return;
    uint16_t panduan = vReadttp229Task();
    uint16_t mew_dat = 0;
    uint16_t datt;
    while (1)
    {
        if (!panduan == 0)
        {
            mew_dat = (uint16_t)vReadttp229Task();
        }
        else
        {
            datt = (uint16_t)(~vReadttp229Task());
            mew_dat = (datt);
        }
        if (((~mew_dat) & 32) == 32)
        {
            check_oled = 0;
            return;
        }
        if (((~mew_dat) & 128) == 128)
        {
            speedy = 6;
            vTaskDelay(pdMS_TO_TICKS(50));
            speedy = 3;
            continue;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// 触摸板任务
void vReadTask(void *pvParameters)
{
    uint16_t mew_dat = 0;
    uint16_t last_dat = 0;
    uint16_t datt;
    uint16_t ledpanduan = 0;
    int voice = 0;
    int panduan = vReadttp229Task();
    last_dat = vReadttp229Task();
    ttp_mutex_write(last_dat);
    TickType_t last_time = xTaskGetTickCount();

    while (1)
    {
        if (!panduan == 0)
        {
            mew_dat = vReadttp229Task();
        }
        else
        {
            datt = (~vReadttp229Task());
            mew_dat = (datt);
        }
        ttp_mutex_write(mew_dat);
        if ((((~dat) & 128) == 128) && check_oled < 5)
        {
            if (check_oled == 0)
                check_oled = 1;
            else if (check_oled < 5)
                check_oled = 0;
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }
        if ((check_oled == 0 || (check_oled >= 5 && check_oled <= 7)) && ((~dat) & 32) == 32)
        {
            if (check_oled == 0)
                check_oled = 5;
            else if (check_oled > 4)
                check_oled = 0;
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }
        if (check_oled == 5 && ((~dat) & 128) == 128)
        {
            check_oled = 7;
            music_start();
            continue;
        }
        if (check_oled == 6 && ((~dat) & 128) == 128)
        {
            int y = 128;
            check_oled = 9;
            thank();
            continue;
        }
        if (check_oled <= 6 && check_oled >= 5 && ((~dat) & 16) == 16)
        {
            check_oled++;
            if (check_oled == 7)
                check_oled = 5;
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }
        if (check_oled <= 6 && check_oled >= 5 && ((~dat) & 64) == 64)
        {
            check_oled--;
            if (check_oled == 4)
                check_oled = 6;
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }
        if (check_oled && check_oled < 5 && ((~dat) & 16) == 16)
        {
            check_oled++;
            if (check_oled == 5)
                check_oled = 1;
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }
        if (check_oled && check_oled < 5 && ((~dat) & 64) == 64)
        {
            check_oled--;
            if (check_oled == 0)
                check_oled = 4;
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }
        if (voice)
        {
            buzzer_set_tone(400);
            vTaskDelay(pdMS_TO_TICKS(500));
            buzzer_set_tone(0);
            voice = 0;
        }
        if ((((~dat) & 32) == 32) && (check_oled == 1))
        {
            set_voice++;
            if (set_voice == 4)
                set_voice = 0;
            voice++;
            vTaskDelay(pdMS_TO_TICKS(150));
            continue;
        }
        if ((((~dat) & 32) == 32) && (check_oled == 2))
        {
            float ledmax[4] = {1, 1.25, 0, 0.5};
            set_led = ledmax[ledpanduan];
            ledpanduan++;
            if (ledpanduan == 4)
                ledpanduan = 0;
            vTaskDelay(pdMS_TO_TICKS(150));
            continue;
        }
        if ((((~dat) & 32) == 32) && (check_oled == 3))
        {
            check_led++;
            if (check_led == 5)
                check_led = 0;
            vTaskDelay(pdMS_TO_TICKS(150));
            continue;
        }
        if ((((~dat) & 32) == 32) && (check_oled == 4))
        {
            check_speed++;
            if (check_speed == 5)
                check_speed = 0;
            vTaskDelay(pdMS_TO_TICKS(150));
            continue;
        }
        // 只有当按键状态发生变化时才处理
        if (!check_oled)
        {
            unsigned short changed_bits = 0;
            unsigned short pressed_bits = 0;
            if ((xTaskGetTickCount() - last_time) < 50)
            {
                changed_bits = (~mew_dat) ^ (~last_dat);  // 找出变化的位
                pressed_bits = (~mew_dat) & changed_bits; // 找出 '新按下的' 位
            }
            else
            {
                pressed_bits = (~mew_dat);
                last_time = xTaskGetTickCount();
            }
            for (int i = 0; i < NUM_TOUCH; i++)
            {
                unsigned short current_bit = (1 << i);

                if (pressed_bits & current_bit)
                {
                    // 找到了一个新按键事件
                    int pressed_button_bit = current_bit; // 按键的位掩码 (例如 1, 2, 4, 8, ...)

                    if (xQueueSend(xQueue, &pressed_button_bit, 0) != pdPASS)
                    {
                        ESP_LOGW(TAG, "Animation Queue full, event %u not sent", current_bit);
                    }
                }
            }

            last_dat = mew_dat; // 更新上次的值
        }
        vTaskDelay(pdMS_TO_TICKS(15));
    }
}
