/*
 * common.h
 *
 *  Created on: 2020. 3. 26.
 *      Author: lispect-shj
 */

#ifndef COMMON_H_
#define COMMON_H_


#include "fsl_common.h"
#include "fsl_debug_console.h"

// option.h

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif


/*****************************************************************************/
// System Definition
// Deprecated. it have to move another header file.
// because these definitions are not suitable for the option.h.
/*****************************************************************************/
#define SYSTEM_CLOCK_FREQ           (600000000)   // 600Mhz

/* GD32F425RG: 1024KB, GD32F427RK: 3072KB */
#define FLASH_MEM_SIZE              (2048)  //(512)
//-----------------------------------------------------------------------------

/*****************************************************************************/
// Model Definition
/*****************************************************************************/
#define CTSI_0650_NXP       10000
#define WMC_0650_NXP        20000
#define WMC_0750_NXP        30000
#define WMC_0850_NXP        40000
#define WMC_0650_VE_NXP		50000
#define SLIM_0320_NXP		60000

#define MODEL_TYPE          WMC_0850_NXP
//-----------------------------------------------------------------------------

/*****************************************************************************/
// Options for Function
/*****************************************************************************/
#if (MODEL_TYPE == CTSI_0650_NXP)
#define LED_ON_TIME_SHORT                  TRUE    // nsmoon@210910
#define CAL_HIGH_TO_LOW                    FALSE   // TRUE   //FALSE for test
#define FORCED_FULL_SCAN                   TRUE    // nsmoon@210906
#define SKIP_CAL_CRC_FOR_XFER_BLOCK        TRUE
#define SPM_MODE_ENABLE					   TRUE
#define ONEPOINT_6MSFIX_REPORT			   FALSE
#elif (MODEL_TYPE == WMC_0650_NXP)
#define LED_ON_TIME_SHORT                  TRUE    // nsmoon@210910
#define CAL_HIGH_TO_LOW                    FALSE   // TRUE   //FALSE for test
#define FORCED_FULL_SCAN                   TRUE    // nsmoon@210906
#define SKIP_CAL_CRC_FOR_XFER_BLOCK        TRUE
#define SPM_MODE_ENABLE         		   TRUE
#define ONEPOINT_6MSFIX_REPORT			   FALSE
#elif (MODEL_TYPE == WMC_0650_VE_NXP)
#define LED_ON_TIME_SHORT                  TRUE    // nsmoon@210910
#define CAL_HIGH_TO_LOW                    FALSE   // TRUE   //FALSE for test
#define FORCED_FULL_SCAN                   TRUE    // nsmoon@210906
#define SKIP_CAL_CRC_FOR_XFER_BLOCK        TRUE
#define SPM_MODE_ENABLE         		   TRUE
#define ONEPOINT_6MSFIX_REPORT			   FALSE
#elif (MODEL_TYPE == WMC_0750_NXP)
#define LED_ON_TIME_SHORT                  TRUE    // nsmoon@210910
#define CAL_HIGH_TO_LOW                    FALSE   // TRUE   //FALSE for test
#define FORCED_FULL_SCAN                   TRUE    // nsmoon@210906
#define SKIP_CAL_CRC_FOR_XFER_BLOCK        TRUE
#define SPM_MODE_ENABLE         		   TRUE
#define ONEPOINT_6MSFIX_REPORT			   FALSE	// TRUE
#elif (MODEL_TYPE == WMC_0850_NXP)
#define LED_ON_TIME_SHORT                  TRUE    // nsmoon@210910
#define CAL_HIGH_TO_LOW                    FALSE   // TRUE   //FALSE for test
#define FORCED_FULL_SCAN                   TRUE    // nsmoon@210906
#define SKIP_CAL_CRC_FOR_XFER_BLOCK        TRUE
#define SPM_MODE_ENABLE         TRUE
#define ONEPOINT_6MSFIX_REPORT			   FALSE//TRUE
#elif (MODEL_TYPE == SLIM_0320_NXP)
#define LED_ON_TIME_SHORT                  TRUE    // nsmoon@210910
#define CAL_HIGH_TO_LOW                    FALSE   // TRUE   //FALSE for test
#define FORCED_FULL_SCAN                   TRUE    // nsmoon@210906
#define SKIP_CAL_CRC_FOR_XFER_BLOCK        TRUE
#define SPM_MODE_ENABLE         TRUE
#define ONEPOINT_6MSFIX_REPORT			   FALSE//TRUE
#endif

