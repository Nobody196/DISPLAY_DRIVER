#ifndef DISPLAY_PORT_H
#define DISPLAY_PORT_H

#include <stdint.h>
#include <stdbool.h>

bool Display_Port_Init(void);
bool Display_Port_Write(const uint8_t *data, uint32_t length);
void Display_Port_CS_Low(void);
void Display_Port_CS_High(void);
void Display_Port_DC_Command(void);
void Display_Port_DC_Data(void);
void Display_Port_Backlight_Set(uint16_t level);
void Display_Port_Backlight_On(void);
void Display_Port_Backlight_Off(void);
uint32_t Display_Port_GetTick(void);
uint32_t Display_Port_DelayMs(uint32_t delay_ms);

#endif
