#include "TASK_OLED.h"

extern int check_oled;
extern int check_led;
extern int check_speed;
extern int set_voice;
extern float set_led;
extern int time;
extern float speed;
extern long long int score;
extern int ii;
int x = 64;
int y = 128;
int speedy = 3;
u8g2_t u8g2;
static const char *TAG = "log_u8g2";

void buzzer_mutex_write(int new_duty);

// 设置一个数组用于在屏幕上显示大了上面字
char letter[3] = {0};

int yinfuu[] = {0, 3, 2, 1, 2,
                3, 4, 3, 2,
                3, 2, 1, 2,
                3, 4, 3, 2,
                3, 2, 1, 2,
                3, 4, 3, 2,
                3, 2, 1, 2,
                3, 4, 3, 2, 1, 2,
                3, 3, 2, 4, 3, 2,
                2, 2, 1, 1, 4, 3, 2,
                2, 1, 2, 3,
                0, 3, 5, 1,
                7, 1, 7, 1,
                7, 6, 5, 5, 2, 4,
                4, 3, 3, 5,
                4, 3, 2, 3, 5,
                1, 0, 1,
                2, 1, 1, 1, 5, 1,
                4, 3, 2, 1,
                1, 0, 1, 2,
                3, 3, 2, 4, 3, 2,
                2, 2, 1, 1, 4, 3, 2,
                2, 1, 2, 3,
                0, 3, 5, 1,
                7, 1, 7, 1,
                7, 6, 5, 5, 2, 4,
                4, 3, 3, 5,
                4, 3, 2, 3, 5,
                1, 0, 1, 1,
                2, 1, 1, 5, 1,
                4, 4, 4, 3, 2, 2, 1,
                1, 0, 0,
                6, 5, 5, 5, 4, 4,
                3, 2, 2, 2, 0, 5,
                4, 4, 4, 3, 2, 2, 1,
                2, 1, 7, 1, 0,
                6, 5, 5, 5, 4, 4,
                3, 2, 2, 2, 0, 3,
                3, 3, 3, 3, 3, 3, 2, 3,
                2, 1, 1, 0, 1,
                7, 6, 6,
                0, 6, 6, 5, 4, 4,
                4, 3, 4, 5, 5, 0, 0, 0};

// 尝试创建一个互斥锁
void letter_mutex_start(void)
{
    letter_mutex = xSemaphoreCreateMutex();
    if (letter_mutex == NULL)
        ESP_LOGE(TAG, "FATAL: Failed to create led_mutex");
}

// 创建一个函数用于写入互斥锁数据
void letter_mutex_write(int tag, int value)
{
    if (tag < 0 || tag >= 3)
        return;
    if (xSemaphoreTake(letter_mutex, portMAX_DELAY) == pdTRUE)
    {
        letter[tag] = value;
        xSemaphoreGive(letter_mutex);
    }
}

// 创建一个函数用于读取互斥锁数据
int letter_mutex_get(int tag)
{
    int value = 0;
    if (tag < 0 || tag >= 3)
        return value;
    if (xSemaphoreTake(letter_mutex, portMAX_DELAY) == pdTRUE)
    {
        value = letter[tag];
        xSemaphoreGive(letter_mutex);
    }
    return value;
}

uint8_t u8g2_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_GPIO_SPI_CLOCK:
        gpio_set_level(OLED_SCL, arg_int);
        break;
    case U8X8_MSG_GPIO_SPI_DATA:
        gpio_set_level(OLED_SDA, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        gpio_set_level(OLED_CS, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        gpio_set_level(OLED_DC, arg_int);
        break;

    case U8X8_MSG_GPIO_RESET:
        gpio_set_level(OLED_RES, arg_int);
        break;

    case U8X8_MSG_DELAY_MILLI:
        vTaskDelay(pdMS_TO_TICKS(arg_int));
        break;
    default:
        return 0;
    }
    return 1;
}

void u8g2_app_init(void)
{
    u8g2_Setup_ssd1306_128x64_noname_f(&u8g2,
                                       U8G2_R0,
                                       u8x8_byte_4wire_sw_spi, // <-- 使用 U8G2 内置的软件 SPI 驱动
                                       u8g2_gpio_and_delay_cb);

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_ClearBuffer(&u8g2);
    u8g2_SendBuffer(&u8g2);
}

