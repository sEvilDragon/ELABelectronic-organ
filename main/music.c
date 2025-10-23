#include "TASK_BUZZER.h"
#include "TASK_LED.h"

extern int check;
static const char *TAG = "log_music";
// 播放神秘小曲的代码
void musictask(int message, int continue_time, int stop_time)
{
    uint16_t dat = (~message);
    message = (message & 0b1111111100001111);

    if (xQueueSend(xQueue, &message, pdMS_TO_TICKS(100)) != pdPASS)
    {
        ESP_LOGW(TAG, "Queue full, message not sent");
    }

    buzzer_set_tone(0);
    double j = 1;
    if (((~dat) & 16) == 16)
    {
        j = 2;
        led_mutex_write(check, 90);
    }
    else if (((~dat) & 64) == 64)
    {
        j = 0.5;
        led_mutex_write(check, 70);
    }

    if (((~dat) & 2048) == 2048) // 如果是第12个按键被按下
    {
        buzzer_set_tone((int)(493 * j));
    }
    if (((~dat) & 1024) == 1024)
    {
        buzzer_set_tone((int)(466 * j));
    }
    if (((~dat) & 512) == 512)
    {
        buzzer_set_tone((int)(440 * j));
    }
    if (((~dat) & 256) == 256)
    {
        buzzer_set_tone((int)(415 * j));
    }
    if (((~dat) & 4096) == 4096)
    {
        buzzer_set_tone((int)(392 * j));
    }
    if (((~dat) & 8192) == 8192)
    {
        buzzer_set_tone((int)(370 * j));
    }
    if (((~dat) & 16384) == 16384)
    {
        buzzer_set_tone((int)(349 * j));
    }
    if (((~dat) & 32768) == 32768)
    {
        buzzer_set_tone((int)(329 * j));
    }
    if (((~dat) & 1) == 1)
    {
        buzzer_set_tone((int)(311 * j));
    }
    if (((~dat) & 2) == 2)
    {
        buzzer_set_tone((int)(293 * j));
    }
    if (((~dat) & 4) == 4)
    {
        buzzer_set_tone((int)(277 * j));
    }
    if (((~dat) & 8) == 8)
    {
        buzzer_set_tone((int)(261 * j));
    }

    vTaskDelay(pdMS_TO_TICKS(continue_time));
    buzzer_set_tone(0);
    vTaskDelay(pdMS_TO_TICKS(stop_time));
}
// 神秘小曲的代码
void music_start(void)
{
    int old_check = check;
    check = 4;
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
    check = old_check;
}