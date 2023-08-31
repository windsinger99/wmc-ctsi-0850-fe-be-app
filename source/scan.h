#ifndef _SCAN_H
#define _SCAN_H

#if 1

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "TSP_Layout.h"
#include "scan_table.h"

typedef enum {
    SCAN_NOT_INITIALIZED = 0,
    SCAN_INIT_COMPLETE,
    SCAN_RESET,
    SCAN_STATUS_ERR
} SCAN_STATUS_T;

/*****************************************************************************/
// Section: Constants
/*****************************************************************************/

#define MAIN_THRESHOLD_MODE             0
#define SUB_THRESHOLD_MODE              1

#define WITH_GD_USB

/*****************************************************************************/
typedef struct {
  uint8_t isPdDead    : 1;
  uint8_t isLedDead   : 1;
  uint8_t isPdNoise   : 1;
    uint8_t             : 5;
} cellStatus_t;

typedef struct {
  uint16_t isPartialScan          : 1; //
  uint16_t requestSubThreshold    : 1;
  uint16_t isLedAgcChanged        : 1;
  uint16_t isPdShiftChk           : 1;
  uint16_t isExtLightDelayChanged : 1;
  uint16_t isCmdReceived          : 1;
  uint16_t isFullScanForPartial   : 1;
  uint16_t                        : 1; //
    uint16_t isUsualExtLightNoiseCal: 1; // kjs 210315 add
    uint16_t isUsualAgcCal          : 1; // kjs 210315 add
    uint16_t                        : 1;
    uint16_t                        : 1;
    uint16_t                        : 1;
    uint16_t                        : 1;
    uint16_t                        : 1;
    uint16_t                        : 1;
} scanStatus_t;

typedef struct {
    uint8_t pdIdxStart;
    uint8_t pdIdxEnd;
    uint8_t ledIdxStart;
    uint8_t ledIdxEnd;
} partialScanData_t;


typedef union TEST_DATA {
  struct {
  unsigned int bit0   :1;
  unsigned int bit1   :1;
  unsigned int bit2   :1;
  unsigned int bit3   :1;
  unsigned int bit4   :1;
  unsigned int bit5   :1;
  unsigned int bit6   :1;
  unsigned int bit7   :1;
  };
  uint8_t VAL;
}_DAC_DATA;
extern _DAC_DATA dac_data;
// Macro Definition
#define M_Update_MinMax(min, max, data) {   \
            if (data < min) min = data;     \
            if (data > max) max = data;     \
        }

#if 1 //nsmoon@220112
#define M_ABS(a)       (((a) < 0) ? -(a) : (a))
#define M_MIN(a, b)    (((a) < (b)) ? (a) : (b))
#define M_MAX(a, b)    (((a) > (b)) ? (a) : (b))
#endif

extern uint32_t gADC_DATA;
/*****************************************************************************/
// Section: Data Types
/*****************************************************************************/
extern scanStatus_t scanStatus;

extern uint8_t xScanResultData[X_CELL_SIZE + 1][X_TOTAL_OFFSET + 1 + 1]; // [pd Index in X][offset between ledindex and pd], last element is for dummy
extern uint8_t yScanResultData[Y_CELL_SIZE + 1][Y_TOTAL_OFFSET + 1 + 1]; // [pd Index in Y][offset between ledindex and pd], last element is for dummy

extern partialScanData_t xPartialScanData, yPartialScanData;
extern cellStatus_t xCellStatus[X_CELL_SIZE]; // [cell Index in X]
extern cellStatus_t yCellStatus[Y_CELL_SIZE]; // [cell Index in X]
extern uint8_t xPdDeadCellCnt, xLedDeadCellCnt, yPdDeadCellCnt, yLedDeadCellCnt;
extern uint8_t subThresholdDropPercent;
extern uint32_t noTouchObjectCnt;
extern uint16_t procExtLightNoiseCnt;
extern uint32_t receivedCmdCnt;

//extern uint8_t xAxisLedOnTimeIdx[X_TOTAL_SCAN_STEPS];
//extern uint8_t yAxisLedOnTimeIdx[Y_TOTAL_SCAN_STEPS];
extern uint8_t xAxisDacIdx[X_TOTAL_SCAN_STEPS];
extern uint8_t yAxisDacIdx[Y_TOTAL_SCAN_STEPS];
extern uint8_t xAxisLedCurrentTblIdx[X_TOTAL_SCAN_STEPS];
extern uint8_t yAxisLedCurrentTblIdx[Y_TOTAL_SCAN_STEPS];

