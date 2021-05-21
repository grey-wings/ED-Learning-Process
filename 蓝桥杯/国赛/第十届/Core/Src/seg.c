#include "main.h"
#include "seg.h"

uint8_t segNum[12]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x39};

void seg_show(uint8_t num1,uint8_t num2,uint8_t num3)
{
	uint8_t num,i;
	num=segNum[num3];
	for (i=0;i<8;i++)
	{
		if (num & 0x80)
			SER_H;
		else
			SER_L;
		SCK_H;
		num <<= 1;
		SCK_L;
	}
	num=segNum[num2];
	for (i=0;i<8;i++)
	{
		if (num & 0x80)
			SER_H;
		else
			SER_L;
		SCK_H;
		num <<= 1;
		SCK_L;
	}
	num=segNum[num1];
	for (i=0;i<8;i++)
	{
		if (num & 0x80)
			SER_H;
		else
			SER_L;
		SCK_H;
		num <<= 1;
		SCK_L;
	}
	RCK_H;
	RCK_L;
}