#pragma once

#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pimoroni_common.hpp"
#include "pimoroni_bus.hpp"
#include "pico_graphics.hpp"

#include <algorithm>

enum reg {
    SWRESET = 0x01,
    TEOFF = 0x34,
    TEON = 0x35,
    MADCTL = 0x36,
    COLMOD = 0x3A,
    GCTRL = 0xB7,
    VCOMS = 0xBB,
    LCMCTRL = 0xC0,
    VDVVRHEN = 0xC2,
    VRHS = 0xC3,
    VDVS = 0xC4,
    FRCTRL2 = 0xC6,
    PWCTRL1 = 0xD0,
    PORCTRL = 0xB2,
    GMCTRP1 = 0xE0,
    GMCTRN1 = 0xE1,
    INVOFF = 0x20,
    SLPOUT = 0x11,
    DISPON = 0x29,
    GAMSET = 0x26,
    DISPOFF = 0x28,
    RAMWR = 0x2C,
    INVON = 0x21,
    CASET = 0x2A,
    RASET = 0x2B,
    PWMFRSEL = 0xCC
};

class ST7789 : public DisplayDriver {
    spi_inst_t * spi = spi1;

public:
    bool round;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
private:

    // interface pins with our standard defaults where appropriate
    uint const cs;        // chip select
    uint const dc;        // data/command
    uint const wr_sck;    // clock
    uint const d0;        // MOSI
    uint const bl;        // backlight
    int st_dma;


    // The ST7789 requires 16 ns between SPI rising edges.
    // 16ns = 62,500,000Hz
    static uint32_t const SPI_BAUD = 250'000'000 / 4;


public:
    ST7789(uint16_t width, uint16_t height, Rotation rotation, bool round, SPIPins pins) :
            DisplayDriver(width, height, rotation),
            spi(pins.spi), round(round),
            cs(pins.cs), dc(pins.dc), wr_sck(pins.sck), d0(pins.mosi), bl(pins.bl) {

        // configure SPI interface and pins
        uint speed = spi_init(spi, SPI_BAUD);
        spi_get_hw(spi)->cpsr = 4;
        hw_write_masked(&spi_get_hw(spi)->cr0, 0, SPI_SSPCR0_SCR_BITS);

        gpio_set_function(wr_sck, GPIO_FUNC_SPI);
        gpio_set_function(d0, GPIO_FUNC_SPI);

        // configure DMA
        st_dma = dma_claim_unused_channel(true);
        dma_channel_config config = dma_channel_get_default_config(st_dma);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
        channel_config_set_bswap(&config, false);
        channel_config_set_dreq(&config, spi_get_dreq(spi, true));
        dma_channel_configure(st_dma, &config, &spi_get_hw(spi)->dr, nullptr, 0, false);

        common_init();
    }

    void cleanup() override;

    void update(PicoGraphics * graphics) override;

    void set_backlight(uint8_t brightness) override;

private:
    void common_init();

    void configure_display(Rotation rotate);

    void write_blocking_dma(uint8_t const * src, size_t len) const;

    void command(uint8_t command, size_t len = 0, char const * data = nullptr);
};
