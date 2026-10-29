/**
******************************************************************************
@brief Test Program
@file test.c
@author Bafoday Barrow
@version 1.0
@date 13-December-2025
@brief Test functions for drivers and traffic light FSM
******************************************************************************
*/
#include "test.h"
#include "leddriver.h"
#include "buttondriver.h"
#include "traffic_light.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
// Used for probing bits
static uint8_t probe_bit =0;

/**
@brief Test_program, entry point for running tests
@param void
Comment/uncomment function content to run tests. While loop for continous testing
*/
void Test_program(void)
{
	while(1){
			Test_shiftreg();
			Test_leddriver();
//			Clear all LEDs
			ShiftReg_Write(0x000000);
			Test_button();
			Test_Traffic_Light_FSM();
			Test_OLED_Display();
	}
}

/**
@brief Tests shift register by lighting one LED at a time
@param void
*/
void Test_shiftreg(void){
	for(int i=0; i<24; i++){
		//Clear all LEDs
		ShiftReg_Write(0x000000);

		// Light up on LED at a time
		uint32_t mask = (1UL << probe_bit);
		ShiftReg_Write(mask);
		HAL_Delay(750);

		probe_bit++;
		if(probe_bit >=24){ probe_bit =0;}
	}
	ShiftReg_Write(0x000000);
}

/**
@brief Tests LED driver using LED_Set and LED_Update
@param void
*/
void Test_leddriver(void){
	for(int i=0; i<LED_COUNT; i++){

			// Light up on LED at a time
			LED_Set(i, true);
			LED_Update();
			HAL_Delay(750);

			LED_Set(i, false);
			LED_Update();
			HAL_Delay(350);
	}

}

/**
@brief Tests all buttons by scanning them
@param void
*/
void Test_button(void){
	//Clear all LEDs
	ShiftReg_Write(0xffffff);
	HAL_Delay(1000);
	ShiftReg_Write(0x000000);
	while(1){
		Button_Scan();

		//printf("PL1 = %d   PL2 = %d\n", Button_Pressed(PL1_Switch), Button_Pressed(PL2_Switch));
	}
}
void Test_vertical_has_cars(void){
	vertical_has_cars();
	LED_Update();
	HAL_Delay(1000);
}
void Test_horizontal_has_cars(void){
	horizontal_has_cars();
	LED_Update();
	HAL_Delay(1000);
}
void Test_clear_all_cars(void){
	clear_all_cars();
	LED_Update();
	HAL_Delay(1000);
}
void Test_clear_pedestrian(void){
	clear_pedestrian();
	LED_Update();
	HAL_Delay(1000);
}
void Test_set_vertical_green(void){
	set_vertical_green();
	LED_Update();
	HAL_Delay(1000);
}
void Test_set_horizontal_green(void){
	set_horizontal_green();
	LED_Update();
	HAL_Delay(1000);
}
void Test_set_all_orange(void){
	set_all_orange();
	LED_Update();
	HAL_Delay(1000);
}
void Test_set_ped_red(void){
	set_ped_red();
	LED_Update();
		HAL_Delay(1000);
}
void Test_set_ped_green(void){
	set_ped_green();
	LED_Update();
		HAL_Delay(1000);
}
/**
@brief Tests traffic light FSM helper functions
@param void
*/
void Test_Traffic_Light_FSM(void){
	while(1){
	Test_vertical_has_cars();
	Test_horizontal_has_cars();
	Test_clear_all_cars();
	Test_clear_pedestrian();
	Test_set_vertical_green();
	Test_set_horizontal_green();
	Test_set_all_orange();
	Test_set_ped_red();
	Test_set_ped_green();
	}
}

/**

@brief Tests the OLED-Display by printing text.*/
void Test_OLED_Display(void){
        ssd1306_Init();
        ssd1306_Fill(Black);

        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("OLED OK!", Font_7x10, White);

        ssd1306_SetCursor(0, 20);
        ssd1306_WriteString("Hello!", Font_7x10, White);

        ssd1306_UpdateScreen();

}
