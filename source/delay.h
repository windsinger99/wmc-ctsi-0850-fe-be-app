#ifndef _DELAY_H
#define _DELAY_H

#include "fsl_common.h"

#define DELAY_US(delay)		SDK_DelayAtLeastUs(delay, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY)

extern void Ctrl_Delay(uint32_t delay);
extern void LED_On_Delay(uint32_t delay, uint32_t max_delay);

#endif //_DELAY_H
