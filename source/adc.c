/*
 * adc.c
 *
 *  Created on: 2020. 3. 31.
 *      Author: lispect-shj
 */

#include "adc.h"
#include "peripherals.h"

inline __attribute__((always_inline))
void ADC_SamplesStart(void)
{
	ADC_SetChannelConfig(ADC1_PERIPHERAL, ADC1_CHANNEL_GROUP, &ADC1_channels_config[0]);
	ADC_SetChannelConfig(ADC2_PERIPHERAL, ADC2_CHANNEL_GROUP, &ADC2_channels_config[0]);
}


void ADC_SamplesStop(void)
{
  ADC1->HC[0] = ADC_HC_ADCH_MASK;
  ADC2->HC[0] = ADC_HC_ADCH_MASK;
}

uint8_t *ADC_SamplesRead(void)
{
	static uint8_t adc_buff[2];

	ADC_SetChannelConfig(ADC1_PERIPHERAL, ADC1_CHANNEL_GROUP, &ADC1_channels_config[0]);
	ADC_SetChannelConfig(ADC2_PERIPHERAL, ADC2_CHANNEL_GROUP, &ADC2_channels_config[0]);

	while (0U == ADC_GetChannelStatusFlags(ADC1_PERIPHERAL, ADC1_CHANNEL_GROUP)) {}
	while (0U == ADC_GetChannelStatusFlags(ADC2_PERIPHERAL, ADC2_CHANNEL_GROUP)) {}
	adc_buff[0] = (uint8_t)ADC_GetChannelConversionValue(ADC1_PERIPHERAL, ADC1_CHANNEL_GROUP);
	adc_buff[1] = (uint8_t)ADC_GetChannelConversionValue(ADC2_PERIPHERAL, ADC2_CHANNEL_GROUP);

	return &adc_buff[0];
}

void ADC_Conversion_State_Flag(void)
{
  while ((ADC1_PERIPHERAL->HS & 1UL) == 0 && (ADC2_PERIPHERAL->HS & 1UL) == 0 ){;} // 0
}