void OLED_menu(void)
{
    char display_buffer[64]; // 临时缓冲区
    int array_index = 1;     // 想要打印的数组索引

    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_helvR08_tf);
    u8g2_DrawUTF8(&u8g2, 50, 8, "Ciallo~");

    u8g2_SetFont(&u8g2, u8g2_font_helvR14_tf);
    array_index = 0;
    snprintf(display_buffer, sizeof(display_buffer),
             "%c", letter[array_index]);
    u8g2_DrawUTF8(&u8g2, 10, 40, display_buffer);

    array_index = 1;
    snprintf(display_buffer, sizeof(display_buffer),
             "%c", letter[array_index]);
    u8g2_DrawUTF8(&u8g2, 32, 40, display_buffer);

    u8g2_SetFont(&u8g2, u8g2_font_helvR24_tf);
    array_index = 2;
    snprintf(display_buffer, sizeof(display_buffer),
             "%c", letter[array_index]);
    u8g2_DrawUTF8(&u8g2, 55, 40, display_buffer);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "设定B");
    u8g2_DrawUTF8(&u8g2, 85, 62, "菜单L");

    u8g2_SendBuffer(&u8g2);
}

void caidan2(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "向上E");
    u8g2_DrawUTF8(&u8g2, 85, 62, "向下A");
    u8g2_DrawUTF8(&u8g2, 0, 15, "------设定------");
    u8g2_DrawUTF8(&u8g2, 10, 40, "亮度：");
    int led = set_led * 100;
    switch (led)
    {
    case 100:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 正常 ");
        break;
    case 125:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 很亮 ");
        break;
    case 0:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 不亮 ");
        break;
    case 50:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 不太亮 ");
        break;
    }

    u8g2_SendBuffer(&u8g2);
}

void caidan3(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "向上E");
    u8g2_DrawUTF8(&u8g2, 85, 62, "向下A");
    u8g2_DrawUTF8(&u8g2, 0, 15, "------设定------");
    u8g2_DrawUTF8(&u8g2, 10, 40, "光种：");
    switch (check_led)
    {
    case 0:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 绿流光 ");
        break;
    case 1:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 红流光 ");
        break;
    case 2:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 蓝流光 ");
        break;
    case 3:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 彩虹光 ");
        break;
    case 4:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 红底光 ");
        break;
    default:
        break;
    }

    u8g2_SendBuffer(&u8g2);
}

void caidan4(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "向上E");
    u8g2_DrawUTF8(&u8g2, 85, 62, "向下A");
    u8g2_DrawUTF8(&u8g2, 0, 15, "------设定------");
    u8g2_DrawUTF8(&u8g2, 10, 40, "流光速：");
    switch (check_speed)
    {
    case 0:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 正常 ");
        speed = 20;
        break;
    case 1:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 快 ");
        speed = 10;
        break;
    case 2:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 很快 ");
        speed = 5;
        break;
    case 3:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 不快 ");
        speed = 35;
        break;
    default:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 很不快 ");
        speed = 50;
        break;
    }

    u8g2_SendBuffer(&u8g2);
}

void caidan5(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "向上E");
    u8g2_DrawUTF8(&u8g2, 85, 62, "向下A");
    u8g2_DrawUTF8(&u8g2, 0, 15, "------菜单------");
    u8g2_DrawUTF8(&u8g2, 32, 40, "播放音乐");

    u8g2_SendBuffer(&u8g2);
}

void caidan6(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "向上E");
    u8g2_DrawUTF8(&u8g2, 85, 62, "向下A");
    u8g2_DrawUTF8(&u8g2, 0, 15, "------菜单------");
    u8g2_DrawUTF8(&u8g2, 32, 40, "一点感想");

    u8g2_SendBuffer(&u8g2);
}

void caidan7(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "停止B");
    u8g2_DrawUTF8(&u8g2, 85, 62, "快进L");
    switch (time)
    {
    case 1:
        u8g2_DrawUTF8(&u8g2, 0, 15, "     -音乐-     ");
        break;
    case 2:
        u8g2_DrawUTF8(&u8g2, 0, 15, "    --音乐--    ");
        break;
    case 3:
        u8g2_DrawUTF8(&u8g2, 0, 15, "   ---音乐---   ");
        break;
    case 4:
        u8g2_DrawUTF8(&u8g2, 0, 15, "  ----音乐----  ");
        break;
    case 5:
        u8g2_DrawUTF8(&u8g2, 0, 15, " -----音乐----- ");
        break;
    case 6:
        u8g2_DrawUTF8(&u8g2, 0, 15, "------音乐------");
        break;
    default:
        break;
    }
    OLED_scroll_music();
}

