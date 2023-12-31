/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v12.0
processor: MIMXRT1064xxxxA
package_id: MIMXRT1064DVL6A
mcu_data: ksdk2_0
processor_version: 13.0.2
board: MIMXRT1064-EVK
functionalGroups:
- name: BOARD_InitPeripherals
  UUID: 1c6563a6-c68b-40e5-8828-2853c99f95fa
  called_from_default_init: true
  id_prefix: BOARD_
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system_54b53072540eeeb8f8e9343e71f28176'
- global_system_definitions:
  - user_definitions: ''
  - user_includes: ''
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'uart_cmsis_common'
- type_id: 'uart_cmsis_common_9cb8e302497aa696fdbb5a4fd622c2a8'
- global_USART_CMSIS_common:
  - quick_selection: 'default'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'gpio_adapter_common'
- type_id: 'gpio_adapter_common_57579b9ac814fe26bf95df0a384c36b6'
- global_gpio_adapter_common:
  - quick_selection: 'default'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "peripherals.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * NVIC initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'NVIC'
- type: 'nvic'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'nvic_57b5eef3774cc60acaede6f5b8bddc67'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'NVIC'
- config_sets:
  - nvic:
    - interrupt_table:
      - 0: []
    - interrupts: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/* Empty initialization function (commented out)
static void BOARD_NVIC_init(void) {
} */

/***********************************************************************************************************************
 * LPUART1 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'LPUART1'
- type: 'lpuart'
- mode: 'polling'
- custom_name_enabled: 'false'
- type_id: 'lpuart_2053c37bb3ba5ee6d54a5fc8bcca8bfa'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'LPUART1'
- config_sets:
  - lpuartConfig_t:
    - lpuartConfig:
      - clockSource: 'LpuartClock'
      - lpuartSrcClkFreq: 'ClocksTool_DefaultInit'
      - baudRate_Bps: '115200'
      - parityMode: 'kLPUART_ParityDisabled'
      - dataBitsCount: 'kLPUART_EightDataBits'
      - isMsb: 'false'
      - stopBitCount: 'kLPUART_OneStopBit'
      - enableMatchAddress1: 'false'
      - matchAddress1: '0'
      - enableMatchAddress2: 'false'
      - matchAddress2: '0'
      - txFifoWatermark: '0'
      - rxFifoWatermark: '1'
      - enableRxRTS: 'false'
      - enableTxCTS: 'false'
      - txCtsSource: 'kLPUART_CtsSourcePin'
      - txCtsConfig: 'kLPUART_CtsSampleAtStart'
      - rxIdleType: 'kLPUART_IdleTypeStartBit'
      - rxIdleConfig: 'kLPUART_IdleCharacter1'
      - enableTx: 'true'
      - enableRx: 'true'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const lpuart_config_t BOARD_LPUART1_config = {
  .baudRate_Bps = 115200UL,
  .parityMode = kLPUART_ParityDisabled,
  .dataBitsCount = kLPUART_EightDataBits,
  .isMsb = false,
  .stopBitCount = kLPUART_OneStopBit,
  .txFifoWatermark = 0U,
  .rxFifoWatermark = 1U,
  .enableRxRTS = false,
  .enableTxCTS = false,
  .txCtsSource = kLPUART_CtsSourcePin,
  .txCtsConfig = kLPUART_CtsSampleAtStart,
  .rxIdleType = kLPUART_IdleTypeStartBit,
  .rxIdleConfig = kLPUART_IdleCharacter1,
  .enableTx = true,
  .enableRx = true
};

static void BOARD_LPUART1_init(void) {
  LPUART_Init(BOARD_LPUART1_PERIPHERAL, &BOARD_LPUART1_config, BOARD_LPUART1_CLOCK_SOURCE);
}

/***********************************************************************************************************************
 * WDOG1 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'WDOG1'
- type: 'wdog01'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'wdog01_510a9c5ba7e0ced8deac305549f23fa9'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'WDOG1'
- config_sets:
  - fsl_wdog:
    - wdogConfig:
      - clockSource: 'LowFreqClock'
      - timeoutValue_str: '2s'
      - enablePowerDown: 'false'
      - softwareResetExtension: 'true'
      - enableTimeOutAssert: 'false'
      - enableWdog: 'true'
      - workMode:
        - enableWait: 'false'
        - enableStop: 'false'
        - enableDebug: 'false'
    - interrupt_config:
      - enableInterrupt: 'true'
      - interruptTimeValue_str: '1'
      - interrupt_vectors:
        - enable_irq: 'true'
        - interrupt:
          - IRQn: 'WDOG1_IRQn'
          - enable_interrrupt: 'enabled'
          - enable_priority: 'true'
          - priority: '0'
          - enable_custom_name: 'false'
    - signals: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const wdog_config_t BOARD_WDOG1_config = {
  .timeoutValue = 3,
  .enablePowerDown = false,
  .softwareResetExtension = true,
  .enableTimeOutAssert = false,
  .enableWdog = true,
  .workMode = {
    .enableWait = false,
    .enableStop = false,
    .enableDebug = false
  }, 
  .enableInterrupt = true,
  .interruptTimeValue = 0
};

static void BOARD_WDOG1_init(void) {
  /* WDOG peripheral initialization */
  WDOG_Init(BOARD_WDOG1_PERIPHERAL, &BOARD_WDOG1_config);
  /* Interrupt vector WDOG1_IRQn priority settings in the NVIC. */
  NVIC_SetPriority(BOARD_WDOG1_IRQN, BOARD_WDOG1_IRQ_PRIORITY);
  /* Enable interrupt WDOG1_IRQn request in the NVIC. */
  EnableIRQ(BOARD_WDOG1_IRQN);
}

