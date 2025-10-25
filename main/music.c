#include "TASK_BUZZER.h"
#include "TASK_LED.h"
#include "u8g2.h"
#include "u8x8.h"

extern u8g2_t u8g2;

void ttp_mutex_write(uint16_t new_dat);

extern int check_led;
extern int check_oled;
static const char *TAG = "log_music";
// 播放神秘小曲的代码
void musictask(int message, int continue_time, int stop_time)
{
    ttp_mutex_write(~message);
    message = (message & 0b1111111100001111);

    if (xQueueSend(xQueue, &message, pdMS_TO_TICKS(100)) != pdPASS)
    {
        ESP_LOGW(TAG, "Queue full, message not sent");
    }
    while (continue_time)
    {
        if (check_oled = 8)
        {
            while (1)
            {
                u8g2_ClearBuffer(&u8g2);

                u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
                u8g2_DrawUTF8(&u8g2, 1, 63, "暂停B");
                u8g2_DrawUTF8(&u8g2, 85, 63, "跳转L");
                u8g2_DrawUTF8(&u8g2, 0, 15, "------音乐------");
                OLED_scroll_text();
                vTaskDelay(pdMS_TO_TICKS(50));
                if (check_oled == 7)
                    break;
            }
        }
        continue_time -= 20;
        if (continue_time < 0)
            continue_time = 20;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    ttp_mutex_write(65535);
    vTaskDelay(pdMS_TO_TICKS(stop_time));
}
// 神秘小曲的代码
void music_start(void)
{
    vTaskDelay(pdMS_TO_TICKS(500));
    musictask(0b1000000000010000, 600, 10);
    musictask(0b0000000000010010, 300, 10);
    musictask(0b0000000000011000, 600, 10);
    musictask(0b0000000000010010, 300, 10); // 3212

    musictask(0b1000000000010000, 450, 10);
    musictask(0b0100000000010000, 150, 10);
    musictask(0b1000000000010000, 300, 10);
    musictask(0b0000000000010010, 900, 10); // 3432

    musictask(0b1000000000010000, 600, 10);
    musictask(0b0000000000010010, 300, 10);
    musictask(0b0000000000011000, 600, 10);
    musictask(0b0000000000010010, 300, 10);

    musictask(0b1000000000010000, 450, 10);
    musictask(0b0100000000010000, 150, 10);
    musictask(0b1000000000010000, 300, 10);
    musictask(0b0000000000010010, 900, 10); // 3432

    musictask(0b1000000000010000, 600, 10);
    musictask(0b0000000000010010, 300, 10);
    musictask(0b0000000000011000, 600, 10);
    musictask(0b0000000000010010, 300, 10);

    musictask(0b1000000000010000, 450, 10);
    musictask(0b0100000000010000, 150, 10);
    musictask(0b1000000000010000, 300, 10);
    musictask(0b0000000000010010, 900, 10); // 3432

    musictask(0b1000000000010000, 600, 10);
    musictask(0b0000000000010010, 300, 10);
    musictask(0b0000000000011000, 600, 10);
    musictask(0b0000000000010010, 300, 10);

    musictask(0b1000000000010000, 450, 10);
    musictask(0b0100000000010000, 150, 10);
    musictask(0b1000000000010000, 300, 10);
    musictask(0b0000000000010010, 900, 10);
    musictask(0b0000000000011000, 115, 10);
    musictask(0b0000000000010010, 115, 10); // 343212

    musictask(0b1000000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10);
    musictask(0b0100000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10); // 332432

    musictask(0b0000000000000010, 300, 10);
    musictask(0b0000000000000010, 300, 10);
    musictask(0b0000000000001000, 300, 10);
    musictask(0b0100000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10); // 211432

    musictask(0b0000000000000010, 600, 10);
    musictask(0b0000000000001000, 150, 10);
    musictask(0b0000000000000010, 150, 10);
    musictask(0b1000000000000000, 900, 10); // 2123

    musictask(0b0000000000000000, 900, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0001000000000000, 300, 10);
    musictask(0b0000000000011000, 300, 10); // 0351

    musictask(0b0000100000000000, 600, 10);
    musictask(0b0000000000011000, 300, 10);
    musictask(0b0000100000000000, 600, 10);
    musictask(0b0000000000011000, 300, 10); // 7171

    musictask(0b0000100000000000, 150, 10);
    musictask(0b0000001000000000, 150, 10);
    musictask(0b0001000000000000, 600, 10);
    musictask(0b0001000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10);
    musictask(0b0100000000000000, 300, 10); // 765524

    musictask(0b0100000000000000, 600, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b1000000000000000, 600, 10);
    musictask(0b0001000001000000, 300, 10); // 4335

    musictask(0b0100000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10);
    musictask(0b1000000000000000, 600, 10);
    musictask(0b0001000000000000, 300, 10); // 43235

    musictask(0b0000000000001000, 900, 10);
    musictask(0b0000000000000000, 600, 10);
    musictask(0b0000000000001000, 300, 10); // 101

    musictask(0b0000000000000010, 300, 10);
    musictask(0b0000000000001000, 450, 10);
    musictask(0b0000000000001000, 150, 10);
    musictask(0b0000000000001000, 300, 10);
    musictask(0b0001000000000000, 300, 10);
    musictask(0b0000000000001000, 300, 10); // 211151

    musictask(0b0100000000000000, 600, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 600, 10);
    musictask(0b0000000000001000, 300, 10); // 4321

    musictask(0b0000000000001000, 900, 10);
    musictask(0b0000000000000000, 600, 10);
    musictask(0b0000000000001000, 150, 10);
    musictask(0b0000000000000010, 150, 10); // 1012

    musictask(0b1000000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10);
    musictask(0b0100000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10); // 332432

    musictask(0b0000000000000010, 300, 10);
    musictask(0b0000000000000010, 300, 10);
    musictask(0b0000000000001000, 300, 10);
    musictask(0b0100000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10); // 211432

    musictask(0b0000000000000010, 600, 10);
    musictask(0b0000000000001000, 150, 10);
    musictask(0b0000000000000010, 150, 10);
    musictask(0b1000000000000000, 900, 10); // 2123

    musictask(0b0000000000000000, 900, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0001000000000000, 300, 10);
    musictask(0b0000000000011000, 300, 10); // 0351

    musictask(0b0000100000000000, 600, 10);
    musictask(0b0000000000011000, 300, 10);
    musictask(0b0000100000000000, 600, 10);
    musictask(0b0000000000011000, 300, 10); // 7171

    musictask(0b0000100000000000, 150, 10);
    musictask(0b0000001000000000, 150, 10);
    musictask(0b0001000000000000, 600, 10);
    musictask(0b0001000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10);
    musictask(0b0100000000000000, 300, 10); // 765524

    musictask(0b0100000000000000, 600, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b1000000000000000, 600, 10);
    musictask(0b0001000001000000, 300, 10); // 4335

    musictask(0b0100000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10);
    musictask(0b1000000000000000, 600, 10);
    musictask(0b0001000000000000, 300, 10); // 43235

    musictask(0b0000000000001000, 900, 10);
    musictask(0b0000000000000000, 600, 10);
    musictask(0b0000000000001000, 150, 10);
    musictask(0b0000000000001000, 150, 10); // 1011

    musictask(0b0000000000000010, 450, 10);
    musictask(0b0000000000001000, 600, 10);
    musictask(0b0000000000001000, 300, 10);
    musictask(0b0001000000000000, 300, 10);
    musictask(0b0000000000001000, 300, 10); // 21151

    musictask(0b0100000000000000, 300, 10);
    musictask(0b0100000000000000, 150, 10);
    musictask(0b0100000000000000, 150, 10);
    musictask(0b1000000000000000, 150, 10);
    musictask(0b0000000000000010, 150, 10);
    musictask(0b0000000000000010, 600, 10);
    musictask(0b0000000000001000, 300, 10); // 4443221

    musictask(0b0000000000001000, 450, 10);
    musictask(0b0000000000000000, 450, 10);
    musictask(0b0000000000000000, 900, 10); // 100

    musictask(0b0000001000010000, 300, 10);
    musictask(0b0001000000010000, 300, 10);
    musictask(0b0001000000010000, 300, 10);
    musictask(0b0001000000010000, 300, 10);
    musictask(0b0100000000010000, 300, 10);
    musictask(0b0100000000010000, 300, 10); // 655544

    musictask(0b1000000000010000, 300, 10);
    musictask(0b0000000000010010, 300, 10);
    musictask(0b0000000000010010, 300, 10);
    musictask(0b0000000000010010, 300, 10);
    musictask(0b0000000000000000, 300, 10);
    musictask(0b0001000000000000, 300, 10); // 322205

    musictask(0b0001000000000000, 300, 10);
    musictask(0b0100000000000000, 150, 10);
    musictask(0b0100000000000000, 150, 10);
    musictask(0b0100000000000000, 300, 10);
    musictask(0b0100000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10); // 4443221

    musictask(0b0000000000000010, 600, 10);
    musictask(0b0000000000001000, 150, 10);
    musictask(0b0000100001000000, 150, 10);
    musictask(0b0000000000001000, 450, 10);
    musictask(0b0000000000000000, 450, 10); // 21710

    musictask(0b0000001000010000, 300, 10);
    musictask(0b0001000000010000, 300, 10);
    musictask(0b0001000000010000, 300, 10);
    musictask(0b0001000000010000, 300, 10);
    musictask(0b0100000000010000, 300, 10);
    musictask(0b0100000000010000, 300, 10); // 655544

    musictask(0b1000000000010000, 300, 10);
    musictask(0b0000000000010010, 300, 10);
    musictask(0b0000000000010010, 300, 10);
    musictask(0b0000000000010010, 300, 10);
    musictask(0b0000000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10); // 322203

    musictask(0b1000000000000000, 300, 10);
    musictask(0b1000000000000000, 150, 10);
    musictask(0b1000000000000000, 150, 10);
    musictask(0b1000000000000000, 150, 10);
    musictask(0b1000000000000000, 150, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10);
    musictask(0b1000000000000000, 300, 10); // 33333323

    musictask(0b0000000000010010, 600, 10);
    musictask(0b0000000000011000, 300, 10);
    musictask(0b0000000000011000, 300, 10);
    musictask(0b0000000000000000, 300, 10);
    musictask(0b0000000000011000, 300, 10);

    musictask(0b0000100000000000, 600, 10);
    musictask(0b0000001000000000, 300, 10);
    musictask(0b0000001000000000, 900, 10);

    musictask(0b0000000000000000, 600, 10);
    musictask(0b0000001000000000, 300, 10);
    musictask(0b0000001000000000, 300, 10);
    musictask(0b0001000000000000, 300, 10);
    musictask(0b0100000000000000, 150, 10);
    musictask(0b0100000000000000, 150, 10); // 066544

    musictask(0b0100000000000000, 900, 10);
    musictask(0b1000000000000000, 150, 10);
    musictask(0b0100000000000000, 150, 10);
    musictask(0b0001000000000000, 1500, 10);
    musictask(0b0000000000000000, 900, 10); // 434550

    vTaskDelay(pdMS_TO_TICKS(500));
}