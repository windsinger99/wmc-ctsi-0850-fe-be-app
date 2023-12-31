/*
 * backend.h
 *
 *  Created on: Jan 11, 2019
 *      Author: Namsu Mun
 */
#ifndef BACKEND_H_
#define BACKEND_H_

#include "common.h"

#if (MODEL_TYPE ==  CTSI_0650_NXP) //kang@211015
#define TILT_EA    3133     //4545//final //3578    //3133  //3578  //3564  //3335
#define MODEL_CTSK_N650_V100  6502 //samsung, nxp stand alone
#define MODEL_SPT       MODEL_CTSK_N650_V100 //MODEL_CTKS_750_V120
#elif (MODEL_TYPE ==  WMC_0650_NXP)
#define TILT_EA    3333     //4545//final //3578    //3133  //3578  //3564  //3335
#define MODEL_CTSK_N650_V100  6502 //samsung, nxp stand alone
#define MODEL_SPT       MODEL_CTSK_N650_V100 //MODEL_CTKS_750_V120
#elif (MODEL_TYPE ==  WMC_0650_VE_NXP)
#define TILT_EA    3663     //4545//final //3578    //3133  //3578  //3564  //3335
#define MODEL_CTSK_N650_V100  6502 //samsung, nxp stand alone
#define MODEL_SPT       MODEL_CTSK_N650_V100 //MODEL_CTKS_750_V120
#elif (MODEL_TYPE ==  WMC_0750_NXP)
#define TILT_EA    3838     //4545//final //3578    //3133  //3578  //3564  //3335
#define MODEL_CTSK_N750_V100  7502  //samsung, nxp stand alone
#define MODEL_SPT       MODEL_CTSK_N750_V100 //MODEL_CTKS_750_V120
#elif (MODEL_TYPE ==  WMC_0850_NXP)
#define TILT_EA    4343     //4545//final //3578    //3133  //3578  //3564  //3335
#define MODEL_CTSK_N850_V100  8502 //samsung, nxp stand alone
#define MODEL_SPT       MODEL_CTSK_N850_V100 //MODEL_CTKS_750_V120
#elif (MODEL_TYPE ==  SLIM_0320_NXP)
#define TILT_EA    2963     //4545//final //3578    //3133  //3578  //3564  //3335
#define MODEL_CTSK_N650_V100  6502 //samsung, nxp stand alone
#define MODEL_SPT       MODEL_CTSK_N650_V100 //MODEL_CTKS_750_V120
#elif (MODEL_TYPE ==  WMC_0850_NXP_VE)
#define TILT_EA    3863     //4545//final //3578    //3133  //3578  //3564  //3335
#define MODEL_CTSK_N850_V100  8502 //samsung, nxp stand alone
#define MODEL_SPT       MODEL_CTSK_N850_V100 //MODEL_CTKS_750_V120
#elif (MODEL_TYPE ==  SLIM_0750_NXP_VE)
#define TILT_EA    3763     //4545//final //3578    //3133  //3578  //3564  //3335
#define MODEL_CTSK_N650_V100  6502 //samsung, nxp stand alone
#define MODEL_SPT       MODEL_CTSK_N650_V100 //MODEL_CTKS_750_V120
#elif (MODEL_TYPE ==  WMC_0750_NXP_VE)
#define TILT_EA    3563     //4545//final //3578    //3133  //3578  //3564  //3335
#define MODEL_CTSK_N750_V100  7502  //samsung, nxp stand alone
#define MODEL_SPT       MODEL_CTSK_N750_V100 //MODEL_CTKS_750_V120
#else
#endif


