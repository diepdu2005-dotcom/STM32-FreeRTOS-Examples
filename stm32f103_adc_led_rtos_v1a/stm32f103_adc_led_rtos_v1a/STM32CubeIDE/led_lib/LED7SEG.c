#include "main.h"
#include "LED7SEG.h"

void Delay(uint16_t D)
{
	uint16_t i;
	for (i=0;i<D;i++);
	return;
}
////////////////////////////////////////////
void ShiftingLatch()
{
	HAL_GPIO_WritePin(LED7SEG_Port, STR, GPIO_PIN_SET);
	Delay(2);
	HAL_GPIO_WritePin(LED7SEG_Port, STR, GPIO_PIN_RESET);
	return;
}
///////////////////////////////////////////////
void ShiftingOut(unsigned char sdata)
{
	unsigned char i, buffer, tmp;
	for (i=0;i<8;i++){
		buffer = sdata;
		sdata = sdata << 1;
		tmp = buffer & 0x80;
		if (tmp) {
			HAL_GPIO_WritePin(LED7SEG_Port, Data, GPIO_PIN_SET);
		}
		else 
		{
			HAL_GPIO_WritePin(LED7SEG_Port, Data, GPIO_PIN_RESET);
		}
		HAL_GPIO_WritePin(LED7SEG_Port, SCK, GPIO_PIN_SET);
		Delay(2);
		HAL_GPIO_WritePin(LED7SEG_Port, SCK, GPIO_PIN_RESET);
	}
	return;
}
void Display_7_SEG(uint16_t value)
{
	unsigned char ledcode[10] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
	//unsigned char ledcode[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
	unsigned char ch0, ch1, ch2, ch3, ch4, ch5;
	HAL_GPIO_WritePin(LED7SEG_Port, OE, GPIO_PIN_RESET);
	ch5 = (value/100000)%10;
	ch4 = (value%100000)/10000;
	ch3 = (value%10000)/1000;
	ch2 = (value%1000)/100;
	ch1 = (value%100)/10;
	ch0 = value%10;
	ShiftingOut(ledcode[ch0]);
	ShiftingOut(ledcode[ch1]);
	ShiftingOut(ledcode[ch2]);
	ShiftingOut(ledcode[ch3]);
	ShiftingOut(ledcode[ch4]);
	ShiftingOut(ledcode[ch5]);
	ShiftingLatch();
}