extern uint8_t xScanThresholdData[X_CELL_SIZE][X_MAX_OFFSET * 2 + 1]; // [pd Index in X][offset between ledindex and pd]
extern uint8_t yScanThresholdData[Y_CELL_SIZE][Y_MAX_OFFSET * 2 + 1]; // [pd Index in Y][offset between ledindex and pd]

//extern uint32_t adc_value[ADC_DMA_SIZE];
extern uint32_t adc_value[ADC_SIZE];

#ifdef ENABLE_UART_CMD_PROCESS  //nsmoon@210915
extern uint8_t Current_Test_index_X, Current_Test_index_Y;
extern uint8_t Led_On_Time_Test_X, Led_On_Time_Test_Y;

extern int debug_print_on;      //yj@210713
extern int16_t gCurrentTotalLedIdx, gCurrentStartTotalPdIdx;
#if ENABLE_TRIGER
extern uint8_t Current_Triger_Pd, Current_Triger_Axis, Usb_Off_Test;
#endif
extern const uint8_t (*xAxisSequenceTbl)[2];
extern const uint8_t (*xPdIdxAtANOutTbl)[PD_SIGNAL_OUT_NUM];
extern const uint8_t (*xUOffsetAtANOutTbl)[PD_SIGNAL_OUT_NUM];
extern const uint8_t (*yAxisSequenceTbl)[2];
extern const uint8_t (*yPdIdxAtANOutTbl)[PD_SIGNAL_OUT_NUM];
extern const uint8_t (*yUOffsetAtANOutTbl)[PD_SIGNAL_OUT_NUM];

extern uint8_t xScanNonBlockLevel[X_CELL_SIZE][X_MAX_OFFSET * 2 + 1]; // [pd Index in X][offset between ledindex and pd]
extern uint8_t yScanNonBlockLevel[Y_CELL_SIZE][Y_MAX_OFFSET * 2 + 1]; // [pd Index in Y][offset between ledindex and pd]
//extern uint8_t xScanThresholdData[X_CELL_SIZE][X_MAX_OFFSET * 2 + 1]; // [pd Index in X][offset between ledindex and pd]
//extern uint8_t yScanThresholdData[Y_CELL_SIZE][Y_MAX_OFFSET * 2 + 1]; // [pd Index in Y][offset between ledindex and pd]
extern uint8_t xScanSubThresholdData[X_CELL_SIZE][X_MAX_OFFSET * 2 + 1]; // [pd Index in X][offset between ledindex and pd]
extern uint8_t yScanSubThresholdData[Y_CELL_SIZE][Y_MAX_OFFSET * 2 + 1]; // [pd Index in Y][offset between ledindex and pd]
#endif

extern uint16_t setValueDAC, setADCSel;
extern uint8_t fixedCurrentEnable;
extern uint8_t dltThresholdEnbale;

extern uint32_t diffCYC[10];

#if 0 //(FORCE_AGC_SET == 1)
extern uint8_t forced_agc, forced_saved;  //YJ@210624
#endif
// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************
//inline void codeDelay(uint16_t delay);
//inline void disableAllLEDBoard(void);
//inline void disableAllPD(void);

void Enable_Group_Led_Data(void);
void Disable_Group_Led_Data(void);

void Enable_V_All_Led_Cell(void);
void Enable_H_All_Led_Cell(void);

void Scan_Group_Led(void);
void Scan_All_Group_Led(void);
void Scan_All_Group_HV_PD_Led(void);

void Enable_Group_Pd(void);
void Disable_Group_Pd(void);
void Group_Pd_clk_count(uint32_t _clk_num);

void Scan_All_Group_Pd_Led(void);

void Scan_Group1_led16(void);
void Scan_Group2_led16(void);
void Scan_Group3_led16(void);
void Scan_Group4_led16(void);
void Scan_Group5_led16(void);
void Scan_Group6_led16(void);
void Scan_Group7_led16(void);
void Scan_Group7_led16_1(void);
void Scan_Group7_led16_2(void);

void Init_Group1_pd_on(void);
void Init_Group1_pd_off(void);
void scan_group1_pd9(void);

void Scan_Initialize(void);
void initializeScanHW(void);
SCAN_STATUS_T Scan_Process(void);

int16_t scanAxis(axis_type_enum axisType, uint8_t bLedOn, uint8_t pdIdxMin, uint8_t pdIdxMax, uint8_t ledIdxMin, uint8_t ledIdxMax);

