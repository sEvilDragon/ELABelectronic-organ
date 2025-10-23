#include "TASK_OLED.h"

u8g2_t u8g2;

uint8_t u8g2_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        // 在你的 oled_spi_start() 中已经做了 GPIO 初始化和 RES 复位，这里可以跳过或只做简单检查
        break;

        // 【重要：移除 U8X8_MSG_GPIO_SPI_DATA 和 CLOCK 的处理，因为我们用硬件SPI】

    case U8X8_MSG_GPIO_CS:
        // 这里的 CS 控制逻辑必须与 devcfg.spics_io_num 匹配
        // 如果 spi_bus_add_device 自动管理 CS，这里可能不需要操作
        // 如果要手动控制 CS，则：gpio_set_level(OLED_CS, arg_int);
        break;

    case U8X8_MSG_GPIO_DC:
        // 在硬件 SPI 模式下，U8g2 也会在这里设置 DC。
        gpio_set_level(OLED_DC, arg_int);
        break;

    case U8X8_MSG_GPIO_RESET:
        gpio_set_level(OLED_RES, arg_int);
        break;

    case U8X8_MSG_DELAY_MILLI:
        vTaskDelay(pdMS_TO_TICKS(arg_int));
        break;
        // ... 其他延时消息 ...

    default:
        return 0;
    }
    return 1;
}

uint8_t u8g2_hw_spi_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_BYTE_INIT:
        // 初始化 SPI 句柄（你已在 oled_spi_start 中完成）
        break;

    case U8X8_MSG_BYTE_SET_DC:
        break;

    case U8X8_MSG_BYTE_SEND:
    {
        uint8_t *data = (uint8_t *)arg_ptr;
        size_t len = arg_int;

        if (len > 0)
        {
            spi_transaction_t t;
            memset(&t, 0, sizeof(t));
            t.length = len * 8;
            t.tx_buffer = data;
            // 不需要 t.user 和 pre_cb，因为 U8g2 会在调用 SEND 之前，先调用 SET_DC

            // 使用你的全局 SPI 句柄进行传输
            spi_device_transmit(spi, &t);
        }
        break;
    }

    // U8g2 可能会发送 START/END TRANSFER 消息，但如果使用 ESP-IDF 硬件SPI，通常可以忽略
    case U8X8_MSG_BYTE_START_TRANSFER:
    case U8X8_MSG_BYTE_END_TRANSFER:
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
                                       u8g2_hw_spi_byte_cb,
                                       u8g2_gpio_and_delay_cb);

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}

