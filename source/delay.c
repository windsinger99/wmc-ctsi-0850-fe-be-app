#include "delay.h"
#include "app.h"

void Ctrl_Delay(uint32_t delay)
{
	volatile uint32_t cnt;
	for(cnt=0; cnt<delay; cnt++) __NOP();
}

void LED_On_Delay(uint32_t delay, uint32_t max_delay)
{
	volatile uint32_t cnt;
	if(delay > max_delay)
		delay = max_delay;

	for(cnt=0; cnt<delay; cnt++) __NOP();
}