#if (MODEL_SPT == MODEL_CTSK_N650_V100)
#define FRONTEND_CTKS_650_V100 //frontend version
#define FRONTEND_LINE_THRESHOLD
//#define FRONTEND_RESET_ENABLE //for new boot loader
#define SIZE_UP_FILTER_ENABLE
//#define SMOOTH_FILTER_AVG
//#define WAIT_FOR_READ_VERSION //nsmoon@211222
#define SIZE_UNKNOWN_DISABLE
//#define MARKER_TOP_AAREA
#elif (MODEL_SPT == MODEL_CTSK_N750_V100)
#define FRONTEND_CTKS_750_V100 //frontend version
#define FRONTEND_LINE_THRESHOLD
//#define FRONTEND_RESET_ENABLE //for new boot loader
#define SIZE_UP_FILTER_ENABLE
//#define SMOOTH_FILTER_AVG
//#define WAIT_FOR_READ_VERSION //nsmoon@211222
#elif (MODEL_SPT == MODEL_CTSK_N850_V100)
#define FRONTEND_CTKS_850_V100 //frontend version
#define FRONTEND_LINE_THRESHOLD
//#define FRONTEND_RESET_ENABLE //for new boot loader
#define SIZE_UP_FILTER_ENABLE
//#define SMOOTH_FILTER_AVG
//#define WAIT_FOR_READ_VERSION //nsmoon@211222
#endif

#define FRONTEND_NO_ADC
#define FRONTEND_AAREA  1
#define DEAD_CELL_TBL	1
#define SECOND_OFST_TBL	1
#define USE_FW_KEY_VALIDATION	(0U) //nsmoon@200720 not-use for ss
#define BRUSH_MODE_ENABLE		(1U)
#define FLIP2_COMPATIBLE_DESCRIPTOR	(1U)
#define HOR_EDGE_TOUCH_ENABLE  //nsmoon@200415
//#define FRONTEND_RESET_ENABLE  //nsmoon@200609
//#define BM_SCROLL			  // YJ_20200610
#define TOUCH_SCROLL_INFO_ENABLE
#define NORMAL_RESET_DELAY	//nsmoon@200810
#define FRONTEND_NO_CRC //nsmoon@210311
#define FRONTEND_FORCED_AGC	//YJ@210624
#define FE_USB_RESET_TEST						//YJ@211021
#define USB_HOST_DELAYED_INT_ENABLE		//YJ@211203  --> usb\host\usb_host_ehci.c, usb_host.h enable/Disable Manually change
#define FE_WDT_RST			//YJ@211217
//#define WHEEL_REPORT_TIME_FIX

#define WMC_MODE_CHANGE	//YJ@230206

#define ENABLE_SCAN_HIGH_GAIN_DLT (0U) //nsmoon@220420


#if defined(DEBUG)
#define DEBUG_FUNCTION_ENABLE_ALL
#endif

#ifdef FRONTEND_LINE_THRESHOLD
#define LINE_THRESHOLD_SIZE		(1)
#define LINE_THRESHOLD_VALUE	(75)	//(75)	//(70)//(90)	//(85) //(95)	//(90) R0004		//90
#endif

#if (_DEBUG == 1) || defined(DEBUG)
//#error FIXME
#define ENABLE_DEBUG_MESSAGE	1
#endif

#if (ENABLE_DEBUG_MESSAGE == 1)
extern int DbgConsole_Printf(const char *formatString, ...);
#define DEBUG_PRINTF(...)       DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#define TRACE(...)				DbgConsole_Printf(__VA_ARGS__);//DEBUG_PRINTF(__VA_ARGS__)
#define TRACENR(...)		//DEBUG_PRINTF(__VA_ARGS__)
#define DEBUG_INFO(...)         //usb_echo(__VA_ARGS__);
#define TRACE_ERROR(...)        DEBUG_PRINTF(__VA_ARGS__)
#define TRACE_RCP(...) 			//DEBUG_PRINTF(__VA_ARGS__)
#define TRACEYJ(...)			// DbgConsole_Printf(__VA_ARGS__);DbgConsole_Printf("\r\n")
#else
#define DBG_PRINTF(...)        // DbgConsole_Printf(__VA_ARGS__);
#define DEBUG_PRINTF(...)       //DBG_PRINTF(__VA_ARGS__);DBG_PRINTF("\r\n")
#define TRACE(...)
#define TRACENR(...)
#define TRACE_ERROR(...)       //DbgConsole_Printf(__VA_ARGS__);//DEBUG_PRINTF(__VA_ARGS__)
#define DEBUG_INFO(...)
#define TRACEYJ(...)			//DbgConsole_Printf(__VA_ARGS__);
#define TRACEYJ2(...)			//DbgConsole_Printf(__VA_ARGS__);
#define TRACE_RCP(...) 			//DbgConsole_Printf(__VA_ARGS__);
#define TRACEBE(...)			//DbgConsole_Printf(__VA_ARGS__);
#define TRACSCAN(...)			//DbgConsole_Printf(__VA_ARGS__);
#endif

