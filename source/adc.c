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
	ADC_SetChannelConfig(BOARD_ADC1_PERIPHERAL, ADC1_CHANNEL_GROUP, &BOARD_ADC1_channels_config[0]);
	ADC_SetChannelConfig(BOARD_ADC2_PERIPHERAL, ADC2_CHANNEL_GROUP, &BOARD_ADC2_channels_config[0]);
}


void ADC_SamplesStop(void)
{
  ADC1->HC[0] = ADC_HC_ADCH_MASK;
  ADC2->HC[0] = ADC_HC_ADCH_MASK;
}

uint8_t *ADC_SamplesRead(void)
{
	static uint8_t adc_buff[2];

	ADC_SetChannelConfig(BOARD_ADC1_PERIPHERAL, ADC1_CHANNEL_GROUP, &BOARD_ADC1_channels_config[0]);
	ADC_SetChannelConfig(BOARD_ADC2_PERIPHERAL, ADC2_CHANNEL_GROUP, &BOARD_ADC2_channels_config[0]);

	while (0U == ADC_GetChannelStatusFlags(BOARD_ADC1_PERIPHERAL, ADC1_CHANNEL_GROUP)) {}
	while (0U == ADC_GetChannelStatusFlags(BOARD_ADC2_PERIPHERAL, ADC2_CHANNEL_GROUP)) {}
	adc_buff[0] = (uint8_t)ADC_GetChannelConversionValue(BOARD_ADC1_PERIPHERAL, ADC1_CHANNEL_GROUP);
	adc_buff[1] = (uint8_t)ADC_GetChannelConversionValue(BOARD_ADC2_PERIPHERAL, ADC2_CHANNEL_GROUP);

	return &adc_buff[0];
}

void ADC_Conversion_State_Flag(void)
{
  while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ){;} // 0
}
