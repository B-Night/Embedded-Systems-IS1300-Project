/**
******************************************************************************
@brief Button driver
@file buttondriver.c
@author Bafoday Barrow
@version 1.0
@date 13-December-2025
@brief Functions for reading and handling button inputs on the traffic shield
******************************************************************************
*/
#include "buttondriver.h"
#include "gpio.h"
#include "main.h"

#include <stdint.h>
#include <stdbool.h>

/**
@brief Table mapping button IDs to GPIO ports
*/
static GPIO_TypeDef* Port[BUTTON_COUNT] = {
    [USER_BTN]   = USER_BTN_GPIO_Port,
	[TL1_Car] 	 = TL1_Car_GPIO_Port,
	[TL2_Car] 	 = TL2_Car_GPIO_Port,
	[TL3_Car] 	 = TL3_Car_GPIO_Port,
	[TL4_Car] 	 = TL4_Car_GPIO_Port,
    [PL1_Switch] = PL1_Switch_GPIO_Port,
    [PL2_Switch] = PL2_Switch_GPIO_Port
};

/**
@brief Table mapping button IDs to GPIO pins
*/
static uint16_t Pin[BUTTON_COUNT] = {
    [USER_BTN]   = USER_BTN_Pin,
	[TL1_Car] 	 = TL1_Car_Pin,
	[TL2_Car] 	 = TL2_Car_Pin,
	[TL3_Car] 	 = TL3_Car_Pin,
	[TL4_Car] 	 = TL4_Car_Pin,
    [PL1_Switch] = PL1_Switch_Pin,
    [PL2_Switch] = PL2_Switch_Pin
};

#define DEBOUNCE_MS 25
/**
@brief Button_Pressed, checks if a button is pressed with debounce
@param ButtonId id, button to check
@return bool, true if button is pressed and stable
*/
bool Button_Pressed(ButtonId id)
{
    // read initial value
    GPIO_PinState first = HAL_GPIO_ReadPin(Port[id], Pin[id]);

    // button pressed on traffic shield = GPIO_PIN_RESET
    bool firstPressed = (first == GPIO_PIN_RESET);

    // wait debounce time
    HAL_Delay(DEBOUNCE_MS);

    // read again
    GPIO_PinState second = HAL_GPIO_ReadPin(Port[id], Pin[id]);
    bool secondPressed = (second == GPIO_PIN_RESET);

    // return true only if stable AND pressed
    return (firstPressed && secondPressed);
}
/**
@brief Button_Scan, scans all buttons and reacts to presses
@param void
Used when testing button presses; specifically in test.c
@return void
*/
void Button_Scan(void)
{
    // Iterate through all possible button IDs
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        ButtonId id = (ButtonId)i;

        if (Button_Pressed(id))
        {
            switch (id)
            {
                case USER_BTN:
                    // USER_BTN press
                    break;
                case TL1_Car:
                	ShiftReg_Write(0xffffff);
					HAL_Delay(1000);
					ShiftReg_Write(0x000000);
					HAL_Delay(300);
                    break;
                case TL2_Car:
                	ShiftReg_Write(0xffffff);
					HAL_Delay(1000);
					ShiftReg_Write(0x000000);
					HAL_Delay(300);
                    break;
                case TL3_Car:
                	ShiftReg_Write(0xffffff);
					HAL_Delay(1000);
					ShiftReg_Write(0x000000);
					HAL_Delay(300);
                    break;
                case TL4_Car:
                	ShiftReg_Write(0xffffff);
					HAL_Delay(1000);
					ShiftReg_Write(0x000000);
					HAL_Delay(300);
                    break;
                case PL1_Switch:
                    // PL1_Switch press
                    ShiftReg_Write(0xffffff);
                    HAL_Delay(1000);
                    ShiftReg_Write(0x000000);
                    HAL_Delay(300);
                    break;
                case PL2_Switch:
                	ShiftReg_Write(0xffffff);
					HAL_Delay(1000);
					ShiftReg_Write(0x000000);
					HAL_Delay(300);
                    break;
                default:
                    //
                    break;
            }
        }
    }
}
