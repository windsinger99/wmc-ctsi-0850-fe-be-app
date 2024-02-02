/*
 * gpio.h
 *
 *  Created on: 2020. 3. 31.
 *      Author: lispect-shj
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "peripherals.h"
#include "fsl_gpio.h"

#define GPIO_HIGH (1U)
#define GPIO_LOW  (0U)

#if 1

typedef enum {
  nop                     = 0,
  MCU_PD_TEST_TRIGGER     = 14U,  // GPIO1 // TP6
  MCU_LED_TEST_TRIGGER    = 15U,   // GPIO1 // TP7

  ADD_MUX11               = 19U,  // GPIO1
  ADD_MUX12               = 20U,  // GPIO1
  ADD_MUX13               = 21U,  // GPIO1

  TEST_POING_1            = 24U,
  TEST_POING_2            = 25U,
  TEST_POING_3            = 26U,

  MCU_LED_HGROUP_DATA     = 27U,  // GPIO1
#if (MODEL_TYPE == WMC_0750_NXP_VE)
  MCU_ADC_GROUP_SELECT_0  = 28U,  // GPIO1
  MCU_ADC_GROUP_SELECT_1  = 29U,  // GPIO1
  MCU_TR_GROUP_SELECT_0   = 30U,  // GPIO1
  MCU_TR_GROUP_SELECT_1   = 31U  // GPIO1
#else
  MCU_ADC_GROUP_SELECT_0  = 30U,  // GPIO1
  MCU_ADC_GROUP_SELECT_1  = 31U  // GPIO1
#endif


} GPIO_CTRL_BIT_1_T;

typedef enum {
  MCU_LED_CELL_CTRL1      = 1U,   // GPIO2
  MCU_LED_CELL_CTRL2      = 2U,   // GPIO2
  MCU_LED_CELL_CTRL3      = 3U,   // GPIO2

  MCU_LED_SINK_DAC0       = 16U,  // GPIO2
  MCU_LED_SINK_DAC1       = 17U,  // GPIO2
  MCU_LED_SINK_DAC2       = 18U,  // GPIO2
  MCU_LED_SINK_DAC3       = 19U,  // GPIO2
  MCU_LED_SINK_DAC4       = 20U,  // GPIO2
  MCU_LED_SINK_DAC5       = 21U,  // GPIO2
  MCU_LED_SINK_DAC6       = 22U,  // GPIO2
  MCU_LED_SINK_DAC7       = 23U,  // GPIO2

  MCU_PD_SHIFT_CLK        = 24U,   // GPIO2

  MCU_LED_GROUP_EN        = 26U,   // GPIO2

  MCU_PD_SHIFT_DATA       = 30U   // GPIO2
} GPIO_CTRL_BIT_2_T;
extern void Scan_GPIO_PortSet(GPIO_Type *base, uint32_t pin, uint32_t mask);

#define MCU_ADC0        11U         //ADC1
#define MCU_ADC1        12U         //ADC2



/*****************************************************************************/
// Macros for ADC_GROUP_SELECT_0 pin(MCU_ADC_GROUP_SELECT_0) - GPIO_PIN_30U
/*****************************************************************************/
#define MCU_ADC_GROUP_SELECT_0_Set()            (GPIO1->DR_SET = 1UL<< MCU_ADC_GROUP_SELECT_0)
#define MCU_ADC_GROUP_SELECT_0_Clear()          (GPIO1->DR_CLEAR = 1UL<< MCU_ADC_GROUP_SELECT_0)
#define MCU_ADC_GROUP_SELECT_0_Toggle()         (GPIO1->DR_TOGGLE = 1UL<< MCU_ADC_GROUP_SELECT_0)


/*****************************************************************************/
// Macros for ADC_GROUP_SELECT_1 pin(MCU_ADC_GROUP_SELECT_1) - GPIO_PIN_31U
/*****************************************************************************/
#define MCU_ADC_GROUP_SELECT_1_Set()            (GPIO1->DR_SET = 1UL<< MCU_ADC_GROUP_SELECT_1)
#define MCU_ADC_GROUP_SELECT_1_Clear()          (GPIO1->DR_CLEAR = 1UL<< MCU_ADC_GROUP_SELECT_1)
#define MCU_ADC_GROUP_SELECT_1_Toggle()         (GPIO1->DR_TOGGLE = 1UL<< MCU_ADC_GROUP_SELECT_1)