#define PRG_VERSION			"nxp_backend@190125A"
#define FRONTEND_VID_HEX	0x04D8
#define FRONTEND_PID_HEX	0x0053 //0x0320
#define FRONTEND_BT_PID_HEX	0x003C //bootloader pid

#define MAX_SENSOR_POS_SIZE			500
#define USB_HOST_PACKET_SIZE		(512U)
#define USB_HOSTPACKET_SIZE_FEBT	(64U)

#define PD_DATA_HEADER			0xA5
#define PD_OFST_HEADER			0xA0
#define PD_DATA_HEADER			0xA5
#define PD_DATA_IDX				8  //PD-data start, 0~7  header
#define PD_IDX_NUM_OF_PD_X		4
#define PD_IDX_NUM_OF_PD_Y		6
#define PD_IDX_SCAN_MODE		8
#define PD_IDX_OFST_LEN_X		9
#define PD_IDX_OFST_LEN_Y		10
#define PD_IDX_VAR_HD			11 //idx of variable header

#if (DEAD_CELL_TBL == 1) //nsmoon@190926
#define PD_IDX_DCELL_LEN_X_LED  11
#define PD_IDX_DCELL_LEN_X_PD   12
#define PD_IDX_DCELL_LEN_Y_LED  13
#define PD_IDX_DCELL_LEN_Y_PD   14
#endif

#if ENABLE_SCAN_GAIN_HIGH //nsmoon@220420
#define PD_IDX_SCAN_GAIN_HIGH   15 //same as PD_IDX_OFST_LEN_X2
#endif
#if 1 //nsmoon@200706
#define PD_IDX_VER_LEN			17 //same as old PD_IDX_SEN_LEN_X2
#endif

#if (SECOND_OFST_TBL == 1) //nsmoon@190624
#define PD_IDX_OFST_LEN_X2      15
#define PD_IDX_OFST_LEN_Y2      16
#define PD_IDX_SEN_LEN_X2       17
#define PD_IDX_SEN_LEN_Y2       18
#define PD_IDX_POS_START        19 //position data
#else
#define PD_IDX_POS_START        15 //position data
#endif
#define PD_IDX_LED_POS_X		(PD_IDX_POS_START)
#define PD_IDX_PD_POS_X			(PD_IDX_LED_POS_X + g_back_conf.numPdX)
#define PD_IDX_LED_POS_Y		(PD_IDX_PD_POS_X + g_back_conf.numPdX)
#define PD_IDX_PD_POS_Y			(PD_IDX_LED_POS_Y + g_back_conf.numPdY)

#define PD_DATA_IDX2			4 //PD-data start

#define USB_OFST_CMD			0x80
#define USB_DATA_CMD			0x81
#define USB_DATA_ADC_CMD		0x82
#define USB_DATA_SEN_LIFE_TEST  0x83
#define USB_DATA_THRESHOLD 		0x84 //nsmoon@191212
#define USB_RESET_CMD			0x0A //nsmoon@191022
#define USB_REQ_SCAN_RUN		0xA1 //DL-TEST@0604
#define USB_REQ_SCAN_STOP		0xA2 //DL-TEST@0604
#if ENABLE_SCAN_HIGH_GAIN_DLT //nsmoon@220404
#define USB_REQ_GAIN_HIGH		0xA3
#endif
#define USD_MAX_RECV_IDL_CNT	10

