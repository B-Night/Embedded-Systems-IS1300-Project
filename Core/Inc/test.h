/**
******************************************************************************
@brief Test module
@file test.h
@author Bafoday Barrow
@version 1.0
@date 13-December-2025
@brief Function declarations for test programs
******************************************************************************
*/
#ifndef _TEST_H_
#define _TEST_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdbool.h>

/**
@brief Test_program, main test entry point
@param void
@return void
*/
void Test_program(void);

/**
@brief Test_shiftreg, tests shift register outputs
@param void
@return void
*/
void Test_shiftreg(void);

/**
@brief Test_leddriver, tests LED driver functionality
@param void
@return void
*/
void Test_leddriver(void);

/**
@brief Test_button, tests button inputs
@param void
@return void
*/
void Test_button(void);

/**
@brief Test_Traffic_Light_FSM, tests traffic light state machine
@param void
Tests the help functions that make up the logic in the traffic light state machine
@return void
*/
void Test_Traffic_Light_FSM(void);

#endif /* INC_TEST_H_ */