#if LED_ON_TIME_SHORT
#define ENABLE_LED_ON_TIME_FINE             TRUE    // true:20ns,false:100ns //nsmoon@210930
#else
#define ENABLE_LED_ON_TIME_FINE             FALSE   // 100ns
#endif
#define X_AXIS_GAIN_NORMAL                  TRUE    // nsmoon@211215
#define Y_AXIS_GAIN_NORMAL                  FALSE

//-----------------------------------------------------------------------------
// LED_SINK_CONTROL_STEPS is not used in the GD32F425RG B'd,
// because it uses the DAC.
#define NOT_USE_LED_SINK_CONTROL    FALSE
//-----------------------------------------------------------------------------

#define DEBUG_UART_MOME	1
#define DEBUG_VCOM_MODE	2

#if defined(DEBUG)
#define DEBUG_VCOM_ENABLE	DEBUG_UART_MOME	//DEBUG_VCOM_MODE //nsmoon@230321
#if (DEBUG_VCOM_ENABLE == DEBUG_VCOM_MODE)
#define ENABLE_UART_CMD_PROCESS				DEBUG_VCOM_MODE
#else
#define ENABLE_UART_CMD_PROCESS				DEBUG_UART_MOME
#endif

#endif
#ifdef ENABLE_UART_CMD_PROCESS
#define ENABLE_TRIGER                       TRUE    // only for debugging
//#define ENABLE_HIGH_LOW_CNT               FALSE
//#define ENABLE_CAL_DEBUG_PRINT              2
//#define SELECT_PD_GROUP_TRACE
#else
#define ENABLE_TRIGER                       TRUE    // FALSE   //only for debugging
#define ENABLE_FIXED_CURRENT                FALSE   // only for debugging
#endif

#define FORCE_AGC_SET                       TRUE    // YJ@210719
#define REMAINED_LINE_FRAME_CNT_MAX 		(100U) 		   // YJ@221219

#define SCAN_INT_DISABLE_SHJ                TRUE    // nsmoon@211228 FALSE=>TRUE
#define THRESHOLD_ZERO_COUNT_ENABLE         TRUE    // nsmoon@211230
#define WATCHDOG_ENABLE                     FALSE   // TRUE  //nsmoon@211217
#define ENABLE_EXB_BUF_BANKING              FALSE   // FALSE : single buffer, TRUE : double buffer
#define POWER_ON_CAL_PD_SHIFT               FALSE   // calibratePdShiftNoise() is not working, so define FALSE
#define DLT_LED_ON_TIME_ENABLE              TRUE    // nsmoon@220117

#if (MODEL_TYPE == CTSI_0650_NXP)
#define DLT_THRESHOLD_ENABLE                FALSE   // nsmoon@220119
#elif (MODEL_TYPE == WMC_0650_NXP)
#define DLT_THRESHOLD_ENABLE                FALSE   // nsmoon@220119
#elif (MODEL_TYPE == WMC_0650_VE_NXP)
#define DLT_THRESHOLD_ENABLE                FALSE   // nsmoon@220119
#elif (MODEL_TYPE == WMC_0750_NXP)
#define DLT_THRESHOLD_ENABLE                FALSE   // nsmoon@220119
#elif (MODEL_TYPE == WMC_0850_NXP)
#define DLT_THRESHOLD_ENABLE                FALSE   // nsmoon@220119
#elif (MODEL_TYPE == SLIM_0320_NXP)
#define DLT_THRESHOLD_ENABLE                FALSE   // nsmoon@220119
#endif