/*****************************************************************************/
// Macros for ADC_GROUP_SELECT_0 pin(MCU_ADC_GROUP_SELECT_0) - GPIO_PIN_30U
/*****************************************************************************/
#define MCU_TR_GROUP_SELECT_0_Set()            (GPIO1->DR_SET = 1UL<< MCU_TR_GROUP_SELECT_0)
#define MCU_TR_GROUP_SELECT_0_Clear()          (GPIO1->DR_CLEAR = 1UL<< MCU_TR_GROUP_SELECT_0)
#define MCU_TR_GROUP_SELECT_0_Toggle()         (GPIO1->DR_TOGGLE = 1UL<< MCU_TR_GROUP_SELECT_0)


/*****************************************************************************/
// Macros for ADC_GROUP_SELECT_1 pin(MCU_ADC_GROUP_SELECT_1) - GPIO_PIN_31U
/*****************************************************************************/
#define MCU_TR_GROUP_SELECT_1_Set()            (GPIO1->DR_SET = 1UL<< MCU_TR_GROUP_SELECT_1)
#define MCU_TR_GROUP_SELECT_1_Clear()          (GPIO1->DR_CLEAR = 1UL<< MCU_TR_GROUP_SELECT_1)
#define MCU_TR_GROUP_SELECT_1_Toggle()         (GPIO1->DR_TOGGLE = 1UL<< MCU_TR_GROUP_SELECT_1)


/*****************************************************************************/
// Macros for LED_TEST_TRIGGER(DEBUG_TP1) pin(MCU_LED_TEST_TRIGGER) - GPIO_PIN_15U
/*****************************************************************************/
#define DEBUG_TP1_HIGH()               (GPIO1->DR_SET = 1UL<< MCU_LED_TEST_TRIGGER)
#define DEBUG_TP1_LOW()             (GPIO1->DR_CLEAR = 1UL<< MCU_LED_TEST_TRIGGER)
#define DEBUG_TP1_Toggle()            (GPIO1->DR_TOGGLE = 1UL<< MCU_LED_TEST_TRIGGER)


/*****************************************************************************/
// Macros for PD_TEST_TRIGGER(DEBUG_TP2) pin(MCU_PD_TEST_TRIGGER) - GPIO_PIN_14U
/*****************************************************************************/
#define DEBUG_TP2_HIGH()               (GPIO1->DR_SET = 1UL<< MCU_PD_TEST_TRIGGER)
#define DEBUG_TP2_LOW()             (GPIO1->DR_CLEAR = 1UL<< MCU_PD_TEST_TRIGGER)
#define DEBUG_TP2_Toggle()            (GPIO1->DR_TOGGLE = 1UL<< MCU_PD_TEST_TRIGGER)


/*****************************************************************************/
// Macros for ADD_MUX11 pin(MCU_ADD_MUX11) - GPIO_PIN_19U
/*****************************************************************************/
#define MCU_ADD_MUX11_Set()                   (GPIO1->DR_SET = 1UL<< ADD_MUX11)
#define MCU_ADD_MUX11_Clear()                 (GPIO1->DR_CLEAR = 1UL<< ADD_MUX11)
#define MCU_ADD_MUX11_Toggle()                (GPIO1->DR_TOGGLE = 1UL<< ADD_MUX11)

/*****************************************************************************/
// Macros for ADD_MUX12 pin(MCU_ADD_MUX12) - GPIO_PIN_20U
/*****************************************************************************/
#define MCU_ADD_MUX12_Set()                   (GPIO1->DR_SET = 1UL<< ADD_MUX12)
#define MCU_ADD_MUX12_Clear()                 (GPIO1->DR_CLEAR = 1UL<< ADD_MUX12)
#define MCU_ADD_MUX12_Toggle()                (GPIO1->DR_TOGGLE = 1UL<< ADD_MUX12)

/*****************************************************************************/
// Macros for ADD_MUX13 pin(MCU_ADD_MUX13) - GPIO_PIN_21U
/*****************************************************************************/
#define MCU_ADD_MUX13_Set()                   (GPIO1->DR_SET = 1UL<< ADD_MUX13)
#define MCU_ADD_MUX13_Clear()                 (GPIO1->DR_CLEAR = 1UL<< ADD_MUX13)
#define MCU_ADD_MUX13_Toggle()                (GPIO1->DR_TOGGLE = 1UL<< ADD_MUX13)

