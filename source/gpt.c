/*
 * gpt.c
 *
 *  Created on: 2020. 2. 20.
 *      Author: lispect-shj
 */

#include "gpt.h"
//#include "wdog.h"
#include "fsl_gpt.h"
#include "peripherals.h"

volatile bool gptIsrFlag = false;
uint32_t s_prevTime, s_prevTime2;
volatile uint32_t start_scan_ctrl_time, scan_ctrl_time;
void GPT_2_GPT_IRQHANDLER(void)
{
    /* Clear interrupt flag.*/
    GPT_ClearStatusFlags(BOARD_GPT2_PERIPHERAL, kGPT_OutputCompare1Flag);

    gptIsrFlag = true;
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F, Cortex-M7, Cortex-M7F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U || __CORTEX_M == 7U)
    __DSB();
#endif
}

void GPT1_Time_Var_Init(void)
{
    s_prevTime = 0;
    s_prevTime2 = 0;
    start_scan_ctrl_time = 0;
    scan_ctrl_time = 0;
}

/*
uint32_t s_get_frame_span(void)
{
	uint32_t diffTime;
	uint32_t curTime = GPT_GetCurrentTimerCount(GPT1_PERIPHERAL);
	if (curTime > s_prevTime) {
		diffTime = curTime - s_prevTime;
	}
	else {
		diffTime = (0xFFFFFFFF - s_prevTime + 1) + curTime;
	}
	s_prevTime = curTime;
	diffTime = diffTime / GPT_TIME_SCALE_100US;
	//TRACE("BG_get_frame_span: %d(%x) %d(%x) %d(%x)", diffTime, diffTime, curTime, curTime, s_prevTime, s_prevTime);
    return (diffTime);
}

uint32_t s_get_time_diff(void)
{
	uint32_t diffTime;
	uint32_t curTime = GPT_GetCurrentTimerCount(GPT1_PERIPHERAL);
	if (curTime > s_prevTime2) {
		diffTime = curTime - s_prevTime2;
	}
	else {
		diffTime = (0xFFFFFFFF - s_prevTime2 + 1) + curTime;
	}
	//TRACE("BG_get_time_diff: %d(%x) %d(%x) %d(%x)", diffTime, diffTime, curTime, curTime, s_prevTime2, s_prevTime2);
	s_prevTime2 = curTime;
    return (diffTime / GPT_TIME_SCALE_100US);
}
*/
int DEBUG_delay(uint32_t cnt)
{
    volatile uint32_t i = 0;
    uint32_t cntTotal = cnt * 1000;
    for (i = 0; i < cntTotal; ++i)
    {
        __asm("NOP"); /* delay */
    }
    return 0;
}

uint32_t tttt;
uint32_t rrrr;

void Scan_Start_Time(void)
{
	start_scan_ctrl_time = GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL);
}

void Scan_Wait_Time(uint32_t wait_time)
{
	volatile uint32_t diffTime = 0;
	volatile uint32_t curTime;
	while (diffTime < wait_time)
	{
		//WDOG_Reset_Refresh();	//YJ@221122
		curTime = GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL);

		if (curTime > start_scan_ctrl_time)
		{
			diffTime = curTime - start_scan_ctrl_time;
		}
		else
		{
			diffTime = (0xFFFFFFFF - start_scan_ctrl_time + 1) + curTime;
		}
	}
}
