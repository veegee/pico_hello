#include <cmath>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "SSD1306/SSD1306.h"
#include "ST7789VW/pico_graphics.hpp"
#include "ST7789VW/st7789.hpp"
#include "ST7789VW/hal_impl.h"
#include "ST7789VW/ST7789VW.h"


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
    static uint8_t const thickness_y = 6;

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

int main() {
//    set_sys_clock_khz(270'000, true);

    // initialize hardware for LED
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    uint8_t led_status = 0;

    // initialize hardware for I2C OLED display
    i2c_init(i2c1, 3'400'000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

//    SSD1306 disp(128, 64, 0x3C, i2c1);
//    disp.clear();
//    disp.show();

    // initialize hardware for SPI IPS LCD
//    DEV_Module_Init();
//    ST7789VW disp2(320, 240, HORIZONTAL, spi1);

    SPIPins pins{spi0, LCD_CS_PIN, LCD_CLK_PIN, LCD_MOSI_PIN, PIN_UNUSED, LCD_DC_PIN, PIN_UNUSED};
    ST7789 st7789(320, 240, ROTATE_0, false, pins);
    PicoGraphics_PenRGB565 graphics(st7789.width, st7789.height, nullptr);
    Pen bg_pen = graphics.create_pen(0, 0, 0);
    Pen pen = graphics.create_pen(255, 255, 255);
    graphics.set_pen(bg_pen);
    graphics.clear();
    st7789.update(&graphics);


    while (true) {
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

//        for (uint16_t offset = 0; offset < 128; offset++) {
//            led_status = ~led_status;
//            gpio_put(LED_PIN, led_status);
//
//            disp.clear();
//            draw_sin(disp, offset, 31);
//            disp.show();
//        }

        for (uint16_t offset = 0; offset < 300; offset++) {
            led_status = ~led_status;
            gpio_put(LED_PIN, led_status);

            graphics.set_pen(bg_pen);
            graphics.clear();
            graphics.set_pen(pen);
            draw_fast(graphics, offset);
            st7789.update(&graphics);
        }
    }
}