#define AGC_TOGGLE_PREVENTION_ENABLE        TRUE    // FALSE  //nsmoon@220302
#define EDGE_SLOPE_AGC_ENABLE               FALSE   // nsmoon@220314

#define SIDE_DATA_FIXED                     FALSE   // side Data Fix for 65inch Sample    //YJ@210719
#define DEAD_CELL_FIXED                     FALSE   // YJ@210719

#define DEBUG_LED_CUR_TBL_IDX               FALSE

#define ENABLE_TEST_GD_USB_DLT              TRUE

#define USE_TEST_BOARD                      FALSE

#define SCAN_DMA_DISABLE        FALSE   //YJ@220823
#define ADC_REGULAR_PARALLEL_ENABLE 0
#define ADC_REGULAL_FOLLOW_UP_ENABLE  1

#define ADC_MODE    ADC_REGULAR_PARALLEL_ENABLE

#define ENABLE_CHECK_ADC_HEADER //nsmoon@230118

#if (USE_TEST_BOARD == FALSE) // kjsxx 0 for test b'd
#define POWER_ON_LED_AGC_CONTROL            TRUE
#define USUAL_LED_AGC_CONTROL               TRUE   //YJ220915				// @hj230215
#define ENABLE_DEAD_CELL_CHK                TRUE
#define POWER_ON_EXT_LIGHT_NOISE_CONTROL    FALSE	//TRUE		//	FALSE	//
#define USUAL_EXT_LIGHT_NOISE_CONTROL       FALSE	//TRUE		//FALSE	//
#define FIXED_LED_OFF_TIME                  FALSE
#if ENABLE_TEST_GD_USB_DLT
#define DEBUG_USB_OFF                       TRUE
#else
#define DEBUG_USB_OFF                       FALSE
#endif
#else
#define POWER_ON_LED_AGC_CONTROL            TRUE
#define USUAL_LED_AGC_CONTROL               FALSE
#define ENABLE_DEAD_CELL_CHK                TRUE
#define POWER_ON_EXT_LIGHT_NOISE_CONTROL    TRUE
#define USUAL_EXT_LIGHT_NOISE_CONTROL       FALSE
#define FIXED_LED_OFF_TIME                  TRUE
#if ENABLE_TEST_GD_USB_DLT
#define DEBUG_USB_OFF                       TRUE
#else
#define DEBUG_USB_OFF                       FALSE
#endif
#endif
//-----------------------------------------------------------------------------



/*****************************************************************************/
// Options for debugging information
/*****************************************************************************/
#define ENABLE_DISPLAY_AGC_RESULT   FALSE
//#define TRACE_AGC(...) DbgConsole_Printf(__VA_ARGS__)
#define TRACE_AGC(...)

#define ENABLE_DISPLAY_DEAD_CELL   FALSE
//#define TRACE_DEAD_CELL(...) printf(__VA_ARGS__)
#define TRACE_DEAD_CELL(...)

#define TRACE_SCAN_TASK(...) //DbgConsole_Printf(__VA_ARGS__)
//#define TRACE_SCAN_TASK(...)

//#define TRACE_CAL_PROC(...) DbgConsole_Printf(__VA_ARGS__)
#define TRACE_CAL_PROC(...)

//#define TRACE_CAL_PD_SHIFT(...) printf(__VA_ARGS__)
#define TRACE_CAL_PD_SHIFT(...)

//#define TRACE_PROC_EXT_LIGHT(...) printf(__VA_ARGS__)
#define TRACE_PROC_EXT_LIGHT(...)  //DbgConsole_Printf(__VA_ARGS__)