//#if  (MODEL_SPT == MODEL_CTSK_N650_V100)||(MODEL_SPT == MODEL_CTSK_N750_V100)||(MODEL_SPT == MODEL_CTSK_N850_V100)
#define MAX_NUM_PD_X_DLT		X_CELL_SIZE //  96
#define MAX_NUM_PD_Y_DLT		Y_CELL_SIZE //  78
#if (MODEL_TYPE == CTSI_0650_NXP)
#define MAX_X_SLOPE_VAL     31 //256, 8-bits
#define MAX_Y_SLOPE_VAL     33 //256, 8-bits
#elif (MODEL_TYPE == WMC_0650_NXP)
#define MAX_X_SLOPE_VAL     33 //256, 8-bits
#define MAX_Y_SLOPE_VAL     33 //256, 8-bits
#elif (MODEL_TYPE ==  WMC_0650_VE_NXP)
#define MAX_X_SLOPE_VAL     36 //256, 8-bits
#define MAX_Y_SLOPE_VAL     63 //256, 8-bits
#elif (MODEL_TYPE == WMC_0750_NXP)
#if 0
#define MAX_X_SLOPE_VAL     38 //256, 8-bits
#define MAX_Y_SLOPE_VAL     38 //256, 8-bits
#else
#define MAX_X_SLOPE_VAL     36 //256, 8-bits
#define MAX_Y_SLOPE_VAL     38 //256, 8-bits
#endif
#elif (MODEL_TYPE ==  WMC_0850_NXP)
#define MAX_X_SLOPE_VAL     43 //256, 8-bits
#define MAX_Y_SLOPE_VAL     43 //256, 8-bits
#elif (MODEL_TYPE ==  SLIM_0320_NXP)
#define MAX_X_SLOPE_VAL     29 //256, 8-bits
#define MAX_Y_SLOPE_VAL     63 //256, 8-bits
#elif (MODEL_TYPE ==  WMC_0850_NXP_VE)
#define MAX_X_SLOPE_VAL     38//38//26 //256, 8-bits
#define MAX_Y_SLOPE_VAL     63//63//63 //256, 8-bits
#elif (MODEL_TYPE ==  SLIM_0750_NXP_VE)
#define MAX_X_SLOPE_VAL     37//38//26 //256, 8-bits
#define MAX_Y_SLOPE_VAL     63//63//63 //256, 8-bits
#elif (MODEL_TYPE ==  WMC_0750_NXP_VE)
#define MAX_X_SLOPE_VAL     35//38//26 //256, 8-bits
#define MAX_Y_SLOPE_VAL     63//63//63 //256, 8-bits
#endif
//#else
//#ERROR MODEL_SPT NOT DEFINED!
//#endif

#define MAX_NUM_SLOPES_DLT_X	(MAX_X_SLOPE_VAL * 2 + 1)
#define MAX_NUM_SLOPES_DLT_Y	(MAX_Y_SLOPE_VAL * 2 + 1)
#define MAX_VER_TOUCH_PD		(MAX_NUM_PD_Y_DLT * MAX_NUM_SLOPES_DLT_Y)
#define MAX_HOR_TOUCH_PD		(MAX_NUM_PD_X_DLT * MAX_NUM_SLOPES_DLT_X)

#if (DEAD_CELL_TBL == 1) //nsmoon@190829
#define MAX_DEADCELL_SIZE_X		(MAX_NUM_PD_X_DLT)
#define MAX_DEADCELL_SIZE_Y		(MAX_NUM_PD_Y_DLT)
#endif
#if (SECOND_OFST_TBL == 1) //nsmoon@190624
#define MAX_NUM_2ND_SENSOR_X	20
#define MAX_NUM_2ND_SENSOR_Y	20
#endif
#define USB_CRC_SIZE			2 //bytes

#define MAX_SHIFT_TBL_BYTE		8
#define MAX_SHIFT_TBL			64

#if 1 //nsmoon@191119
#define NUM_ADC_BUFF			3
#define USB_CMD_LEN				10
#define USB_PARAM_LEN			(USB_CMD_LEN - 2)
#endif