/***********************************************************************************************************************
 * ADC1 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'ADC1'
- type: 'adc_12b1msps_sar'
- mode: 'ADC_GENERAL'
- custom_name_enabled: 'false'
- type_id: 'adc_12b1msps_sar_6a490e886349a7b2b07bed10ce7b299b'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'ADC1'
- config_sets:
  - fsl_adc:
    - clockConfig:
      - clockSource: 'kADC_ClockSourceIPG'
      - clockSourceFreq: 'ClocksTool_DefaultInit'
      - clockDriver: 'kADC_ClockDriver2'
      - samplePeriodMode: 'kADC_SamplePeriodShort2Clocks'
      - enableAsynchronousClockOutput: 'false'
    - conversionConfig:
      - resolution: 'kADC_Resolution8Bit'
      - hardwareAverageMode: 'kADC_HardwareAverageDisable'
      - enableHardwareTrigger: 'software'
      - enableHighSpeed: 'true'
      - enableLowPower: 'false'
      - enableContinuousConversion: 'true'
      - enableOverWrite: 'false'
      - enableDma: 'false'
    - resultingTime: []
    - resultCorrection:
      - doAutoCalibration: 'true'
      - offset: '0'
    - hardwareCompareConfiguration:
      - hardwareCompareMode: 'disabled'
      - value1: '0'
      - value2: '0'
    - enableInterrupt: 'false'
    - adc_interrupt:
      - IRQn: 'ADC1_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
    - adc_channels_config:
      - 0:
        - channelNumber: 'IN.11'
        - channelName: ''
        - channelGroup: '0'
        - initializeChannel: 'false'
        - enableInterruptOnConversionCompleted: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const adc_config_t BOARD_ADC1_config = {
  .enableOverWrite = false,
  .enableContinuousConversion = true,
  .enableHighSpeed = true,
  .enableLowPower = false,
  .enableLongSample = false,
  .enableAsynchronousClockOutput = false,
  .referenceVoltageSource = kADC_ReferenceVoltageSourceAlt0,
  .samplePeriodMode = kADC_SamplePeriodShort2Clocks,
  .clockSource = kADC_ClockSourceIPG,
  .clockDriver = kADC_ClockDriver2,
  .resolution = kADC_Resolution8Bit
};
const adc_channel_config_t BOARD_ADC1_channels_config[1] = {
  {
    .channelNumber = 11U,
    .enableInterruptOnConversionCompleted = false
  }
};
static void BOARD_ADC1_init(void) {
  /* Initialize ADC1 peripheral. */
  ADC_Init(BOARD_ADC1_PERIPHERAL, &BOARD_ADC1_config);
  /* Perform ADC1 auto calibration. */
  ADC_DoAutoCalibration(BOARD_ADC1_PERIPHERAL);
}

