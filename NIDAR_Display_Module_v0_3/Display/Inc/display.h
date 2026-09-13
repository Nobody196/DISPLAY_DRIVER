#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#define DISPLAY_WIDTH       80U
#define DISPLAY_HEIGHT      160U

#define DISPLAY_BLACK       0x0000U
#define DISPLAY_WHITE       0xFFFFU
#define DISPLAY_RED         0xF800U
#define DISPLAY_GREEN       0x07E0U
#define DISPLAY_BLUE        0x001FU
#define DISPLAY_CYAN        0x07FFU
#define DISPLAY_MAGENTA     0xF81FU
#define DISPLAY_YELLOW      0xFFE0U
#define DISPLAY_ORANGE      0xFD20U

#define DISPLAY_RGB565(r,g,b) \
    (uint16_t)((((uint16_t)(r) & 0xF8U) << 8) | \
               (((uint16_t)(g) & 0xFCU) << 3) | \
               (((uint16_t)(b) & 0xF8U) >> 3))

typedef enum
{
    DISPLAY_ROTATION_0 = 0,
    DISPLAY_ROTATION_90,
    DISPLAY_ROTATION_180,
    DISPLAY_ROTATION_270
} Display_Rotation_t;

bool Display_Init(void);
void Display_Clear(uint16_t color);
void Display_SetRotation(Display_Rotation_t rotation);
uint16_t Display_GetWidth(void);
uint16_t Display_GetHeight(void);

void Display_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void Display_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void Display_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void Display_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void Display_DrawCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color);
void Display_FillCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color);

void Display_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg);
void Display_DrawString(uint16_t x, uint16_t y, const char *text,
                        uint16_t color, uint16_t bg);

/* Draw an already-packed RGB565 image, row-major, MSB first. */
void Display_DrawRGB565(uint16_t x, uint16_t y,
                        uint16_t width, uint16_t height,
                        const uint8_t *pixels);

void Display_Backlight_Set(uint16_t level_0_to_1000);
void Display_Backlight_On(void);
void Display_Backlight_Off(void);

#endif
