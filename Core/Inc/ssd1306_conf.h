#ifndef __SSD1306_CONF_H__
#define __SSD1306_CONF_H__
#include "main.h"
// MCU
#define STM32L4

// SPI
#define SSD1306_USE_SPI

// SPI2
#define SSD1306_SPI_PORT hspi2

// OLED Pins
#define SSD1306_CS_Port        Disp_CS_GPIO_Port
#define SSD1306_CS_Pin         Disp_CS_Pin

#define SSD1306_DC_Port        Disp_Data_GPIO_Port
#define SSD1306_DC_Pin         Disp_Data_Pin

#define SSD1306_Reset_Port     Disp_Reset_GPIO_Port
#define SSD1306_Reset_Pin      Disp_Reset_Pin

// Fonts
#define SSD1306_INCLUDE_FONT_6x8
#define SSD1306_INCLUDE_FONT_7x10
#define SSD1306_INCLUDE_FONT_11x18
#define SSD1306_INCLUDE_FONT_16x26

#endif
