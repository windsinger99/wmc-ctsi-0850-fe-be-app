/*
 * adc.h
 *
 *  Created on: 2020. 3. 31.
 *      Author: lispect-shj
 */

#ifndef ADC_H_
#define ADC_H_

#include "fsl_common.h"

#define ADC1_CHANNEL_GROUP (0U)
#define ADC2_CHANNEL_GROUP (0U)

extern void ADC_SamplesStart(void);
extern void ADC_SamplesStop(void);
extern uint8_t *ADC_SamplesRead(void);
extern void ADC_Conversion_State_Flag(void);
#endif /* ADC_H_ */
