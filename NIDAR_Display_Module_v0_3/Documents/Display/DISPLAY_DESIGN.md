# Display Driver Design and Reasoning

## Goal

This is a reusable **display component**, not a display application and not a
TOF dependency. The TFT is currently a monitoring/control interface and a
visual validation target.

## Source hierarchy

For this exact board, use this priority:

1. **Supplied `STM32H7xx SchDoc V12.pdf`** — authoritative board wiring.
2. **Supplied WeAct `MiniSTM32H7xx-master`** — board-specific firmware,
   CubeMX configuration and panel implementation.
3. **ST7735S controller datasheet** supplied by WeAct — controller protocol
   and electrical/command reference.
4. Other ST7735 repositories — cross-checks and alternative implementation
   ideas, not overrides of the board-specific configuration.

## Board-specific facts established from the references

The WeAct 0.96-inch configuration selects:

```text
Type:         ST7735_0_9_inch_screen
Panel:        HannStar_Panel
Orientation:  ST7735_ORIENTATION_LANDSCAPE_ROT180
Logical size: 80 x 160
X offset:     26
Y offset:     1
Pixel format: RGB565
```

The WeAct OpenMV example independently uses an 80x160 display with BGR,
vertical flip and horizontal mirror and uses the same 26/1 addressing
origin. This is a useful independent cross-check of the panel-specific
geometry.

## Hardware boundary

`display_port.c` contains the board/HAL knowledge:

```text
PE14 -> SPI4 MOSI -> LCD_SDA
PE12 -> SPI4 SCK  -> LCD_SCL
PE11 -> GPIO      -> LCD_CS
PE13 -> GPIO      -> LCD_WR_RS
PE10 -> TIM1_CH2N -> LCD_LED
SYS_RESET -> LCD_RESET
```

The ST7735 controller code does not contain PE pin numbers.

## Controller boundary

The supplied WeAct STMicroelectronics-derived files are retained with their
original filenames:

```text
st7735.c
st7735.h
st7735_reg.c
st7735_reg.h
```

This preserves traceability to the supplied board implementation and keeps
controller-specific register knowledge localized.

## Why the existing WeAct driver is the foundation

The supplied WeAct `03-LCD_Test` is not merely a generic ST7735 example. It
contains the exact 0.96-inch selection and its special display geometry. It
also contains the board's 15 MHz SPI4 configuration and PE11/PE13 control
signals.

Therefore we use that implementation as the baseline instead of replacing
its initialization table with a generic ST7735 sequence.

## Initialization sequence

The ST7735 initialization retained from the WeAct/ST implementation includes
software reset, sleep-out, frame-rate control, inversion/power configuration,
color mode, gamma configuration, display-on and orientation/window setup.

The sequence is intentionally kept inside `st7735.c`.

Application code should never contain raw initialization commands such as
`0xC0`, `0xE0`, `0xE1`, `0x2A`, etc.

## Addressing

The physical ST7735 controller has a larger internal address space than the
visible 80x160 panel. The WeAct driver compensates for the panel's visible
window using the 26-column and 1-row offsets for the default 0.96-inch
HannStar configuration.

This is one of the most important reasons to use the board-specific driver.
A generic 80x160 ST7735 driver can have the correct dimensions but still
produce shifted, mirrored or partially clipped output.

## Graphics scope

We deliberately keep graphics modest:

- pixel
- line
- rectangle
- filled rectangle
- circle
- filled circle
- RGB565 image
- basic ASCII text

This is enough for status screens, controls, diagnostics and visual effects.
A full GUI framework is not a goal at this stage.

## Boot animation

`display_boot.c` is a separate consumer of the public display API.

It does not know about SPI, GPIO, ST7735 registers or panel offsets.

That gives us a clean test of the abstraction:

```text
Display_BootAnimation()
        |
        +-- Display_DrawRect()
        +-- Display_FillRect()
        +-- Display_Clear()
        +-- Display_Backlight_Set()
```

If a future application wants a different startup sequence, it can replace
this file without touching the driver.

## Backlight

The board uses a P-channel MOSFET and TIM1 complementary output for the
backlight. This is why brightness is handled as a timer compare value rather
than a simple GPIO high/low state.

The public API uses:

```c
Display_Backlight_Set(0);      // off
Display_Backlight_Set(500);    // approximately half-scale
Display_Backlight_Set(1000);   // full-scale
```

The actual optical brightness is not assumed to be perfectly linear with
PWM duty cycle.

## No framebuffer initially

An 80x160 RGB565 framebuffer consumes 25,600 bytes. The H7 has enough RAM for
this, but a framebuffer is not required for simple status/control screens.

Direct windowed writes keep the first implementation straightforward.
DMA, batching and an optional framebuffer can be added later if measurement
shows that they are useful.

## Interrupt independence

The display driver is intentionally independent of the future TOF module.

The intended higher-level relationship is:

```text
VL53L5 interrupt
      |
      v
TOF driver/module
      |
      v
measurement/state
      |
      v
application
      |
      v
Display API
```

The display must not poll or own the TOF sensor.

## Verification strategy

Before adding GUI features, validate in this order:

1. SPI clocks/data are correct.
2. CS/DC transitions are correct.
3. ST7735 software reset and sleep-out work.
4. Panel initialization produces a stable image.
5. 26/1 offsets produce the correct 80x160 visible region.
6. Default landscape-180 orientation is correct.
7. RGB565 colours are correct.
8. Full-screen fills work.
9. Primitive drawing works.
10. Text works.
11. Backlight PWM works.
12. Boot animation works.

Only after these pass should the module be considered ready to freeze.

## Cross-checks from other implementations

The WeAct OpenMV example uses:

```text
80 x 160
vflip = true
hmirror = true
bgr = true
xPos = 26
yPos = 1
```

Generic ST7735 libraries also demonstrate that 80x160 panels require
panel-specific offsets and MADCTL/inversion choices. These are useful
cross-checks but should not replace the WeAct configuration for this board.

## Deliberate non-goals

Not included initially:

- LVGL
- a complete widget toolkit
- touch UI
- complex retained-mode GUI
- automatic layout engine
- full framebuffer compositor
- animation scheduler
- RTOS dependency

Those would add complexity without helping the immediate control/status
use case.
