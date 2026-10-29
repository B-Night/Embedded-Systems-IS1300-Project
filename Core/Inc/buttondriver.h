/**
******************************************************************************
@brief Button driver
@file buttondriver.h
@author Bafoday Barrow
@version 1.0
@date 13-December-2025
@brief Function declarations for button handling
******************************************************************************
*/
#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdbool.h>

/**
@brief Button identifiers
*/
typedef enum {
    USER_BTN,
	TL1_Car,
	TL2_Car,
	TL3_Car,
	TL4_Car,
    PL1_Switch,
    PL2_Switch,
    BUTTON_COUNT
} ButtonId;

/**
@brief Button_Init, initializes button GPIOs
@param void
@return void
*/
void Button_Init(void);

/**
@brief Button_Scan, scans all buttons
@param void
@return void
*/
void Button_Scan(void);

/**
@brief Button_Pressed, reads button state
@param id button identifier
@return true if button is pressed, false otherwise
*/
bool Button_Pressed(ButtonId id);

#endif