/***********************************************************************************************************************
 * ADC2 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'ADC2'
- type: 'adc_12b1msps_sar'
- mode: 'ADC_GENERAL'
- custom_name_enabled: 'false'
- type_id: 'adc_12b1msps_sar_6a490e886349a7b2b07bed10ce7b299b'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'ADC2'
- config_sets:
  - fsl_adc:
    - clockConfig:
      - clockSource: 'kADC_ClockSourceIPG'
      - clockSourceFreq: 'ClocksTool_DefaultInit'
      - clockDriver: 'kADC_ClockDriver2'
      - samplePeriodMode: 'kADC_SamplePeriodShort2Clocks'
      - enableAsynchronousClockOutput: 'false'
    - conversionConfig:
      - resolution: 'kADC_Resolution8Bit'
      - hardwareAverageMode: 'kADC_HardwareAverageDisable'
      - enableHardwareTrigger: 'software'
      - enableHighSpeed: 'true'
      - enableLowPower: 'false'
      - enableContinuousConversion: 'true'
      - enableOverWrite: 'false'
      - enableDma: 'false'
    - resultingTime: []
    - resultCorrection:
      - doAutoCalibration: 'true'
      - offset: '0'
    - hardwareCompareConfiguration:
      - hardwareCompareMode: 'disabled'
      - value1: '0'
      - value2: '0'
    - enableInterrupt: 'false'
    - adc_interrupt:
      - IRQn: 'ADC2_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
    - adc_channels_config:
      - 0:
        - channelNumber: 'IN.12'
        - channelName: ''
        - channelGroup: '0'
        - initializeChannel: 'false'
        - enableInterruptOnConversionCompleted: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const adc_config_t BOARD_ADC2_config = {
  .enableOverWrite = false,
  .enableContinuousConversion = true,
  .enableHighSpeed = true,
  .enableLowPower = false,
  .enableLongSample = false,
  .enableAsynchronousClockOutput = false,
  .referenceVoltageSource = kADC_ReferenceVoltageSourceAlt0,
  .samplePeriodMode = kADC_SamplePeriodShort2Clocks,
  .clockSource = kADC_ClockSourceIPG,
  .clockDriver = kADC_ClockDriver2,
  .resolution = kADC_Resolution8Bit
};
const adc_channel_config_t BOARD_ADC2_channels_config[1] = {
  {
    .channelNumber = 12U,
    .enableInterruptOnConversionCompleted = false
  }
};
static void BOARD_ADC2_init(void) {
  /* Initialize ADC2 peripheral. */
  ADC_Init(BOARD_ADC2_PERIPHERAL, &BOARD_ADC2_config);
  /* Perform ADC2 auto calibration. */
  ADC_DoAutoCalibration(BOARD_ADC2_PERIPHERAL);
}

