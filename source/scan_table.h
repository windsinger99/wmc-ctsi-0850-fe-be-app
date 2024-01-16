/*
 * scan_table.h
 *
 *  Created on: 2018. 11. 3.
 *      Author: shj9816
 */
#ifndef SCAN_TABLE_H_
#define SCAN_TABLE_H_

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "common.h"
#include "TSP_Layout.h"
#include "backend.h"


#if NOT_USE_LED_SINK_CONTROL
typedef struct {
  uint8_t sinkCurrentControl;
  float resistance;
  float currentRatio;
} ledSinkControl_t;

    #if LED_ON_TIME_SHORT //nsmoon@210929
        /*
         * LED_SINK_CONTROL_STEPS is not used in the GD32F425RG B'd,
         * because it uses the DAC.
         *
         */
        //#define LED_SINK_CONTROL_STEPS      3
    //#else
        //#define LED_SINK_CONTROL_BITS       6
        //#define LED_SINK_CONTROL_STEPS      63
    #endif

    #define LED_SINK_CONTROL_STEP_MAX     (LED_SINK_CONTROL_STEPS - 1)

    #define X_LED_CURRENT_MIN_INDEX         0
    #define Y_LED_CURRENT_MIN_INDEX         0
    #define X_LED_CURRENT_MAX_INDEX         (LED_SINK_CONTROL_STEPS - 1)
    #define Y_LED_CURRENT_MAX_INDEX         (LED_SINK_CONTROL_STEPS - 1)

    #if LED_ON_TIME_SHORT //nsmoon@220330
        #if (CAL_HIGH_TO_LOW) //nsmoon@22030
        #define X_LED_CURRENT_DEFAULT_INDEX     (X_LED_CURRENT_MAX_INDEX/2) //42
        #define Y_LED_CURRENT_DEFAULT_INDEX     (Y_LED_CURRENT_MAX_INDEX/2)
        #else
        #define X_LED_CURRENT_DEFAULT_INDEX     X_LED_CURRENT_MIN_INDEX
        #define Y_LED_CURRENT_DEFAULT_INDEX     Y_LED_CURRENT_MIN_INDEX
        #endif
    #else
        #define X_LED_CURRENT_DEFAULT_INDEX     42
        #define Y_LED_CURRENT_DEFAULT_INDEX     Y_LED_CURRENT_MAX_INDEX
    #endif  // LED_ON_TIME_SHORT
#else
    typedef struct {
      uint8_t sinkCurrentControl;
      float resistance;
      float currentRatio;
    } ledSinkControl_t;


    #if LED_ON_TIME_SHORT //nsmoon@210929
        /*
         * LED_SINK_CONTROL_STEPS is not used in the GD32F425RG B'd,
         * because it uses the DAC.
         *
         */
#if (MODEL_TYPE == WMC_0750_NXP_VE)
    #define LED_SINK_CONTROL_STEPS  16
#else
	#define LED_SINK_CONTROL_STEPS  4
#endif
    //#else
        //#define LED_SINK_CONTROL_BITS       6
        //#define LED_SINK_CONTROL_STEPS      63
    #endif

    #define LED_SINK_CONTROL_STEP_MAX     (LED_SINK_CONTROL_STEPS - 1)

    #define X_LED_CURRENT_MIN_INDEX         0
    #define Y_LED_CURRENT_MIN_INDEX         0
    #define X_LED_CURRENT_MAX_INDEX         (LED_SINK_CONTROL_STEPS - 1)
    #define Y_LED_CURRENT_MAX_INDEX         (LED_SINK_CONTROL_STEPS - 1)

    #if LED_ON_TIME_SHORT //nsmoon@220330
        #if (CAL_HIGH_TO_LOW) //nsmoon@22030
        #define X_LED_CURRENT_DEFAULT_INDEX     (X_LED_CURRENT_MAX_INDEX/2) //42
        #define Y_LED_CURRENT_DEFAULT_INDEX     (Y_LED_CURRENT_MAX_INDEX/2)
        #else
        #define X_LED_CURRENT_DEFAULT_INDEX     X_LED_CURRENT_MIN_INDEX
        #define Y_LED_CURRENT_DEFAULT_INDEX     Y_LED_CURRENT_MIN_INDEX
        #endif
    #else
        #define X_LED_CURRENT_DEFAULT_INDEX     42
        #define Y_LED_CURRENT_DEFAULT_INDEX     Y_LED_CURRENT_MAX_INDEX
    #endif  // LED_ON_TIME_SHORT