void caidan8(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "开始B");
    u8g2_DrawUTF8(&u8g2, 85, 62, "完事L");
    switch (time)
    {
    case 1:
        u8g2_DrawUTF8(&u8g2, 0, 15, "     -音乐-     ");
        break;
    case 2:
        u8g2_DrawUTF8(&u8g2, 0, 15, "    --音乐--    ");
        break;
    case 3:
        u8g2_DrawUTF8(&u8g2, 0, 15, "   ---音乐---   ");
        break;
    case 4:
        u8g2_DrawUTF8(&u8g2, 0, 15, "  ----音乐----  ");
        break;
    case 5:
        u8g2_DrawUTF8(&u8g2, 0, 15, " -----音乐----- ");
        break;
    case 6:
        u8g2_DrawUTF8(&u8g2, 0, 15, "------音乐------");
        break;
    default:
        break;
    }
    u8g2_DrawUTF8(&u8g2, 32, 40, "音乐停止");

    u8g2_SendBuffer(&u8g2);
}

void caidan9(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "快进B");
    u8g2_DrawUTF8(&u8g2, 85, 62, "完事L");
    u8g2_DrawUTF8(&u8g2, 0, 15, "------感谢------");
    OLED_scroll_word();

    u8g2_SendBuffer(&u8g2);
}

void caidan1(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "向上E");
    u8g2_DrawUTF8(&u8g2, 85, 62, "向下A");
    u8g2_DrawUTF8(&u8g2, 0, 15, "------设定------");
    u8g2_DrawUTF8(&u8g2, 10, 40, "声音：");

    switch (set_voice)
    {
    case 0:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 不太大 ");
        buzzer_mutex_write(80);
        break;
    case 1:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 正常 ");
        buzzer_mutex_write(2048);
        break;
    case 2:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 没声 ");
        buzzer_mutex_write(0);
        break;
    case 3:
        u8g2_DrawUTF8(&u8g2, 64, 40, " 小声 ");
        buzzer_mutex_write(20);
        break;
    }

    u8g2_SendBuffer(&u8g2);
}

void caidan10(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 16, 32, "->播放音乐<-");
    u8g2_DrawUTF8(&u8g2, 0, 15, "------音乐------");
    u8g2_DrawUTF8(&u8g2, 32, 56, "开始音游");

    u8g2_SendBuffer(&u8g2);
}

void caidan11(void)
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 16, 56, "->开始音游<-");
    u8g2_DrawUTF8(&u8g2, 0, 15, "------音乐------");
    u8g2_DrawUTF8(&u8g2, 32, 32, "播放音乐");

    u8g2_SendBuffer(&u8g2);
}

void caidan12(void)
{
    char display_buffer[64]; // 临时缓冲区

    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "停下B");
    if (score >= 0 && score < 1000)
        u8g2_DrawUTF8(&u8g2, 80, 62, "加油!");
    else if (score > 1000 && score < 5000)
        u8g2_DrawUTF8(&u8g2, 80, 62, "GOOD!");
    else if (score > 5000 && score < 10000)
        u8g2_DrawUTF8(&u8g2, 80, 62, "强!");
    else if (score > 10000 && score < 20000)
        u8g2_DrawUTF8(&u8g2, 80, 62, "非人!");
    else
        u8g2_DrawUTF8(&u8g2, 80, 62, "神!!");

    u8g2_SetFont(&u8g2, u8g2_font_helvR08_tf);
    u8g2_DrawUTF8(&u8g2, 20, 8, "score:");

    snprintf(display_buffer, sizeof(display_buffer),
             "%lld", score);
    u8g2_DrawUTF8(&u8g2, 80, 8, display_buffer);

    u8g2_SetFont(&u8g2, u8g2_font_helvR24_tf);

    snprintf(display_buffer, sizeof(display_buffer),
             "%d", yinfuu[ii]);
    u8g2_DrawUTF8(&u8g2, 55, 40, display_buffer);

    u8g2_SetFont(&u8g2, u8g2_font_helvR14_tf);

    snprintf(display_buffer, sizeof(display_buffer),
             "%d", yinfuu[ii + 2]);
    u8g2_DrawUTF8(&u8g2, 104, 40, display_buffer);

    snprintf(display_buffer, sizeof(display_buffer),
             "%d", yinfuu[ii + 1]);
    u8g2_DrawUTF8(&u8g2, 82, 40, display_buffer);

    u8g2_SendBuffer(&u8g2);
}