/*****************************************************************************/
// Macros for TEST_POING_2 pin(TEST_POING_2) - GPIO_PIN_25U
/*****************************************************************************/
#define TEST_POING_2_Set()                   (GPIO1->DR_SET = 1UL<< TEST_POING_2)
#define TEST_POING_2_Clear()                 (GPIO1->DR_CLEAR = 1UL<< TEST_POING_2)
#define TEST_POING_2_Toggle()                (GPIO1->DR_TOGGLE = 1UL<< TEST_POING_2)


/*****************************************************************************/
// Macros for TEST_POING_3 pin(TEST_POING_3) - GPIO_PIN_26U
/*****************************************************************************/
#define TEST_POING_3_Set()                   (GPIO1->DR_SET = 1UL<< TEST_POING_3)
#define TEST_POING_3_Clear()                 (GPIO1->DR_CLEAR = 1UL<< TEST_POING_3)
#define TEST_POING_3_Toggle()                (GPIO1->DR_TOGGLE = 1UL<< TEST_POING_3)

/*****************************************************************************/
// Macros for LED_HGROUP_DATA pin(MCU_LED_HGROUP_DATA) - GPIO_PIN_27U
/*****************************************************************************/
#define MCU_LED_HGROUP_DATA_Set()               (GPIO1->DR_SET = 1UL<< MCU_LED_HGROUP_DATA)
#define MCU_LED_HGROUP_DATA_Clear()             (GPIO1->DR_CLEAR = 1UL<< MCU_LED_HGROUP_DATA)
#define MCU_LED_HGROUP_DATA_Toggle()            (GPIO1->DR_TOGGLE = 1UL<< MCU_LED_HGROUP_DATA)



/*****************************************************************************/
// Macros for LED_CELL_CTRL1 pin(MCU_LED_CELL_CTRL1) - GPIO_PIN_0U
/*****************************************************************************/
#define MCU_LED_CELL_CTRL1_Set()                (GPIO2->DR_SET = 1UL<< MCU_LED_CELL_CTRL1)
#define MCU_LED_CELL_CTRL1_Clear()              (GPIO2->DR_CLEAR = 1UL<< MCU_LED_CELL_CTRL1)
#define MCU_LED_CELL_CTRL1_Toggle()             (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_CELL_CTRL1)


/*****************************************************************************/
// Macros for LED_CELL_CTRL2 pin(MCU_LED_CELL_CTRL2) - GPIO_PIN_1U
/*****************************************************************************/
#define MCU_LED_CELL_CTRL2_Set()                (GPIO2->DR_SET = 1UL<< MCU_LED_CELL_CTRL2)
#define MCU_LED_CELL_CTRL2_Clear()              (GPIO2->DR_CLEAR = 1UL<< MCU_LED_CELL_CTRL2)
#define MCU_LED_CELL_CTRL2_Toggle()             (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_CELL_CTRL2)

/*****************************************************************************/
// Macros for LED_CELL_CTRL3 pin(MCU_LED_CELL_CTRL3) - GPIO_PIN_2U
/*****************************************************************************/
#define MCU_LED_CELL_CTRL3_Set()                (GPIO2->DR_SET = 1UL<< MCU_LED_CELL_CTRL3)
#define MCU_LED_CELL_CTRL3_Clear()              (GPIO2->DR_CLEAR = 1UL<< MCU_LED_CELL_CTRL3)
#define MCU_LED_CELL_CTRL3_Toggle()             (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_CELL_CTRL3)

/*****************************************************************************/
// Macros for LED_GROUP_EN pin(MCU_LED_GROUP_EN) - GPIO_PIN_26U
/*****************************************************************************/
#define MCU_LED_GROUP_EN_Set()                  (GPIO2->DR_SET = 1UL<< MCU_LED_GROUP_EN)
#define MCU_LED_GROUP_EN_Clear()                (GPIO2->DR_CLEAR = 1UL<< MCU_LED_GROUP_EN)
#define MCU_LED_GROUP_EN_Toggle()               (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_GROUP_EN)