void OLED_word(void)
{
    uint8_t data1[] = {0b00000100,
                       0b00100100,
                       0b00100100,
                       0b00100100,
                       0b01111111,
                       0b11000100,
                       0b01000100,
                       0b00000100,
                       0b00000100,
                       0b11111111,
                       0b00000100,
                       0b01000100,
                       0b00110100,
                       0b00000101,
                       0b00000100,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b11111110,
                       0b10010010,
                       0b10010010,
                       0b10010010,
                       0b10010010,
                       0b10010010,
                       0b10010010,
                       0b10010010,
                       0b11111110,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00011111,
                       0b00010001,
                       0b00010001,
                       0b00010001,
                       0b00010001,
                       0b11111111,
                       0b00010001,
                       0b00010001,
                       0b00010001,
                       0b00010001,
                       0b00011111,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000001,
                       0b01000001,
                       0b01000001,
                       0b01000001,
                       0b01000001,
                       0b01000001,
                       0b01000001,
                       0b01000111,
                       0b01000101,
                       0b01001001,
                       0b01010001,
                       0b01100001,
                       0b01000001,
                       0b00000001,
                       0b00000001,
                       0b00000000,
                       0b01000001,
                       0b01001001,
                       0b01001001,
                       0b01111111,
                       0b01001001,
                       0b01001001,
                       0b01000001,
                       0b00000000,
                       0b01000001,
                       0b01001001,
                       0b01001001,
                       0b01111111,
                       0b01001001,
                       0b01001001,
                       0b01000001,
                       0b00000000,
                       0b00000101,
                       0b01000101,
                       0b00101001,
                       0b00010001,
                       0b11111111,
                       0b00000001,
                       0b00010001,
                       0b00100101,
                       0b01100101,
                       0b10110101,
                       0b00101001,
                       0b00101001,
                       0b00110001,
                       0b00100001,
                       0b00000001,
                       0b00000000};
    uint8_t data2[] = {0b00000000,
                       0b00010000,
                       0b00010010,
                       0b00100001,
                       0b11111110,
                       0b01000000,
                       0b10000010,
                       0b00000010,

                       0b00000100,
                       0b11001000,
                       0b00110000,
                       0b01000100,
                       0b10000010,
                       0b00011111,
                       0b00000000,
                       0b00000000,
                       0b10000001,

                       0b10000010,
                       0b10000100,
                       0b10111000,
                       0b10000100,
                       0b10000010,
                       0b10000001,
                       0b11111111,

                       0b10010001,
                       0b10010001,
                       0b10010001,
                       0b10010001,
                       0b10010001,
                       0b10000001,
                       0b10000001,
                       0b00000000,

                       0b00000000,
                       0b00000000,
                       0b11111000,
                       0b00010000,
                       0b00010000,
                       0b00010000,
                       0b00010000,
                       0b11111110,

                       0b00010001,
                       0b00010001,
                       0b00010001,
                       0b00010001,
                       0b11111001,
                       0b00000001,
                       0b00001111,
                       0b00000000,

                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000010,
                       0b00000001,
                       0b11111110,

                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,

                       0b00010000,
                       0b00010000,
                       0b00010000,
                       0b00101000,
                       0b00101000,
                       0b01001000,
                       0b01101000,
                       0b10011001,

                       0b01001010,
                       0b01001100,
                       0b00101000,
                       0b00100000,
                       0b00010000,
                       0b00010000,
                       0b00010000,
                       0b00000000,

                       0b00000000,
                       0b00000000,
                       0b01111111,
                       0b01001101,
                       0b01010101,
                       0b11100101,
                       0b01000101,
                       0b01000101,

                       0b11100101,
                       0b01010101,
                       0b01010101,
                       0b01010101,
                       0b01111111,
                       0b00000000,
                       0b00000000,
                       0b00000000};

    uint8_t data3[] = {0b00111110,
                       0b01000010,
                       0b01000010,
                       0b01100100,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,

                       0b00010010,
                       0b01111110,
                       0b00000010,
                       0b00000000,

                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,

                       0b00001110,
                       0b00010110,
                       0b00011110,
                       0b00000010,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,

                       0b00100010,
                       0b01111110,
                       0b00000010,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,

                       0b00100010,
                       0b01111110,
                       0b00000010,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,

                       0b00001110,
                       0b00010010,
                       0b00010010,
                       0b00001110,
                       0b00000000,
                       0b00000000,
                       0b00000000,
                       0b00000000,

                       0b00001000,
                       0b00011000,
                       0b00010000,
                       0b00001000,
                       0b00001000,
                       0b00001000,
                       0b00011000,
                       0b00000000};

    location(5, 16);
    oled_send_data(data1, sizeof(data1));

    location(4, 16);
    oled_send_data(data2, sizeof(data2));

    location(2, 38);
    oled_send_data(data3, sizeof(data3));

    oled_send_cmd(0x2E);
    oled_send_cmd(0x27);
    oled_send_cmd(0x00); // Dummy byte A
    oled_send_cmd(0x00); // P2: 开始页 (0-7)
    oled_send_cmd(0x00); // P3: 时间间隔
    oled_send_cmd(0x03); // P4: 结束页 (0-7)
    oled_send_cmd(0x00); // Dummy byte B
    oled_send_cmd(0xFF); // Dummy byte C
    oled_send_cmd(0x2F);
}