void scanXAxisAuto(void);
void scanYAxisAuto(void);
int16_t Calibration_Process(void);

void updateThresholdData(uint8_t thresholdMode);

int16_t controlLEDLightIntensity(axis_type_enum axisType);
#if AGC_TOGGLE_PREVENTION_ENABLE
int16_t LED_Auto_Gain_ControlWithTryCnt(uint8_t tryCnt);
#else
int16_t LED_Auto_Gain_Control(void);
#endif
void displayLedAgcResult(void);

void setAgcControlTimerMsec( uint32_t delay_ms);
void clearAgcControlTimer(void);
int16_t isAgcControlTimerExpired(void);

void setDelayScanTimer10Msec(uint32_t val);
void clearDelayScanTimer(void);
int16_t isDelayScanTimerExpired(void);

void setPDDelayTimerUsec(uint32_t val);
void set_DelayTimerUsec(uint32_t val);
int16_t isPDDelayTimerExpired(void);
int16_t isDelayTimerExpired(void);
void Timer_Delay_us(uint32_t val);

void setProcessExtLightTimerMsec( uint32_t delay_ms);
void clearProcessExtLightTimer(void);
int16_t isProcessExtLightTimerExpired(void);

uint16_t decideDeadCell(void);

void software_reset(void);

#if 1 //nsmoon@210719
extern int16_t scanAxisFull(axis_type_enum axisType, uint8_t bLedOn);
extern int16_t compensateExtLightNoise(uint8_t axis);
extern int16_t checkExtLightNoise(uint8_t axisType);
extern int16_t averageNonBlockLevel(uint8_t cnt);
extern void setThresholdDataByAvgNonBlockLevel(uint8_t thresholdMode);
extern uint32_t Scan_Data_Process(void);
extern uint8_t getADCResultByPdIdxAndOffset(axis_type_enum axisType, uint8_t pdIdx, int8_t offset);
extern uint8_t getADCResultByDeviceIdx(axis_type_enum axisType, uint8_t pdIdx, uint8_t ledIdx);
extern uint8_t getADCResultByLedIdxAndOffset(axis_type_enum axisType, uint8_t ledIdx, int8_t offset);
#endif

extern void copySequenceTbl(void);

extern void selectPDGroupExt(int16_t startTotalPdIdx); //nsmoon@220113
extern uint16_t findStartScanSequenceIdx(axis_type_enum axisType, uint8_t pdIdxStart, uint8_t ledIdxMin, uint8_t ledIdxMax);

#ifdef ENABLE_UART_CMD_PROCESS
extern int16_t scanAxisTestSeqNo(axis_type_enum axisType, uint8_t bLedOn, int16_t seqNo);
extern void display_led_agc(axis_type_enum axisType, uint8_t pdIdx0);
extern void displayLedAgcResult(void);
extern uint8_t isInSideArea(uint8_t pdIdx, uint8_t ledIdx, uint8_t cellSize, uint8_t cornerCellSize);
extern void dac_data_set(uint8_t data);
#endif

#if (PROCESS_EXT_LIGHT_DISABLE == 0)
extern int16_t processExtLightNoise(void);
#endif

#else
#define ATTR_FRONTEND_RAM3 __attribute__((section(".bss.$RAM3")))

//#define DEF_DEBUG_TP_ENABLE

#define VERTICAL_SCAN                   0
#define HORIZENTAL_SCAN                 1

#define SHIFT_DELAY_TIME                8
#define SHIFT_RESET_TIME                8
#define AN_SW_CTRL1_DELAY_TIME			8

#define MAX_LED_AGC_STEP            	17	//4
#define MAX_VLED_AGC_STEP           	(MAX_LED_AGC_STEP - 1)
#define MAX_HLED_AGC_STEP           	(MAX_LED_AGC_STEP -3)

#define NO_SIGNAL                       0
#define LOW_SIGNAL                      93  //1.2V
#define SATURATION_SIGNAL               232//250    // 3V
#define DEAD_CELL_LEVEL                 20//15
#define CALI_CELL_LEVEL                 20//15
#define DEAD_CELL_RATIO                 90

#define SCAN_LED_AGC_INITIALIZE_COUNT 	(MAX_LED_AGC_STEP+1)
#define SCAN_THRESHOD_INITIALIZE_COUNT  10
#define SCAN_INITIALIZE_COUNT           (SCAN_LED_AGC_INITIALIZE_COUNT + SCAN_THRESHOD_INITIALIZE_COUNT)