void caidan13(void)
{
    char display_buffer[64]; // 临时缓冲区

    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);
    u8g2_DrawUTF8(&u8g2, 1, 62, "停下B");

    if (score >= 0 && score < 1000)
        u8g2_DrawUTF8(&u8g2, 80, 62, "加油!");
    else if (score > 1000 && score < 5000)
        u8g2_DrawUTF8(&u8g2, 80, 62, "GOOD!");
    else if (score > 5000 && score < 10000)
        u8g2_DrawUTF8(&u8g2, 80, 62, "强!");
    else if (score > 10000 && score < 20000)
        u8g2_DrawUTF8(&u8g2, 80, 62, "非人!");
    else
        u8g2_DrawUTF8(&u8g2, 80, 62, "神!!");

    u8g2_SetFont(&u8g2, u8g2_font_helvR08_tf);
    u8g2_DrawUTF8(&u8g2, 35, 8, "GAME OVER!!");

    u8g2_SetFont(&u8g2, u8g2_font_helvR14_tf);
    snprintf(display_buffer, sizeof(display_buffer),
             "%lld", score);
    u8g2_DrawUTF8(&u8g2, 70, 36, display_buffer);

    u8g2_SetFont(&u8g2, u8g2_font_helvR24_tf);

    if (score < 1000)
        u8g2_DrawUTF8(&u8g2, 10, 40, "E");
    else if (score < 5000)
        u8g2_DrawUTF8(&u8g2, 10, 40, "D");
    else if (score < 10000)
        u8g2_DrawUTF8(&u8g2, 10, 40, "C");
    else if (score < 20000)
        u8g2_DrawUTF8(&u8g2, 10, 40, "B");
    else if (score < 30000)
        u8g2_DrawUTF8(&u8g2, 10, 40, "A");
    else if (score < 35000)
        u8g2_DrawUTF8(&u8g2, 10, 40, "S");
    else
        u8g2_DrawUTF8(&u8g2, 10, 40, "S+");

    u8g2_SendBuffer(&u8g2);
}

void vWordTask(void *pvParameters)
{
    while (1)
    {
        switch (check_oled)
        {
        case 0:
            OLED_menu();
            break;
        case 1:
            caidan1();
            break;
        case 2:
            caidan2();
            break;
        case 3:
            caidan3();
            break;
        case 4:
            caidan4();
            break;
        case 5:
            caidan5();
            break;
        case 6:
            caidan6();
            break;
        case 7:
            caidan7();
            break;
        case 8:
            caidan8();
            break;
        case 9:
            caidan9();
            break;
        case 10:
            caidan10();
            break;
        case 11:
            caidan11();
            break;
        case 12:
            caidan12();
            break;
        case 13:
            caidan13();
            break;
        default:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void OLED_scroll_music(void)
{
    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);

    const char *text_to_scroll = "  正在播放<春日影> ";
    u8g2_uint_t text_width = u8g2_GetUTF8Width(&u8g2, text_to_scroll);

    u8g2_DrawUTF8(&u8g2, x, 40, text_to_scroll);

    u8g2_SendBuffer(&u8g2);
    x -= 2;
    if (x <= -text_width)
    {
        x = 128;
    }
}

void OLED_scroll_word(void)
{
    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_chinese3);

    const char *text_to_scroll = " First of all, I have to curse the u8g2 library. It's just too bad. There are so many characters missing. I went through so much trouble just to save some effort, but it ended up being even more troublesome! Anyway, these are all the functions I've achieved in the past four weeks. I'm very grateful for everyone's help and guidance. Of course, there are still many functions that haven't been realized. I hope I can make more progress in my future studies. Thank you! ";
    u8g2_uint_t text_width = u8g2_GetUTF8Width(&u8g2, text_to_scroll);

    u8g2_DrawUTF8(&u8g2, y, 40, text_to_scroll);

    u8g2_SendBuffer(&u8g2);
    y -= speedy;
    if (y <= -text_width)
    {
        check_oled = 0;
        return;
    }
}
