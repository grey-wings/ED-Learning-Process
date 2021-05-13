#include "seg.h"

uint8_t segNum[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x72,0x00};

void seg_showNum(uint8_t num1,uint8_t num2,uint8_t num3)
{
	uint8_t b=segNum[num3];
	for (int i=0;i<8;i++)
	{
		if (b & 0x80)
		{
			SER_H;
		}
		else
		{
			SER_L;
		}
		SCK_H;
		b <<= 1;
		SCK_L;
	}
	
	b=segNum[num2];
	for (int i=0;i<8;i++)
	{
		if (b & 0x80)
		{
			SER_H;
		}
		else
		{
			SER_L;
		}
		SCK_H;
		b <<= 1;
		SCK_L;
	}
	
	b=segNum[num1];
	for (int i=0;i<8;i++)
	{
		if (b & 0x80)
		{
			SER_H;
		}
		else
		{
			SER_L;
		}
		SCK_H;
		b <<= 1;
		SCK_L;
	}
	RCK_H;
	RCK_L;
}
