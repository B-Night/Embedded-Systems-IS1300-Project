/*
 * oled.c
 *
 *  Created on: Dec 11, 2025
 *      Author: Hem
 */


// oled.c
#include "oled.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "string.h"

// external counters
volatile uint32_t remaining_ped1   = 0;
volatile uint32_t remaining_walk1  = 0;
volatile uint32_t remaining_ped2   = 0;
volatile uint32_t remaining_walk2  = 0;
volatile uint32_t remaining_green1 = 0;
volatile uint32_t remaining_red1   = 0;
volatile uint32_t remaining_green2 = 0;
volatile uint32_t remaining_red2   = 0;

extern uint32_t pedestrianDelay;
extern uint32_t walkingDelay;
extern uint32_t greenDelay;
extern uint32_t redDelayMax;

// Hardware peripheral handles
extern SPI_HandleTypeDef hspi2;
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;

// Pin definitions
#define POT_GPIO_Port GPIOA
#define POT_Pin       GPIO_PIN_4

#define DISP_CS_Port     Disp_CS_GPIO_Port
#define DISP_CS_Pin      Disp_CS_Pin

#define DISP_Data_Port   Disp_Data_GPIO_Port
#define DISP_Data_Pin    Disp_Data_Pin

#define DISP_Reset_Port  Disp_Reset_GPIO_Port
#define DISP_Reset_Pin   Disp_Reset_Pin


// SHIFT register OE pin (PWM target)
#define SR_OE_GPIO_Port GPIOC
#define SR_OE_Pin       GPIO_PIN_7

// UI layout constants (128x64)
#define BAR_MAX_WIDTH 80
#define BAR_X_START   40
#define LINE_HEIGHT   8

// helper to convert ms remaining to bar width
static uint8_t time_to_bar(uint32_t remaining, uint32_t total) {
    if (total == 0) return 0;
    if (remaining > total) remaining = total;
    return (uint8_t)(( (uint32_t)remaining * BAR_MAX_WIDTH) / total);
}


static void ssd1306_spi_write_cmd(uint8_t cmd) {
    HAL_GPIO_WritePin(DISP_Data_Port, DISP_Data_Pin, GPIO_PIN_RESET); // DC = 0 for cmd
    HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, GPIO_PIN_RESET); // CS low
    HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, GPIO_PIN_SET);   // CS high
}

static void ssd1306_spi_write_data(uint8_t *data, uint16_t len) {
    HAL_GPIO_WritePin(DISP_Data_Port, DISP_Data_Pin, GPIO_PIN_SET); // DC = 1 for data
    HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, data, len, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(DISP_CS_Port, DISP_CS_Pin, GPIO_PIN_SET);
}


void Oled_Init(void)
{
    // Reset the display
    HAL_GPIO_WritePin(DISP_Reset_Port, DISP_Reset_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(DISP_Reset_Port, DISP_Reset_Pin, GPIO_PIN_SET);
    HAL_Delay(10);

    // Initialize SSD1306
    ssd1306_Init();

    // Clear screen
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    // Start PWM on SR_OE
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

// Read potentiometer using ADC
static uint16_t read_pot_adc(void)
{
    uint16_t adc = 0;
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        adc = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
    return adc; // 12-bit 0..4095
}

// Apply brightness scaling by setting PWM on SR_OE.
static void update_brightness_from_pot(void)
{
    uint16_t adc = read_pot_adc();
    uint32_t arr = htim3.Instance->ARR;
    uint32_t pwm_val = (uint32_t)adc * (arr) / 4095U;

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_val);
}

// helper to draw a single vertical bar column and label at the bottom
static void draw_bar_column(const char *label, uint32_t remaining, uint32_t total, uint8_t col_index)
{
    // Define Constants for this new layout
    const uint8_t COL_WIDTH     = 16;  // Width reserved per column (16*8 = 128)
    const uint8_t BAR_MAX_HEIGHT= 50;
    const uint8_t LABEL_Y_START = 56;
    const uint8_t SCREEN_HEIGHT = 64;

    // Calculate X position
    uint8_t x_center = col_index * COL_WIDTH + (COL_WIDTH / 2);

    // Use the calculated column boundary exactly to clear previous text
    ssd1306_FillRectangle(col_index * COL_WIDTH, 0, (col_index + 1) * COL_WIDTH - 1, SCREEN_HEIGHT - 1, Black);

    // Draw the label at the bottom, centered on the X_center point
    uint8_t label_len = strlen(label);
    //Calculate X_start based on center point.
    uint8_t label_x = x_center - ((label_len * 6) / 2);
    ssd1306_SetCursor(label_x, LABEL_Y_START);
    ssd1306_WriteString(label, Font_6x8, White);

    // Calculate Bar Height
    uint8_t bar_h = 0;
    if (total > 0 && remaining > 0) {
        if (remaining > total) remaining = total;
        bar_h = (remaining * BAR_MAX_HEIGHT) / total;
    }

    // Draw the vertical bar above the label
    if (bar_h > 0)
    {
        uint8_t bar_y_bottom = LABEL_Y_START - 2;
        uint8_t bar_y_top    = bar_y_bottom - bar_h;
        uint8_t bar_width_actual = 8;

        ssd1306_FillRectangle(
            x_center - (bar_width_actual / 2), // Center the bar around x_center
            bar_y_top,
            x_center + (bar_width_actual / 2) - 1, // End X must be width - 1
            bar_y_bottom,
            White
        );
    }
}


void Oled_Periodic(void)
{
    // update brightness
    update_brightness_from_pot();

    // update OLED - Clear the whole screen buffer
    ssd1306_Fill(Black);

    extern uint32_t pedestrianDelay, walkingDelay, greenDelay, redDelayMax;

    // Use the new column layout (using indexes 0 through 7)
    draw_bar_column("P1", remaining_ped1, pedestrianDelay, 0);
    draw_bar_column("W1", remaining_walk1, walkingDelay, 1);
    draw_bar_column("P2", remaining_ped2, pedestrianDelay, 2);
    draw_bar_column("W2", remaining_walk2, walkingDelay, 3);
    draw_bar_column("G1", remaining_green1, greenDelay, 4);
    draw_bar_column("R1", remaining_red1, redDelayMax, 5);
    draw_bar_column("G2", remaining_green2, greenDelay, 6);
    draw_bar_column("R2", remaining_red2, redDelayMax, 7);


    ssd1306_UpdateScreen();
}

