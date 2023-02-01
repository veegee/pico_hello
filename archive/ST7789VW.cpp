#include <cstdlib>
#include <cstdio>

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
    send_command(0x36);
    send_data_8(0x00);

    send_command(0x3A);
    send_data_8(0x05);

    send_command(0x21);

    send_command(0x2A);
    send_data_8(0x00);
    send_data_8(0x00);
    send_data_8(0x01);
    send_data_8(0x3F);

    send_command(0x2B);
    send_data_8(0x00);
    send_data_8(0x00);
    send_data_8(0x00);
    send_data_8(0xEF);

    send_command(0xB2);
    send_data_8(0x0C);
    send_data_8(0x0C);
    send_data_8(0x00);
    send_data_8(0x33);
    send_data_8(0x33);

    send_command(0xB7);
    send_data_8(0x35);

    send_command(0xBB);
    send_data_8(0x1F);

    send_command(0xC0);
    send_data_8(0x2C);

    send_command(0xC2);
    send_data_8(0x01);

    send_command(0xC3);
    send_data_8(0x12);

    send_command(0xC4);
    send_data_8(0x20);

    send_command(0xC6);
    send_data_8(0x0F);

    send_command(0xD0);
    send_data_8(0xA4);
    send_data_8(0xA1);

    send_command(0xE0);
    send_data_8(0xD0);
    send_data_8(0x08);
    send_data_8(0x11);
    send_data_8(0x08);
    send_data_8(0x0C);
    send_data_8(0x15);
    send_data_8(0x39);
    send_data_8(0x33);
    send_data_8(0x50);
    send_data_8(0x36);
    send_data_8(0x13);
    send_data_8(0x14);
    send_data_8(0x29);
    send_data_8(0x2D);

    send_command(0xE1);
    send_data_8(0xD0);
    send_data_8(0x08);
    send_data_8(0x10);
    send_data_8(0x08);
    send_data_8(0x06);
    send_data_8(0x06);
    send_data_8(0x39);
    send_data_8(0x44);
    send_data_8(0x51);
    send_data_8(0x0B);
    send_data_8(0x16);
    send_data_8(0x14);
    send_data_8(0x2F);
    send_data_8(0x31);
    send_command(0x21);

    send_command(0x11);

    send_command(0x29);
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
    DEV_SET_PWM(90);  // backlight brightness

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
