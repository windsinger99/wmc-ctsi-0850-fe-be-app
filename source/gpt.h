/*
 * gpt.h
 *
 *  Created on: 2020. 2. 20.
 *      Author: lispect-shj
 */

#ifndef GPT_H_
#define GPT_H_

#include "common.h"
#define	GPT_TIME_SCALE_100US	7500
#define	GPT_TIME_SCALE_10US		750
#define	GPT_TIME_SCALE_1US		75

#define	GPT_TIME_SCALE_10MS		375	//750	// 75MHz /2000

#define WAIT_TIME_US(value)		((GPT_TIME_SCALE_10US * value) / 10)
#define WAIT_TIME_10MS(value)		((GPT_TIME_SCALE_10MS * value))

extern volatile bool gptIsrFlag;
extern uint32_t s_prevTime;

extern void GPT1_Time_Var_Init(void);
extern uint32_t s_get_frame_span(void);
extern uint32_t s_get_time_diff(void);
extern int DEBUG_delay(uint32_t cnt);

extern void Scan_Start_Time(void);
extern void Scan_Wait_Time(uint32_t wait_time);


#endif /* GPT_H_ */