/*****************************************************************************/
// Macros for PD_SHIFT_DATA pin(MCU_PD_SHIFT_DATA) - GPIO_PIN_30U
/*****************************************************************************/
#define MCU_PD_SHIFT_DATA_Set()                 (GPIO2->DR_SET = 1UL<< MCU_PD_SHIFT_DATA)
#define MCU_PD_SHIFT_DATA_Clear()               (GPIO2->DR_CLEAR = 1UL<< MCU_PD_SHIFT_DATA)
#define MCU_PD_SHIFT_DATA_Toggle()              (GPIO2->DR_TOGGLE = 1UL<< MCU_PD_SHIFT_DATA)


/*****************************************************************************/
// Macros for PD_SHIFT_CLK pin(MCU_PD_SHIFT_CLK) - GPIO_PIN_31U
/*****************************************************************************/
#define MCU_PD_SHIFT_CLK_Set()                  (GPIO2->DR_SET = 1UL<< MCU_PD_SHIFT_CLK)
#define MCU_PD_SHIFT_CLK_Clear()                (GPIO2->DR_CLEAR = 1UL<< MCU_PD_SHIFT_CLK)
#define MCU_PD_SHIFT_CLK_Toggle()               (GPIO2->DR_TOGGLE = 1UL<< MCU_PD_SHIFT_CLK)





/*****************************************************************************/
// Macros for MCU_LED_SINK_DAC0 pin(MCU_LED_SINK_DAC0) - GPIO_PIN_16U
/*****************************************************************************/
#define MCU_LED_SINK_DAC0_Set()                   (GPIO2->DR_SET = 1UL<< MCU_LED_SINK_DAC0)
#define MCU_LED_SINK_DAC0_Clear()                 (GPIO2->DR_CLEAR = 1UL<< MCU_LED_SINK_DAC0)
#define MCU_LED_SINK_DAC0_Toggle()                (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_SINK_DAC0)

/*****************************************************************************/
// Macros for MCU_LED_SINK_DAC1 pin(MCU_LED_SINK_DAC1) - GPIO_PIN_17U
/*****************************************************************************/
#define MCU_LED_SINK_DAC1_Set()                   (GPIO2->DR_SET = 1UL<< MCU_LED_SINK_DAC1)
#define MCU_LED_SINK_DAC1_Clear()                 (GPIO2->DR_CLEAR = 1UL<< MCU_LED_SINK_DAC1)
#define MCU_LED_SINK_DAC1_Toggle()                (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_SINK_DAC1)

/*****************************************************************************/
// Macros for MCU_LED_SINK_DAC2 pin(MCU_LED_SINK_DAC2) - GPIO_PIN_18U
/*****************************************************************************/
#define MCU_LED_SINK_DAC2_Set()                   (GPIO2->DR_SET = 1UL<< MCU_LED_SINK_DAC2)
#define MCU_LED_SINK_DAC2_Clear()                 (GPIO2->DR_CLEAR = 1UL<< MCU_LED_SINK_DAC2)
#define MCU_LED_SINK_DAC2_Toggle()                (GPIO2->DR_TOGGLE = M1UL<< CU_LED_SINK_DAC2)

/*****************************************************************************/
// Macros for MCU_LED_SINK_DAC3 pin(MCU_LED_SINK_DAC3) - GPIO_PIN_19U
/*****************************************************************************/
#define MCU_LED_SINK_DAC3_Set()                   (GPIO2->DR_SET = 1UL<< MCU_LED_SINK_DAC3)
#define MCU_LED_SINK_DAC3_Clear()                 (GPIO2->DR_CLEAR = 1UL<< MCU_LED_SINK_DAC3)
#define MCU_LED_SINK_DAC3_Toggle()                (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_SINK_DAC3)

/*****************************************************************************/
// Macros for MCU_LED_SINK_DAC4 pin(MCU_LED_SINK_DAC4) - GPIO_PIN_20U
/*****************************************************************************/
#define MCU_LED_SINK_DAC4_Set()                   (GPIO2->DR_SET = 1UL<< MCU_LED_SINK_DAC4)
#define MCU_LED_SINK_DAC4_Clear()                 (GPIO2->DR_CLEAR = 1UL<< MCU_LED_SINK_DAC4)
#define MCU_LED_SINK_DAC4_Toggle()                (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_SINK_DAC4)

/*****************************************************************************/
// Macros for MCU_LED_SINK_DAC5 pin(MCU_LED_SINK_DAC5) - GPIO_PIN_21U
/*****************************************************************************/
#define MCU_LED_SINK_DAC5_Set()                   (GPIO2->DR_SET = 1UL<< MCU_LED_SINK_DAC5)
#define MCU_LED_SINK_DAC5_Clear()                 (GPIO2->DR_CLEAR = 1UL<< MCU_LED_SINK_DAC5)
#define MCU_LED_SINK_DAC5_Toggle()                (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_SINK_DAC5)

