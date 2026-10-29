/**
******************************************************************************
@brief Traffic light finite state machine
@file traffic_light.h
@author Bafoday Barrow
@version 1.0
@date 13-December-2025
@brief Function declarations for traffic light and pedestrian control logic
******************************************************************************
*/

#ifndef TRAFFIC_LIGHT_H_
#define TRAFFIC_LIGHT_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stdint.h>
#include <stdbool.h>

/**
@brief Main traffic light task
@param void
Implements the traffic light and pedestrian FSM.
Called from the RTOS scheduler or main loop.
*/
void traffic_light_tasks(void) ;

/**
@brief Check if there are active cars in the vertical lane
@param void
@return true if at least one vertical car button is pressed
*/
void vertical_has_cars(void);

/**
@brief Check if there are active cars in the horizontal lane
@param void
@return true if at least one horizontal car button is pressed
*/
void horizontal_has_cars(void);

/**
@brief Clear all car traffic light LEDs
@param void
Turns off red, yellow, and green for all car signals
*/
void clear_all_cars(void);

/**
@brief Clear all pedestrian LEDs
@param void
*/
void clear_pedestrian(void);

/**
@brief Set vertical lane to green and horizontal lane to red
@param void
*/
void set_vertical_green(void);

/**
@brief Set horizontal lane to green and vertical lane to red
@param void
*/
void set_horizontal_green(void);

/**
@brief Set all car traffic lights to yellow
@param void
 Used during transitions between directions
*/
void set_all_orange(void);

/**
@brief Set pedestrian signal to red
@param void
*/
void set_ped_red(void);

/**
@brief Set pedestrian signal to green
@param void
*/
void set_ped_green(void);

#endif