#endif

#if LED_ON_TIME_SHORT //nsmoon@210929
#if (MODEL_TYPE == CTSI_0650_NXP)||(MODEL_TYPE == WMC_0650_NXP)

/*                                max     min    uint  step
                        DAC Step  254      110      2   72     (8bit)*/

        #define LED_ON_DAC_MIN           120		// == 95mA
        #define LED_ON_DAC_UINT           2

        #define LED_ON_DAC_MIN_X         120	// 180	//LED_ON_DAC_MIN
        #define LED_ON_DAC_MAX_X         254	// 254
		#define LED_ON_DAC_INIT_X        180	// 150 ~ 이상

        #define LED_ON_DAC_MIN_Y         120	// LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_Y         254	// 254
        #define LED_ON_DAC_INIT_Y        180  	// 220

		#define LED_GAIN_INIT_X			 0
		#define LED_GAIN_INIT_Y			 0

        #define LED_ON_TIME_TEST_X        LED_ON_DAC_MIN_X
        #define LED_ON_TIME_TEST_Y        LED_ON_DAC_MIN_Y

		#define PD_GAIN_OFFSET_1		70
		#define PD_GAIN_OFFSET_2		90
		#define PD_GAIN_OFFSET_3		110
#elif (MODEL_TYPE == WMC_0650_VE_NXP)

/*                                max     min    uint  step
                        DAC Step  254      110      2   72     (8bit)*/

        #define LED_ON_DAC_MIN           120		// == 95mA
        #define LED_ON_DAC_UINT           2

        #define LED_ON_DAC_MIN_X         120	// 180	//LED_ON_DAC_MIN
        #define LED_ON_DAC_MAX_X         254	// 254
		#define LED_ON_DAC_INIT_X        180	// 150 ~ 이상

        #define LED_ON_DAC_MIN_Y         120	// LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_Y         254	// 254
        #define LED_ON_DAC_INIT_Y        180  	// 220

		#define LED_GAIN_INIT_X			 0
		#define LED_GAIN_INIT_Y			 0

        #define LED_ON_TIME_TEST_X        LED_ON_DAC_MIN_X
        #define LED_ON_TIME_TEST_Y        LED_ON_DAC_MIN_Y

		#define PD_GAIN_OFFSET_1		90
		#define PD_GAIN_OFFSET_2		90
		#define PD_GAIN_OFFSET_3		90

#elif (MODEL_TYPE == SLIM_0320_NXP)

/*                                max     min    uint  step
                        DAC Step  254      110      2   72     (8bit)*/

        #define LED_ON_DAC_MIN           120		// == 95mA
        #define LED_ON_DAC_UINT           2

        #define LED_ON_DAC_MIN_X         120	// 180	//LED_ON_DAC_MIN
        #define LED_ON_DAC_MAX_X         254	// 254
		#define LED_ON_DAC_INIT_X        180	// 150 ~ 이상

        #define LED_ON_DAC_MIN_Y         120	// LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_Y         254	// 254
        #define LED_ON_DAC_INIT_Y        180  	// 220

		#define LED_GAIN_INIT_X			 0
		#define LED_GAIN_INIT_Y			 0

        #define LED_ON_TIME_TEST_X        LED_ON_DAC_MIN_X
        #define LED_ON_TIME_TEST_Y        LED_ON_DAC_MIN_Y

		#define PD_GAIN_OFFSET_1		80
		#define PD_GAIN_OFFSET_2		80
		#define PD_GAIN_OFFSET_3		80
