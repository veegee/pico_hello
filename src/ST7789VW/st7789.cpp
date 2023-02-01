#include "st7789.hpp"

#include <cstdlib>
#include <cmath>
#include "hal_impl.h"

uint8_t madctl;
uint16_t caset[2] = {0, 0};
uint16_t raset[2] = {0, 0};

enum MADCTL : uint8_t {
    ROW_ORDER = 0b10000000,
    COL_ORDER = 0b01000000,
    SWAP_XY = 0b00100000,  // AKA "MV"
    SCAN_ORDER = 0b00010000,
    RGB_BGR = 0b00001000,
    HORIZ_ORDER = 0b00000100
};


void ST7789::common_init() {
    gpio_set_function(dc, GPIO_FUNC_SIO);
    gpio_set_dir(dc, GPIO_OUT);

    gpio_set_function(cs, GPIO_FUNC_SIO);
    gpio_set_dir(cs, GPIO_OUT);

    gpio_init(LCD_RST_PIN);
    gpio_set_dir(LCD_RST_PIN, GPIO_OUT);

    // if a backlight pin is provided then set it up for pwm control
    if (bl != PIN_UNUSED) {
        pwm_config cfg = pwm_get_default_config();
        pwm_set_wrap(pwm_gpio_to_slice_num(bl), 65535);
        pwm_init(pwm_gpio_to_slice_num(bl), &cfg, true);
        gpio_set_function(bl, GPIO_FUNC_PWM);
        set_backlight(0); // Turn backlight off initially to avoid nasty surprises
    }

    // reset display
//    gpio_put(LCD_RST_PIN, 1);
//    sleep_ms(100);
//    gpio_put(LCD_RST_PIN, 0);
//    sleep_ms(100);
//    gpio_put(LCD_RST_PIN, 1);
//    sleep_ms(100);

    command(reg::SWRESET);

    sleep_ms(100);

    // Common init
    command(reg::COLMOD, 1, "\x05");  // 16 bits per pixel

    command(reg::PORCTRL, 5, "\x0c\x0c\x00\x33\x33");
    command(reg::LCMCTRL, 1, "\x2c");
    command(reg::VDVVRHEN, 1, "\x01");
    command(reg::VRHS, 1, "\x12");
    command(reg::VDVS, 1, "\x20");
    command(reg::PWCTRL1, 2, "\xa4\xa1");
    command(reg::FRCTRL2, 1, "\x0f");

    if (width == 320 && height == 240) {
        command(reg::GCTRL, 1, "\x35");
        command(reg::VCOMS, 1, "\x1f");
//        command(0xd6, 1, "\xa1"); // ???
        command(reg::GMCTRP1, 14, "\xD0\x08\x11\x08\x0C\x15\x39\x33\x50\x36\x13\x14\x29\x2D");
        command(reg::GMCTRN1, 14, "\xD0\x08\x10\x08\x06\x06\x39\x44\x51\x0B\x16\x14\x2F\x31");
    }

    command(reg::INVON);   // set inversion mode
    command(reg::SLPOUT);  // leave sleep mode
    command(reg::DISPON);  // turn display on

    sleep_ms(100);

    configure_display(rotation);
}

void ST7789::cleanup() {
    if (dma_channel_is_claimed(st_dma)) {
        dma_channel_abort(st_dma);
        dma_channel_unclaim(st_dma);
    }
}

void ST7789::configure_display(Rotation rotate) {
    bool rotate180 = rotate == ROTATE_180 || rotate == ROTATE_90;

    if (rotate == ROTATE_90 || rotate == ROTATE_270) {
        std::swap(width, height);
    }

    // Pico Display 2.0
    if (width == 320 && height == 240) {
        caset[0] = 0;
        caset[1] = 319;
        raset[0] = 0;
        raset[1] = 239;
        madctl = rotate180 ? MADCTL::ROW_ORDER : MADCTL::COL_ORDER;
        madctl |= MADCTL::SWAP_XY | MADCTL::SCAN_ORDER;
    }

    // Pico Display 2.0 at 90 degree rotation
    if (width == 240 && height == 320) {
        caset[0] = 0;
        caset[1] = 239;
        raset[0] = 0;
        raset[1] = 319;
        madctl = rotate180 ? (MADCTL::COL_ORDER | MADCTL::ROW_ORDER) : 0;
    }

    // Byte swap the 16bit rows/cols values
    caset[0] = __builtin_bswap16(caset[0]);
    caset[1] = __builtin_bswap16(caset[1]);
    raset[0] = __builtin_bswap16(raset[0]);
    raset[1] = __builtin_bswap16(raset[1]);

    command(reg::CASET, 4, (char *) caset);
    command(reg::RASET, 4, (char *) raset);
    command(reg::MADCTL, 1, (char *) &madctl);
}

void ST7789::write_blocking_dma(uint8_t const * src, size_t len) const {
    while (dma_channel_is_busy(st_dma));
    dma_channel_set_trans_count(st_dma, len, false);
    dma_channel_set_read_addr(st_dma, src, true);
}

void ST7789::command(uint8_t command, size_t len, char const * data) {
    gpio_put(dc, 0); // command mode
    gpio_put(cs, 0);
    spi_write_blocking(spi, &command, 1);
    if (data) {
        gpio_put(dc, 1); // data mode
        spi_write_blocking(spi, (uint8_t const *) data, len);
    }
    gpio_put(cs, 1);
}

void ST7789::update(PicoGraphics * graphics) {
    uint8_t cmd = reg::RAMWR;

    if (graphics->pen_type == PicoGraphics::PEN_RGB565) {
        // display buffer is screen native
        command(cmd, width * height * sizeof(uint16_t), (char const *) graphics->frame_buffer);
    } else {
        gpio_put(dc, 0); // command mode
        gpio_put(cs, 0);
        spi_write_blocking(spi, &cmd, 1);

        gpio_put(dc, 1); // data mode

        graphics->frame_convert(PicoGraphics::PEN_RGB565, [this](void * data, size_t length) {
            if (length > 0) {
                write_blocking_dma((uint8_t const *) data, length);
            } else {
                dma_channel_wait_for_finish_blocking(st_dma);
            }
        });

        gpio_put(cs, 1);
    }
}

void ST7789::set_backlight(uint8_t brightness) {
    // gamma correct the provided 0-255 brightness value onto a
    // 0-65535 range for the pwm counter
    float gamma = 2.8;
    auto value = (uint16_t) (pow((float) (brightness) / 255.0f, gamma) * 65535.0f + 0.5f);
    pwm_set_gpio_level(bl, value);
}
