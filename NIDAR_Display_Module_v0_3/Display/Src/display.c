/**
 * @file display.c
 * @brief Small application-facing API built on the WeAct/ST ST7735 driver.
 */
#include "display.h"
#include "display_port.h"
#include "st7735.h"
#include "font.h"
#include <string.h>

static ST7735_Object_t display_obj;
static ST7735_Ctx_t display_ctx;
static bool display_initialized = false;

static int32_t display_io_init(void)
{
    return Display_Port_Init() ? ST7735_OK : ST7735_ERROR;
}

static int32_t display_io_deinit(void) { return ST7735_OK; }
static int32_t display_io_gettick(void) { return (int32_t)Display_Port_GetTick(); }

static int32_t display_io_write(uint8_t reg, uint8_t *data, uint32_t length)
{
    Display_Port_CS_Low();
    Display_Port_DC_Command();
    bool ok = Display_Port_Write(&reg, 1U);
    if (ok && length > 0U)
    {
        Display_Port_DC_Data();
        ok = Display_Port_Write(data, length);
    }
    Display_Port_CS_High();
    return ok ? ST7735_OK : ST7735_ERROR;
}

static int32_t display_io_read(uint8_t reg, uint8_t *data)
{
    /* The onboard interface is configured as write-only SPI for this module. */
    (void)reg;
    (void)data;
    return ST7735_ERROR;
}

static int32_t display_io_send(uint8_t *data, uint32_t length)
{
    Display_Port_CS_Low();
    Display_Port_DC_Data();
    bool ok = Display_Port_Write(data, length);
    Display_Port_CS_High();
    return ok ? ST7735_OK : ST7735_ERROR;
}

static int32_t display_io_recv(uint8_t *data, uint32_t length)
{
    (void)data;
    (void)length;
    return ST7735_ERROR;
}

static ST7735_IO_t display_io =
{
    display_io_init,
    display_io_deinit,
    0U,
    display_io_write,
    display_io_read,
    display_io_send,
    display_io_recv,
    display_io_gettick
};

bool Display_Init(void)
{
    display_ctx.Orientation = ST7735_ORIENTATION_LANDSCAPE_ROT180;
    display_ctx.Panel = HannStar_Panel;
    display_ctx.Type = ST7735_0_9_inch_screen;

    if (ST7735_RegisterBusIO(&display_obj, &display_io) != ST7735_OK)
        return false;

    if (ST7735_LCD_Driver.Init(&display_obj,
                               ST7735_FORMAT_RBG565,
                               &display_ctx) != ST7735_OK)
    {
        return false;
    }

    /* The WeAct 0.96-inch configuration uses landscape 180° by default. */
    ST7735_LCD_Driver.FillRect(&display_obj, 0U, 0U,
                               ST7735Ctx.Width, ST7735Ctx.Height,
                               DISPLAY_BLACK);

    Display_Backlight_On();
    display_initialized = true;
    return true;
}

void Display_Clear(uint16_t color)
{
    if (!display_initialized) return;
    ST7735_LCD_Driver.FillRect(&display_obj, 0U, 0U,
                               ST7735Ctx.Width, ST7735Ctx.Height, color);
}

void Display_SetRotation(Display_Rotation_t rotation)
{
    if (!display_initialized) return;
    uint32_t r = (uint32_t)rotation;
    if (r > 3U) return;
    display_ctx.Orientation = r;
    ST7735_LCD_Driver.SetOrientation(&display_obj, &display_ctx);
}

uint16_t Display_GetWidth(void) { return (uint16_t)ST7735Ctx.Width; }
uint16_t Display_GetHeight(void) { return (uint16_t)ST7735Ctx.Height; }

void Display_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (!display_initialized) return;
    (void)ST7735_LCD_Driver.SetPixel(&display_obj, x, y, color);
}