#define MAX_SLOPE_BYTE_X        ((MAX_NUM_SLOPES_DLT_X + 7) / 8)
#define MAX_SLOPE_BYTE_Y        ((MAX_NUM_SLOPES_DLT_Y + 7) / 8)
#define MAX_LINE_DATA_X			(MAX_NUM_PD_X_DLT * MAX_SLOPE_BYTE_X)
#define MAX_LINE_DATA_Y			(MAX_NUM_PD_Y_DLT * MAX_SLOPE_BYTE_Y)
//#define MAX_EXB_SEND_BUF_SIZE   (PD_DATA_IDX2 + (MAX_NUM_PD_X_DLT * MAX_SLOPE_BYTE_X) + (MAX_NUM_PD_Y_DLT * MAX_SLOPE_BYTE_Y) + (MAX_DEADCELL_SIZE_X * 2) + (MAX_NUM_2ND_SENSOR_X * 2))
#ifdef FRONTEND_LINE_THRESHOLD
#define MAX_HOR_LINE_THRESHOLD	(MAX_LINE_DATA_X * LINE_THRESHOLD_SIZE)
#define MAX_VER_LINE_THRESHOLD	(MAX_LINE_DATA_Y * LINE_THRESHOLD_SIZE)
#define MAX_EXB_SEND_BUF_SIZE   (PD_DATA_IDX2 +  MAX_LINE_DATA_X + MAX_LINE_DATA_Y + MAX_HOR_LINE_THRESHOLD + MAX_VER_LINE_THRESHOLD + USB_CRC_SIZE)
#else
#define MAX_EXB_SEND_BUF_SIZE   (PD_DATA_IDX2 + (MAX_NUM_PD_X_DLT * MAX_SLOPE_BYTE_X) + (MAX_NUM_PD_Y_DLT * MAX_SLOPE_BYTE_Y) + ((MAX_NUM_PD_X_DLT + MAX_NUM_PD_Y_DLT) * NUM_ADC_BUFF) + USB_CRC_SIZE)
#endif
#define MAX_USB_IN_PACKET_CNT	(((MAX_EXB_SEND_BUF_SIZE + (USB_HOST_PACKET_SIZE - 1)) / USB_HOST_PACKET_SIZE))
#define MAX_USB_IN_BUF_SIZE		(USB_HOST_PACKET_SIZE * MAX_USB_IN_PACKET_CNT)
#define MAX_USB_OUT_BUF_SIZE	(USB_HOST_PACKET_SIZE)
#define MAX_OFST_DATA_SIZE		(PD_IDX_OFST_X + (MAX_NUM_SLOPES_DLT_X + MAX_NUM_SLOPES_DLT_Y) + (4 * MAX_NUM_PD_X_DLT * 2) + (4 * MAX_NUM_PD_Y_DLT * 2) + (4 * 8))


#if (MODEL_TYPE ==  CTSI_0650_NXP) //kang@211015
#define TEST_LOGICAL_X_MAX    22856 //22856 =14285*1.6
#define TEST_LOGICAL_Y_MAX    12860 //12860.8= 8038*1.6
#define TEST_PHYSICAL_X_MAX   14285 //1458.35-29.85 = 1428.5
#define TEST_PHYSICAL_Y_MAX   8038  //833.45-29.65 = 803.8

#elif ((MODEL_TYPE ==  WMC_0650_NXP))
#define TEST_LOGICAL_X_MAX    22856 //22856 =14285*1.6
#define TEST_LOGICAL_Y_MAX    12856 //12856= 8035*1.6
#define TEST_PHYSICAL_X_MAX   14285 //1454.12-25.62 = 1428.5
#define TEST_PHYSICAL_Y_MAX   8035  //829.14-25.64 = 803.5

#elif ((MODEL_TYPE ==  WMC_0650_VE_NXP))
#define TEST_LOGICAL_X_MAX    22856 //22856 =14285*1.6
#define TEST_LOGICAL_Y_MAX    12856 //12856= 8035*1.6
#define TEST_PHYSICAL_X_MAX   14285 //1454.12-25.62 = 1428.5
#define TEST_PHYSICAL_Y_MAX   8035  //829.14-25.64 = 803.5

#elif ((MODEL_TYPE ==  WMC_0750_NXP))
#define TEST_LOGICAL_X_MAX    26403 //26403.2 =16502*1.6
#define TEST_LOGICAL_Y_MAX    14852 //14852.8= 9283*1.6
#define TEST_PHYSICAL_X_MAX   16502 //1677.72-27.52 = 1650.2
#define TEST_PHYSICAL_Y_MAX   9283  //955.78-27.48 = 928.3

#elif (MODEL_TYPE ==  WMC_0850_NXP)
#define TEST_LOGICAL_X_MAX    29952 //16502*1.6 = 2995.2
#define TEST_LOGICAL_Y_MAX    16848 //10530*1.6 = 1684.8
#define TEST_PHYSICAL_X_MAX   18720 //1900.00-28.00 = 1872.0
#define TEST_PHYSICAL_Y_MAX   10530  //1081.00-28.00 = 1053.0

