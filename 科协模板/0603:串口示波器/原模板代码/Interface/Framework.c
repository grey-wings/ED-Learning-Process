#include "Framework.h"

#if SYSTEM_TIMER==1

#define SYSTICK_1US_VALUE   (SYSTEM_TIMER_CLOCK/1000000)
#define SYSTICK_MAX_US      (16777216/SYSTICK_1US_VALUE)

#define SYSTICK_1MS_VALUE   (SYSTEM_TIMER_CLOCK/1000)
#define SYSTICK_MAX_MS      (16777216/SYSTICK_1MS_VALUE)
                                            
void _Delay_Us(uint32_t time)
{
    SystemTimer_Reset();
    SysTick->LOAD=SYSTICK_1US_VALUE*time;
    SysTick->VAL=0;
    SysTick->CTRL|=1;
    while (((SysTick->CTRL>>16)&1)==0);
    SystemTimer_Reset();
}

void _Delay_Ms(uint32_t time)
{
    SystemTimer_Reset();
    SysTick->LOAD=SYSTICK_1MS_VALUE*time;
    SysTick->VAL=0;
    SysTick->CTRL|=1;
    while (((SysTick->CTRL>>16)&1)==0);
    SystemTimer_Reset();
}

void SystemTimer_Delay_Us(uint32_t time)
{
    uint32_t integer=time/SYSTICK_MAX_US;
    uint32_t fraction=time%SYSTICK_MAX_US;
    while (integer--)
        _Delay_Us(SYSTICK_MAX_US);
    _Delay_Us(fraction);
}

void SystemTimer_Delay_Ms(uint32_t time)
{
    uint32_t integer=time/SYSTICK_MAX_MS;
    uint32_t fraction=time%SYSTICK_MAX_MS; 
    while (integer--)
        _Delay_Ms(SYSTICK_MAX_MS);
    _Delay_Ms(fraction);
}

void SystemTimer_Delay_S(uint32_t time)
{
    SystemTimer_Delay_Ms(time*1000);
}

void SystemTimer_Timing_Ready(float time_us)
{
    SystemTimer_Reset();
    SysTick->LOAD=SYSTICK_1US_VALUE*time_us;
    SysTick->VAL=0;
}

#elif SYSTEM_TIMER==2

#endif
