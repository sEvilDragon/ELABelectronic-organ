#include <TASK_LED.h>
#include <math.h>

extern int check_led;
extern int check_oled;
extern int set_voice;
extern float set_led;
extern float speed;
static const char *TAG = "log_led";
static int max_shine[5] = {25, 25, 25, 25, 25};
animation_instance_t g_animations[MAX_CONCURRENT_ANIMATIONS];

// 尝试创建一个互斥锁
void led_mutex_start(void)
{
    led_mutex = xSemaphoreCreateMutex();
    if (led_mutex == NULL)
        ESP_LOGE(TAG, "FATAL: Failed to create led_mutex");
}

// 创建一个函数用于写入互斥锁数据
void led_mutex_write(int tag, int value)
{
    if (tag < 0 || tag >= 5)
        return;
    if (xSemaphoreTake(led_mutex, portMAX_DELAY) == pdTRUE)
    {
        max_shine[tag] = value;
        xSemaphoreGive(led_mutex);
    }
}

// 创建一个函数用于读取互斥锁数据
int led_mutex_get(int tag)
{
    int value = 0;
    if (tag < 0 || tag >= 5)
        return value;
    if (xSemaphoreTake(led_mutex, portMAX_DELAY) == pdTRUE)
    {
        value = max_shine[tag];
        xSemaphoreGive(led_mutex);
    }
    return value;
}

// 灯rmt初始化（官方文档内容）
led_strip_handle_t configure_led(void)
{
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_PIN,                        // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_COUNT,                             // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812,                               // LED strip model
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color order of the strip: GRB
        .flags = {
            .invert_out = false, // don't invert the output signal
        }};

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                    // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ,             // RMT counter clock frequency
        .mem_block_symbols = LED_STRIP_MEMORY_BLOCK_WORDS, // the memory block size used by the RMT channel
        .flags = {
            .with_dma = LED_STRIP_USE_DMA, // Using DMA can improve performance when driving more LEDs
        }};

    // LED Strip object handle
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");
    return led_strip;
}

