#include <cmath>
#include <cstdio>
#include <iostream>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "SSD1306/SSD1306.h"
#include "ST7789VW/pico_graphics.hpp"
#include "ST7789VW/st7789.hpp"
#include "ST7789VW/hal_impl.h"
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"

#include "main.hpp"


void draw_sin(SSD1306 & disp, uint8_t offset, uint8_t y_scale) {
    float const offset_rad = 4 * M_PI * (offset / 128.0);
    float const y_offset = 31 - y_scale;  // vertically center the graph

    static constexpr float TWOPI_3 = M_TWOPI / 3;
    static constexpr float FOURPI_3 = M_TWOPI * 2 / 3;

    for (uint8_t x = 0; x < 128; x++) {
        float const xr = 4 * M_PI * (x / 128.0);
        float const dy = round(y_scale * (sin(xr - offset_rad) + 1)) + y_offset;
        auto const y = static_cast<uint8_t>(dy);
        disp.draw_pixel(x, y);
    }

    for (uint8_t x = 0; x < 128; x++) {
        float const xr = 4 * M_PI * (x / 128.0);
        float const dy = round(y_scale * (sin(xr - TWOPI_3 - offset_rad) + 1)) + y_offset;
        auto const y = static_cast<uint8_t>(dy);
        disp.draw_pixel(x, y);
    }

    for (uint8_t x = 0; x < 128; x++) {
        float const xr = 4 * M_PI * (x / 128.0);
        float const dy = round(y_scale * (sin(xr - FOURPI_3 - offset_rad) + 1)) + y_offset;
        auto const y = static_cast<uint8_t>(dy);
        disp.draw_pixel(x, y);
    }
}

/**
 * Return a triangular graph similar to sin(x), but very fast to compute, using integer math only
 * @param x any integer where 0 <= x <= 99
 * @return value y where 0 <= y <= 240
 */
int fast_function(int x) {
    // sin(x) returns -1 to 1 where 0 <= x <= 2pi
    x = abs(x) % 100;
    if (x <= 50) {
        return (240 * x) / 50;
    } else {
        return (240 * (50 - x % 50)) / 50;
    }
}

void draw_fast(PicoGraphics & graphics, int offset) {
    static uint const width = 320;
    static uint const height = 240;

    static uint8_t const thickness_x = 1;
    static uint8_t const thickness_y = 6;

    for (uint x = 0; x < width; x++) {
        uint const y = fast_function(x - offset);
//        graphics.pixel(Point(x, y));
        graphics.rectangle(Rect(x, y, thickness_x, thickness_y));
    }

    for (uint x = 0; x < width; x++) {
        uint const y = fast_function(x - offset - 33);
//        graphics.pixel(Point(x, y));
        graphics.rectangle(Rect(x, y, thickness_x, thickness_y));
    }

    for (uint x = 0; x < width; x++) {
        uint const y = fast_function(x - offset - 66);
//        graphics.pixel(Point(x, y));
        graphics.rectangle(Rect(x, y, thickness_x, thickness_y));
    }

}

void draw_sin(PicoGraphics & graphics, uint16_t offset, uint16_t y_scale) {
    static float const width = 320;
    static uint16_t const height = 240;

    static constexpr float TWOPI_3 = M_TWOPI / 3;
    static constexpr float FOURPI_3 = M_TWOPI * 2 / 3;

    float const offset_rad = 4 * M_PI * (offset / width);
    float const y_offset = (height / 2.0f - 1) - y_scale;  // vertically center the graph

    static uint8_t const thickness_x = 1;
    static uint8_t const thickness_y = 4;

    for (uint16_t x = 0; x < width; x++) {
        float const xr = 4 * M_PI * (x / width);
        float const dy = round(y_scale * (sin(xr - offset_rad) + 1)) + y_offset;
        auto const y = static_cast<uint16_t>(dy);
//        graphics.pixel(Point(x, y));
        graphics.rectangle(Rect(x, y, thickness_x, thickness_y));
    }

    for (uint16_t x = 0; x < width; x++) {
        float const xr = 4 * M_PI * (x / width);
        float const dy = round(y_scale * (sin(xr - TWOPI_3 - offset_rad) + 1)) + y_offset;
        auto const y = static_cast<uint16_t>(dy);
//        graphics.pixel(Point(x, y));
        graphics.rectangle(Rect(x, y, thickness_x, thickness_y));
    }

    for (uint16_t x = 0; x < width; x++) {
        float const xr = 4 * M_PI * (x / width);
        float const dy = round(y_scale * (sin(xr - FOURPI_3 - offset_rad) + 1)) + y_offset;
        auto const y = static_cast<uint16_t>(dy);
//        graphics.pixel(Point(x, y));
        graphics.rectangle(Rect(x, y, thickness_x, thickness_y));
    }
}

