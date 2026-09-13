# NIDAR Display Module

Reusable onboard TFT component for the WeAct MiniSTM32H7xx / STM32H743 board.

## Current basis

This version is built around the supplied WeAct `03-LCD_Test` implementation,
with the supplied board schematic treated as the authoritative hardware
wiring reference and the ST7735S datasheet used to validate controller-level
behaviour.

## Architecture

```text
Application / future GUI
        |
        v
    display.h
        |
        v
    display.c
        |
        v
    WeAct/ST st7735.c
        |
        v
 display_port.c
        |
        v
 STM32H7 SPI4 / GPIO / TIM1
        |
        v
   onboard TFT
```

## Important board mapping

```text
PE14 -> LCD_SDA    -> SPI4_MOSI
PE12 -> LCD_SCL    -> SPI4_SCK
PE11 -> LCD_CS     -> GPIO
PE13 -> LCD_WR_RS  -> GPIO
PE10 -> LCD_LED    -> TIM1_CH2N
LCD_RESET -> SYS_RESET / NRST
```

## Documentation

- `Documents/Display/CUBEMX_TFT_CONFIGURATION.md`
- `Documents/Display/DISPLAY_USAGE.md`
- `Documents/Display/DISPLAY_DESIGN.md`

## Scope

Basic graphics and text are included. A full GUI framework is deliberately
not included; the display is intended mainly for control/status information,
diagnostics and simple visual effects.

The boot animation is optional and is a separate client of the display API.

## Hardware-readiness rule

The display initialization is considered the critical part of this component.
Do not replace the board-specific WeAct configuration with a generic ST7735
initialization sequence without documenting and validating the change.