/***********************************************************************************************************************
 * TMR1 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'TMR1'
- type: 'qtmr'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'qtmr_460dd7aa3f3371843c2548acd54252b0'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'TMR1'
- config_sets:
  - fsl_qtmr:
    - clockSource: 'BusInterfaceClock'
    - clockSourceFreq: 'ClocksTool_DefaultInit'
    - qtmr_channels:
      - 0:
        - channel_prefix_id: 'Channel_0'
        - channel: 'kQTMR_Channel_0'
        - primarySource: 'kQTMR_ClockDivide_1'
        - secondarySource: 'kQTMR_Counter1InputPin'
        - countingMode: 'kQTMR_NoOperation'
        - enableMasterMode: 'false'
        - enableExternalForce: 'false'
        - faultFilterCount: '3'
        - faultFilterPeriod: '0'
        - debugMode: 'kQTMR_RunNormalInDebug'
        - timerModeInit: 'pwmOutput'
        - pwmMode:
          - freq_value_str: '435'
          - dutyCyclePercent: '34'
          - outputPolarity: 'true'
        - dmaIntMode: 'polling'
    - interruptVector:
      - enable_irq: 'false'
      - interrupt:
        - IRQn: 'TMR1_IRQn'
        - enable_interrrupt: 'enabled'
        - enable_priority: 'false'
        - priority: '0'
        - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const qtmr_config_t BOARD_TMR1_Channel_0_config = {
  .primarySource = kQTMR_ClockDivide_1,
  .secondarySource = kQTMR_Counter1InputPin,
  .enableMasterMode = false,
  .enableExternalForce = false,
  .faultFilterCount = 0,
  .faultFilterPeriod = 0,
  .debugMode = kQTMR_RunNormalInDebug
};

static void BOARD_TMR1_init(void) {
  /* Quad timer channel Channel_0 peripheral initialization */
  QTMR_Init(BOARD_TMR1_PERIPHERAL, BOARD_TMR1_CHANNEL_0_CHANNEL, &BOARD_TMR1_Channel_0_config);
  /* Setup the PWM mode of the timer channel */
  QTMR_SetupPwm(BOARD_TMR1_PERIPHERAL, BOARD_TMR1_CHANNEL_0_CHANNEL, 344828UL, 34U, true, BOARD_TMR1_CHANNEL_0_CLOCK_SOURCE);
}

/***********************************************************************************************************************
 * TMR2 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'TMR2'
- type: 'qtmr'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'qtmr_460dd7aa3f3371843c2548acd54252b0'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'TMR2'
- config_sets:
  - fsl_qtmr:
    - clockSource: 'BusInterfaceClock'
    - clockSourceFreq: 'ClocksTool_DefaultInit'
    - qtmr_channels:
      - 0:
        - channel_prefix_id: 'Channel_3'
        - channel: 'kQTMR_Channel_3'
        - primarySource: 'kQTMR_ClockDivide_1'
        - secondarySource: 'kQTMR_Counter1InputPin'
        - countingMode: 'kQTMR_NoOperation'
        - enableMasterMode: 'false'
        - enableExternalForce: 'false'
        - faultFilterCount: '3'
        - faultFilterPeriod: '0'
        - debugMode: 'kQTMR_RunNormalInDebug'
        - timerModeInit: 'pwmOutput'
        - pwmMode:
          - freq_value_str: '435'
          - dutyCyclePercent: '34'
          - outputPolarity: 'true'
        - dmaIntMode: 'polling'
    - interruptVector:
      - enable_irq: 'false'
      - interrupt:
        - IRQn: 'TMR2_IRQn'
        - enable_interrrupt: 'enabled'
        - enable_priority: 'false'
        - priority: '0'
        - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const qtmr_config_t BOARD_TMR2_Channel_3_config = {
  .primarySource = kQTMR_ClockDivide_1,
  .secondarySource = kQTMR_Counter1InputPin,
  .enableMasterMode = false,
  .enableExternalForce = false,
  .faultFilterCount = 0,
  .faultFilterPeriod = 0,
  .debugMode = kQTMR_RunNormalInDebug
};

static void BOARD_TMR2_init(void) {
  /* Quad timer channel Channel_3 peripheral initialization */
  QTMR_Init(BOARD_TMR2_PERIPHERAL, BOARD_TMR2_CHANNEL_3_CHANNEL, &BOARD_TMR2_Channel_3_config);
  /* Setup the PWM mode of the timer channel */
  QTMR_SetupPwm(BOARD_TMR2_PERIPHERAL, BOARD_TMR2_CHANNEL_3_CHANNEL, 344828UL, 34U, true, BOARD_TMR2_CHANNEL_3_CLOCK_SOURCE);
}

