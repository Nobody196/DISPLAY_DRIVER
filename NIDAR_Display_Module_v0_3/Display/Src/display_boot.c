/**
 * @file display_boot.c
 * @brief Optional short boot/status animation.
 *
 * This is deliberately a client of display.h. It contains no ST7735
 * registers and no board pin access.
 */
#include "display_boot.h"
#include "display.h"
#include "display_port.h"

void Display_BootAnimation(void)
{
    Display_Backlight_Off();
    Display_Clear(DISPLAY_BLACK);

    /* Simple self-test: border + moving progress bar. */
    Display_DrawRect(2U, 2U, 76U, 156U, DISPLAY_BLUE);
    Display_FillRect(10U, 70U, 60U, 8U, DISPLAY_BLUE);

    for (uint16_t w = 0U; w <= 60U; w += 3U)
    {
        Display_FillRect(10U, 70U, w, 8U, DISPLAY_CYAN);
        Display_Port_DelayMs(35U);
    }

    Display_FillRect(10U, 70U, 60U, 8U, DISPLAY_GREEN);
    Display_Port_DelayMs(250U);
    Display_Backlight_Set(1000U);
    Display_Port_DelayMs(500U);
}
