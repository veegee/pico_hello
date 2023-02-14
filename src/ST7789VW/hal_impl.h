#pragma once

#include <cstdio>
#include <cstdint>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"

#define LCD_RST_PIN  12
#define LCD_DC_PIN   8
#define LCD_BL_PIN   13

#define LCD_CS_PIN   9
#define LCD_CLK_PIN  6
#define LCD_MOSI_PIN 7


void DEV_Digital_Write(uint16_t Pin, uint8_t Value);

uint8_t DEV_Digital_Read(uint16_t Pin);

void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode);

void DEV_KEY_Config(uint16_t Pin);

void DEV_SPI_WriteByte(uint8_t Value);

void DEV_SPI_Write_nByte(uint8_t * pData, uint32_t Len);

void DEV_Delay_ms(uint32_t xms);

void DEV_Delay_us(uint32_t xus);


void DEV_SET_PWM(uint8_t Value);

void DEV_Module_Init();


