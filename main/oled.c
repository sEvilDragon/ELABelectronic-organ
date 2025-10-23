#include "TASK_OLED.h"

static const char *TAG = "log_oled";

// 发送
void oled_send_cmd(uint8_t cmd)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8; // 8位数据
    t.tx_buffer = &cmd;
    t.user = (void *)(0); // DC 引脚拉低 (命令)
    spi_device_transmit(spi, &t);
}

// 发送数据
void oled_send_data(uint8_t data[], size_t length)
{ // 这里的length是字节长度
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8 * length;
    t.tx_buffer = data;
    t.user = (void *)(1);
    spi_device_transmit(spi, &t);
}

// 定位
void location(int page, int y)
{
    int m = 176 + page;
    oled_send_cmd(m);
    int under = y & 0b00001111;
    oled_send_cmd(under);
    int up = y & 0b11110000;
    up = up >> 4;
    up += 16;
    oled_send_cmd(up);
}

// OLED清屏（去除雪花）这里只写了清除一页的来做测试
void OLED_clear(unsigned int page)
{
    location(page, 0);
    uint8_t data[128] = {0};
    oled_send_data(data, 128);
}

void oled_spi_pre_transfer_callback(spi_transaction_t *t)
{
    // t->user 存储了是命令还是数据的标识符
    int dc_level = (int)t->user;
    gpio_set_level(OLED_DC, dc_level);
}

void oled_init(void)
{
    oled_send_cmd(0xAE); // 说是一开始要关，为了安全
    // 设置时针用的，这里套用建议的平均值
    oled_send_cmd(0xD5);
    oled_send_cmd(0x80);
    // 这个好像是用来确定多少行的
    oled_send_cmd(0xA8);
    oled_send_cmd(0x3F);
    // 让屏幕像素点可以亮
    oled_send_cmd(0x8D);
    oled_send_cmd(0x14);
    // 就是设置亮度的好像，也是套用常用值
    oled_send_cmd(0x81);
    oled_send_cmd(0xCF);
    // 这个好像就是预加载，提高显示效果的
    oled_send_cmd(0xD9);
    oled_send_cmd(0xF1);
    // 设置电压的，也是用了推荐的，说是防止鬼影
    oled_send_cmd(0xDB);
    oled_send_cmd(0x20);
    // 启用页地址，我想实现的功能需要这个
    oled_send_cmd(0x20);
    oled_send_cmd(0x02);
    // 这个就是调整用的
    oled_send_cmd(0xA1); // (如果图像是镜像的，使用A0h)
    // oled_send_cmd(0xC8); // (如果上下颠倒，使用C0h)
    //  显示自己想让他实现的功能
    oled_send_cmd(0xA4);

    oled_send_cmd(0xAF); // 配置完了再打开屏幕

    OLED_clear(0);
    OLED_clear(1);
    OLED_clear(2);
    OLED_clear(3);
    OLED_clear(4);
    OLED_clear(5);
    OLED_clear(6);
    OLED_clear(7);

    OLED_word();
}

// spi初始化
void oled_spi_start(void)
{
    ESP_LOGI(TAG, "安装SPI");
    spi_bus_config_t buscfg = {
        .sclk_io_num = OLED_SCL,
        .mosi_io_num = OLED_SDA,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };

    esp_err_t ret;
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "SPI初始化故障");
    }

    spi_device_interface_config_t devcfg = {
        .command_bits = 0,                        // 传输事务中的命令位长度 (0 表示不使用命令阶段)
        .address_bits = 0,                        // 传输事务中的地址位长度 (0 表示不使用地址阶段)
        .dummy_bits = 0,                          // 传输事务中的虚拟位长度 (用于满足特定设备的时序要求)
        .clock_speed_hz = 10 * 1000 * 1000,       // 设置 SPI 时钟频率为 10MHz
        .duty_cycle_pos = 128,                    // 设置时钟占空比 (128 表示 50%)
        .mode = 0,                                // 设置 SPI 模式 (Mode 0: CPOL=0, CPHA=0)
        .spics_io_num = OLED_CS,                  // 设置该设备的片选信号连接的 GPIO
        .queue_size = 6,                          // 设置可排队传输事务的数量
        .pre_cb = oled_spi_pre_transfer_callback, // OLED必须有，用于CD的切换
        .post_cb = NULL,                          // 传输后回调函数 (可选)
    };

    // --- 关键改进：初始化 DC 和 RES GPIO ---
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << OLED_DC) | (1ULL << OLED_RES);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // --- 关键改进：硬件复位操作 ---
    // RES 为低电平有效
    gpio_set_level(OLED_RES, 0);    // 拉低 RES
    vTaskDelay(pdMS_TO_TICKS(10));  // 延迟一段时间
    gpio_set_level(OLED_RES, 1);    // 释放 RES
    vTaskDelay(pdMS_TO_TICKS(100)); // 再次等待设备启动

    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "SPI初始化故障");
    }

    oled_init();
    OLED_word();
}

void menu(void)
{
    OLED_clear(0);
    OLED_clear(1);
    OLED_clear(2);
    OLED_clear(3);
    OLED_clear(4);
    OLED_clear(5);
    OLED_clear(6);
    OLED_clear(7);
}

// 模拟实现
// void oled_init();

// void OLED_start(void)
// {
//     gpio_set_direction(OLED_SCL, GPIO_MODE_OUTPUT);
//     gpio_set_direction(OLED_SDA, GPIO_MODE_OUTPUT);
//     gpio_set_direction(OLED_CS, GPIO_MODE_OUTPUT);
//     gpio_set_direction(OLED_DC, GPIO_MODE_OUTPUT);
//     gpio_set_direction(OLED_RES, GPIO_MODE_OUTPUT);

//     gpio_set_level(OLED_RES, 0);    // 拉低 RES
//     vTaskDelay(pdMS_TO_TICKS(10));  // 延迟一段时间
//     gpio_set_level(OLED_RES, 1);    // 释放 RES
//     vTaskDelay(pdMS_TO_TICKS(100)); // 再次等待设备启动

//     oled_init();
// }

// void oled_send_cmd(uint8_t cmd)
// {
//     gpio_set_level(OLED_CS, 0);
//     gpio_set_level(OLED_DC, 0);
//     gpio_set_level(OLED_SCL, 0);
//     esp_rom_delay_us(1);
//     for (int i = 0; i < 8; i++)
//     {
//         uint8_t bit = (cmd >> (7 - i)) & 0x01;
//         gpio_set_level(OLED_SDA, bit);
//         esp_rom_delay_us(1);
//         gpio_set_level(OLED_SCL, 1);
//         esp_rom_delay_us(1);
//         gpio_set_level(OLED_SCL, 0);
//         esp_rom_delay_us(1);
//     }
//     gpio_set_level(OLED_SDA, 0);
//     gpio_set_level(OLED_CS, 1);
// }

// void oled_send_data(uint8_t data[], size_t length)
// {
//     gpio_set_level(OLED_CS, 0);
//     gpio_set_level(OLED_DC, 1);
//     gpio_set_level(OLED_SCL, 0);
//     esp_rom_delay_us(1);
//     for (int i = 0; i < length; i++)
//     {
//         for (int j = 0; j < 8; j++)
//         {
//             uint8_t bit = ((data[i]) >> (7 - j)) & 0x01;
//             gpio_set_level(OLED_SDA, bit);
//             esp_rom_delay_us(1);
//             gpio_set_level(OLED_SCL, 1);
//             esp_rom_delay_us(1);
//             gpio_set_level(OLED_SCL, 0);
//             esp_rom_delay_us(1);
//         }
//     }
// }
