/**
******************************************************************************
@brief Shift register driver
@file shiftreg.h
@author Bafoday Barrow
@version 1.0
@date 13-December-2025
@brief Function declarations for Shift Register 74HC595D
******************************************************************************
*/
#ifndef SHIFTREG_H
#define SHIFTREG_H

#include <stdint.h>

/**
@brief ShiftReg_Init, initializes the shift register
Enables output, releases reset and clears all outputs
@return void
*/
void ShiftReg_Init(void);
/**
@brief ShiftReg_Write, writes data to shift register
@param bits 24-bit value written to the shift register
@return void
*/
void ShiftReg_Write(uint32_t bits);

#endif
