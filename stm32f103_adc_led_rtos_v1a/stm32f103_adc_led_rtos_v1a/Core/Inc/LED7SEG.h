#include "stm32f1xx_hal.h"

#define LED7SEG_Port GPIOB

#define SCK  GPIO_PIN_0
#define STR  GPIO_PIN_1
#define OE   GPIO_PIN_8
#define Data  GPIO_PIN_9

void Delay(uint16_t D);
void ShiftingLatch();
void ShiftingOut(unsigned char sdata);
void Display_7_SEG(uint16_t value);
