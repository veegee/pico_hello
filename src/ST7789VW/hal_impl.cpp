#include "hal_impl.h"

#define SPI_PORT spi1

uint slice_num;

void DEV_Digital_Write(uint16_t Pin, uint8_t Value) {
    gpio_put(Pin, Value);
}

uint8_t DEV_Digital_Read(uint16_t Pin) {
    return gpio_get(Pin);
}

void DEV_SPI_WriteByte(uint8_t Value) {
    spi_write_blocking(SPI_PORT, &Value, 1);
}

void DEV_SPI_Write_nByte(uint8_t pData[], uint32_t Len) {
    spi_write_blocking(SPI_PORT, pData, Len);
}


void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode) {
    gpio_init(Pin);
    if (Mode == 0 || Mode == GPIO_IN) {
        gpio_set_dir(Pin, GPIO_IN);
    } else {
        gpio_set_dir(Pin, GPIO_OUT);
    }
}

void DEV_KEY_Config(uint16_t Pin) {
    gpio_init(Pin);
    gpio_pull_up(Pin);
    gpio_set_dir(Pin, GPIO_IN);
}

void DEV_Delay_ms(uint32_t xms) {
    sleep_ms(xms);
}

void DEV_Delay_us(uint32_t xus) {
    sleep_us(xus);
}


void DEV_GPIO_Init() {
    DEV_GPIO_Mode(LCD_RST_PIN, 1);
    DEV_GPIO_Mode(LCD_DC_PIN, 1);
    DEV_GPIO_Mode(LCD_CS_PIN, 1);
    DEV_GPIO_Mode(LCD_BL_PIN, 1);

    DEV_GPIO_Mode(LCD_CS_PIN, 1);
    DEV_GPIO_Mode(LCD_BL_PIN, 1);

    DEV_Digital_Write(LCD_CS_PIN, 1);
    DEV_Digital_Write(LCD_DC_PIN, 0);
    DEV_Digital_Write(LCD_BL_PIN, 1);
}

void DEV_Module_Init() {
    // SPI Config
    spi_init(SPI_PORT, 10000 * 1000);
    gpio_set_function(LCD_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI_PIN, GPIO_FUNC_SPI);

    // GPIO Config
    DEV_GPIO_Init();

    // PWM Config
    gpio_set_function(LCD_BL_PIN, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(LCD_BL_PIN);
    pwm_set_wrap(slice_num, 100);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 1);
    pwm_set_clkdiv(slice_num, 50);
    pwm_set_enabled(slice_num, true);
}

void DEV_SET_PWM(uint8_t Value) {
    if (Value < 0 || Value > 100) {
        printf("DEV_SET_PWM Error \r\n");
    } else {
        pwm_set_chan_level(slice_num, PWM_CHAN_B, Value);
    }
}