#elif (MODEL_TYPE ==  SLIM_0320_NXP)
#define TEST_LOGICAL_X_MAX    11174 //698.4*1.6 = 1117.4
#define TEST_LOGICAL_Y_MAX    6285 //3928*1.6 = 628.5
#define TEST_PHYSICAL_X_MAX   6984 //718.1-19.7 = 698.4
#define TEST_PHYSICAL_Y_MAX   3928  //412.57-19.73 = 392.84

#elif (MODEL_TYPE ==  WMC_0850_NXP_VE)
#define TEST_LOGICAL_X_MAX    29952 //16502*1.6 = 2995.2
#define TEST_LOGICAL_Y_MAX    16848 //10530*1.6 = 1684.8
#define TEST_PHYSICAL_X_MAX   18720 //1900.00-28.00 = 1872.0
#define TEST_PHYSICAL_Y_MAX   10530 //1081.00-28.00 = 1053.0

#elif (MODEL_TYPE ==  SLIM_0750_NXP_VE)
#define TEST_LOGICAL_X_MAX     26469//16502*1.6 = 2995.2
#define TEST_LOGICAL_Y_MAX     14883//10530*1.6 = 1684.8
#define TEST_PHYSICAL_X_MAX    16543//1900.00-28.00 = 1872.0
#define TEST_PHYSICAL_Y_MAX    9302//1081.00-28.00 = 1053.0

#elif (MODEL_TYPE ==  WMC_0750_NXP_VE)
#define TEST_LOGICAL_X_MAX     26403//16502*1.6 = 2640.3
#define TEST_LOGICAL_Y_MAX     14852//9282*1.6 = 1684.8
#define TEST_PHYSICAL_X_MAX    16502//1650.24-29.48 = 1650.24
#define TEST_PHYSICAL_Y_MAX    9282//957.73-29.47 = 928.26


#else
#endif


#define ALLOWABLE_TOUCH_BE		(20)	//(15)
#define ALLOWABLE_TOUCH_IO		(ALLOWABLE_TOUCH_BE*2) //10-multi,4-fine
#define	BYTES_PER_POS			10 // 10 bytes per position
#define TP_PER_PACKET			5  // 5 TP per packet

#ifndef LOBYTE_16
#define LOBYTE_16(x)  ((uint8_t)((x) & 0x00FF))
#endif
#ifndef HIBYTE_16
#define HIBYTE_16(x)  ((uint8_t)(((x) & 0xFF00) >> 8))
#endif
#ifndef GET_MAX
#define GET_MAX(a, b)			(((a) > (b)) ? (a) : (b))
#endif
#ifndef GET_MIN
#define GET_MIN(a, b)			(((a) < (b)) ? (a) : (b))
#endif
#ifndef GET_ABS
#define GET_ABS(a)				(((a) < 0) ? -(a) : (a))
#endif

#define MIN_INITIAL_VAL         9999
#define MIN_INITIAL_VAL_SQ      90000000 //3000*3000
#define EPSILON					0.0001f //do not change
#define IS_DEBUG_FLAG
#define TRACE_NOP

#include "board.h"
#if 0 //gpio, nsmoon@190116
#include "fsl_gpio.h"
/* for debugging */
#define DEBUG_LED_GPIO		BOARD_USER_LED_GPIO
#define DEBUG_LED_GPIO_PIN	BOARD_USER_LED_GPIO_PIN
#define DEBUG_LED_TOGGLE	GPIO_PortToggle(DEBUG_LED_GPIO, 1u << DEBUG_LED_GPIO_PIN)
#define DEBUG_LED_ON		GPIO_PortSet(DEBUG_LED_GPIO, 1u << DEBUG_LED_GPIO_PIN)
#define DEBUG_LED_OFF		GPIO_PortClear(DEBUG_LED_GPIO, 1u << DEBUG_LED_GPIO_PIN)
#define DEBUG_DELAY_COUNT	8000000
#define DEBUG_DELAY_1M \
	DEBUG_LED_TOGGLE; \
    volatile uint32_t debugCnt = 0; \
	for (debugCnt = 0; debugCnt < 1000000; debugCnt++) { \
	    __asm("NOP"); /* delay */ \
	} /*sys clock check, -O0, 8-instuctions */
#else
#define DEBUG_LED_GPIO
#define DEBUG_LED_GPIO_PIN
#define DEBUG_LED_TOGGLE
#define DEBUG_LED_ON
#define DEBUG_LED_OFF
#define DEBUG_DELAY_1M
#endif //gpio

