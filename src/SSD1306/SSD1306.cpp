#include "SSD1306.h"
#include <hardware/spi.h>
#include "../ST7789VW/hal_impl.h"

SSD1306::SSD1306(uint8_t const width, uint8_t const height, uint8_t const address, i2c_inst_t * const i2c_instance)
        : width(width), height(height), address(address), i2c_i(i2c_instance) {
    pages = height / 8;
    bufsize = pages * width;
    external_vcc = false;

    if ((buffer = static_cast<uint8_t *>(malloc(bufsize + 1))) == nullptr) {
        bufsize = 0;
        // error
    }

    buffer++;

    gpio_put(SPI_Pins::RST, 1);
    sleep_ms(100);
//    gpio_put(SPI_Pins::RST, 0);
//    sleep_ms(100);
//    gpio_put(SPI_Pins::RST, 1);
//    sleep_ms(100);

    std::vector<uint8_t> cmds = {
            SET_DISP,
            // timing and driving scheme
            SET_DISP_CLK_DIV,
            0xF0,  // default: 0x80
            SET_MUX_RATIO,
            static_cast<unsigned char>(height - 1),
            SET_DISP_OFFSET,
            0x00,
            // resolution and layout
            SET_DISP_START_LINE,
            // charge pump
            SET_CHARGE_PUMP,
            static_cast<unsigned char>(external_vcc ? 0x10 : 0x14),
            SET_SEG_REMAP | 0x01,           // column addr 127 mapped to SEG0
            SET_COM_OUT_DIR | 0x08,         // scan from COM[N] to COM0
            SET_COM_PIN_CFG,
            static_cast<unsigned char>(width > 2 * height ? 0x02 : 0x12),
            // display
            SET_CONTRAST,
            0xff,
            SET_PRECHARGE,
            //static_cast<unsigned char>(external_vcc ? 0x22 : 0xF1),
            static_cast<unsigned char>(external_vcc ? 0x22 : 0xF1),
            SET_VCOM_DESEL,
            0x30,                           // or 0x40?
            SET_ENTIRE_ON,                  // output follows RAM contents
            SET_NORM_INV,                   // not inverted
            SET_DISP | 0x01,
            // address setting
            SET_MEM_ADDR,
            0x00  // horizontal
    };

    for (uint8_t cmd: cmds) {
        write_command(cmd);
    }

}

SSD1306::~SSD1306() {
    free(buffer - 1);
}

void SSD1306::write(uint8_t addr, const uint8_t * src, size_t len) {
    spi_write_blocking(spi0, src, len);
    i2c_write_blocking(i2c_i, addr, src, len, false);
}

void SSD1306::write_command(uint8_t val) {
    gpio_put(SPI_Pins::DC, 0);
    uint8_t d[2] = {0x00, val};
    write(address, d, 2);
    gpio_put(SPI_Pins::DC, 1);
}

void SSD1306::invert(uint8_t inv) {
    write_command(SET_NORM_INV | (inv & 1));
}

void SSD1306::clear() {
    memset(buffer, 0, bufsize);
}

void SSD1306::draw_pixel(uint32_t x, uint32_t y) {
    if (x >= width || y >= height) return;

    buffer[x + width * (y >> 3)] |= 0x1 << (y & 0x07); // y>>3==y/8 && y&0x7==y%8
}

void SSD1306::draw_char_with_font(const uint32_t x, const uint32_t y,
                                  const uint32_t scale, const uint8_t * const font, const char c) {
    if (c < font[3] || c > font[4])
        return;

    const uint8_t height = font[0];
    const uint8_t width = font[1];
    const uint8_t index = (c - font[3]);  // first column of the character data

    for (uint8_t i = 0; i < width; i++) {
        const uint8_t col = font[index * width + i + 5];

        for (uint8_t j = 0; j < height; j++) {
            if (col & (1 << j)) {
                //draw_square(p, x + i * scale, y + j * scale, scale, scale);
                draw_pixel(x + i, y + j);
            }
        }
    }
}

void SSD1306::draw_string_with_font(uint32_t x, uint32_t y, uint32_t scale, const uint8_t * font, const char * s) {
    for (int32_t x_n = x; *s; x_n += (font[1] + font[2]) * scale) {
        draw_char_with_font(x_n, y, scale, font, *(s++));
    }
}

void SSD1306::draw_char(uint32_t x, uint32_t y, uint32_t scale, char c) {
    draw_char_with_font(x, y, scale, font_8x5, c);
}

void SSD1306::draw_string(uint32_t x, uint32_t y, uint32_t scale, const char * s) {
    draw_string_with_font(x, y, scale, font_8x5, s);
}

void SSD1306::show() {
    std::vector<uint8_t> payload = {SET_COL_ADDR, 0, static_cast<uint8_t>(width - 1), SET_PAGE_ADDR, 0,
                                    static_cast<uint8_t>(pages - 1)};

    if (width == 64) {
        payload[1] += 32;
        payload[2] += 32;
    }

    for (const uint8_t data: payload) {
        write_command(data);
    }

    *(buffer - 1) = 0x40;

    write(address, buffer - 1, bufsize + 1);
}
