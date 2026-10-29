/**
******************************************************************************
@brief Traffic Light Application
@file traffic_light.c
@author Bafoday Barrow
@version 1.0
@date 13-December-2025
@brief Finite State Machine controlling traffic lights and pedestrian signals
******************************************************************************
*/

#include "leddriver.h"
#include "buttondriver.h"
#include "shiftreg.h"
#include "main.h"
#include "ssd1306.h"
#include "oled.h"
#include "adc.h"
#include "ssd1306_fonts.h"
#include <stdint.h>
#include <stdbool.h>

/**
@brief DelayWithPWM, non-blocking delay allowing PWM interrupt to run
@param uint32_t ms, delay time in milliseconds
@return void
*/
void DelayWithPWM(uint32_t ms)
{
    uint32_t start = HAL_GetTick();
    while (HAL_GetTick() - start < ms)
    {
        // Allow TIM6 ISR to run
    }
}



/**
@brief FSM states for traffic light control
*/
typedef enum {
    STATE_START,               // Initialize
    STATE_VERTICAL_GREEN,	   // Vertical lane is active
    STATE_HORIZONTAL_GREEN,    // Horizontal lane is active
    STATE_PENDING_REQUEST,     // pedestrian waiting (blinking indicator)
    STATE_ORANGE_TO_ALL_RED,   // active lane turning orange -> then all-red
    STATE_PEDESTRIAN_GREEN     // Cross walk 2 and horizontal lane are active
} State;

static State state = STATE_START;

// timers
static uint32_t requestTime = 0;
static uint32_t lastToggle  = 0;
static uint32_t stateStart  = 0;
static uint32_t bothLanesStartTime = 0;


// flags
static bool indicatorBlink = false;
// true when the lane currently green (before a pending request) is vertical (TL2/TL4)
static bool currentGreenIsVertical = true;
static bool bothLanesTimerRunning = false;

// constants (tune if needed)
const uint32_t walkingDelay     = 3000;
const uint32_t orangeDelay      = 2000;
const uint32_t pedestrianDelay  = 3000;
const uint16_t toggleFreq       = 300;
const uint32_t greenDelay       = 3000;
const uint32_t redDelayMax         = 3000;

// variable
static uint32_t lastAdcRead = 0;

// Help functions

/**
@brief Checks if vertical lanes have cars
@param void
@return bool, true if car present
*/
bool vertical_has_cars(void){
    return Button_Pressed(TL2_Car) || Button_Pressed(TL4_Car);
}

/**
@brief Checks if horizontal lanes have cars
@param void
@return bool, true if car present
*/
bool horizontal_has_cars(void){
    return Button_Pressed(TL1_Car) || Button_Pressed(TL3_Car);
}

/**
@brief Clears all car LEDs
@param void
@return void
*/
void clear_all_cars(void)
{
    LED_Set(TL1_RED, false);   LED_Set(TL1_YELLOW, false); LED_Set(TL1_GREEN, false);
    LED_Set(TL2_RED, false);   LED_Set(TL2_YELLOW, false); LED_Set(TL2_GREEN, false);
    LED_Set(TL3_RED, false);   LED_Set(TL3_YELLOW, false); LED_Set(TL3_GREEN, false);
    LED_Set(TL4_RED, false);   LED_Set(TL4_YELLOW, false); LED_Set(TL4_GREEN, false);
}

/**
@brief Clears all pedestrian LEDs
@param void
@return void
*/
void clear_pedestrian(void)
{
    LED_Set(PL2_RED, false);
    LED_Set(PL2_GREEN, false);
    LED_Set(PL2_BLUE, false);
}

/**
@brief Sets vertical green and horizontal red
@param void
@return void
*/
void set_vertical_green(void){
    clear_all_cars();

    // vertical green
    LED_Set(TL2_GREEN, true);
    LED_Set(TL4_GREEN, true);

    // vertical not red or yellow (ensured by clear_all_cars)
    // horizontal red
    LED_Set(TL1_RED, true);
    LED_Set(TL3_RED, true);

}

/**
@brief Sets horizontal green and vertical red
@param void
@return void
*/
void set_horizontal_green(void){
    clear_all_cars();

    // horizontal green
    LED_Set(TL1_GREEN, true);
    LED_Set(TL3_GREEN, true);

    // vertical red
    LED_Set(TL2_RED, true);
    LED_Set(TL4_RED, true);

}