#elif (MODEL_TYPE == WMC_0750_NXP)
    /*                                max     min    uint  step
                            DAC Step  254      110      2   72     (8bit)*/

		#define LED_ON_DAC_MIN           120		// == 95mA
		#define LED_ON_DAC_UINT           2

		#define LED_ON_DAC_MIN_X         120	//140	// 180	//LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_X         254	// 254
		#define LED_ON_DAC_INIT_X        200	// 150 ~ 이상

		#define LED_ON_DAC_MIN_Y         120	// LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_Y         254	// 254
		#define LED_ON_DAC_INIT_Y        200  	// 220

		#define LED_GAIN_INIT_X			 0
		#define LED_GAIN_INIT_Y			 0

		#define LED_ON_TIME_TEST_X        LED_ON_DAC_MIN_X
		#define LED_ON_TIME_TEST_Y        LED_ON_DAC_MIN_Y

		#define PD_GAIN_OFFSET_1		70
		#define PD_GAIN_OFFSET_2		90
		#define PD_GAIN_OFFSET_3		110
#elif (MODEL_TYPE == WMC_0850_NXP)||(MODEL_TYPE == WMC_0850_NXP_VE)
    /*                                max     min    uint  step
                            DAC Step  254      110      2   72     (8bit)*/

		#define LED_ON_DAC_MIN           80		// == 95mA
		#define LED_ON_DAC_UINT           2

		#define LED_ON_DAC_MIN_X         100	//120	// 180	//LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_X         254	// 254
		#define LED_ON_DAC_INIT_X        120	// 150 ~ 이상

		#define LED_ON_DAC_MIN_Y         130 //120  // LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_Y         254	// 254
		#define LED_ON_DAC_INIT_Y        150		//130  	// 220

		#define LED_GAIN_INIT_X			 1
		#define LED_GAIN_INIT_Y			 1//3

		#define LED_ON_TIME_TEST_X        LED_ON_DAC_MIN_X
		#define LED_ON_TIME_TEST_Y        LED_ON_DAC_MIN_Y

		#define PD_GAIN_OFFSET_1		70
		#define PD_GAIN_OFFSET_2		90
		#define PD_GAIN_OFFSET_3		110


#elif (MODEL_TYPE == SLIM_0750_NXP_VE)
    /*                                max     min    uint  step
                            DAC Step  254      110      2   72     (8bit)*/

		#define LED_ON_DAC_MIN           100		// == 95mA
		#define LED_ON_DAC_UINT           2

		#define LED_ON_DAC_MIN_X         100	// 180	//LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_X         254	// 254
		#define LED_ON_DAC_INIT_X        110	// 150 ~ 이상

		#define LED_ON_DAC_MIN_Y         100	// LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_Y         254	// 254
		#define LED_ON_DAC_INIT_Y        150  	// 220

		#define LED_GAIN_INIT_X			 1
		#define LED_GAIN_INIT_Y			 1//3

		#define LED_ON_TIME_TEST_X        LED_ON_DAC_MIN_X
		#define LED_ON_TIME_TEST_Y        LED_ON_DAC_MIN_Y

		#define PD_GAIN_OFFSET_1		70
		#define PD_GAIN_OFFSET_2		90
		#define PD_GAIN_OFFSET_3		110
#elif (MODEL_TYPE == WMC_0750_NXP_VE)
    /*                                max     min    uint  step
                            DAC Step  254      110      2   72     (8bit)*/

		#define LED_ON_DAC_MIN           100		// == 95mA
		#define LED_ON_DAC_UINT           2

		#define LED_ON_DAC_MIN_X         100	// 180	//LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_X         254	// 254
		#define LED_ON_DAC_INIT_X        180	// 150 ~ 이상

		#define LED_ON_DAC_MIN_Y         100	// LED_ON_DAC_MIN
		#define LED_ON_DAC_MAX_Y         254	// 254
		#define LED_ON_DAC_INIT_Y        180  	// 220

		#define LED_GAIN_INIT_X			 1
		#define LED_GAIN_INIT_Y			 1//3

		#define LED_ON_TIME_TEST_X        LED_ON_DAC_MIN_X
		#define LED_ON_TIME_TEST_Y        LED_ON_DAC_MIN_Y

		#define PD_GAIN_OFFSET_1		70
		#define PD_GAIN_OFFSET_2		90
		#define PD_GAIN_OFFSET_3		110