/*****************************************************************************/
// Macros for MCU_LED_SINK_DAC6 pin(MCU_LED_SINK_DAC6) - GPIO_PIN_22U
/*****************************************************************************/
#define MCU_LED_SINK_DAC6_Set()                   (GPIO2->DR_SET = 1UL<< MCU_LED_SINK_DAC6)
#define MCU_LED_SINK_DAC6_Clear()                 (GPIO2->DR_CLEAR = 1UL<< MCU_LED_SINK_DAC6)
#define MCU_LED_SINK_DAC6_Toggle()                (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_SINK_DAC6)

/*****************************************************************************/
// Macros for MCU_LED_SINK_DAC7 pin(MCU_LED_SINK_DAC7) - GPIO_PIN_23U
/*****************************************************************************/
#define MCU_LED_SINK_DAC7_Set()                   (GPIO2->DR_SET = 1UL<< MCU_LED_SINK_DAC7)
#define MCU_LED_SINK_DAC7_Clear()                 (GPIO2->DR_CLEAR = 1UL<< MCU_LED_SINK_DAC7)
#define MCU_LED_SINK_DAC7_Toggle()                (GPIO2->DR_TOGGLE = 1UL<< MCU_LED_SINK_DAC7)


#else
typedef enum {
	LED_ONOFF_POS 			= 0U,
	LED_SHIFT_H_DATA_POS	= 1U,
	LED_SHIFT_CLK_POS		= 2U,
	LED_SHIFT_RESET_POS		= 3U,
	LED_SHIFT_V_DATA_POS	= 4U,
	LED_CELL_CTRL1_POS		= 5U,
	LED_CELL_CTRL2_POS		= 6U,
	LED_CELL_CTRL3_POS 		= 7U,

	LED_SINK_CTRL1_POS 		= 8U,
	LED_SINK_CTRL2_POS 		= 9U,
	LED_SINK_CTRL3_POS 		= 10U,
	LED_SINK_CTRL4_POS 		= 11U,
	LED_SINK_CTRL5_POS 		= 12U,
	LED_SINK_CTRL6_POS 		= 13U,

	PD_SHIFT_CLK_POS 		= 16U,
	PD_SHIFT_V_DATA_POS 	= 17U,

	AN_SW_CTRL1_POS 		= 18U,
	AN_SW_CTRL2_POS 		= 19U,
	AN_SW_CTRL3_POS 		= 20U,
	AN_SW_CTRL4_POS 		= 21U,

	PD_SHIFT_H_DATA_POS 	= 22U,
	PD_SHIFT_RESET_POS 		= 23U,

	DEBUG_TP1_POS 			= 25U,
	DEBUG_TP2_POS 			= 26U,
	DEBUG_TP3_POS 			= 27U
} GPIO_CTRL_BIT_POS_T;

extern void Scan_GPIO_PortSet(GPIO_Type *base, uint32_t pin, uint32_t mask);

#define LED_CELL_ON          		GPIO_PinWrite(GPIO2, LED_ONOFF_POS, GPIO_HIGH)
#define LED_CELL_OFF                GPIO_PinWrite(GPIO2, LED_ONOFF_POS, GPIO_LOW)

#define LED_SINK_MASK_BIT       		0x3F00
#define LED_CURRENT_CONTROL_SET(value)	Scan_GPIO_PortSet(GPIO2, value << 8, LED_SINK_MASK_BIT)

#define LED_CELL_MASK_BIT    0xE0
#define LED_CELL_CONTROL_SET(value)	Scan_GPIO_PortSet(GPIO2, value, LED_CELL_MASK_BIT)


