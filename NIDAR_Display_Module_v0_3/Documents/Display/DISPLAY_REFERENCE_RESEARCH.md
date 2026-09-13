# Display Reference Research Record

## Source priority

For this board the evidence is intentionally weighted:

1. `STM32H7xx SchDoc V12.pdf` supplied with NIDAR-SENSE — authoritative
   electrical wiring.
2. WeAct `MiniSTM32H7xx-master` supplied with the project — authoritative
   board-specific firmware/configuration reference.
3. WeAct's ST7735S datasheet in `Datasheet/ST7735S_V1.5_20150303.pdf` —
   controller-level reference.
4. WeAct OpenMV 0.96-inch example — independent board-specific software
   cross-check.
5. Generic ST7735 repositories/libraries — secondary cross-check only.

## Findings

### Board wiring

The supplied schematic shows:

- PE14 -> MOSI -> `LCD_SDA`
- PE13 -> GPIO -> `LCD_WR_RS`
- PE12 -> SCK -> `LCD_SCL`
- PE11 -> GPIO -> `LCD_CS`
- PE10 -> TIM -> `LCD_LED`
- `LCD_RESET` -> `SYS_RESET`

Therefore `LCD_RESET` must not be treated as a free MCU GPIO.

### WeAct HAL example

`SDK/HAL/STM32H743/03-LCD_Test` contains a complete board-specific LCD
example.

Its CubeMX project uses:

- STM32H743VITx
- SPI4 master
- 1-line SPI
- 8-bit transfers
- CPOL low
- CPHA 1 edge
- software NSS
- MSB first
- SPI45 clock 120 MHz
- /8 baud prescaler
- 15 Mbit/s calculated baud rate

The GPIO labels are:

- PE11 `LCD_CS`
- PE13 `LCD_WR_RS`

SPI4 uses:

- PE12 `SPI4_SCK`
- PE14 `SPI4_MOSI`

The backlight is PE10 / TIM1_CH2N. WeAct starts the complementary PWM
output with `HAL_TIMEx_PWMN_Start()` and controls brightness using the
TIM1 CH2 compare value.

### Panel configuration

The WeAct 0.96-inch path selects:

```text
TFT96
ST7735_0_9_inch_screen
HannStar_Panel
ST7735_ORIENTATION_LANDSCAPE_ROT180
```

The ST7735 driver uses the panel-specific visible-area offsets:

```text
Portrait:       X + 26, Y + 1
Landscape:      X + 1,  Y + 26
```

The OpenMV 0.96-inch example independently uses:

```text
width = 80
height = 160
vflip = true
hmirror = true
bgr = true
xPos = 26
yPos = 1
```

and contains the same characteristic panel setup values, providing a useful
second board-specific confirmation.

## Controller datasheet checks

The supplied ST7735S datasheet documents:

- `11h` Sleep Out
- `29h` Display On
- `2Ah` Column Address Set
- `2Bh` Row Address Set
- `2Ch` Memory Write
- `36h` MADCTL
- `3Ah` COLMOD
- RGB565 as 16-bit/pixel format

These are the controller operations used by the retained WeAct/ST driver.

## Generic repository cross-check

Generic ST7735 libraries commonly require a special configuration for 160x80
panels and explicitly account for offsets and MADCTL/BGR choices. This
supports the decision to retain the WeAct-specific 26/1 geometry rather than
using a generic zero-offset initialization.

## Result

The module's controller layer is now based on the supplied WeAct/ST source
files rather than a newly invented initialization table. Board-specific
wiring remains isolated in `display_port.c`, while application-facing
drawing functions remain in `display.c`.

This record should be updated if a future hardware revision changes the
panel or board routing.