#define X_THRESHOLD_RATIO               50//50 //70//50//30=70%//90=10%
#define X_CORNER_THRESHOLD_RATIO        30//30//30
#define Y_THRESHOLD_RATIO               50//50//50 //70//50//30=70%//90=10%
#define Y_CORNER_THRESHOLD_RATIO        30//30//25

#define CALIBRATION_LEVEL_RATIO         80//15

//#define CAL_OPTION
#define CAL_OBJ_CNT						400 //100

#define INVALID_VALUE                   255

#define X_INC_RANGE_L                  	-15	// -25
#define X_INC_RANGE_R                   15	//25
#define Y_INC_RANGE_L                   -25	//-45
#define Y_INC_RANGE_R                   25	//45

typedef enum {
	SCAN_NOT_INITIALIZED = 0,
	SCAN_INIT_COMPLETE,
	SCAN_RESET,
	SCAN_STATUS_ERR
} SCAN_STATUS_T;

typedef struct {
	const uint16_t *LED_Ctrl_Tbl;
	const uint8_t *LED_Shift_Ctrl_Tbl;
	const uint8_t (*Incline_Idx_Tbl)[X_CELL_SIZE];
} DEF_SCAN_COMMON_DATA_TABLE_T;

typedef struct {
    bool    Scan_Direction;
    uint16_t Curr_LED_Shift_Num;
    uint16_t Next_LED_Shift_Num;
    uint16_t LED_Shift_Num;
    uint16_t Curr_LED_On_Num;
    uint16_t Next_LED_On_Num;
    uint16_t Curr_LED_On_Idx;
    uint16_t Next_LED_On_Idx;
    uint16_t MAX_LED_On_Idx;
    uint8_t LED_Group_Cell_Ctrl_Num;
    uint8_t LED_Current_Set_Num;

    uint16_t Curr_PD_Shift_Num;
    uint16_t Next_PD_Shift_Num;
    uint16_t PD_Shift_Num;
    uint16_t PD_Shift_Idx;
}DEF_SCAN_DATA_VAR_T;

typedef struct {
    const uint16_t *PD_Shift_Tbl;
    const uint16_t *LED_On_Tbl;
    const uint16_t (*PD_Group_Shift_Seq_Tbl)[MAX_PD_GROUP_SIZE];

    uint8_t (*VPD_Data_Tbl)[MAX_PD_GROUP_SIZE];
    uint8_t (*VPD_Ref_Data_Tbl)[MAX_PD_GROUP_SIZE];
    uint8_t (*HPD_Data_Tbl)[MAX_PD_GROUP_SIZE];
    uint8_t (*HPD_Ref_Data_Tbl)[MAX_PD_GROUP_SIZE];

    uint8_t (*PD_Data_Tbl)[MAX_PD_GROUP_SIZE];
    uint8_t (*PD_Ref_Data_Tbl)[MAX_PD_GROUP_SIZE];
} DEF_SCAN_DATA_TABLE_T;

typedef struct {
    DEF_SCAN_DATA_TABLE_T Tbl;
    DEF_SCAN_COMMON_DATA_TABLE_T Common_Tbl;
    DEF_SCAN_DATA_VAR_T Var;
} SCAN_DATA_INFO_T;

#define T_HDATA_BUFF_SIZE (X_CELL_SIZE * MAX_X_INCLINE_NUMBER)
#define T_YDATA_BUFF_SIZE (Y_CELL_SIZE * MAX_Y_INCLINE_NUMBER)

extern SCAN_DATA_INFO_T Scan;
extern uint32_t ScanFlag_Initialized_Count;
extern uint8_t VScan_PD_Data_Buf[VERTICAL_LED_ON_MAX_NUMBER][MAX_PD_GROUP_SIZE];
extern uint8_t HScan_PD_Data_Buf[HORIZONTAL_LED_ON_MAX_NUMBER][MAX_PD_GROUP_SIZE];

extern void Scan_Initialize(void);
extern void Make_Test_Data(void);

extern uint8_t s_hor_touch_pd__[T_HDATA_BUFF_SIZE];
extern uint8_t s_hor_touch_led__[T_HDATA_BUFF_SIZE];
extern uint8_t s_ver_touch_pd__[T_YDATA_BUFF_SIZE];
extern uint8_t s_ver_touch_led__[T_YDATA_BUFF_SIZE];

extern uint16_t v_data_len;
extern uint16_t h_data_len;

extern SCAN_STATUS_T scanStatus;
#endif

extern SCAN_STATUS_T Scan_Process(void);

#endif //_SCAN_H

