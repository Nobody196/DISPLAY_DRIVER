/**
 * @file display_port.c
 * @brief Board/HAL adapter for the WeAct MiniSTM32H7xx onboard TFT.
 *
 * Board mapping is intentionally kept here. The board schematic is the
 * authority for these connections; the supplied WeAct LCD test project is
 * used as the firmware reference.
 */
#include "display_port.h"
#include <stddef.h>
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "stm32h7xx_hal.h"

#define DISPLAY_BACKLIGHT_TIMER      (&htim1)
#define DISPLAY_BACKLIGHT_CHANNEL    TIM_CHANNEL_2
#define DISPLAY_BACKLIGHT_MAX       1000U

bool Display_Port_Init(void)
{
    /* PE10/TIM1_CH2N is the board backlight control. */
    if (HAL_TIMEx_PWMN_Start(DISPLAY_BACKLIGHT_TIMER,
                             DISPLAY_BACKLIGHT_CHANNEL) != HAL_OK)
    {
        return false;
    }

    Display_Port_Backlight_Off();
    Display_Port_CS_High();
    Display_Port_DC_Data();
    return true;
}

bool Display_Port_Write(const uint8_t *data, uint32_t length)
{
    if ((data == NULL) || (length == 0U))
        return true;

    if (length > 0xFFFFU)
        return false;

    return HAL_SPI_Transmit(&hspi4, (uint8_t *)data,
                            (uint16_t)length, HAL_MAX_DELAY) == HAL_OK;
}

void Display_Port_CS_Low(void)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
}

void Display_Port_CS_High(void)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void Display_Port_DC_Command(void)
{
    HAL_GPIO_WritePin(LCD_WR_RS_GPIO_Port, LCD_WR_RS_Pin, GPIO_PIN_RESET);
}

void Display_Port_DC_Data(void)
{
    HAL_GPIO_WritePin(LCD_WR_RS_GPIO_Port, LCD_WR_RS_Pin, GPIO_PIN_SET);
}

void Display_Port_Backlight_Set(uint16_t level)
{
    if (level > DISPLAY_BACKLIGHT_MAX)
        level = DISPLAY_BACKLIGHT_MAX;

    __HAL_TIM_SET_COMPARE(DISPLAY_BACKLIGHT_TIMER,
                          DISPLAY_BACKLIGHT_CHANNEL, level);
}

void Display_Port_Backlight_On(void)
{
    Display_Port_Backlight_Set(DISPLAY_BACKLIGHT_MAX);
}

void Display_Port_Backlight_Off(void)
{
    Display_Port_Backlight_Set(0U);
}

uint32_t Display_Port_GetTick(void)
{
    return HAL_GetTick();
}

uint32_t Display_Port_DelayMs(uint32_t delay_ms)
{
    HAL_Delay(delay_ms);
    return delay_ms;
}
