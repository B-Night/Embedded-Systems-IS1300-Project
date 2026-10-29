/*
 * oled.h
 *
 *  Created on: Dec 11, 2025
 *      Author: Hem
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_

// oled.h
#pragma once
#include "main.h"

// external timing variables from your traffic controller
extern volatile uint32_t remaining_ped1;
extern volatile uint32_t remaining_walk1;
extern volatile uint32_t remaining_ped2;
extern volatile uint32_t remaining_walk2;
extern volatile uint32_t remaining_green1;
extern volatile uint32_t remaining_red1;
extern volatile uint32_t remaining_green2;
extern volatile uint32_t remaining_red2;

void Oled_Init(void);
void Oled_Periodic(void); // call periodically




#endif /* INC_OLED_H_ */