void measure_freqs() {
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);

    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);
    printf("\n");

    // Can't measure clk_ref / xosc as it is the ref
}

int main() {
//    set_sys_clock_khz(270'000, true);  // 270MHz max

//    clock_configure(clk_peri,
//                    0,
//                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
//                    48 * MHZ,
//                    48 * MHZ);

//    stdio_init_all();
//    sleep_ms(3000);
//    measure_freqs();


    // initialize hardware for LED
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    uint8_t led_status = 0;

    // initialize hardware for I2C OLED display
    i2c_init(i2c0, 3'000'000);
    gpio_set_function(I2C_Pins::SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_Pins::SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_Pins::SDA);
    gpio_pull_up(I2C_Pins::SCL);

    // initialize hardware for SPI OLED display
    gpio_init(SPI_Pins::CLK);
    gpio_init(SPI_Pins::MOSI);
    gpio_init(SPI_Pins::CS);
    gpio_init(SPI_Pins::DC);
    gpio_init(SPI_Pins::RST);

//    gpio_set_dir(SPI_Pins::CS, GPIO_OUT);
    gpio_set_dir(SPI_Pins::DC, GPIO_OUT);
    gpio_set_dir(SPI_Pins::RST, GPIO_OUT);
    gpio_put(SPI_Pins::RST, 0);

    gpio_set_slew_rate(SPI_Pins::CLK, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(SPI_Pins::MOSI, GPIO_SLEW_RATE_SLOW);

    gpio_set_function(SPI_Pins::CLK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_Pins::MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SPI_Pins::CS, GPIO_FUNC_SPI);
    spi_init(spi0, 1'000'000);

//    spi_get_hw(spi0)->cpsr = 4;
//    hw_write_masked(&spi_get_hw(spi0)->cr0, 0, SPI_SSPCR0_SCR_BITS);

    SSD1306 disp(128, 64, 0x3C, i2c0);
    disp.clear();
    disp.show();

    // initialize hardware for SPI IPS LCD
//    SPIPins pins{spi0, LCD_CS_PIN, LCD_CLK_PIN, LCD_MOSI_PIN, PIN_UNUSED, LCD_DC_PIN, PIN_UNUSED};
//    ST7789 st7789(320, 240, ROTATE_0, false, pins);
//    PicoGraphics_PenRGB565 graphics(st7789.width, st7789.height, nullptr);
//    Pen bg_pen = graphics.create_pen(0, 0, 0);
//    Pen pen = graphics.create_pen(255, 255, 255);
//    graphics.set_pen(bg_pen);
//    graphics.clear();
//    st7789.update(&graphics);

    while (true) {
//        SSD1306 disp(128, 64, 0x3C, i2c0);
//        continue;

//        led_status = ~led_status;
//        gpio_put(LED_PIN, led_status);
//        disp.clear();
//        disp.draw_string(0, 8 * 0, 1, "ABCDEFGHIJKLMNOPQRSTU");
//        disp.draw_string(0, 8 * 1, 1, "VWXYZabcdefghijklmnop");
//        disp.draw_string(0, 8 * 2, 1, "qrstuvwxyzABCDEFGHIJK");
//        disp.draw_string(0, 8 * 3, 1, "012345678901234567890");
//        disp.draw_string(0, 8 * 4, 1, "ABCDEFGHIJKLMNOPQRSTU");
//        disp.draw_string(0, 8 * 5, 1, "VWXYZabcdefghijklmnop");
//        disp.draw_string(0, 8 * 6, 1, "qrstuvwxyzABCDEFGHIJK");
//        disp.draw_string(0, 8 * 7, 1, "012345678901234567890");
//        disp.show();
//        sleep_ms(1000);

        for (uint16_t offset = 0; offset < 128; offset++) {
            led_status = ~led_status;
            gpio_put(LED_PIN, led_status);

            disp.clear();
            draw_sin(disp, offset, 31);
            disp.show();

            sleep_us(100);
        }

//        for (uint16_t offset = 0; offset < 300; offset++) {
//            led_status = ~led_status;
//            gpio_put(LED_PIN, led_status);
//
//            graphics.set_pen(bg_pen);
//            graphics.clear();
//            graphics.set_pen(pen);
//            draw_fast(graphics, offset);
//            st7789.update(&graphics);
//        }

//        for (uint16_t offset = 0; offset < 320; offset++) {
//            led_status = ~led_status;
//            gpio_put(LED_PIN, led_status);
//
//            graphics.set_pen(bg_pen);
//            graphics.clear();
//            graphics.set_pen(pen);
//            draw_sin(graphics, offset, 110);
//            st7789.update(&graphics);
//        }

//        measure_freqs();
//        sleep_ms(3000);
    }
}

