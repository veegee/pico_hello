#pragma once

#include "pimoroni_common.hpp"
#include "hardware/gpio.h"
#include "hardware/spi.h"

struct SPIPins {
    spi_inst_t * spi;
    uint cs;
    uint sck;
    uint mosi;
    uint miso;
    uint dc;
    uint bl;
};