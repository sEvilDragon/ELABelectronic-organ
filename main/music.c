#include "TASK_BUZZER.h"
#include "TASK_LED.h"
#include "u8g2.h"
#include "u8x8.h"

extern u8g2_t u8g2;
int time = 1;
int speedm = 20;
int yinfu = 0;
int ii = 0;
extern int panduan;
long long int score = 0;

void ttp_mutex_write(uint16_t new_dat);
unsigned short vReadttp229Task(void);

extern int check_led;
extern int check_oled;
static const char *TAG = "log_music";
// 播放神秘小曲的代码
void musictask(int message, int continue_time, int stop_time)
{
    if (check_oled == 0)
        return;
    ttp_mutex_write(~message);
    message = (message & 0b1111111100001111);

    if (xQueueSend(xQueue, &message, pdMS_TO_TICKS(100)) != pdPASS)
    {
        ESP_LOGW(TAG, "Queue full, message not sent");
    }

    uint16_t mew_dat = 0;
    uint16_t datt;
    while (continue_time)
    {
        if (check_oled == 0)
            return;
        if (!panduan == 0)
        {
            mew_dat = vReadttp229Task();
        }
        else
        {
            datt = (~vReadttp229Task());
            mew_dat = (datt);
        }
        if (((~mew_dat) & 65503) == 65503)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            mew_dat = (~mew_dat);
        }
        if (((~mew_dat) & 32) == 32)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            return;
        }
        if (check_oled == 8 || ((~mew_dat) & 128) == 128)
        {
            check_oled = 8;
            while (1)
            {
                vTaskDelay(pdMS_TO_TICKS(200));
                if (!panduan == 0)
                {
                    mew_dat = vReadttp229Task();
                }
                else
                {
                    datt = (~vReadttp229Task());
                    mew_dat = (datt);
                }
                if ((((~mew_dat) & 16) == 16))
                {
                    check_led++;
                    if (check_led == 5)
                        check_led = 0;
                }
                if ((((~mew_dat) & 64) == 64))
                {
                    if (speedm == 20)
                        speedm = 40;
                    else
                        speedm = 20;
                }
                if (((~mew_dat) & 128) == 128)
                {
                    vTaskDelay(pdMS_TO_TICKS(150));
                    check_oled = 7;
                    break;
                }
                if (((~mew_dat) & 32) == 32)
                {
                    vTaskDelay(pdMS_TO_TICKS(150));
                    ttp_mutex_write(65535);
                    check_oled = 0;
                    break;
                }
            }
        }
        if (check_oled == 7)
        {
            continue_time -= speedm;
            if (continue_time <= 0)
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    ttp_mutex_write(65535);
    vTaskDelay(pdMS_TO_TICKS(stop_time));
}

