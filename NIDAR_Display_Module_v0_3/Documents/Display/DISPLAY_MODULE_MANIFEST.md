# Display Module Manifest

## Reusable source files

### Public
- `Display/Inc/display.h`
- `Display/Inc/display_boot.h`
- `Display/Src/display.c`
- `Display/Src/display_boot.c`

### Board/HAL port
- `Display/Inc/display_port.h`
- `Display/Src/display_port.c`

### Preserved controller source
- `Display/Inc/st7735.h`
- `Display/Src/st7735.c`
- `Display/Inc/st7735_reg.h`
- `Display/Src/st7735_reg.c`
- `Display/Inc/font.h`

## Documentation

- `CUBEMX_TFT_CONFIGURATION.md`
- `DISPLAY_USAGE.md`
- `DISPLAY_DESIGN.md`
- `DISPLAY_REFERENCE_RESEARCH.md`
- `DISPLAY_MODULE_MANIFEST.md`

## Design rule

The ST7735 initialization and panel geometry are the critical, board-specific
part. Do not replace them with a generic ST7735 setup without validating the
result against the WeAct board.

The graphics layer remains intentionally small: primitives, basic ASCII text,
RGB565 images, and a simple boot animation. A full GUI framework is out of
scope.
