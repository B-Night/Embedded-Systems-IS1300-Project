/**
******************************************************************************
@brief Shift register driver
@file shiftreg.c
@author Bafoday Barrow
@version 1.0
@date 13-December-2025
@brief Functions  handling Shift Register 74HC595D, controlled using SPI
******************************************************************************
*/
#include "shiftreg.h"
#include "main.h"

extern SPI_HandleTypeDef hspi3;     // SPI3 is used

/**
@brief ShiftReg_Init, initializes the shift register
@param void
@return void
*/
void ShiftReg_Init(void)
{
    // Enable outputs (OE active-low)
    HAL_GPIO_WritePin(SR_OE_GPIO_Port, SR_OE_Pin, GPIO_PIN_RESET);

    // Take shift register out of reset
    HAL_GPIO_WritePin(SR_RESET_GPIO_Port, SR_RESET_Pin, GPIO_PIN_SET);

    // Clear all outputs
    ShiftReg_Write(0x000000);
}

/**
@brief ShiftReg_Write, writes data to shift register
@param uint32_t bits, 3 bytes of data sent to the shift register
Sending a digital 1 to certain bit positions turns on corresponding LED (if connected, check schematics for 74HC595D)
@return void
*/
void ShiftReg_Write(uint32_t bits)
{
    uint8_t data[3];

    data[0] =  bits        & 0xFF;        // low byte
    data[1] = (bits >> 8)  & 0xFF;        // mid byte
    data[2] = (bits >> 16) & 0xFF;        // high byte

    // Start latch low
    HAL_GPIO_WritePin(SR_LATCH_GPIO_Port, SR_LATCH_Pin, GPIO_PIN_RESET);

    // Transmit 3 bytes over SPI3
    HAL_SPI_Transmit(&hspi3, data, 3, HAL_MAX_DELAY);

    // Wait until the SPI peripheral is completely finished (BSY flag cleared)
    while(HAL_SPI_GetState(&hspi3) != HAL_SPI_STATE_READY);

    // Pulse latch
    HAL_GPIO_WritePin(SR_LATCH_GPIO_Port, SR_LATCH_Pin, GPIO_PIN_SET);
}