void musicgame(int message, int continue_time, int stop_time)
{
    if (check_oled == 0)
        return;

    int message1 = (message & 0b1111111100001111);
    if (xQueueSend(xQueue, &message1, pdMS_TO_TICKS(100)) != pdPASS)
    {
        ESP_LOGW(TAG, "Queue full, message not sent");
    }

    uint16_t mew_dat = 0;
    uint16_t datt;
    int ci = 0;
    while (continue_time)
    {
        if (check_oled == 0)
            return;
        if (!panduan == 0)
        {
            mew_dat = vReadttp229Task();
        }
        else
        {
            datt = (~vReadttp229Task());
            mew_dat = (datt);
        }
        if (((~mew_dat) & message1) == message1)
        {
            ttp_mutex_write(~message);
            ci++;
        }
        if (check_oled == 8 || ((~mew_dat) & 128) == 128)
        {
            check_oled = 8;
            while (1)
            {
                vTaskDelay(pdMS_TO_TICKS(200));
                if (!panduan == 0)
                {
                    mew_dat = vReadttp229Task();
                }
                else
                {
                    datt = (~vReadttp229Task());
                    mew_dat = (datt);
                }
                if (((~mew_dat) & 128) == 128)
                {
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    check_oled = 12;
                    break;
                }
                if (((~mew_dat) & 32) == 32)
                {
                    vTaskDelay(pdMS_TO_TICKS(150));
                    ttp_mutex_write(65535);
                    check_oled = 0;
                    break;
                }
            }
        }
        if (check_oled == 12)
        {
            continue_time -= 20;
            if (continue_time <= 0)
                break;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
        ttp_mutex_write(65535);
    }
    if (message != 0)
        score = score + ci * 10;
    ii++;
    ttp_mutex_write(65535);
    vTaskDelay(pdMS_TO_TICKS(10));
}

// 神秘小曲的代码
void music_start(void)
{
    vTaskDelay(pdMS_TO_TICKS(2000));
    time = 1;
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
    time = 2;
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
    musictask(0b0000000000001000, 300, 10);
    musictask(0b0100000000000000, 300, 10);
    musictask(0b1000000000000000, 300, 10);
    musictask(0b0000000000000010, 300, 10); // 2211432

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
    time = 3;
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
    time = 5;
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
    time = 6;
    musictask(0b0100000000000000, 900, 10);
    musictask(0b1000000000000000, 150, 10);
    musictask(0b0100000000000000, 150, 10);
    musictask(0b0001000000000000, 1500, 10);
    musictask(0b0000000000000000, 900, 10); // 434550
    check_oled = 0;
    time = 1;
    vTaskDelay(pdMS_TO_TICKS(500));
}

void music_game(void)
{
    ii = 0;
    vTaskDelay(pdMS_TO_TICKS(500));
    ii = 1;
    musicgame(0b1000000000010000, 600, 10);
    musicgame(0b0000000000010010, 300, 10);
    musicgame(0b0000000000011000, 600, 10);
    musicgame(0b0000000000010010, 300, 10); // 3212

    musicgame(0b1000000000010000, 450, 10);
    musicgame(0b0100000000010000, 150, 10);
    musicgame(0b1000000000010000, 300, 10);
    musicgame(0b0000000000010010, 900, 10); // 3432

    musicgame(0b1000000000010000, 600, 10);
    musicgame(0b0000000000010010, 300, 10);
    musicgame(0b0000000000011000, 600, 10);
    musicgame(0b0000000000010010, 300, 10); // 3212

    musicgame(0b1000000000010000, 450, 10);
    musicgame(0b0100000000010000, 150, 10);
    musicgame(0b1000000000010000, 300, 10);
    musicgame(0b0000000000010010, 900, 10); // 3432

    musicgame(0b1000000000010000, 600, 10);
    musicgame(0b0000000000010010, 300, 10);
    musicgame(0b0000000000011000, 600, 10);
    musicgame(0b0000000000010010, 300, 10);

    musicgame(0b1000000000010000, 450, 10);
    musicgame(0b0100000000010000, 150, 10);
    musicgame(0b1000000000010000, 300, 10);
    musicgame(0b0000000000010010, 900, 10); // 3432

    musicgame(0b1000000000010000, 600, 10);
    musicgame(0b0000000000010010, 300, 10);
    musicgame(0b0000000000011000, 600, 10);
    musicgame(0b0000000000010010, 300, 10);

    musicgame(0b1000000000010000, 450, 10);
    musicgame(0b0100000000010000, 150, 10);
    musicgame(0b1000000000010000, 300, 10);
    musicgame(0b0000000000010010, 900, 10);
    musicgame(0b0000000000011000, 115, 10);
    musicgame(0b0000000000010010, 115, 10); // 343212

    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b0100000000000000, 300, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10); // 332432

    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b0000000000001000, 300, 10);
    musicgame(0b0000000000001000, 300, 10);
    musicgame(0b0100000000000000, 300, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10); // 211432

    musicgame(0b0000000000000010, 600, 10);
    musicgame(0b0000000000001000, 150, 10);
    musicgame(0b0000000000000010, 150, 10);
    musicgame(0b1000000000000000, 900, 10); // 2123

    musicgame(0b0000000000000000, 900, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0001000000000000, 300, 10);
    musicgame(0b0000000000011000, 300, 10); // 0351

    musicgame(0b0000100000000000, 600, 10);
    musicgame(0b0000000000011000, 300, 10);
    musicgame(0b0000100000000000, 600, 10);
    musicgame(0b0000000000011000, 300, 10); // 7171

    musicgame(0b0000100000000000, 150, 10);
    musicgame(0b0000001000000000, 150, 10);
    musicgame(0b0001000000000000, 600, 10);
    musicgame(0b0001000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b0100000000000000, 300, 10); // 765524

    musicgame(0b0100000000000000, 600, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b1000000000000000, 600, 10);
    musicgame(0b0001000001000000, 300, 10); // 4335

    musicgame(0b0100000000000000, 300, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b1000000000000000, 600, 10);
    musicgame(0b0001000000000000, 300, 10); // 43235

    musicgame(0b0000000000001000, 900, 10);
    musicgame(0b0000000000000000, 600, 10);
    musicgame(0b0000000000001000, 300, 10); // 101

    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b0000000000001000, 450, 10);
    musicgame(0b0000000000001000, 150, 10);
    musicgame(0b0000000000001000, 300, 10);
    musicgame(0b0001000000000000, 300, 10);
    musicgame(0b0000000000001000, 300, 10); // 211151

    musicgame(0b0100000000000000, 600, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 600, 10);
    musicgame(0b0000000000001000, 300, 10); // 4321

    musicgame(0b0000000000001000, 900, 10);
    musicgame(0b0000000000000000, 600, 10);
    musicgame(0b0000000000001000, 150, 10);
    musicgame(0b0000000000000010, 150, 10); // 1012

    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b0100000000000000, 300, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10); // 332432

    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b0000000000001000, 300, 10);
    musicgame(0b0000000000001000, 300, 10);
    musicgame(0b0100000000000000, 300, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10); // 2211432

    musicgame(0b0000000000000010, 600, 10);
    musicgame(0b0000000000001000, 150, 10);
    musicgame(0b0000000000000010, 150, 10);
    musicgame(0b1000000000000000, 900, 10); // 2123

    musicgame(0b0000000000000000, 900, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0001000000000000, 300, 10);
    musicgame(0b0000000000011000, 300, 10); // 0351

    musicgame(0b0000100000000000, 600, 10);
    musicgame(0b0000000000011000, 300, 10);
    musicgame(0b0000100000000000, 600, 10);
    musicgame(0b0000000000011000, 300, 10); // 7171

    musicgame(0b0000100000000000, 150, 10);
    musicgame(0b0000001000000000, 150, 10);
    musicgame(0b0001000000000000, 600, 10);
    musicgame(0b0001000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b0100000000000000, 300, 10); // 765524

    musicgame(0b0100000000000000, 600, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b1000000000000000, 600, 10);
    musicgame(0b0001000001000000, 300, 10); // 4335

    musicgame(0b0100000000000000, 300, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b1000000000000000, 600, 10);
    musicgame(0b0001000000000000, 300, 10); // 43235

    musicgame(0b0000000000001000, 900, 10);
    musicgame(0b0000000000000000, 600, 10);
    musicgame(0b0000000000001000, 150, 10);
    musicgame(0b0000000000001000, 150, 10); // 1011

    musicgame(0b0000000000000010, 450, 10);
    musicgame(0b0000000000001000, 600, 10);
    musicgame(0b0000000000001000, 300, 10);
    musicgame(0b0001000000000000, 300, 10);
    musicgame(0b0000000000001000, 300, 10); // 21151

    musicgame(0b0100000000000000, 300, 10);
    musicgame(0b0100000000000000, 150, 10);
    musicgame(0b0100000000000000, 150, 10);
    musicgame(0b1000000000000000, 150, 10);
    musicgame(0b0000000000000010, 150, 10);
    musicgame(0b0000000000000010, 600, 10);
    musicgame(0b0000000000001000, 300, 10); // 4443221

    musicgame(0b0000000000001000, 450, 10);
    musicgame(0b0000000000000000, 450, 10);
    musicgame(0b0000000000000000, 900, 10); // 100

    musicgame(0b0000001000010000, 300, 10);
    musicgame(0b0001000000010000, 300, 10);
    musicgame(0b0001000000010000, 300, 10);
    musicgame(0b0001000000010000, 300, 10);
    musicgame(0b0100000000010000, 300, 10);
    musicgame(0b0100000000010000, 300, 10); // 655544

    musicgame(0b1000000000010000, 300, 10);
    musicgame(0b0000000000010010, 300, 10);
    musicgame(0b0000000000010010, 300, 10);
    musicgame(0b0000000000010010, 300, 10);
    musicgame(0b0000000000000000, 300, 10);
    musicgame(0b0001000000000000, 300, 10); // 322205

    musicgame(0b0001000000000000, 300, 10);
    musicgame(0b0100000000000000, 150, 10);
    musicgame(0b0100000000000000, 150, 10);
    musicgame(0b0100000000000000, 300, 10);
    musicgame(0b0100000000000000, 300, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10); // 4443221

    musicgame(0b0000000000000010, 600, 10);
    musicgame(0b0000000000001000, 150, 10);
    musicgame(0b0000100001000000, 150, 10);
    musicgame(0b0000000000001000, 450, 10);
    musicgame(0b0000000000000000, 450, 10); // 21710

    musicgame(0b0000001000010000, 300, 10);
    musicgame(0b0001000000010000, 300, 10);
    musicgame(0b0001000000010000, 300, 10);
    musicgame(0b0001000000010000, 300, 10);
    musicgame(0b0100000000010000, 300, 10);
    musicgame(0b0100000000010000, 300, 10); // 655544

    musicgame(0b1000000000010000, 300, 10);
    musicgame(0b0000000000010010, 300, 10);
    musicgame(0b0000000000010010, 300, 10);
    musicgame(0b0000000000010010, 300, 10);
    musicgame(0b0000000000000000, 300, 10);
    musicgame(0b1000000000000000, 300, 10); // 322203

    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b1000000000000000, 150, 10);
    musicgame(0b1000000000000000, 150, 10);
    musicgame(0b1000000000000000, 150, 10);
    musicgame(0b1000000000000000, 150, 10);
    musicgame(0b1000000000000000, 300, 10);
    musicgame(0b0000000000000010, 300, 10);
    musicgame(0b1000000000000000, 300, 10); // 33333323

    musicgame(0b0000000000010010, 600, 10);
    musicgame(0b0000000000011000, 300, 10);
    musicgame(0b0000000000011000, 300, 10);
    musicgame(0b0000000000000000, 300, 10);
    musicgame(0b0000000000011000, 300, 10);

    musicgame(0b0000100000000000, 600, 10);
    musicgame(0b0000001000000000, 300, 10);
    musicgame(0b0000001000000000, 900, 10);

    musicgame(0b0000000000000000, 600, 10);
    musicgame(0b0000001000000000, 300, 10);
    musicgame(0b0000001000000000, 300, 10);
    musicgame(0b0001000000000000, 300, 10);
    musicgame(0b0100000000000000, 150, 10);
    musicgame(0b0100000000000000, 150, 10); // 066544

    musicgame(0b0100000000000000, 900, 10);
    musicgame(0b1000000000000000, 150, 10);
    musicgame(0b0100000000000000, 150, 10);
    musicgame(0b0001000000000000, 1500, 10);
    musicgame(0b0000000000000000, 900, 10); // 434550
    check_oled = 13;
    ii = 0;
    vTaskDelay(pdMS_TO_TICKS(500));
}