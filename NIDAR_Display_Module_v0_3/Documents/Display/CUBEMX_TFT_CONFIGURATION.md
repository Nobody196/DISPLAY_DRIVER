# CubeMX Configuration — WeAct 0.96-inch ST7735 TFT

**Status:** board reference configuration; use this when assigning the display in the NIDAR-SENSE `.ioc`.

## 1. Authoritative board wiring

The supplied `STM32H7xx SchDoc V12.pdf` shows the TFT nets connected as follows:

| Board net / user label | MCU pin | Peripheral/function | Notes |
|---|---|---|---|
| `LCD_SDA` | **PE14** | `SPI4_MOSI` | TFT serial data |
| `LCD_SCL` | **PE12** | `SPI4_SCK` | TFT serial clock |
| `LCD_CS` | **PE11** | `GPIO_Output` | Active-low chip select |
| `LCD_WR_RS` | **PE13** | `GPIO_Output` | Command/data (D/C) |
| `LCD_LED` | **PE10** | `TIM1_CH2N` | PWM backlight control |
| `LCD_RESET` | **SYS_RESET / NRST** | System reset net | **Not an MCU GPIO** |

The important correction is that **LCD_RESET is not a spare GPIO that we should assign in CubeMX**. On the schematic it is tied into the board's system-reset circuitry. The display driver therefore uses the ST7735 software reset command instead of attempting to toggle an `LCD_RESET_GPIO` pin.

## 2. CubeMX pin assignments

Assign:

```text
PE14 -> SPI4_MOSI       User label: LCD_SDA
PE12 -> SPI4_SCK        User label: LCD_SCL
PE11 -> GPIO_Output     User label: LCD_CS
PE13 -> GPIO_Output     User label: LCD_WR_RS
PE10 -> TIM1_CH2N       User label: LCD_LED
```

Do **not** assign PE10 as a normal GPIO if using the board's PWM backlight implementation; use `TIM1_CH2N`.

Do **not** assign a separate MCU GPIO for `LCD_RESET`; it is `SYS_RESET/NRST` on the board.

## 3. GPIO configuration

### PE11 — LCD_CS

```text
Mode:          GPIO Output
Output type:   Push-Pull
Pull:          No Pull
Speed:         Very High
Initial level: High
User label:    LCD_CS
```

### PE13 — LCD_WR_RS

```text
Mode:          GPIO Output
Output type:   Push-Pull
Pull:          No Pull
Speed:         Very High
Initial level: High
User label:    LCD_WR_RS
```

The WeAct project uses this pin as the ST7735 command/data select line.

## 4. SPI4 configuration

The supplied WeAct `03-LCD_Test.ioc` configures SPI4 as:

```text
Peripheral:             SPI4
Mode:                   Master
Direction:              1-Line
Data size:              8 Bits
Clock polarity:         Low
Clock phase:            1 Edge
NSS:                    Software
First bit:              MSB First
Baud-rate prescaler:    /8
Calculated rate:        15.0 Mbit/s
```

The same WeAct project shows the SPI4 clock source as `D2PCLK1` and a
120 MHz SPI45 clock, giving 15 MHz with the /8 prescaler.

For the NIDAR-SENSE project, reproduce these settings unless another existing
peripheral allocation makes that impossible. The display driver itself does
not assume that it owns the rest of SPI4 configuration.

## 5. PE12 / PE14 alternate-function configuration

```text
PE12 -> SPI4_SCK
Alternate function: AF5
GPIO mode:          Alternate Function Push-Pull
Pull:               No Pull
Speed:              High

PE14 -> SPI4_MOSI
Alternate function: AF5
GPIO mode:          Alternate Function Push-Pull
Pull:               No Pull
Speed:              High
```

These are the assignments shown by the supplied WeAct H7 LCD example.

## 6. PE10 backlight PWM

The WeAct board does not drive the TFT backlight directly from a normal GPIO.
The schematic places a P-channel MOSFET in the backlight supply path, with
`LCD_LED` on the MOSFET control node.

The supplied WeAct LCD project configures:

```text
Timer:                 TIM1
Channel:               CH2N
Pin:                   PE10
Alternate function:    AF1
Mode:                  PWM output
Prescaler:             12-1
Period:                1000-1
PWM mode:              PWM1
OC polarity:           High
OCN polarity:          Low
Pulse initial value:   0
```

The project starts the complementary output using:

```c
HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
```

and varies the compare value for brightness.

Our public API therefore exposes a simple 0..1000 brightness level rather
than making application code deal with timer registers.

## 7. Display reset

The TFT FPC has an `LCD_RESET` signal, but the board schematic connects that
net to the system reset circuitry. It is not mapped to an independently
controllable MCU GPIO.

The ST7735 initialization therefore follows the WeAct software approach and
issues the ST7735 `SWRESET` command during initialization.

This is intentional. Do not create an `LCD_RESET_Pin` define merely to make
the driver look symmetrical with CS/DC/LED.

## 8. Why we are following the WeAct configuration

For this particular board, generic ST7735 examples are useful for protocol
background but should not override the board-specific implementation.

The supplied WeAct `03-LCD_Test` project is especially valuable because it
contains:

- the actual STM32H743 CubeMX configuration;
- the actual SPI4 settings;
- the actual LCD user labels;
- the actual ST7735 driver selection for the 0.96-inch display;
- the actual panel/offset configuration;
- the actual backlight timer implementation.

The ST7735 controller datasheet is used to validate command semantics and
controller constraints.

## 9. Quick assignment checklist

```text
[ ] PE10  -> TIM1_CH2N      -> LCD_LED
[ ] PE11  -> GPIO_Output    -> LCD_CS
[ ] PE12  -> SPI4_SCK       -> LCD_SCL
[ ] PE13  -> GPIO_Output    -> LCD_WR_RS
[ ] PE14  -> SPI4_MOSI      -> LCD_SDA
[ ] LCD_RESET -> SYS_RESET/NRST (no extra GPIO assignment)

[ ] SPI4 Master
[ ] SPI4 1-Line
[ ] 8-bit
[ ] CPOL Low
[ ] CPHA 1 Edge
[ ] Software NSS
[ ] MSB First
[ ] Prescaler /8 initially
[ ] SPI45 clock 120 MHz if preserving WeAct clock tree
[ ] Keep semantic LCD_* user labels
```
