#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <hardware/i2c.h>

#include "../font.h"

typedef enum {
    SET_CONTRAST = 0x81,
    SET_ENTIRE_ON = 0xA4,
    SET_NORM_INV = 0xA6,
    SET_DISP = 0xAE,
    SET_MEM_ADDR = 0x20,
    SET_COL_ADDR = 0x21,
    SET_PAGE_ADDR = 0x22,
    SET_DISP_START_LINE = 0x40,
    SET_SEG_REMAP = 0xA0,
    SET_MUX_RATIO = 0xA8,
    SET_COM_OUT_DIR = 0xC0,
    SET_DISP_OFFSET = 0xD3,
    SET_COM_PIN_CFG = 0xDA,
    SET_DISP_CLK_DIV = 0xD5,
    SET_PRECHARGE = 0xD9,
    SET_VCOM_DESEL = 0xDB,
    SET_CHARGE_PUMP = 0x8D
} ssd1306_command_t;

class SSD1306 {
    i2c_inst_t * i2c_i;
    uint8_t * buffer;  // display buffer
    size_t bufsize;  // buffer size

public:
    bool external_vcc;  // whether display uses external vcc
    uint8_t width;
    uint8_t height;
    uint8_t pages;  // stores pages of display (calculated on initialization
    uint8_t address;  // i2c address of display

    SSD1306(uint8_t width, uint8_t height, uint8_t address, i2c_inst_t * i2c_instance);

    ~SSD1306();

    void fancy_write(uint8_t addr, const uint8_t * src, size_t len, std::string name);

    void write(uint8_t val);

    void swap(int32_t * a, int32_t * b);

    void poweroff();

    void poweron();

    void contrast(uint8_t val);

    void invert(uint8_t inv);

    void clear();

    void draw_pixel(uint32_t x, uint32_t y);

    void draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

    void draw_square(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    void draw_empty_square(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    void draw_char_with_font(uint32_t x, uint32_t y, uint32_t scale, const uint8_t * font, char c);

    void draw_string_with_font(uint32_t x, uint32_t y, uint32_t scale, const uint8_t * font, const char * s);

    void draw_char(uint32_t x, uint32_t y, uint32_t scale, char c);

    void draw_string(uint32_t x, uint32_t y, uint32_t scale, const char * s);

    void show();
};