/***********************************************************************************************************************
 * TMR3 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'TMR3'
- type: 'qtmr'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'qtmr_460dd7aa3f3371843c2548acd54252b0'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'TMR3'
- config_sets:
  - fsl_qtmr:
    - clockSource: 'BusInterfaceClock'
    - clockSourceFreq: 'ClocksTool_DefaultInit'
    - qtmr_channels:
      - 0:
        - channel_prefix_id: 'Channel_0'
        - channel: 'kQTMR_Channel_0'
        - primarySource: 'kQTMR_ClockDivide_1'
        - secondarySource: 'kQTMR_Counter1InputPin'
        - countingMode: 'kQTMR_NoOperation'
        - enableMasterMode: 'false'
        - enableExternalForce: 'false'
        - faultFilterCount: '3'
        - faultFilterPeriod: '0'
        - debugMode: 'kQTMR_RunNormalInDebug'
        - timerModeInit: 'pwmOutput'
        - pwmMode:
          - freq_value_str: '445'
          - dutyCyclePercent: '10'
          - outputPolarity: 'false'
        - dmaIntMode: 'polling'
    - interruptVector:
      - enable_irq: 'false'
      - interrupt:
        - IRQn: 'TMR3_IRQn'
        - enable_interrrupt: 'enabled'
        - enable_priority: 'false'
        - priority: '0'
        - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const qtmr_config_t BOARD_TMR3_Channel_0_config = {
  .primarySource = kQTMR_ClockDivide_1,
  .secondarySource = kQTMR_Counter1InputPin,
  .enableMasterMode = false,
  .enableExternalForce = false,
  .faultFilterCount = 0,
  .faultFilterPeriod = 0,
  .debugMode = kQTMR_RunNormalInDebug
};

static void BOARD_TMR3_init(void) {
  /* Quad timer channel Channel_0 peripheral initialization */
  QTMR_Init(BOARD_TMR3_PERIPHERAL, BOARD_TMR3_CHANNEL_0_CHANNEL, &BOARD_TMR3_Channel_0_config);
  /* Setup the PWM mode of the timer channel */
  QTMR_SetupPwm(BOARD_TMR3_PERIPHERAL, BOARD_TMR3_CHANNEL_0_CHANNEL, 337079UL, 10U, false, BOARD_TMR3_CHANNEL_0_CLOCK_SOURCE);
}

/***********************************************************************************************************************
 * TMR4 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'TMR4'
- type: 'qtmr'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'qtmr_460dd7aa3f3371843c2548acd54252b0'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'TMR4'
- config_sets:
  - fsl_qtmr:
    - clockSource: 'BusInterfaceClock'
    - clockSourceFreq: 'ClocksTool_DefaultInit'
    - qtmr_channels:
      - 0:
        - channel_prefix_id: 'Channel_3'
        - channel: 'kQTMR_Channel_3'
        - primarySource: 'kQTMR_ClockDivide_1'
        - secondarySource: 'kQTMR_Counter1InputPin'
        - countingMode: 'kQTMR_NoOperation'
        - enableMasterMode: 'false'
        - enableExternalForce: 'false'
        - faultFilterCount: '3'
        - faultFilterPeriod: '0'
        - debugMode: 'kQTMR_RunNormalInDebug'
        - timerModeInit: 'pwmOutput'
        - pwmMode:
          - freq_value_str: '25'
          - dutyCyclePercent: '50'
          - outputPolarity: 'false'
        - dmaIntMode: 'polling'
    - interruptVector:
      - enable_irq: 'false'
      - interrupt:
        - IRQn: 'TMR4_IRQn'
        - enable_interrrupt: 'enabled'
        - enable_priority: 'false'
        - priority: '0'
        - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const qtmr_config_t BOARD_TMR4_Channel_3_config = {
  .primarySource = kQTMR_ClockDivide_1,
  .secondarySource = kQTMR_Counter1InputPin,
  .enableMasterMode = false,
  .enableExternalForce = false,
  .faultFilterCount = 0,
  .faultFilterPeriod = 0,
  .debugMode = kQTMR_RunNormalInDebug
};

static void BOARD_TMR4_init(void) {
  /* Quad timer channel Channel_3 peripheral initialization */
  QTMR_Init(BOARD_TMR4_PERIPHERAL, BOARD_TMR4_CHANNEL_3_CHANNEL, &BOARD_TMR4_Channel_3_config);
  /* Setup the PWM mode of the timer channel */
  QTMR_SetupPwm(BOARD_TMR4_PERIPHERAL, BOARD_TMR4_CHANNEL_3_CHANNEL, 6000000UL, 50U, false, BOARD_TMR4_CHANNEL_3_CLOCK_SOURCE);
}

