/**
******************************************************************************
@brief LED driver
@file leddriver.h
@author Bafoday Barrow
@version 1.0
@date 13-December-2025
@brief Function declarations for LED control
******************************************************************************
*/
#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#include "main.h"


/**
@brief LED identifiers
*/
typedef enum {
    TL1_RED,
    TL1_YELLOW,
    TL1_GREEN,

    TL2_RED,
    TL2_YELLOW,
    TL2_GREEN,

    TL3_RED,
    TL3_YELLOW,
    TL3_GREEN,

    TL4_RED,
    TL4_YELLOW,
    TL4_GREEN,

    PL1_RED,
    PL1_GREEN,
    PL1_BLUE,

    PL2_RED,
    PL2_GREEN,
    PL2_BLUE,

    LED_COUNT
} LedId;

/**
@brief LED_Init, initializes LED driver
@param void
@return void
*/
void LED_Init(void);

/**
@brief LED_Set, sets LED state
@param id LED identifier
@param on true to turn LED on, false to turn LED off
@return void
*/
void LED_Set(LedId id, bool on);

/**
@brief LED_Update, updates LEDs via shift register
@param void
@return void
*/
void LED_Update(void);

/**
@brief LED brightness value used for dimming
*/
extern volatile uint8_t LED_Brightness;

#endif
