/**
******************************************************************************
@brief Led driver
@file leddriver.c
@author Bafoday Barrow
@version 1.0
@date 13-December-2025
@brief Functions making it easier to controll which LEDs to turn on and/or off
******************************************************************************
*/
#include "leddriver.h"
#include "shiftreg.h"

static uint32_t led_state;
volatile uint8_t LED_Brightness = 50;

/**
@brief LED to shift register bit mapping
Maps each LED identifier to its corresponding bit position
in the 74HC595 shift register chain. The bit position determines
which output pin controls each LED on the traffic light shield.
*/
static const uint8_t led_to_bit[LED_COUNT] = {
    [TL1_RED]    = 16,
    [TL1_YELLOW] = 17,
    [TL1_GREEN]  = 18,

    [TL2_RED]    = 8,
    [TL2_YELLOW] = 9,
    [TL2_GREEN]  = 10,

    [TL3_RED]    = 0,
    [TL3_YELLOW] = 1,
    [TL3_GREEN]  = 2,

    [TL4_RED]    = 3,
    [TL4_YELLOW] = 4,
    [TL4_GREEN]  = 5,

    [PL1_RED]    = 19,
    [PL1_GREEN]  = 20,
    [PL1_BLUE]   = 21,

    [PL2_RED]    = 11,
    [PL2_GREEN]  = 12,
    [PL2_BLUE]   = 13,
};

/**
@brief LED_Init, initializes the LEDs
@param void
@return void
*/
void LED_Init(void)
{
    led_state = 0;
    LED_Update();
}

/**
@brief LED_Set, changes the state of LED.
@param uint8_t a value from the struct containing all the possible LED, a bool statement that says if it should be on or off
Sending a name of the LED (from the schematics) you want to change and on/off will switch on or off said LED
@return void
*/
void LED_Set(LedId id, bool on)
{
    uint32_t mask = (1UL << led_to_bit[id]);

    if (on)
        led_state |= mask;
    else
        led_state &= ~mask;
}

/**
@brief LED_Update, update the state of the LEDs
@param void
Updates the state of the leds with while taking into account the brightness settings currently used.
Writes it to the shift register (using our function ShiftReg_Write).
@return void
*/
void LED_Update(void)
{
    static uint8_t pwmCounter = 0;
    pwmCounter++;

    if (pwmCounter >= 100) {  // 100-step PWM
        pwmCounter = 0;
    }
    uint32_t outputBits = 0;

    if (pwmCounter < LED_Brightness){   // LED_Brightness = 0–100
        outputBits = led_state;
    }
    ShiftReg_Write(outputBits);
}