/***********************************************************************************************************************
 * GPT1 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'GPT1'
- type: 'gpt'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'gpt_e92a0cbd07e389b82a1d19b05eb9fdda'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'GPT1'
- config_sets:
  - fsl_gpt:
    - gpt_config:
      - clockSource: 'kGPT_ClockSource_Periph'
      - clockSourceFreq: 'ClocksTool_DefaultInit'
      - oscDivider: '1'
      - divider: '1'
      - enableFreeRun: 'true'
      - enableRunInWait: 'false'
      - enableRunInStop: 'false'
      - enableRunInDoze: 'false'
      - enableRunInDbg: 'false'
      - enableMode: 'true'
    - input_capture_channels: []
    - output_compare_channels: []
    - interrupt_requests: ''
    - isInterruptEnabled: 'false'
    - interrupt:
      - IRQn: 'GPT1_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
    - EnableTimerInInit: 'true'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const gpt_config_t BOARD_GPT1_config = {
  .clockSource = kGPT_ClockSource_Periph,
  .divider = 1UL,
  .enableFreeRun = true,
  .enableRunInWait = false,
  .enableRunInStop = false,
  .enableRunInDoze = false,
  .enableRunInDbg = false,
  .enableMode = true
};

static void BOARD_GPT1_init(void) {
  /* GPT device and channels initialization */
  GPT_Init(BOARD_GPT1_PERIPHERAL, &BOARD_GPT1_config);
  GPT_SetOscClockDivider(BOARD_GPT1_PERIPHERAL, 1);
  /* Enable GPT interrupt sources */
  GPT_EnableInterrupts(BOARD_GPT1_PERIPHERAL, 0);
  /* Start the GPT timer */ 
  GPT_StartTimer(BOARD_GPT1_PERIPHERAL);
}

/***********************************************************************************************************************
 * GPT2 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'GPT2'
- type: 'gpt'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'gpt_e92a0cbd07e389b82a1d19b05eb9fdda'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'GPT2'
- config_sets:
  - fsl_gpt:
    - gpt_config:
      - clockSource: 'kGPT_ClockSource_Periph'
      - clockSourceFreq: 'ClocksTool_DefaultInit'
      - oscDivider: '1'
      - divider: '2000'
      - enableFreeRun: 'true'
      - enableRunInWait: 'false'
      - enableRunInStop: 'false'
      - enableRunInDoze: 'false'
      - enableRunInDbg: 'false'
      - enableMode: 'true'
    - input_capture_channels: []
    - output_compare_channels: []
    - interrupt_requests: ''
    - isInterruptEnabled: 'false'
    - interrupt:
      - IRQn: 'GPT2_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
    - EnableTimerInInit: 'true'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const gpt_config_t BOARD_GPT2_config = {
  .clockSource = kGPT_ClockSource_Periph,
  .divider = 2000UL,
  .enableFreeRun = true,
  .enableRunInWait = false,
  .enableRunInStop = false,
  .enableRunInDoze = false,
  .enableRunInDbg = false,
  .enableMode = true
};

static void BOARD_GPT2_init(void) {
  /* GPT device and channels initialization */
  GPT_Init(BOARD_GPT2_PERIPHERAL, &BOARD_GPT2_config);
  GPT_SetOscClockDivider(BOARD_GPT2_PERIPHERAL, 1);
  /* Enable GPT interrupt sources */
  GPT_EnableInterrupts(BOARD_GPT2_PERIPHERAL, 0);
  /* Start the GPT timer */ 
  GPT_StartTimer(BOARD_GPT2_PERIPHERAL);
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void)
{
  /* Initialize components */
  BOARD_LPUART1_init();
  BOARD_WDOG1_init();
  BOARD_ADC1_init();
  BOARD_ADC2_init();
  BOARD_TMR1_init();
  BOARD_TMR2_init();
  BOARD_TMR3_init();
  BOARD_TMR4_init();
  BOARD_GPT1_init();
  BOARD_GPT2_init();
}

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void)
{
  BOARD_InitPeripherals();
}