#endif
extern  ledSinkControl_t LedSinkCurrentTbl[LED_SINK_CONTROL_STEPS];

#endif  // LED_ON_TIME_SHORT

#if NOT_USE_LED_SINK_CONTROL
#if 0 //LED_ON_TIME_SHORT //nsmoon@220330   //YJ@220906
extern const ledSinkControl_t LedSinkCurrentTblX[LED_SINK_CONTROL_STEPS];
extern const ledSinkControl_t LedSinkCurrentTblY[LED_SINK_CONTROL_STEPS];
#else
extern const ledSinkControl_t LedSinkCurrentTbl[LED_SINK_CONTROL_STEPS];
#endif
#endif



/*****************************************************************************/
#if (MODEL_TYPE == CTSI_0650_NXP)
 #define X_TOTAL_SCAN_STEPS 576
 #define Y_TOTAL_SCAN_STEPS 456
#elif (MODEL_TYPE == WMC_0650_NXP)
 #define X_TOTAL_SCAN_STEPS 659
 #define Y_TOTAL_SCAN_STEPS 456
#elif (MODEL_TYPE == WMC_0650_VE_NXP)
 #define X_TOTAL_SCAN_STEPS 734
 #define Y_TOTAL_SCAN_STEPS 612
#elif (MODEL_TYPE == WMC_0750_NXP)
#if(MAX_X_SLOPE_VAL == 38)
 #define X_TOTAL_SCAN_STEPS 712
#elif(MAX_X_SLOPE_VAL == 37)
 #define X_TOTAL_SCAN_STEPS 700
#elif(MAX_X_SLOPE_VAL == 36)
 #define X_TOTAL_SCAN_STEPS 688
#else
 #define X_TOTAL_SCAN_STEPS 712
#endif
 #define Y_TOTAL_SCAN_STEPS 520
#elif (MODEL_TYPE == WMC_0850_NXP)
  #define X_TOTAL_SCAN_STEPS 878
  #define Y_TOTAL_SCAN_STEPS 657
#elif (MODEL_TYPE == SLIM_0320_NXP)
  #define X_TOTAL_SCAN_STEPS 790
  #define Y_TOTAL_SCAN_STEPS 600
#elif (MODEL_TYPE == WMC_0850_NXP_VE)
  #define X_TOTAL_SCAN_STEPS 896
  #define Y_TOTAL_SCAN_STEPS 810
#elif (MODEL_TYPE == SLIM_0750_NXP_VE)
  #define X_TOTAL_SCAN_STEPS 794
  #define Y_TOTAL_SCAN_STEPS 684
#elif (MODEL_TYPE == WMC_0750_NXP_VE)
  #define X_TOTAL_SCAN_STEPS 753
  #define Y_TOTAL_SCAN_STEPS 672
/*****************************************************************************/
#else
 #error "MODEL_TYPE is not defined!!!"
#endif






extern const uint8_t xAxisSequenceTbl_flash[X_TOTAL_SCAN_STEPS][2];
extern const uint8_t xPdIdxAtANOutTbl_flash[X_TOTAL_SCAN_STEPS][PD_SIGNAL_OUT_NUM];
extern const uint8_t xUOffsetAtANOutTbl_flash[X_TOTAL_SCAN_STEPS][PD_SIGNAL_OUT_NUM];

extern const uint8_t yAxisSequenceTbl_flash[Y_TOTAL_SCAN_STEPS][2];
extern const uint8_t yPdIdxAtANOutTbl_flash[Y_TOTAL_SCAN_STEPS][PD_SIGNAL_OUT_NUM];
extern const uint8_t yUOffsetAtANOutTbl_flash[Y_TOTAL_SCAN_STEPS][PD_SIGNAL_OUT_NUM];

//-----------------------------------------------------------------------------
/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif


#endif /* SCAN_TABLE_H_ */
