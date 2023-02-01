#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstdio>

#include <hardware/spi.h>

#include "hal_impl.h"

#define HORIZONTAL 0
#define VERTICAL   1


class ST7789VW {
private:
    uint16_t const width;
    uint16_t const height;
    uint8_t const scan_dir;
    spi_inst_t * const spi_inst;

    void init_regs();
    void set_attributes(uint8_t scan_dir);

public:
    ST7789VW(uint16_t width, uint16_t height, uint8_t scan_dir, spi_inst_t * spi_inst);

    ~ST7789VW();

    void reset();

    void send_command(uint8_t data);

    void send_data_8(uint8_t data);

    void send_data_16(uint16_t data);

    void set_windows(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

    void clear(uint16_t color);

    void display(uint8_t const * image);
};