#define ATTR_BACKEND_RAM
#define ATTR_BACKEND_RAM2	__attribute__((section(".bss.$RAM2")))  //SRAM_ITC
#define ATTR_BACKEND_RAM3	__attribute__((section(".bss.$RAM3")))  //SRAM_OC
#define ATTR_BACKEND_CONST	__attribute__((aligned(4))) const
#define ATTR_BACKEND_RAMFUNC __attribute__((section(".ramfunc.$RAM2")))  //SRAM_ITC


#if 1 //debug
#define DEBUG_CYCCNT_CLEAR() { \
	    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; \
	    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; \
	    DWT->CYCCNT = 0; \
}
#define DEBUG_CYCCNT_PRINT() { \
	    uint32_t cyccnt = DWT->CYCCNT; \
	    TRACE_ERROR("cyccnt=%d\r\n", cyccnt); \
}
#define DEBUG_CYCCNT_PRINT_US() { \
	    uint32_t cyccnt = DWT->CYCCNT; \
	    TRACE_ERROR("cyccnt(us)=%d\r\n", cyccnt/600); \
}
#endif
//-----------------------------------------------------------------
// DO NOT CHANGE WITHOUT CHANGING BFRONTEND DEFINITION
// MUST BE SAME STRUCTURE WITH BACKEND !!!
//-----------------------------------------------------------------
typedef struct {
    uint8_t len;
    int8_t *tbl;
} ofs_tbl_t;

typedef struct {
    uint8_t len;
    uint8_t *tbl;
} sen_tbl_t;

typedef struct {
    uint8_t pd;
    uint8_t led; //slope-idx, 0:-max
#if (USE_ADC_DATA == 1)
    uint8_t adc;
#endif
} DEF_PD;

typedef struct {
    uint16_t hor_len;
    uint16_t ver_len;
    DEF_PD	*ver_touch_pd;
    DEF_PD	*hor_touch_pd;
#ifdef FRONTEND_LINE_THRESHOLD
    uint16_t threshold_x_cnt;
    uint16_t threshold_y_cnt;
    uint8_t *threshold_x;
    uint8_t *threshold_y;
#endif
} DEF_PD_INFO;

#if (DEAD_CELL_TBL == 1) //nsmoon@190829
typedef struct {
    uint8_t hor_pd_len;
    uint8_t hor_led_len;
    uint8_t ver_pd_len;
    uint8_t ver_led_len;
    uint8_t	*hor_dead_pd;
    uint8_t	*hor_dead_led;
    uint8_t	*ver_dead_pd;
    uint8_t	*ver_dead_led;
} DEF_DEADCELL_INFO;
#endif

typedef struct {
#if 1 //(SECOND_OFST_TBL == 1) //nsmoon@190625
    uint16_t  hor_pd_num;		//number of sensor on X axis
    uint16_t  ver_pd_num;		//number of sensor on Y axis
    uint16_t  logical_x_size;	//logical horizontal size
    uint16_t  logical_y_size;	//logical vertical size
    uint8_t maxOfstValX;		//max ofst value of X axis
    uint8_t maxOfstValY;		//max ofst value of Y axis
    uint8_t maxOfstValX2;		//max 2nd ofst value of X axis
    uint8_t maxOfstValY2;		//max 2nd ofst value of Y axis
    sen_tbl_t senTblX2;			//X sensor table for 2nd offset
    sen_tbl_t senTblY2;			//Y sensor table for 2nd offset
#endif
#if (USE_VALID_OFST_TBL == 1)  //nsmoon@191129
    uint8_t *validOfstX;
    uint8_t *validOfstY;
#endif
    float sensor_zero_x;
    float sensor_end_x;  //(sensor_pos_x[] + sensor_pos_x[0])
    float sensor_zero_y;
    float sensor_end_y;  //(sensor_pos_y[] + sensor_pos_y[0])

    float aarea_zero_x;  // Active Area
    float aarea_end_x;   // sensor_end_x - aarea_zero_x
    float aarea_zero_y;
    float aarea_end_y;

    float *pd_pos_x;		//X pd position table
    float *pd_pos_y;		//X led position table
    float *led_pos_x;		//Y pd position table
    float *led_pos_y;		//Y led position table
} DEF_TP_LAYOUT_INFO;

