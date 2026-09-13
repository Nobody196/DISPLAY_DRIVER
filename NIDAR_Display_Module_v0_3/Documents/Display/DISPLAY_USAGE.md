# Display Module Usage Manual

## 1. What this module provides

A reusable driver for the WeAct MiniSTM32H7xx onboard 0.96-inch 80x160 TFT.

Preferred application-facing include:

```c
#include "display.h"
#include "display_boot.h"
```

The ST7735-specific headers are implementation details.

## 2. CubeMX prerequisites

Configure the board exactly as described in:

`Documents/Display/CUBEMX_TFT_CONFIGURATION.md`

In particular:

```text
PE14 -> SPI4_MOSI -> LCD_SDA
PE12 -> SPI4_SCK  -> LCD_SCL
PE11 -> GPIO      -> LCD_CS
PE13 -> GPIO      -> LCD_WR_RS
PE10 -> TIM1_CH2N -> LCD_LED
LCD_RESET -> SYS_RESET/NRST
```

## 3. Minimal integration

After CubeMX-generated peripheral initialization:

```c
#include "display.h"

if (!Display_Init())
{
    Error_Handler();
}
```

Then:

```c
Display_Clear(DISPLAY_BLACK);
Display_DrawString(4, 4, "NIDAR-SENSE", DISPLAY_WHITE, DISPLAY_BLACK);
```

## 4. Boot animation

Optional:

```c
#include "display_boot.h"

Display_BootAnimation();
```

The boot animation is not part of the low-level ST7735 driver.

## 5. Screen information

```c
uint16_t width = Display_GetWidth();
uint16_t height = Display_GetHeight();
```

Default logical size:

```text
80 x 160
```

The default orientation is the board's WeAct-tested landscape-180 setting.

## 6. Clearing the display

```c
Display_Clear(DISPLAY_BLACK);
Display_Clear(DISPLAY_WHITE);
Display_Clear(DISPLAY_BLUE);
```

## 7. Pixel

```c
Display_DrawPixel(10, 20, DISPLAY_RED);
```

Coordinates outside the display are ignored by the underlying driver.

## 8. Rectangles

Outline:

```c
Display_DrawRect(2, 2, 76, 156, DISPLAY_CYAN);
```

Filled:

```c
Display_FillRect(10, 30, 60, 20, DISPLAY_BLUE);
```

## 9. Lines

```c
Display_DrawLine(0, 0, 79, 159, DISPLAY_GREEN);
```

## 10. Circles

```c
Display_DrawCircle(40, 80, 20, DISPLAY_YELLOW);
Display_FillCircle(40, 80, 10, DISPLAY_RED);
```

## 11. Text

The basic text layer uses the supplied WeAct `font.h` 8x16 ASCII font.

```c
Display_DrawChar(0, 0, 'A', DISPLAY_WHITE, DISPLAY_BLACK);

Display_DrawString(0, 20, "Distance: 123 mm",
                   DISPLAY_WHITE,
                   DISPLAY_BLACK);
```

Supported character range is printable ASCII (`0x20` through `0x7E`).

Text is intentionally basic and suitable for status/control information.

## 12. RGB565 colour conversion

For custom RGB888 colours:

```c
uint16_t orange = DISPLAY_RGB565(255, 165, 0);
Display_FillRect(10, 10, 20, 20, orange);
```

## 13. RGB565 images

`Display_DrawRGB565()` accepts an already-packed row-major RGB565 image.

```c
Display_DrawRGB565(x, y, width, height, image_data);
```

The image buffer is expected to contain two bytes per pixel in the order
expected by the ST7735 data transfer.

For large static assets, keeping them in external/QSPI/flash storage can be
considered later.

## 14. Rotation

```c
Display_SetRotation(DISPLAY_ROTATION_0);
Display_SetRotation(DISPLAY_ROTATION_90);
Display_SetRotation(DISPLAY_ROTATION_180);
Display_SetRotation(DISPLAY_ROTATION_270);
```

The underlying WeAct/ST7735 implementation handles the controller's MADCTL
configuration and panel-specific window offsets.

## 15. Backlight

The board uses PWM on `PE10 / TIM1_CH2N`.

```c
Display_Backlight_Off();
Display_Backlight_Set(250);
Display_Backlight_Set(500);
Display_Backlight_Set(1000);
Display_Backlight_On();
```

Range is 0..1000.

## 16. Recommended control/status usage

A simple status screen can be built without a GUI framework:

```c
Display_Clear(DISPLAY_BLACK);
Display_DrawString(2, 4, "NIDAR", DISPLAY_CYAN, DISPLAY_BLACK);
Display_DrawString(2, 24, "TOF: READY", DISPLAY_GREEN, DISPLAY_BLACK);
Display_DrawString(2, 44, "USB: OK", DISPLAY_GREEN, DISPLAY_BLACK);
Display_DrawRect(2, 68, 76, 30, DISPLAY_BLUE);
Display_FillRect(5, 71, 70, 24, DISPLAY_BLACK);
```

This is intentionally enough for a control/status display without bringing
in a complete GUI framework.

## 17. Animation

Animations belong in a separate application/demo file.

Example concept:

```c
for (uint16_t x = 5; x < 70; x += 5)
{
    Display_Clear(DISPLAY_BLACK);
    Display_FillCircle(x, 80, 5, DISPLAY_CYAN);
    HAL_Delay(30);
}
```

For production applications, avoid long blocking animations if they prevent
sensor servicing or other time-critical work.

## 18. Main.c usage

Keep `main.c` thin. The intended pattern is:

```c
MX_GPIO_Init();
MX_SPI4_Init();
MX_TIM1_Init();

Display_Init();
Display_BootAnimation();
```

The actual calls should be placed in the appropriate existing user-code
sections of the NIDAR-SENSE generated `main.c`.

Do not copy ST7735 register sequences into `main.c`.

## 19. Files to integrate

```text
Display/Inc/
    display.h
    display_port.h
    display_boot.h
    st7735.h
    st7735_reg.h
    font.h

Display/Src/
    display.c
    display_port.c
    display_boot.c
    st7735.c
    st7735_reg.c
```

`st7735.c`, `st7735.h`, `st7735_reg.c`, `st7735_reg.h`, and `font.h` retain the
original source filenames for traceability.

## 20. Troubleshooting

### Backlight works but screen is black

Check:

- SPI4 clock/data configuration
- CS and DC pins
- `Display_Init()` return value
- TIM1 CH2N configuration
- panel power
- exact initialization sequence

### Image shifted

Do not change application coordinates first. Verify the panel-specific
26/1 offsets and orientation configuration.

### Colours wrong

Check RGB565 byte order and the BGR/MADCTL configuration.

### Nothing transfers

Check:

```text
PE14 = SPI4_MOSI
PE12 = SPI4_SCK
PE11 = LCD_CS
PE13 = LCD_WR_RS
```

and verify SPI4 is actually initialized before `Display_Init()`.

### Reset confusion

`LCD_RESET` is connected to the board's `SYS_RESET/NRST` net. There is no
separate LCD reset GPIO to assign.

## 21. Validation demo

Before using this module with TOF, validate:

1. Backlight off/on.
2. Black screen.
3. Red full-screen fill.
4. Green full-screen fill.
5. Blue full-screen fill.
6. Pixel corners.
7. Rectangle.
8. Line.
9. Circle.
10. Text.
11. Rotation.
12. Boot animation.

A display that passes these tests is a good foundation for the monitoring UI.
