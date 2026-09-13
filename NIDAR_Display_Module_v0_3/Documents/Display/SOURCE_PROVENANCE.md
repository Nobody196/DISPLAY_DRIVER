# Source Provenance

The following files are preserved from the supplied WeAct/ST implementation
with their original filenames and header/license notices:

- `st7735.c`
- `st7735.h`
- `st7735_reg.c`
- `st7735_reg.h`
- `font.h`

They originate from the WeAct `SDK/HAL/STM32H743/03-LCD_Test` display example,
which in turn contains STMicroelectronics component-driver code.

Our newly written files are:

- `display.c`
- `display.h`
- `display_boot.c`
- `display_boot.h`
- `display_port.c`
- `display_port.h`

The new files form the reusable application/board boundary around the
preserved controller implementation.