#define TRACE_FULL(...) 	//DbgConsole_Printf(__VA_ARGS__)

#ifdef ENABLE_UART_CMD_PROCESS //YJ@210805
#if (DEBUG_VCOM_ENABLE == DEBUG_VCOM_MODE)
#define TRACE_UART(...)    DbgConsole_Printf(__VA_ARGS__);vcom_delay(100);
#else
#define TRACE_UART(...)    DbgConsole_Printf(__VA_ARGS__);
#endif
#else
#define TRACE_UART(...)
#endif
//-----------------------------------------------------------------------------


/*****************************************************************************/
// Tunning parameters
/*****************************************************************************/
#define SCAN_TASK_POWER_ON_INIT_FORCED_EXIT_CNT 0   //kjs 210429 5->0   // kjs 210414 add
#define X_PD_SHIFT_DELAY_USEC_INIT          10	//4   // 0 -> 4
#define Y_PD_SHIFT_DELAY_USEC_INIT          10	//4   // 1 -> 4
#define X_EXT_LIGHT_DELAY_USEC_INIT         0   //
#define Y_EXT_LIGHT_DELAY_USEC_INIT         0   //

//-----------------------------------------------------------------------------
// calibratePdShiftNoise()
//-----------------------------------------------------------------------------
#define CAL_PD_SHIFT_MAX_TRY_CNT        40
#define CAL_PD_SHIFT_OK_CNT             1

//-----------------------------------------------------------------------------
// Power-on LED AGC
//-----------------------------------------------------------------------------

#define LED_AGC_POWER_ON_MAX_TRY_CNT    400 // 120 // 80 -> 120 for test
#define THRESHOLD_AVG_CNT               5   //


//-----------------------------------------------------------------------------
// Calibration_Process()
//-----------------------------------------------------------------------------
#define NO_TOUCH_OBJECT_LINE_CNT            5
#define LED_AGC_ENTRY_CNT                   50 //
#define RUN_THRESHOLD_AVG_CNT               2   // kjs 210315 3 -> 2
#define FORCED_THRESHOLD_AVG_CNT            30  // YJ@210719
#define EXT_LIGHT_LED_AGC_TRY_CNT           5
#define EXT_LIGHT_LED_AGC_ENTRY_LINE_CNT    5

#define LED_AGC_ENTRY_TIME              200    //4000  // [ms] YJ@220207

//-----------------------------------------------------------------------------
// adjustPdShiftDelay()
//-----------------------------------------------------------------------------
#define PD_NOISE_LEVEL                  0
#define PD_SHIFT_DELAY_USEC_MAX         50

//-----------------------------------------------------------------------------
// averageNonBlockLevel()
//-----------------------------------------------------------------------------
#define MIN_THRESHOLD_LEVEL             20

//-----------------------------------------------------------------------------
// checkExtLightNoise()
//-----------------------------------------------------------------------------
#define EXTL_MIN_LEVEL                  20

//-----------------------------------------------------------------------------
// compensateExtLightNoise()
//-----------------------------------------------------------------------------
#define EXT_LIGHT_SHIFT_DELAY_MAX   50
#define EXT_LIGHT_SHIFT_DELAY_STEP  10
#define EXT_LIGHT_INC_DELAY         1
#define EXT_LIGHT_DEC_DELAY         0

//-----------------------------------------------------------------------------
// processExtLightNoise()
//-----------------------------------------------------------------------------
#define PROC_EXT_LIGHT_NOISE_ENTRY_CNT  100
#define PROC_EXT_LIGHT_NOISE_ENTRY_CNT2 50
#define PROC_EXT_LIGHT_NOISE_ENTRY_TIME 100	//1000 // 10[ms]

