#include "ADKey.h"
#include "main.h"

extern uint16_t adc_value[20];
extern uint16_t readkey;

uint16_t midFliter(uint16_t *a,uint16_t len)
{	
	uint16_t temp;
	for (int i=0;i<len;i++)
	for (int j=i+1;j<len;j++)
	{
		if (a[i]>a[j])
		{
			temp=a[i];
			a[i]=a[j];
			a[j]=temp;
		}
	}
	if (len%2==0)
	{
		return (a[len/2]+a[len/2-1])/2;
	}
	else
	{
		return (a[len/2]);
	}
}

uint8_t adKey_scan()
{
	if (readkey < 268)
		return 1;
	else if (readkey < 867)
		return 2;
	else if (readkey < 1514)
		return 3;
	else if (readkey < 2133)
		return 4;
	else if (readkey < 2681)
		return 5;
	else if (readkey < 3218)
		return 6;
	else if (readkey < 3719)
		return 7;
	else if (readkey < 4005)
		return 8;
	else
		return 0;
}