#define LED_SHIFT_RESET_HIGH       	GPIO_PinWrite(GPIO2, LED_SHIFT_RESET_POS, GPIO_HIGH)
#define LED_SHIFT_RESET_LOW         GPIO_PinWrite(GPIO2, LED_SHIFT_RESET_POS, GPIO_LOW)
#define LED_SHIFT_CLOCK_HIGH        GPIO_PinWrite(GPIO2, LED_SHIFT_CLK_POS, GPIO_HIGH)
#define LED_SHIFT_CLOCK_LOW         GPIO_PinWrite(GPIO2, LED_SHIFT_CLK_POS, GPIO_LOW)
#define LED_SHIFT_H_DATA_HIGH       GPIO_PinWrite(GPIO2, LED_SHIFT_H_DATA_POS, GPIO_HIGH)
#define LED_SHIFT_H_DATA_LOW        GPIO_PinWrite(GPIO2, LED_SHIFT_H_DATA_POS, GPIO_LOW)
#define LED_SHIFT_V_DATA_HIGH       GPIO_PinWrite(GPIO2, LED_SHIFT_V_DATA_POS, GPIO_HIGH)
#define LED_SHIFT_V_DATA_LOW        GPIO_PinWrite(GPIO2, LED_SHIFT_V_DATA_POS, GPIO_LOW)

/*** Functions for PD Shift & Analog switch Control Pins ***/
#define PD_SHIFT_CLOCK_HIGH         GPIO_PinWrite(GPIO2, PD_SHIFT_CLK_POS, GPIO_HIGH)
#define PD_SHIFT_CLOCK_LOW          GPIO_PinWrite(GPIO2, PD_SHIFT_CLK_POS, GPIO_LOW)

#define PD_SHIFT_H_DATA_HIGH        GPIO_PinWrite(GPIO2, PD_SHIFT_H_DATA_POS, GPIO_HIGH)
#define PD_SHIFT_H_DATA_LOW         GPIO_PinWrite(GPIO2, PD_SHIFT_H_DATA_POS, GPIO_LOW)
#define PD_SHIFT_V_DATA_HIGH        GPIO_PinWrite(GPIO2, PD_SHIFT_V_DATA_POS, GPIO_HIGH)
#define PD_SHIFT_V_DATA_LOW         GPIO_PinWrite(GPIO2, PD_SHIFT_V_DATA_POS, GPIO_LOW)


#define PD_SHIFT_ENABLE		        GPIO_PinWrite(GPIO2, PD_SHIFT_RESET_POS, GPIO_HIGH)
#define PD_SHIFT_DISABLE	        GPIO_PinWrite(GPIO2, PD_SHIFT_RESET_POS, GPIO_LOW)

#define ANALOG_SWITCH1_HIGH         GPIO_PinWrite(GPIO2, AN_SW_CTRL1_POS, GPIO_HIGH)
#define ANALOG_SWITCH1_LOW          GPIO_PinWrite(GPIO2, AN_SW_CTRL1_POS, GPIO_LOW)
#define ANALOG_SWITCH2_HIGH         GPIO_PinWrite(GPIO2, AN_SW_CTRL2_POS, GPIO_HIGH)
#define ANALOG_SWITCH2_LOW          GPIO_PinWrite(GPIO2, AN_SW_CTRL2_POS, GPIO_LOW)
#define ANALOG_SWITCH3_HIGH         GPIO_PinWrite(GPIO2, AN_SW_CTRL3_POS, GPIO_HIGH)
#define ANALOG_SWITCH3_LOW          GPIO_PinWrite(GPIO2, AN_SW_CTRL3_POS, GPIO_LOW)
#define AMP_GAIN_5X			        GPIO_PinWrite(GPIO2, AN_SW_CTRL4_POS, GPIO_HIGH)
#define AMP_GAIN_10X		        GPIO_PinWrite(GPIO2, AN_SW_CTRL4_POS, GPIO_LOW)

/*** Functions for Debug Test Point Pins ***/
#define DEBUG_TP1_HIGH              GPIO_PinWrite(GPIO2, DEBUG_TP1_POS, GPIO_HIGH)
#define DEBUG_TP1_LOW               GPIO_PinWrite(GPIO2, DEBUG_TP1_POS, GPIO_LOW)
#define DEBUG_TP2_HIGH              GPIO_PinWrite(GPIO2, DEBUG_TP2_POS, GPIO_HIGH)
#define DEBUG_TP2_LOW               GPIO_PinWrite(GPIO2, DEBUG_TP2_POS, GPIO_LOW)
#define DEBUG_TP3_HIGH              GPIO_PinWrite(GPIO2, DEBUG_TP3_POS, GPIO_HIGH)
#define DEBUG_TP3_LOW               GPIO_PinWrite(GPIO2, DEBUG_TP3_POS, GPIO_LOW)
#endif
#endif /* GPIO_H_ */