typedef enum
{
    INIT_SCAN = 0,
    NORMAL_SCAN = 1,
} SCAN_STATES;

typedef struct {
    SCAN_STATES currScan;
    SCAN_STATES nextScan;
    uint16_t numTouch;
    uint16_t x1, y1;
    uint16_t x2, y2;
#if (BRUSH_MODE_ENABLE == 1) //brush mode, FIXME
    uint8_t brushMode;
#endif
} next_scan_t;

typedef enum
{
    NO_BACKEND_ERROR = 0,
    NOT_INITIALIZED = 10,   // 10 error, not initialized
    INVALID_SCAN_MODE = 20, // 20 error, invalid scan mode
    SKIP_HID_REPORT = 30,   // 30 no error, skip hid report
    SKIP_FRONTEND = 40,     // 40 no error, skip frontend
} BACKEND_STATUS;

typedef struct {
    uint8_t status;
    uint8_t contactID;
    uint16_t xCord;
    uint16_t yCord;
    uint16_t xSize;
    uint16_t ySize;

#if 1	//shj	for brush, simultaneous release & tip switch
    uint8_t ID_staus;
    uint16_t pressure;
    uint8_t nReportSendCnt;
#endif
} DEF_TOUCH_OUT;

typedef struct {
    uint8_t len;
#if 1	//shj	for brush, simultaneous release & tip switch
    uint8_t reSend;
    uint32_t outcnt;
#endif
    DEF_TOUCH_OUT *buf;
} DEF_DATA_INFO;

#if 1 //refer to backend_util.h
typedef struct {
    float x, y;
} vec_t, *vec;
#endif

typedef struct {
    float xSize, ySize;
} tp_size_t;

#define MAX_THRESHOLD_CNT   0xFF
typedef struct {
    uint8_t th50CntX;
    uint8_t th10CntX;
    uint8_t th50CntY;
    uint8_t th10CntY;
    float th10WidthX;
    float th10WidthY;
} tp_line_cnt_t;

typedef struct {
    int touch_count;
    vec_t *touch_data;
    tp_size_t *touch_size;
    tp_line_cnt_t *touch_line_cnt;
    int8_t *multi_fine;
#ifdef HOR_EDGE_TOUCH_ENABLE
    vec_t touch_data_edge;
#endif
} DEF_DATA_INFO2;
//---------------------------------------------

typedef struct {
    uint16_t numPdX, numPdY;   //num of pd
    uint8_t  numOfsX, numOfsY; //num of offset
    uint8_t  maxOfsX, maxOfsY; //max offset //nsmoon@210603
    uint8_t slope_byte_x, slope_byte_y; //num of slope byte
} backend_config_t;

typedef struct {
    uint64_t upper;
    uint64_t lower;
} slope_mask_t;

typedef enum
{
    ENUM_HOR_X = 0,
    ENUM_VER_Y = 1,
} axis_t;

typedef enum
{
    backendDataOffset = 0x00,
    backendDataLine = 0x01,
    backendDataLineAdc = 0x02,
    backendDataLineAdcReady = 0x03,
    backendDataIdle,
    backendDataError = 0xFF
} backend_data_t;

////////////////////////////////////
//extern backend functions
////////////////////////////////////
#if (SECOND_OFST_TBL == 1) //nsmoon@190624
#if (DEAD_CELL_TBL == 1) //nsmoon@190829
int BG_init_backend(
    DEF_TP_LAYOUT_INFO *tpLayout, 	//sensor laypout
    DEF_DEADCELL_INFO  *InvalidTbl,			//not used yet
    uint16_t  init_flag          // reserved: 0
);
#else
int BG_init_backend(
    DEF_TP_LAYOUT_INFO *tpLayout, 	//sensor laypout
    DEF_PD_INFO  *ValidTbl,			//not used yet
    uint16_t  init_flag          // reserved: 0
);
#endif

extern BACKEND_STATUS BG_call_backend2(
    DEF_PD_INFO *InBuf,       // (hor_pd_num + ver_pd_num)
    DEF_DATA_INFO2	*OutBuf,
    next_scan_t *nextScan     // next scan infomation
);
#endif //1
#endif /* BACKEND_H_ */