void Display_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t dy = (y1 > y0) ? (y0 - y1) : (y1 - y0);
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx + dy;

    while (1)
    {
        if ((x0 >= 0) && (y0 >= 0))
            Display_DrawPixel((uint16_t)x0, (uint16_t)y0, color);
        if ((x0 == x1) && (y0 == y1)) break;
        int16_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void Display_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    if (!width || !height) return;
    Display_FillRect(x, y, width, 1U, color);
    if (height > 1U) Display_FillRect(x, y + height - 1U, width, 1U, color);
    Display_FillRect(x, y, 1U, height, color);
    if (width > 1U) Display_FillRect(x + width - 1U, y, 1U, height, color);
}

void Display_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    if (!display_initialized || !width || !height) return;
    (void)ST7735_LCD_Driver.FillRect(&display_obj, x, y, width, height, color);
}

void Display_DrawCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color)
{
    int16_t x = -radius, y = 0, err = 2 - 2 * radius;
    do {
        Display_DrawPixel((uint16_t)(x0 - x), (uint16_t)(y0 + y), color);
        Display_DrawPixel((uint16_t)(x0 - y), (uint16_t)(y0 - x), color);
        Display_DrawPixel((uint16_t)(x0 + x), (uint16_t)(y0 - y), color);
        Display_DrawPixel((uint16_t)(x0 + y), (uint16_t)(y0 + x), color);
        radius = err;
        if (radius <= y) err += ++y * 2 + 1;
        if (radius > x || err > y) err += ++x * 2 + 1;
    } while (x < 0);
}

void Display_FillCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color)
{
    for (int16_t y = -radius; y <= radius; y++)
    {
        int32_t span = radius * radius - y * y;
        int16_t dx = 0;
        while ((int32_t)(dx + 1) * (dx + 1) <= span) dx++;
        if (dx > 0)
            Display_FillRect((uint16_t)(x0 - dx), (uint16_t)(y0 + y),
                             (uint16_t)(2 * dx + 1), 1U, color);
    }
}

void Display_DrawRGB565(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                        const uint8_t *pixels)
{
    if (!display_initialized || !pixels || !width || !height) return;
    (void)ST7735_LCD_Driver.FillRGBRect(&display_obj, x, y,
                                        (uint8_t *)pixels, width, height);
}

void Display_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg)
{
    if (!display_initialized || c < ' ' || c > '~') return;

    uint8_t buffer[8U * 16U * 2U];
    const uint8_t *glyph = asc2_1608[(uint8_t)c - ' '];

    for (uint16_t row = 0; row < 16U; row++)
    {
        for (uint16_t col = 0; col < 8U; col++)
        {
            uint8_t bits = glyph[col * 2U + (row / 8U)];
            bool on = (bits & (uint8_t)(0x80U >> (row & 7U))) != 0U;
            uint16_t pixel = on ? color : bg;
            uint32_t i = ((uint32_t)row * 8U + col) * 2U;
            buffer[i] = (uint8_t)(pixel >> 8);
            buffer[i + 1U] = (uint8_t)pixel;
        }
    }
    Display_DrawRGB565(x, y, 8U, 16U, buffer);
}

void Display_DrawString(uint16_t x, uint16_t y, const char *text,
                        uint16_t color, uint16_t bg)
{
    if (!text) return;
    while (*text)
    {
        if (*text == '\n')
        {
            x = 0U;
            y = (uint16_t)(y + 16U);
        }
        else
        {
            if ((uint16_t)(x + 8U) > Display_GetWidth())
            {
                x = 0U;
                y = (uint16_t)(y + 16U);
            }
            if ((uint16_t)(y + 16U) > Display_GetHeight()) break;
            Display_DrawChar(x, y, *text, color, bg);
            x = (uint16_t)(x + 8U);
        }
        text++;
    }
}

void Display_Backlight_Set(uint16_t level_0_to_1000)
{
    Display_Port_Backlight_Set(level_0_to_1000);
}

void Display_Backlight_On(void) { Display_Port_Backlight_On(); }
void Display_Backlight_Off(void) { Display_Port_Backlight_Off(); }