void led_animation_task(void *pvParameters)
{
    led_strip_handle_t led_strip = configure_led();
    // 初始化所有动画槽位
    for (int i = 0; i < MAX_CONCURRENT_ANIMATIONS; i++)
    {
        g_animations[i].is_active = false;
    }

    ESP_LOGI(TAG, "Start LED animation task");

    // 动画总时长 (0.5秒)
    const int ANIMATION_DURATION_MS = 300;
    // 动画延伸的最大范围，通常是 LED_STRIP_LED_COUNT / 2
    const float MAX_REACH = (float)LED_STRIP_LED_COUNT / 2.0f;

    while (1)
    {
        if (check_oled == 1)
        {
            switch (set_voice)
            {
            case 2:
                for (int i = 0; i < LED_STRIP_LED_COUNT; i++)
                    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, (60 * set_led), (25 * set_led), (25 * set_led)));
                break;
            case 3:
                for (int i = 0; i < 8; i++)
                    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, (60 * set_led), (25 * set_led), (25 * set_led)));
                for (int i = 8; i < 12; i++)
                    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, (25 * set_led), (uint16_t)(((20 * sin(xTaskGetTickCount() / 50.0f)) + 60) * set_led), (25 * set_led)));
                break;
            case 0:
                for (int i = 0; i < 4; i++)
                    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, (60 * set_led), (25 * set_led), (25 * set_led)));
                for (int i = 4; i < 12; i++)
                    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, (25 * set_led), (uint16_t)(((20 * sin(xTaskGetTickCount() / 50.0f)) + 60) * set_led), (25 * set_led)));
                break;
            case 1:
                for (int i = 0; i < LED_STRIP_LED_COUNT; i++)
                    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, (25 * set_led), (uint16_t)(((20 * sin(xTaskGetTickCount() / 50.0f)) + 60) * set_led), (25 * set_led)));
                break;
            default:
                break;
            }
            ESP_ERROR_CHECK(led_strip_refresh(led_strip));
            continue;
        }

        uint16_t final_r[LED_STRIP_LED_COUNT] = {0};
        uint16_t final_g[LED_STRIP_LED_COUNT] = {0};
        uint16_t final_b[LED_STRIP_LED_COUNT] = {0};

        switch (check_led)
        {
        case 1:
            for (int i = 0; i < LED_STRIP_LED_COUNT; i++)
            {
                final_r[i] = (uint16_t)((25 * sin((xTaskGetTickCount() / speed + i)) + 50) * set_led);
                final_g[i] = (uint16_t)(25 * set_led);
                final_b[i] = (uint16_t)(25 * set_led);
            }
            break;
        case 2:
            for (int i = 0; i < LED_STRIP_LED_COUNT; i++)
            {
                final_r[i] = (uint16_t)(25 * set_led);
                final_g[i] = (uint16_t)(25 * set_led);
                final_b[i] = (uint16_t)((25 * sin((xTaskGetTickCount() / speed + i)) + 35) * set_led);
            }
            break;
        case 3:
            for (int i = 0; i < LED_STRIP_LED_COUNT; i++)
            {
                final_r[i] = (uint16_t)(40 * set_led);
                final_g[i] = (uint16_t)((25 * sin((xTaskGetTickCount() / speed + (i + 1))) + 35) * set_led);
                final_b[i] = (uint16_t)((25 * sin((xTaskGetTickCount() / speed + (i + 2))) + 35) * set_led);
            }
            break;
        case 4:
            for (int i = 0; i < LED_STRIP_LED_COUNT; i++)
            {
                final_r[i] = (uint16_t)(80 * set_led);
                final_g[i] = (uint16_t)((15 * sin((xTaskGetTickCount() / speed + (i + 1))) + 35) * set_led);
                final_b[i] = (uint16_t)(40 * set_led);
            }
            break;
        default:
            for (int i = 0; i < LED_STRIP_LED_COUNT; i++)
            {
                final_r[i] = (uint16_t)(25 * set_led);
                final_g[i] = (uint16_t)((25 * sin((xTaskGetTickCount() / speed + i)) + 35) * set_led);
                final_b[i] = (uint16_t)(25 * set_led);
            }
        }
        TickType_t current_time = xTaskGetTickCount();

        // 2. 遍历所有动画实例，计算它们的颜色贡献
        for (int i = 0; i < MAX_CONCURRENT_ANIMATIONS; i++)
        {
            if (g_animations[i].is_active)
            {
                // 计算动画运行了多少毫秒
                int elapsed_ms = pdTICKS_TO_MS(current_time - g_animations[i].start_time);

                if (elapsed_ms > ANIMATION_DURATION_MS)
                {
                    g_animations[i].is_active = false; // 动画结束
                    continue;
                }

                // 时间衰减率 (从 1.0 递减到 0.0)
                float time_decay_factor = 1.0f - (float)elapsed_ms / ANIMATION_DURATION_MS;

                // 基础颜色 (可以根据按键类型或LED序号来定义)
                int BASE_R = (int)(led_mutex_get(0) * set_led);
                int BASE_G = (int)(led_mutex_get(2) * set_led);
                int BASE_B = (int)(led_mutex_get(1) * set_led);
                switch (check_led)
                {
                case 3:
                    BASE_R = (int)(led_mutex_get(3) * set_led);
                    BASE_G = (int)(led_mutex_get(3) * set_led);
                    BASE_B = (int)(led_mutex_get(3) * set_led);
                    break;
                case 4:
                    BASE_R = (int)(led_mutex_get(4) * set_led);
                    BASE_G = (int)(50 * set_led);
                    BASE_B = (int)(50 * set_led);
                    break;
                default:;
                }

                // 当前时间下的最大延伸距离 (范围从 0 到 MAX_REACH)
                float current_max_reach = MAX_REACH * ((float)elapsed_ms / ANIMATION_DURATION_MS);

                // 3. 实现延伸和渐淡逻辑
                int center_led = g_animations[i].center_led;

                for (int j = 0; j < LED_STRIP_LED_COUNT; j++)
                {
                    // 计算该像素点离中心的距离 (0, 1, 2, ...)
                    int distance = abs(j - center_led);

                    // 只有在当前最大延伸范围内才计算颜色
                    if (distance <= current_max_reach)
                    {
                        // 距离衰减率 (离中心越近越亮，从 1.0 递减到 0.0)
                        float dist_falloff = 1.0f - ((float)distance / current_max_reach);

                        // 最终强度 = (时间衰减) * (距离衰减)
                        float final_intensity = time_decay_factor * dist_falloff;

                        // 累加到最终颜色数组 (使用 uint16_t 防止溢出)
                        final_r[j] += (uint16_t)(BASE_R * final_intensity);
                        final_g[j] += (uint16_t)(BASE_G * final_intensity);
                        final_b[j] += (uint16_t)(BASE_B * final_intensity);
                    }
                }
            }
        }

        // 4. 将最终累加的颜色设置到灯条并显示
        for (int i = 0; i < LED_STRIP_LED_COUNT; i++)
        {
            uint16_t r = (final_r[i] < 25) ? (uint16_t)(25 * set_led) : final_r[i];
            uint16_t g = (final_g[i] < 25) ? (uint16_t)(25 * set_led) : final_g[i];
            uint16_t b = (final_b[i] < 25) ? (uint16_t)(25 * set_led) : final_b[i];

            ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, r, g, b));
        }

        ESP_ERROR_CHECK(led_strip_refresh(led_strip));

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}