//-----------------------------------------------------------------------------
// controlLEDLightIntensity()
//-----------------------------------------------------------------------------
#define PD_LEVEL_HIGH               240//250
#if (MODEL_TYPE == CTSI_0650_NXP)
#define PD_LEVEL_LOW              	130  //<==default 130
#elif (MODEL_TYPE == WMC_0650_NXP)
#define PD_LEVEL_LOW                130  //<==default 130
#elif (MODEL_TYPE == WMC_0650_VE_NXP)
#define PD_LEVEL_LOW                130  //<==default 130
#elif (MODEL_TYPE == WMC_0750_NXP)
#define PD_LEVEL_LOW                130  //<==default 130
#elif (MODEL_TYPE == WMC_0850_NXP)
#define PD_LEVEL_LOW                130  //<==default 130
#elif (MODEL_TYPE == SLIM_0320_NXP)
#define PD_LEVEL_LOW                130  //<==default 130
#endif
#define PD_LEVEL_TOLERANCE_X        30
#define PD_LEVEL_TOLERANCE_Y        60
#define LED_CURRENT_CHANGE_STEP     1   //ref =  1       // kjs 210414 2 -> 1
#define PD_LEVEL_SATURATION_RATIO   50
#define LED_AGC_MAX_LIMIT_ERROR     (-1)
#define LED_AGC_MIN_LIMIT_ERROR     (-2)

//-----------------------------------------------------------------------------
// decideDeadCell()
//-----------------------------------------------------------------------------
#define PD_DEAD_CELL_LEVEL          20
#define DEAD_CELL_RATIO             90

//-----------------------------------------------------------------------------
// scanAxis()
//-----------------------------------------------------------------------------
// CHG_SCHEME FALSE is faster than TRUE (160usec @ SysClk 252MHz)
//#define CHG_SCHEME  TRUE
#define CHG_SCHEME  FALSE

//-----------------------------------------------------------------------------
// Partial scan and Full scan
//-----------------------------------------------------------------------------
#define VALID_LINE_MIN_1ST              0
#define VALID_LINE_MIN_2ND              5
#define PARTIAL_SEARCH_AREA_ADD_WIDTH   15
#define X_PARTIAL_ENTRY_PERCENT_WIDTH   70
#if FORCED_FULL_SCAN
#define X_PARTIAL_ENTRY_WIDTH           0
#else
#define X_PARTIAL_ENTRY_WIDTH           ( (X_PARTIAL_ENTRY_PERCENT_WIDTH * X_CELL_SIZE) / 100 )
#endif
#define FULL_TO_PARTIAL_CONDITION_CNT   100
#define PARTIAL_TO_FULL_EXEC_CNT        3

#if (FRONTEND_AAREA == 1)
#define FRONTEND_LINE_THRESHOLD
#endif

#define DATA_INFO_TYPE			(0U)   //0 old data info, 1 new data info
#define USE_SEND_THRESHOLD_DATA (1U)   // 1 for add line data by sub-threshold
#define DLT_DUAL_BUFFER_ENABLE	(0U)
#define DLT_LINE_THRESHOLD		(1U)
#define SCAN_AUTO_CALIBRATION	(0U) //0: for DLT

//for Test
#define TEST_DEAD_CELL_TBL		  (0U)
#define PROCESS_EXT_LIGHT_DISABLE (0U)


#if (WDOG_ENABLE_DEBUG_MESSAGE == 1)
#define TRACE_WDOG(...)			DbgConsole_Printf(__VA_ARGS__)
//#define TRACE_WDOG(...)			DbgConsole_BlockingPrintf(__VA_ARGS__)
#else
#define TRACE_WDOG(...)
#endif

#if (DLT_ENABLE_DEBUG_MESSAGE == 1)
#define TRACE_DLT(...)			DbgConsole_Printf(__VA_ARGS__)
//#define TRACE_DLT(...)			DbgConsole_BlockingPrintf(__VA_ARGS__)
#else
#define TRACE_DLT(...)		//DbgConsole_Printf(__VA_ARGS__)
#endif

#endif /* COMMON_H_ */