/**
@brief Sets all traffic lights to orange
@param void
@return void
*/
void set_all_orange(void){
    LED_Set(TL1_GREEN, false);
    LED_Set(TL2_GREEN, false);
    LED_Set(TL3_GREEN, false);
    LED_Set(TL4_GREEN, false);

    LED_Set(TL1_RED, false);
    LED_Set(TL2_RED, false);
    LED_Set(TL3_RED, false);
    LED_Set(TL4_RED, false);

    LED_Set(TL1_YELLOW, true);
    LED_Set(TL2_YELLOW, true);
    LED_Set(TL3_YELLOW, true);
    LED_Set(TL4_YELLOW, true);


}

/**
@brief Sets pedestrian signal to red
@param void
@return void
*/
void set_ped_red(void){
    clear_pedestrian();
    LED_Set(PL2_RED, true);
}

/**
@brief Sets pedestrian signal to green
@param void
@return void
*/
void set_ped_green(void){
    clear_pedestrian();
    LED_Set(PL2_GREEN, true);
}

/**
@brief traffic_light_tasks, main FSM loop for traffic control
@param void
@return void
*/
void traffic_light_tasks(void){

	// OLED Initialization
	Oled_Init();
	uint32_t lastOledUpdate = 0;

	HAL_GPIO_WritePin(SR_OE_GPIO_Port, SR_OE_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(SR_OE_GPIO_Port, SR_OE_Pin, GPIO_PIN_RESET);
	HAL_Delay(1000);

    while (1){
        uint32_t now = HAL_GetTick();

        static uint32_t lastAdc = 0;

        if (HAL_GetTick() - lastAdc >= 50)
        {
            lastAdc = HAL_GetTick();

            uint16_t adc = ADC_Read();
            LED_Brightness = (adc * 100) / 4095;

            // Debug: LD2 on when bright
            HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,(LED_Brightness > 50) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }


        // RESET ALL
        remaining_green1 = remaining_green2 = 0;
        remaining_red1   = remaining_red2   = 0;
        remaining_walk2  = remaining_ped2   = 0;

        bool v = vertical_has_cars();
        bool h = horizontal_has_cars();

        // Vertical green state
        if (state == STATE_VERTICAL_GREEN)
        {
            remaining_green2 = greenDelay - (now - stateStart);

            // Horizontal red (waiting)
            if (h)
            {
                if (v && h && bothLanesTimerRunning)
                {
                    // Both lanes have cars -> use redDelayMax countdown
                    remaining_red1 = redDelayMax - (now - bothLanesStartTime);
                }
                else
                {
                    // Only horizontal has cars -> idle green timeout
                    remaining_red1 = greenDelay - (now - stateStart);
                }

                if (remaining_red1 < 0) remaining_red1 = 0;
            }
        }

        // Horizontal green state
        else if (state == STATE_HORIZONTAL_GREEN)
        {
            remaining_green1 = greenDelay - (now - stateStart);

            // Vertical red (waiting)
            if (v)
            {
                if (v && h && bothLanesTimerRunning)
                {
                    // Both lanes have cars -> redDelayMax for vertical
                    remaining_red2 = redDelayMax - (now - bothLanesStartTime);
                }
                else
                {
                    // Only vertical has cars -> idle timeout
                    remaining_red2 = greenDelay - (now - stateStart);
                }

                if (remaining_red2 < 0) remaining_red2 = 0;
            }
        }

        // Pending request
        else if (state == STATE_PENDING_REQUEST)
        {
            remaining_ped2 = pedestrianDelay - (now - requestTime);
            if (remaining_ped2 < 0) remaining_ped2 = 0;
        }

        // Pedestrian green
        else if (state == STATE_PEDESTRIAN_GREEN)
        {
            remaining_walk2 = walkingDelay - (now - stateStart);
            remaining_green1 = greenDelay - (now - stateStart);
            if (remaining_walk2 < 0) remaining_walk2 = 0;
        }

        // Orange to all red
        else if (state == STATE_ORANGE_TO_ALL_RED)
        {
        	remaining_green1 = remaining_green2 = 0;
			remaining_red1   = remaining_red2   = 0;
			remaining_walk2  = remaining_ped2   = 0;

        }


        // OLED Periodic update (10 Hz)
		if (now - lastOledUpdate >= 100) {
			lastOledUpdate = now;
			Oled_Periodic();
		}

        // Accept pedestrian request in almost any traffic state except when pedestrian is walking
        if (Button_Pressed(PL2_Switch) &&
            state != STATE_PENDING_REQUEST &&
            state != STATE_PEDESTRIAN_GREEN &&
            state != STATE_ORANGE_TO_ALL_RED)
        {
            // Start pending pedestrian request
            requestTime = now;
            lastToggle  = now;
            indicatorBlink = false;
            LED_Set(PL2_BLUE, true);
            state = STATE_PENDING_REQUEST;
            // continue loop so PENDING_REQUEST handles blinking immediately
        }

        switch (state){
        // ---------------------------------------------------
        case STATE_START:
            currentGreenIsVertical = true;
            set_vertical_green();
            set_ped_red();
            //LED_Update();
            state = STATE_VERTICAL_GREEN;
            stateStart = now;
            break;

        // ---------------------------------------------------
        case STATE_VERTICAL_GREEN:
            // ensure pedestrian is red while traffic flows
            set_ped_red();
            //LED_Update();

            // TASK 2 logic (part 1)
            {
                bool v = vertical_has_cars();
                bool h = horizontal_has_cars();

                // both lanes have cars
                if (v && h)
                {
                    // Start timing if this is the first moment BOTH lanes contain cars
                    if (!bothLanesTimerRunning) {
                        bothLanesTimerRunning = true;
                        bothLanesStartTime = now;
                    }

                    // If redDelayMax not yet reached -> keep vertical green
                    if (now - bothLanesStartTime < redDelayMax) {
                        break;
                    }

                    // redDelayMax expired -> switch to horizontal
                    set_all_orange();
                    DelayWithPWM(orangeDelay);

                    set_horizontal_green();
                    currentGreenIsVertical = false;
                    state = STATE_HORIZONTAL_GREEN;
                    stateStart = HAL_GetTick();

                    bothLanesTimerRunning = false;   // reset for next cycle
                    break;
                }
                else {
                    // If one lane becomes empty → stop timer
                    bothLanesTimerRunning = false;
                }

                // If vertical lane only has cars
                if (v && !h)
                {
                    break; // remain vertical green
                }

                // If green lane is empty and red lane has cars -> immediate switch
                if (!v && h)
                {
                    // 1) Vertical green -> vertical yellow
                    set_all_orange();
                    //LED_Update();
                    DelayWithPWM(orangeDelay);

                    // 2) Vertical yellow -> vertical red
                    LED_Set(TL2_YELLOW, false);
                    LED_Set(TL4_YELLOW, false);
                    LED_Set(TL2_RED, true);
                    LED_Set(TL4_RED, true);
                    set_horizontal_green();

                    currentGreenIsVertical = false;
                    state = STATE_HORIZONTAL_GREEN;
                    stateStart = HAL_GetTick();
                    break;
                }

                // If no cars anywhere -> idle switch after greenDelay
                if (!v && !h) {
                    if (now - stateStart >= greenDelay) {
                        set_all_orange();
                        DelayWithPWM(orangeDelay);
                        set_horizontal_green();
                        currentGreenIsVertical = false;
                        state = STATE_HORIZONTAL_GREEN;
                        stateStart = HAL_GetTick();
                    }
                    break;
                }
            }
            break;

        // ---------------------------------------------------
        case STATE_HORIZONTAL_GREEN:
            // ensure pedestrian is red while traffic flows
            set_ped_red();

            // TASK 2 logic (part 2)
            {
                bool v = vertical_has_cars();
                bool h = horizontal_has_cars();

                // both lanes have cars
                if (h && v)
                {
                    if (!bothLanesTimerRunning) {
                        bothLanesTimerRunning = true;
                        bothLanesStartTime = now;
                    }

                    if (now - bothLanesStartTime < redDelayMax) {
                        break;
                    }

                    // Switch to vertical
                    set_all_orange();
                    DelayWithPWM(orangeDelay);

                    set_vertical_green();
                    currentGreenIsVertical = true;
                    state = STATE_VERTICAL_GREEN;
                    stateStart = HAL_GetTick();

                    bothLanesTimerRunning = false;
                    break;
                }
                else {
                    bothLanesTimerRunning = false;
                }


                // If horizontal only has cars
                if (!v && h)
                {
                    break; // remain horizontal green
                }

                // If green lane empty and red lane has cars -> immediate switch
                if (!h && v)
                {
                    // Horizontal green -> yellow
                    set_all_orange();
                    DelayWithPWM(orangeDelay);

                    // Horizontal yellow -> red
                    LED_Set(TL1_YELLOW, false);
                    LED_Set(TL3_YELLOW, false);
                    LED_Set(TL1_RED, true);
                    LED_Set(TL3_RED, true);
                    set_vertical_green();

                    currentGreenIsVertical = true;
                    state = STATE_VERTICAL_GREEN;
                    stateStart = HAL_GetTick();
                    break;
                }


                // No cars anywhere -> idle switch
                if (!v && !h) {
                    if (now - stateStart >= greenDelay) {
                    	set_all_orange();
                    	DelayWithPWM(orangeDelay);

                        set_vertical_green();
                        currentGreenIsVertical = true;
                        state = STATE_VERTICAL_GREEN;
                        stateStart = HAL_GetTick();
                    }
                    break;
                }
            }
            break;

        // ---------------------------------------------------
        case STATE_PENDING_REQUEST:

            // blink indicator
            if (HAL_GetTick() - lastToggle >= toggleFreq){
                indicatorBlink = !indicatorBlink;
                LED_Set(PL2_BLUE, indicatorBlink);
                lastToggle = HAL_GetTick();
            }

            // pedestrian delay over -> turn all cars orange
            if (HAL_GetTick() - requestTime >= pedestrianDelay){
            	if (currentGreenIsVertical){
            		set_all_orange();
            		DelayWithPWM(orangeDelay);
            		state = STATE_ORANGE_TO_ALL_RED;
            		stateStart = HAL_GetTick();
            	}
            	else{
            		state = STATE_ORANGE_TO_ALL_RED;
            		stateStart = HAL_GetTick();
            	}



            }

            break;

        // ---------------------------------------------------
        case STATE_ORANGE_TO_ALL_RED:
            if (HAL_GetTick() - stateStart >= orangeDelay)
            {
            	if (!currentGreenIsVertical) {
            	    LED_Set(TL2_YELLOW, false);
            	    LED_Set(TL4_YELLOW, false);
            	    LED_Set(TL2_RED, true);
            	    LED_Set(TL4_RED, true);
            	} else {
            		LED_Set(TL2_YELLOW, false);
					LED_Set(TL4_YELLOW, false);
					LED_Set(TL2_RED, true);
					LED_Set(TL4_RED, true);

            	    LED_Set(TL1_YELLOW, false);
            	    LED_Set(TL3_YELLOW, false);
            	    LED_Set(TL1_GREEN, true);
            	    LED_Set(TL3_GREEN, true);
            	}

                state = STATE_PEDESTRIAN_GREEN;
                stateStart = HAL_GetTick();
            }
            break;

        // ---------------------------------------------------
        case STATE_PEDESTRIAN_GREEN:

            set_ped_green();

            // Update walk2 countdown
            remaining_walk2 = walkingDelay - (now - stateStart);
            if (remaining_walk2 > walkingDelay) remaining_walk2 = walkingDelay;
            if (remaining_walk2 < 0) remaining_walk2 = 0;

            // Update green1 countdown (horizontal lane green)
            remaining_green1 = greenDelay - (now - stateStart);
            if (remaining_green1 > greenDelay) remaining_green1 = greenDelay;
            if (remaining_green1 < 0) remaining_green1 = 0;

            // When WALK is finished -> exit state
            if (now - stateStart >= walkingDelay)
            {
                set_ped_red();
                set_all_orange();
                DelayWithPWM(orangeDelay);

                if (currentGreenIsVertical){
                    set_vertical_green();
                }
                else{
                    set_horizontal_green();
                }

                if (currentGreenIsVertical) {
                    state = STATE_VERTICAL_GREEN;
                } else {
                    state = STATE_HORIZONTAL_GREEN;
                }

                stateStart = HAL_GetTick();
            }

            break;

        }
    }
}
