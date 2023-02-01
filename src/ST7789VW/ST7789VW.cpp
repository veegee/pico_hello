#include <cstdlib>
#include <cstdio>

#include "st7789.hpp"
#include "ST7789VW.h"
#include "hal_impl.h"


void ST7789VW::reset() {
    DEV_Digital_Write(LCD_RST_PIN, 1);
    DEV_Delay_ms(100);
    DEV_Digital_Write(LCD_RST_PIN, 0);
    DEV_Delay_ms(100);
    DEV_Digital_Write(LCD_RST_PIN, 1);
    DEV_Delay_ms(100);
}

void ST7789VW::command(uint8_t command, size_t len, char const * data) {
    gpio_put(LCD_DC_PIN, 0);
    gpio_put(LCD_CS_PIN, 0);
    spi_write_blocking(spi1, &command, 1);
    if (data) {
        gpio_put(LCD_DC_PIN, 1);
        spi_write_blocking(spi1, (uint8_t const *) data, len);
    }
    gpio_put(LCD_CS_PIN, 1);
}

void ST7789VW::send_command(uint8_t data) {
    DEV_Digital_Write(LCD_DC_PIN, 0);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    DEV_SPI_WriteByte(data);
    DEV_Digital_Write(LCD_CS_PIN, 1);
}

void ST7789VW::send_data_8(uint8_t data) {
    DEV_Digital_Write(LCD_DC_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    DEV_SPI_WriteByte(data);
    DEV_Digital_Write(LCD_CS_PIN, 1);
}

void ST7789VW::send_data_16(uint16_t data) {
    DEV_Digital_Write(LCD_DC_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    DEV_SPI_WriteByte((data >> 8) & 0xFF);
    DEV_SPI_WriteByte(data & 0xFF);
    DEV_Digital_Write(LCD_CS_PIN, 1);
}

void ST7789VW::init_regs() {
    send_command(reg::MADCTL);
    send_data_8(0x00);

    command(reg::COLMOD, 1, "\x05");  // 16 bits per pixel

    send_command(reg::INVON);

    send_command(reg::CASET);
    send_data_8(0x00);
    send_data_8(0x00);
    send_data_8(0x01);
    send_data_8(0x3F);

    send_command(reg::RASET);
    send_data_8(0x00);
    send_data_8(0x00);
    send_data_8(0x00);
    send_data_8(0xEF);

    command(reg::PORCTRL, 5, "\x0c\x0c\x00\x33\x33");
    command(reg::GCTRL, 1, "\x35");
    command(reg::VCOMS, 1, "\x1f");
    command(reg::LCMCTRL, 1, "\x2c");
    command(reg::VDVVRHEN, 1, "\x01");
    command(reg::VRHS, 1, "\x12");
    command(reg::VDVS, 1, "\x20");
    command(reg::FRCTRL2, 1, "\x0f");
    command(reg::PWCTRL1, 2, "\xa4\xa1");
    command(reg::GMCTRP1, 14, "\xD0\x08\x11\x08\x0C\x15\x39\x33\x50\x36\x13\x14\x29\x2D");
    command(reg::GMCTRN1, 14, "\xD0\x08\x10\x08\x06\x06\x39\x44\x51\x0B\x16\x14\x2F\x31");
    command(reg::INVON);   // set inversion mode
    command(reg::SLPOUT);  // leave sleep mode
    command(reg::DISPON);  // turn display on
}

/********************************************************************************
function:	Set the resolution and scanning method of the screen
parameter:
		Scan_dir:   Scan direction
********************************************************************************/
void ST7789VW::set_attributes(uint8_t scan_dir) {
    // get the screen scan direction
    //LCD_2IN.SCAN_DIR = scan_dir;
    uint8_t MemoryAccessReg = 0x00;

    // get GRAM and LCD width and height
    if (scan_dir == HORIZONTAL) {
        //LCD_2IN.HEIGHT = width;
        //LCD_2IN.WIDTH = height;
        MemoryAccessReg = 0X70;
    } else {
        //LCD_2IN.HEIGHT = height;
        //LCD_2IN.WIDTH = width;
        MemoryAccessReg = 0X00;
    }

    // Set the read / write scan direction of the frame memory
    send_command(0x36); //MX, MY, RGB mode
    send_data_8(MemoryAccessReg);    //0x08 set RGB
}

ST7789VW::ST7789VW(uint16_t const width, uint16_t const height, uint8_t const scan_dir, spi_inst_t * const spi_inst)
        : width(width), height(height), scan_dir(scan_dir), spi_inst(spi_inst) {
    // hardware reset
    reset();

    // set the resolution and scanning method of the screen
    set_attributes(scan_dir);

    // set the initialization register
    init_regs();
}

ST7789VW::~ST7789VW() {

}

/********************************************************************************
function:	Sets the start position and size of the display area
parameter:
		Xstart 	:   X direction Start coordinates
		Ystart  :   Y direction Start coordinates
		Xend    :   X direction end coordinates
		Yend    :   Y direction end coordinates
********************************************************************************/
void ST7789VW::set_windows(uint16_t const x_start, uint16_t const y_start, uint16_t const x_end, uint16_t const y_end) {
    //set the X coordinates
    send_command(0x2A);
    send_data_8(x_start >> 8);
    send_data_8(x_start & 0xff);
    send_data_8((x_end - 1) >> 8);
    send_data_8((x_end - 1) & 0xFF);

    //set the Y coordinates
    send_command(0x2B);
    send_data_8(y_start >> 8);
    send_data_8(y_start & 0xff);
    send_data_8((y_end - 1) >> 8);
    send_data_8((y_end - 1) & 0xff);

    send_command(0X2C);
    // printf("%d %d\r\n",x,y);
}

void ST7789VW::clear(uint16_t const color) {
    uint16_t i;
    uint16_t image[height];
    for (i = 0; i < height; i++) {
        image[i] = color >> 8 | (color & 0xff) << 8;
    }
    auto * p = (uint8_t *) (image);
    set_windows(0, 0, height, width);
    DEV_Digital_Write(LCD_DC_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    for (i = 0; i < width; i++) {
        DEV_SPI_Write_nByte(p, height * 2);
    }
    DEV_Digital_Write(LCD_CS_PIN, 1);
}

/******************************************************************************
function :	Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void ST7789VW::display(uint8_t const * const image) {
    uint16_t j;
    set_windows(0, 0, width, height);
    DEV_Digital_Write(LCD_DC_PIN, 1);
    DEV_Digital_Write(LCD_CS_PIN, 0);
    for (j = 0; j < width; j++) {
        DEV_SPI_Write_nByte((uint8_t *) image + height * 2 * j, height * 2);
    }
    DEV_Digital_Write(LCD_CS_PIN, 1);
    send_command(0x29);
}
