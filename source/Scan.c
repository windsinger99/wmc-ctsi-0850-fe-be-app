// *****************************************************************************
// Section: Included Files
// *****************************************************************************
#if 1
#include <stdio.h>
#include <stdbool.h>
#include <string.h> // for memset()
#include "scan.h"
#include "dlt.h"
#include "common.h"
#include "macro.h"
#include "dlt.h"
#include "scan_table.h"
#include "uartCommand.h"
#include "adc.h"
#include "TSP_Layout.h"
#include "gpt.h"
#include "macro.h"
#include "app.h"
#include "composite.h"
#include "fsl_common.h"
//#define ENABLE_TRACE_SCAN_AXIS  TRUE

ATTR_BACKEND_RAM2 uint32_t adc_value[ADC_SIZE];      //uint32_t adc_value[ADC_DMA_SIZE];

/*****************************************************************************/
// Section: Constant Table
/*****************************************************************************/

#if 0 //for test
#define SCAN_TIME_LEN 5
#define SCAN_TIME_STR_LEN 80
static uint32_t curTimeScan, prevTimescan;
static uint32_t scan_time[SCAN_TIME_LEN];
static char scan_time_str[SCAN_TIME_STR_LEN];
#define SCAN_TIME_INIT() {prevTimescan=DWT->CYCCNT;}
#define SCAN_TIME_ADD(a) {curTimeScan=DWT->CYCCNT; scan_time[(a)]=(curTimeScan-prevTimescan); prevTimescan=DWT->CYCCNT;}
#define SCAN_TIME_PRINT() { \
  int i, idx; \
  scan_time_str[0] = '\0'; \
  for (i =0; i < SCAN_TIME_LEN; i++) { \
    idx = strlen(scan_time_str); \
    snprintf(&scan_time_str[idx], (SCAN_TIME_STR_LEN-idx), "%d ", scan_time[i]); \
    } \
  DbgConsole_Printf("@@@ %s \r\n", scan_time_str); \
}
#else
#define SCAN_TIME_INIT()
#define SCAN_TIME_ADD(a)
#define SCAN_TIME_PRINT()
#endif


typedef enum
{
    /* Application's state machine's initial state. */
    SCAN_STATE_INIT = 0,

    /* Application waits for device configuration */
    SCAN_STATE_PRE_PROCESSING,

    //
    SCAN_STATE_POWER_ON_CAL_PD_SHIFT,

    //
    SCAN_STATE_POWER_ON_EXT_LIGHT_NOISE_PROC,

    //
    SCAN_STATE_POWER_ON_AGC,

    //
    SCAN_STATE_SET_THRESHOLD_LEVEL,

    //
    SCAN_STATE_CHK_NO_OBJECT,

    /* Application runs the main task */
    SCAN_STATE_MAIN_TASK,

    SCAN_STATE_WAIT_FOR_HOST_CMD,

    SCAN_STATE_XFER_DATA_TO_HOST,

    /* Application error occurred */
    SCAN_STATE_ERROR

} SCAN_TASK_STATES;

typedef struct
{
    SCAN_TASK_STATES state;
} SCAN_TASK_DATA;


#if (MODEL_TYPE == CTSI_0650_NXP)

const uint8_t X_LedNumOfBd[X_LED_BD_NUM] = {
    X_LED_BD1_CELL_NUM,
    X_LED_BD2_CELL_NUM,
    X_LED_BD3_CELL_NUM,
};

const uint8_t X_StartLedIndexOfBd[X_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
};

const uint8_t Y_LedNumOfBd[Y_LED_BD_NUM] = {
    Y_LED_BD1_CELL_NUM,
    Y_LED_BD2_CELL_NUM,
};

const uint8_t Y_StartLedIndexOfBd[Y_LED_BD_NUM] = {
    0,
    Y_LED_BD1_CELL_NUM,
};


const uint8_t StartLedIndexOfBd[TOTAL_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
    X_CELL_SIZE,
    X_CELL_SIZE + Y_LED_BD1_CELL_NUM
};

const uint8_t StartLedIndexOfGroupOffset[TOTAL_LED_BD_NUM] = {
    0,
    2,
    5,
    6,
    7
};

/* cell per group : 6 */
const uint8_t StartLedIndexOfGroup[TOTAL_LED_BD_NUM] = {
    0,          // 34 + 2 = 36  // 37 + 5 = 42
    2,          // 27 + 3 = 30  // 30 + 0 = 30
    2+3,        // 35 + 1 = 36  // 38 + 4 = 42
    2+3+1,      // 41 + 1 = 42  // 45 + 3 = 48
    2+3+1+5,    // 37 + 11= 48  // 37 + 5 = 42
};

#elif (MODEL_TYPE == WMC_0650_NXP)

const uint8_t X_LedNumOfBd[X_LED_BD_NUM] = {
    X_LED_BD1_CELL_NUM,
    X_LED_BD2_CELL_NUM,
    X_LED_BD3_CELL_NUM,
};

const uint8_t X_StartLedIndexOfBd[X_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
};

const uint8_t Y_LedNumOfBd[Y_LED_BD_NUM] = {
    Y_LED_BD1_CELL_NUM,
    Y_LED_BD2_CELL_NUM,
};

const uint8_t Y_StartLedIndexOfBd[Y_LED_BD_NUM] = {
    0,
    Y_LED_BD1_CELL_NUM,
};

const uint8_t StartLedIndexOfBd[TOTAL_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
    X_CELL_SIZE,
    X_CELL_SIZE + Y_LED_BD1_CELL_NUM
};

const uint8_t StartLedIndexOfGroupOffset[TOTAL_LED_BD_NUM] = {
    0,
    1,
    5,
    6,
    9
};

#elif (MODEL_TYPE == WMC_0650_VE_NXP)

const uint8_t X_LedNumOfBd[X_LED_BD_NUM] = {
    X_LED_BD1_CELL_NUM,
    X_LED_BD2_CELL_NUM,
    X_LED_BD3_CELL_NUM,
};

const uint8_t X_StartLedIndexOfBd[X_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
};

const uint8_t Y_LedNumOfBd[Y_LED_BD_NUM] = {
    Y_LED_BD1_CELL_NUM,
    Y_LED_BD2_CELL_NUM,
};

const uint8_t Y_StartLedIndexOfBd[Y_LED_BD_NUM] = {
    0,
    Y_LED_BD1_CELL_NUM,
};

const uint8_t StartLedIndexOfBd[TOTAL_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
    X_CELL_SIZE,
    X_CELL_SIZE + Y_LED_BD1_CELL_NUM
};

const uint8_t StartLedIndexOfGroupOffset[TOTAL_LED_BD_NUM] = {
    0,
    4,
    8,
    10,
    15
};


#elif (MODEL_TYPE == WMC_0750_NXP)

const uint8_t X_LedNumOfBd[X_LED_BD_NUM] = {
    X_LED_BD1_CELL_NUM,
    X_LED_BD2_CELL_NUM,
    X_LED_BD3_CELL_NUM,
    X_LED_BD4_CELL_NUM,
};

const uint8_t X_StartLedIndexOfBd[X_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM + X_LED_BD3_CELL_NUM,
};

const uint8_t Y_LedNumOfBd[Y_LED_BD_NUM] = {
    Y_LED_BD1_CELL_NUM,
    Y_LED_BD2_CELL_NUM,
};

const uint8_t Y_StartLedIndexOfBd[Y_LED_BD_NUM] = {
    0,
    Y_LED_BD1_CELL_NUM,
};

const uint8_t StartLedIndexOfBd[TOTAL_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM + X_LED_BD3_CELL_NUM,
    X_CELL_SIZE,
    X_CELL_SIZE + Y_LED_BD1_CELL_NUM
};

const uint8_t StartLedIndexOfGroupOffset[TOTAL_LED_BD_NUM] = {
    0,
    5,
    10,
    11,
    14,
    17
};
#elif (MODEL_TYPE == WMC_0850_NXP)

const uint8_t X_LedNumOfBd[X_LED_BD_NUM] = {
    X_LED_BD1_CELL_NUM,
    X_LED_BD2_CELL_NUM,
    X_LED_BD3_CELL_NUM,
    X_LED_BD4_CELL_NUM,
};

const uint8_t X_StartLedIndexOfBd[X_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM + X_LED_BD3_CELL_NUM,
};

const uint8_t Y_LedNumOfBd[Y_LED_BD_NUM] = {
    Y_LED_BD1_CELL_NUM,
    Y_LED_BD2_CELL_NUM,
    Y_LED_BD3_CELL_NUM,
};

const uint8_t Y_StartLedIndexOfBd[Y_LED_BD_NUM] = {
    0,
    Y_LED_BD1_CELL_NUM,
    Y_LED_BD1_CELL_NUM + Y_LED_BD2_CELL_NUM,
};

const uint8_t StartLedIndexOfBd[TOTAL_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM + X_LED_BD3_CELL_NUM,
    X_CELL_SIZE,
    X_CELL_SIZE + Y_LED_BD1_CELL_NUM,
    X_CELL_SIZE + Y_LED_BD1_CELL_NUM + Y_LED_BD2_CELL_NUM,
};

const uint8_t StartLedIndexOfGroupOffset[TOTAL_LED_BD_NUM] = {
    0,
    5,
    7,
    9,
    12,
    14,
    15
};


#elif (MODEL_TYPE == SLIM_0320_NXP)

const uint8_t X_LedNumOfBd[X_LED_BD_NUM] = {
    X_LED_BD1_CELL_NUM,
    X_LED_BD2_CELL_NUM,
};

const uint8_t X_StartLedIndexOfBd[X_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
};

const uint8_t Y_LedNumOfBd[Y_LED_BD_NUM] = {
    Y_LED_BD1_CELL_NUM,
};

const uint8_t Y_StartLedIndexOfBd[Y_LED_BD_NUM] = {
    0,
};


const uint8_t StartLedIndexOfBd[TOTAL_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_CELL_SIZE,
};

const uint8_t StartLedIndexOfGroupOffset[TOTAL_LED_BD_NUM] = {
    0,
    3,
    6
};

/* cell per group : 6 */
const uint8_t StartLedIndexOfGroup[TOTAL_LED_BD_NUM] = {
    0,          // 34 + 2 = 36  // 37 + 5 = 42
    3,          // 27 + 3 = 30  // 30 + 0 = 30
    3+3,        // 35 + 1 = 36  // 38 + 4 = 42
};

#elif (MODEL_TYPE == WMC_0850_NXP_VE)

const uint8_t X_LedNumOfBd[X_LED_BD_NUM] = {
    X_LED_BD1_CELL_NUM,
    X_LED_BD2_CELL_NUM,
    X_LED_BD3_CELL_NUM,
    X_LED_BD4_CELL_NUM,
};

const uint8_t X_StartLedIndexOfBd[X_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM + X_LED_BD3_CELL_NUM,
};

const uint8_t Y_LedNumOfBd[Y_LED_BD_NUM] = {
    Y_LED_BD1_CELL_NUM,
    Y_LED_BD2_CELL_NUM,
    Y_LED_BD3_CELL_NUM,
};

const uint8_t Y_StartLedIndexOfBd[Y_LED_BD_NUM] = {
    0,
    Y_LED_BD1_CELL_NUM,
    Y_LED_BD1_CELL_NUM + Y_LED_BD2_CELL_NUM,
};

const uint8_t StartLedIndexOfBd[TOTAL_LED_BD_NUM] = {
    0,
    X_LED_BD1_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM,
    X_LED_BD1_CELL_NUM + X_LED_BD2_CELL_NUM + X_LED_BD3_CELL_NUM,
    X_CELL_SIZE,
    X_CELL_SIZE + Y_LED_BD1_CELL_NUM,
    X_CELL_SIZE + Y_LED_BD1_CELL_NUM + Y_LED_BD2_CELL_NUM,
};

const uint8_t StartLedIndexOfGroupOffset[TOTAL_LED_BD_NUM] = {
    0,
    3,
    6,
    11,
    12,
    13,
    15
};


#else
#error "StartLedIndexOfBd is not defined!!!"
#endif

//-----------------------------------------------------------------------------
// For LED Light Intensity Control
//-----------------------------------------------------------------------------
typedef struct {
    uint8_t fullScan_On;
    uint8_t fullScan_Off;
    uint8_t partialScan_On;
    uint8_t partialScan_Off;
} ledOnOffTime_t;


//-----------------------------------------------------------------------------

/*****************************************************************************/
// Section: File Scope or Global Data
/*****************************************************************************/
scanStatus_t scanStatus;
SCAN_TASK_DATA scanTaskData;

partialScanData_t xPartialScanData, yPartialScanData;

uint8_t gCurrentLedBoardIndex, gCurrentLedGroupIndex;   // add gCurrentLedGroupIndex
int16_t gCurrentTotalLedIdx, gCurrentStartTotalPdIdx;

#if 1 // choh
uint8_t gCurrentLedGroupIndex;
#endif

uint32_t gScanProcessCnt; //nsmoon@210716
uint8_t fixedCurrentEnable;
uint8_t dltThresholdEnbale;
uint8_t gLedShiftOption;

#ifdef ENABLE_UART_CMD_PROCESS  //nsmoon@210915
uint8_t Current_Test_index_X, Current_Test_index_Y;
uint8_t Led_On_Time_Test_X, Led_On_Time_Test_Y;
//uint8_t fixedCurrentEnable;
#if ENABLE_TRIGER
uint8_t Current_Triger_Pd, Current_Triger_Axis, Usb_Off_Test;
#endif
#endif


#if ENABLE_TRACE_SCAN_AXIS //nsmoon@220113
uint8_t gPdSftClkCnt; //nsmoon@210708
uint8_t gLedSftClkCnt; //nsmoon@210719
#endif

uint8_t gLedShift1stTime;

#ifdef SELECT_LED_GROUP_TRACE
uint8_t gLedSftClkState;
#endif

volatile uint32_t coreTimer_timeout;
volatile uint32_t coreTimer_start_cnt;
volatile uint32_t coreTimer_diff;

#if 1 //nsmoon@210708
/* [pd Index in X][offset between ledindex and pd], last element is for dummy */
//ATTR_BACKEND_RAM3 uint8_t xScanResultData[X_CELL_SIZE + 1][X_TOTAL_OFFSET + 1 + 1];		//YJ@230425
uint8_t xScanResultData[X_CELL_SIZE + 1][X_TOTAL_OFFSET + 1 + 1];
/* [pd Index in Y][offset between ledindex and pd], last element is for dummy */
//ATTR_BACKEND_RAM3 uint8_t yScanResultData[Y_CELL_SIZE + 1][Y_TOTAL_OFFSET + 1 + 1];		//YJ@230425
uint8_t yScanResultData[Y_CELL_SIZE + 1][Y_TOTAL_OFFSET + 1 + 1];
#endif

// [pd Index in X][offset between ledindex and pd]
ATTR_BACKEND_RAM3 uint8_t xScanNonBlockLevel[X_CELL_SIZE][X_MAX_OFFSET * 2 + 1];

// [pd Index in Y][offset between ledindex and pd]
ATTR_BACKEND_RAM3 uint8_t yScanNonBlockLevel[Y_CELL_SIZE][Y_MAX_OFFSET * 2 + 1];

// [pd Index in X][offset between ledindex and pd]
ATTR_BACKEND_RAM3 uint8_t xScanThresholdData[X_CELL_SIZE][X_MAX_OFFSET * 2 + 1];

// [pd Index in Y][offset between ledindex and pd]
ATTR_BACKEND_RAM3 uint8_t yScanThresholdData[Y_CELL_SIZE][Y_MAX_OFFSET * 2 + 1];

// [pd Index in X][offset between ledindex and pd]
ATTR_BACKEND_RAM3 uint8_t xScanSubThresholdData[X_CELL_SIZE][X_MAX_OFFSET * 2 + 1];

// [pd Index in Y][offset between ledindex and pd]
ATTR_BACKEND_RAM3 uint8_t yScanSubThresholdData[Y_CELL_SIZE][Y_MAX_OFFSET * 2 + 1];



// pd dead cell, led dead cell, pd noise
ATTR_BACKEND_RAM3 cellStatus_t xCellStatus[X_CELL_SIZE]; // [cell Index in X]
ATTR_BACKEND_RAM3 cellStatus_t yCellStatus[Y_CELL_SIZE]; // [cell Index in X]

uint8_t xPdDeadCellCnt, xLedDeadCellCnt, yPdDeadCellCnt, yLedDeadCellCnt;
uint16_t xBlockedLineCnt, xSubBlockedLineCnt, yBlockedLineCnt, ySubBlockedLineCnt;
uint8_t subThresholdDropPercent;

#if 0 //FORCE_AGC_SET
uint8_t forced_agc;     //YJ@210719
uint8_t forced_saved;   //kang@211029
#endif

//------------------------------------------------------------------------------
// LED Light Intensity Control
ATTR_BACKEND_RAM2 uint8_t xAxisLedCurrentTblIdx[X_TOTAL_SCAN_STEPS];
ATTR_BACKEND_RAM2 uint8_t yAxisLedCurrentTblIdx[Y_TOTAL_SCAN_STEPS];
//uint8_t xAxisPrevLedCurrentTblIdx[X_TOTAL_SCAN_STEPS]; // can be deleted ??
//uint8_t yAxisPrevLedCurrentTblIdx[Y_TOTAL_SCAN_STEPS]; // can be deleted ??
#if AGC_TOGGLE_PREVENTION_ENABLE //nsmoon@220228
ATTR_BACKEND_RAM3 int8_t prevYLedCurrentCtrlDir[Y_TOTAL_SCAN_STEPS];
ATTR_BACKEND_RAM3 int8_t prevXLedCurrentCtrlDir[X_TOTAL_SCAN_STEPS];
#endif


//uint8_t xAxisLedOnTimeIdx[X_TOTAL_SCAN_STEPS];
//uint8_t yAxisLedOnTimeIdx[Y_TOTAL_SCAN_STEPS];
ATTR_BACKEND_RAM2 uint8_t xAxisDacIdx[X_TOTAL_SCAN_STEPS];
ATTR_BACKEND_RAM2 uint8_t yAxisDacIdx[Y_TOTAL_SCAN_STEPS];
//------------------------------------------------------------------------------
uint32_t noTouchObjectCnt;

uint8_t xAxisPdShiftDelayUsec;
uint8_t yAxisPdShiftDelayUsec;
uint8_t xAxisExtLightDelayUsec;
uint8_t yAxisExtLightDelayUsec;

uint16_t procExtLightNoiseCnt;
//------------------------------------------------------------------------------
uint32_t receivedCmdCnt;

uint8_t partialScanEntryCnt;
uint8_t partialScanExecCnt;
//------------------------------------------------------------------------------
uint8_t anOutAdcResult[PD_SIGNAL_OUT_NUM]; // kjs 210316 add
//------------------------------------------------------------------------------

uint32_t startProcessExtLightTimer, endProcessExtLightTimer;
uint32_t startAgcCtrlTimer, endAgcCtrlTimer;
uint32_t startDelayscanTimer, endDelayscanTimer;
uint32_t startPdDelayTimer, endPdDelayTimer;

#if 0
ATTR_BACKEND_RAM2  uint8_t (*xAxisSequenceTbl)[2];
ATTR_BACKEND_RAM2  uint8_t (*xPdIdxAtANOutTbl)[PD_SIGNAL_OUT_NUM];
ATTR_BACKEND_RAM2  uint8_t (*xUOffsetAtANOutTbl)[PD_SIGNAL_OUT_NUM];
ATTR_BACKEND_RAM2  uint8_t (*yAxisSequenceTbl)[2];
ATTR_BACKEND_RAM2  uint8_t (*yPdIdxAtANOutTbl)[PD_SIGNAL_OUT_NUM];
ATTR_BACKEND_RAM2  uint8_t (*yUOffsetAtANOutTbl)[PD_SIGNAL_OUT_NUM];
#else
ATTR_BACKEND_RAM2  const uint8_t (*xAxisSequenceTbl)[2];
ATTR_BACKEND_RAM2  const uint8_t (*xPdIdxAtANOutTbl)[PD_SIGNAL_OUT_NUM];
ATTR_BACKEND_RAM2  const uint8_t (*xUOffsetAtANOutTbl)[PD_SIGNAL_OUT_NUM];
ATTR_BACKEND_RAM2  const uint8_t (*yAxisSequenceTbl)[2];
ATTR_BACKEND_RAM2  const uint8_t (*yPdIdxAtANOutTbl)[PD_SIGNAL_OUT_NUM];
ATTR_BACKEND_RAM2  const uint8_t (*yUOffsetAtANOutTbl)[PD_SIGNAL_OUT_NUM];
#endif

#if 1
ATTR_BACKEND_RAM2 uint8_t xAxisSequenceTbl_ram[X_TOTAL_SCAN_STEPS][2];
ATTR_BACKEND_RAM2 uint8_t xPdIdxAtANOutTbl_ram[X_TOTAL_SCAN_STEPS][PD_SIGNAL_OUT_NUM];
ATTR_BACKEND_RAM2 uint8_t xUOffsetAtANOutTbl_ram[X_TOTAL_SCAN_STEPS][PD_SIGNAL_OUT_NUM];
ATTR_BACKEND_RAM2 uint8_t yAxisSequenceTbl_ram[Y_TOTAL_SCAN_STEPS][2];
ATTR_BACKEND_RAM2 uint8_t yPdIdxAtANOutTbl_ram[Y_TOTAL_SCAN_STEPS][PD_SIGNAL_OUT_NUM];
ATTR_BACKEND_RAM2 uint8_t yUOffsetAtANOutTbl_ram[Y_TOTAL_SCAN_STEPS][PD_SIGNAL_OUT_NUM];
#endif

void copySequenceTbl(void)
{
#if 1
#if 0
    memcpy(&xAxisSequenceTbl_ram[0][0], &xAxisSequenceTbl_flash[0][0], X_TOTAL_SCAN_STEPS*2);
    memcpy(&xPdIdxAtANOutTbl_ram[0][0], &xPdIdxAtANOutTbl_flash[0][0], X_TOTAL_SCAN_STEPS*PD_SIGNAL_OUT_NUM);
    memcpy(&xUOffsetAtANOutTbl_ram[0][0], &xUOffsetAtANOutTbl_flash[0][0], X_TOTAL_SCAN_STEPS*PD_SIGNAL_OUT_NUM);
    memcpy(&yAxisSequenceTbl_ram[0][0], &yAxisSequenceTbl_flash[0][0], Y_TOTAL_SCAN_STEPS*2);
    memcpy(&yPdIdxAtANOutTbl_ram[0][0], &yPdIdxAtANOutTbl_flash[0][0], Y_TOTAL_SCAN_STEPS*PD_SIGNAL_OUT_NUM);
    memcpy(&yUOffsetAtANOutTbl_ram[0][0], &yUOffsetAtANOutTbl_flash[0][0], Y_TOTAL_SCAN_STEPS*PD_SIGNAL_OUT_NUM);
#else
    int i, j, x;

    for (i = 0; i < X_TOTAL_SCAN_STEPS; i++) {
        for (j = 0; j < 2; j++) {
            xAxisSequenceTbl_ram[i][j] = xAxisSequenceTbl_flash[i][j];
        }
        for (x = 0; x < PD_SIGNAL_OUT_NUM; x++) {
            xPdIdxAtANOutTbl_ram[i][x] = xPdIdxAtANOutTbl_flash[i][x];
            xUOffsetAtANOutTbl_ram[i][x] = xUOffsetAtANOutTbl_flash[i][x];
        }
    }

    for (i = 0; i < Y_TOTAL_SCAN_STEPS; i++) {
        for (j = 0; j < 2; j++) {
            yAxisSequenceTbl_ram[i][j] = yAxisSequenceTbl_flash[i][j];
        }
        for (x = 0; x < PD_SIGNAL_OUT_NUM; x++) {
            yPdIdxAtANOutTbl_ram[i][x] = yPdIdxAtANOutTbl_flash[i][x];
            yUOffsetAtANOutTbl_ram[i][x] = yUOffsetAtANOutTbl_flash[i][x];
        }
    }
#endif
    xAxisSequenceTbl = xAxisSequenceTbl_ram;
    xPdIdxAtANOutTbl = xPdIdxAtANOutTbl_ram;
    xUOffsetAtANOutTbl = xUOffsetAtANOutTbl_ram;
    yAxisSequenceTbl = yAxisSequenceTbl_ram;
    yPdIdxAtANOutTbl = yPdIdxAtANOutTbl_ram;
    yUOffsetAtANOutTbl = yUOffsetAtANOutTbl_ram;
#else
    xAxisSequenceTbl = xAxisSequenceTbl_flash;
    xPdIdxAtANOutTbl = xPdIdxAtANOutTbl_flash;
    xUOffsetAtANOutTbl = xUOffsetAtANOutTbl_flash;
    yAxisSequenceTbl = yAxisSequenceTbl_flash;
    yPdIdxAtANOutTbl = yPdIdxAtANOutTbl_flash;
    yUOffsetAtANOutTbl = yUOffsetAtANOutTbl_flash;
#endif
#if 0 //for test
    //int i, j;
    for (i = 0; i < X_TOTAL_SCAN_STEPS; i++) {
        for (j = 0; j < 2; j++) {
            if (xAxisSequenceTbl_ram[i][j] != xAxisSequenceTbl_flash[i][j]) {
                TRACE_ERROR("\r\n1(%d/%d) %d %d", i, j, xAxisSequenceTbl_ram[i][j], xAxisSequenceTbl_flash[i][j]);
            }
            if (xAxisSequenceTbl[i][j] != xAxisSequenceTbl_flash[i][j]) {
                TRACE_ERROR("\r\n2(%d/%d) %d %d", i, j, xAxisSequenceTbl[i][j], xAxisSequenceTbl_flash[i][j]);
            }
        }
    }
    for (i = 0; i < Y_TOTAL_SCAN_STEPS; i++) {
        for (j = 0; j < 2; j++) {
            if (yAxisSequenceTbl_ram[i][j] != yAxisSequenceTbl_flash[i][j]) {
                TRACE_ERROR("\r\n3(%d/%d) %d %d", i, j, yAxisSequenceTbl_ram[i][j], yAxisSequenceTbl_flash[i][j]);
            }
            if (yAxisSequenceTbl[i][j] != yAxisSequenceTbl_flash[i][j]) {
                TRACE_ERROR("\r\n4(%d/%d) %d %d", i, j, yAxisSequenceTbl[i][j], yAxisSequenceTbl_flash[i][j]);
            }
        }
    }
    TRACE_ERROR("&&");
#endif
}





void initializeScanHW(void)
{
    uint8_t val = TOTAL_PD_SHIFT_MAX;
    // Initial value
    M_Disable_All_LED_Board();
    M_LED_HGROUP_SHIFT_DATA_Low();
    gCurrentLedBoardIndex = 0;
    gCurrentLedGroupIndex = 0;

    M_Disable_All_Pd(val);
    M_PD_SHIFT_DATA_Low();
    M_PD_SHIFT_CLK_Low();
    gCurrentStartTotalPdIdx = -1;
}


void Scan_Initialize(void)
{
    uint16_t i, j;

    initializeScanHW();

    /* clear x scan adc result buffer */
    for(i = 0; i < X_CELL_SIZE; i++)
    {
        for(j = 0; j < X_TOTAL_OFFSET; j++) {
            xScanResultData[i][j] = 0xFF;
        }
    }

    /* clear y scan adc result buffer */
    for(i = 0; i < Y_CELL_SIZE; i++)
    {
        for(j = 0; j < Y_TOTAL_OFFSET; j++) {
            yScanResultData[i][j] = 0xFF;
        }
    }

    //-------------------------------------------------------------------------
    // Init LED current table index & LED On time
    //-------------------------------------------------------------------------
    for(i = 0; i < X_TOTAL_SCAN_STEPS; i++)
    {
    	xAxisDacIdx[i] = (uint16_t)LED_ON_DAC_INIT_X;
    }
    for(i = 0; i < Y_TOTAL_SCAN_STEPS; i++)
    {
    	yAxisDacIdx[i] = (uint16_t)LED_ON_DAC_INIT_Y;
    }
    //-------------------------------------------------------------------------
    // clear delay for PD shift
    //-------------------------------------------------------------------------
    xAxisPdShiftDelayUsec = X_PD_SHIFT_DELAY_USEC_INIT;
    xAxisExtLightDelayUsec = X_EXT_LIGHT_DELAY_USEC_INIT;

    // clear y cell status
    yAxisPdShiftDelayUsec = Y_PD_SHIFT_DELAY_USEC_INIT;
    yAxisExtLightDelayUsec = Y_EXT_LIGHT_DELAY_USEC_INIT;

    //-------------------------------------------------------------------------
    // clear x cell status
    //-------------------------------------------------------------------------
    for(i = 0; i < X_CELL_SIZE; i++)
    {
        xCellStatus[i].isPdDead = FALSE;
        xCellStatus[i].isLedDead = FALSE;
        xCellStatus[i].isPdNoise = FALSE;
    }

    // clear y cell status
    for(i = 0; i < Y_CELL_SIZE; i++)
    {
        yCellStatus[i].isPdDead = FALSE;
        yCellStatus[i].isLedDead = FALSE;
        yCellStatus[i].isPdNoise = FALSE;
    }

    //-------------------------------------------------------------------------
    xPdDeadCellCnt = 0;
    xLedDeadCellCnt = 0;
    yPdDeadCellCnt = 0;
    yLedDeadCellCnt = 0;

    noTouchObjectCnt = 0;
    procExtLightNoiseCnt = 0;
    //-------------------------------------------------------------------------
    scanStatus.requestSubThreshold = TRUE;	//FALSE;

    //-------------------------------------------------------------------------
    scanStatus.isPartialScan = FALSE;
    partialScanEntryCnt = 0;
    partialScanExecCnt = 0;

    //-------------------------------------------------------------------------
#if USUAL_EXT_LIGHT_NOISE_CONTROL == TRUE
    setProcessExtLightTimerMsec(PROC_EXT_LIGHT_NOISE_ENTRY_TIME); // kjs 210310 move
    scanStatus.isUsualExtLightNoiseCal = TRUE;
#else
    scanStatus.isUsualExtLightNoiseCal = FALSE;
#endif

#if USUAL_LED_AGC_CONTROL == TRUE
    setAgcControlTimerMsec(LED_AGC_ENTRY_TIME);   //YJ@220915
    scanStatus.isUsualAgcCal = TRUE;
#else
    scanStatus.isUsualAgcCal = FALSE;
#endif

#if FORCE_AGC_SET
    g_forcedagc.forced_agc = 0;     //YJ@210719
    g_forcedagc.forced_saved = 0;   //kang@211029
    g_forcedagc.rmFrameCnt = 0;		//YJ@221219
#endif
    xSubBlockedLineCnt = 0;
    ySubBlockedLineCnt = 0;
    dltThresholdEnbale = 0;
    subThresholdDropPercent = LINE_THRESHOLD_VALUE;
}

inline __attribute__((always_inline)) void disableAllLEDBoard(void)
{
//    M_LED_CELL_Disable_High();
    M_LED_GROUP_Clear_Low();
    M_NOP_Delay_100nsec();
}

#if 0 //for test
inline __attribute__((always_inline)) void disableAllPD(void)
{
    uint8_t pulseCnt;

    pulseCnt = TOTAL_PD_SHIFT_MAX;
    M_PD_SHIFT_DATA_Low();

    while (pulseCnt--) {
        M_PD_SHIFT_CLK_OnePulse();
    }
    //gPdSftClkCnt = 0; //reset //nsmoon@210708
    M_Pd_Shift_Clk_Cnt_Set(0);
}
#endif

#if (SPM_MODE_ENABLE == FALSE)		//IO_MODE
// ledIndex : total index of X LED and Y LED, ordering : X LED B'd -> Y LED B'd
// no shift, 350nsec, 1 shift, 640nsec
inline __attribute__((always_inline))
void selectLED(uint16_t totalLedIndex, uint8_t axisType)
{
    //uint8_t ledIndexInBd;
    uint8_t boardIndex, tempboardIndex, groupCtrl, cellCtrl;
    //uint8_t boardIndexGap;
    uint16_t nextGroupTotal;
// find target Board Index
//--------------------------------------------------------------------------
    boardIndex = gCurrentLedBoardIndex;
#if 0
    /* first totalLedIndex = 105 */
    if ( totalLedIndex < StartLedIndexOfBd[boardIndex])
    {   // minus dir
        while ( totalLedIndex < StartLedIndexOfBd[boardIndex] )
        {
            boardIndex--;
        }
    } else { // plus dir
        while (boardIndex < LED_BD_INDEX_END)
        {
            if ( totalLedIndex < StartLedIndexOfBd[boardIndex + 1] ) break;
            boardIndex++;
        }
    }
    // find groupCtrl and cellCtrl
    //--------------------------------------------------------------------------
    // ledIndexInBd = totalLedIndex - StartLedIndexOfBd[boardIndex];
#if 1 //YJ@220816
    uint16_t ledIndex= (totalLedIndex + StartLedIndexOfGroupOffset[boardIndex]);
    groupCtrl = (ledIndex /6);
#endif
    cellCtrl = ledIndex % 6;
#else
    /* first totalLedIndex = 105 */
    if ( totalLedIndex < StartLedIndexOfBd[boardIndex])
    {   // minus dir
        while ( totalLedIndex < StartLedIndexOfBd[boardIndex] )
        {
            boardIndex--;
        }
    } else { // plus dir
        while (boardIndex < LED_BD_INDEX_END)
        {
            if ( totalLedIndex < StartLedIndexOfBd[boardIndex + 1] ) break;
            boardIndex++;
        }
    }
    uint16_t ledIndex= (totalLedIndex + StartLedIndexOfGroupOffset[boardIndex]);
    cellCtrl = ledIndex % 6;
    tempboardIndex = boardIndex;

    nextGroupTotal = totalLedIndex+1;
    boardIndex = gCurrentLedBoardIndex;
    if ( nextGroupTotal< StartLedIndexOfBd[boardIndex])
    {   // minus dir
        while ( nextGroupTotal < StartLedIndexOfBd[boardIndex] )
        {
            boardIndex--;
        }
    } else { // plus dir
        while (boardIndex < LED_BD_INDEX_END)
        {
            if ( nextGroupTotal < StartLedIndexOfBd[boardIndex + 1] ) break;
            boardIndex++;
        }
    }

    ledIndex= (nextGroupTotal + StartLedIndexOfGroupOffset[boardIndex]);
    groupCtrl = (ledIndex /6);
    gCurrentLedBoardIndex = tempboardIndex;
#endif

    // printf("\r\n **************************************************");
    //printf("\r\n [%s] groupCtrl: %d, cellCtrl: %d, totalLedIndex: %d ", __func__, groupCtrl, cellCtrl,totalLedIndex);
    //printf("\r\n **************************************************");

    // Physical setting
    //--------------------------------------------------------------------------
    //M_LED_HGROUP_SHIFT_DATA_Low();
//    if (groupCtrl > gCurrentLedGroupIndex) {
//
//      //gCurrentLedGroupIndex = groupCtrl;
//      timer_primary_output_config(TIMER7, ENABLE);
//    }
//    else
//    {
//      timer_primary_output_config(TIMER7, DISABLE);
//    }
    gCurrentLedGroupIndex = groupCtrl;
    //if(totalLedIndex == 30) cellCtrl = 0;
    M_LED_CELL_CTRL_SET((uint8_t)cellCtrl);
    //------------
    gCurrentTotalLedIdx = totalLedIndex;
    //   gCurrentLedBoardIndex = boardIndex;


}
#endif

inline __attribute__((always_inline))
void selectLED_PDshift(uint16_t totalLedIndex, uint8_t axisType)
{
    //uint8_t ledIndexInBd;
    uint8_t boardIndex, groupCtrl, cellCtrl;
    uint8_t boardIndexGap;
    // find target Board Index
    //--------------------------------------------------------------------------
    boardIndex = gCurrentLedBoardIndex;

    /* first totalLedIndex = 105 */
    if ( totalLedIndex < StartLedIndexOfBd[boardIndex])
    {   // minus dir
        while ( totalLedIndex < StartLedIndexOfBd[boardIndex] )
        {
            boardIndex--;
        }
    } else { // plus dir
        while (boardIndex < LED_BD_INDEX_END)
        {
            if ( totalLedIndex < StartLedIndexOfBd[boardIndex + 1] ) break;
            boardIndex++;
        }
    }
    // find groupCtrl and cellCtrl
    //--------------------------------------------------------------------------
    // ledIndexInBd = totalLedIndex - StartLedIndexOfBd[boardIndex];
#if 1 //YJ@220816
    uint16_t ledIndex= (totalLedIndex + StartLedIndexOfGroupOffset[boardIndex]);
    groupCtrl = (ledIndex /6);
#endif
    cellCtrl = ledIndex % 6;

    // printf("\r\n **************************************************");
//    printf("\r\n [%s] groupCtrl: %d, cellCtrl: %d, totalLedIndex: %d ", __func__, groupCtrl, cellCtrl,totalLedIndex);
    //printf("\r\n **************************************************");
//    DbgConsole_Printf("(%d)(%d)(%d)\r\n", groupCtrl,gCurrentLedGroupIndex,cellCtrl);
    // Physical setting

    //--------------------------------------------------------------------------
    if (groupCtrl != gCurrentLedGroupIndex|| gLedShift1stTime == TRUE) {
        if (groupCtrl < gCurrentLedGroupIndex|| gLedShift1stTime == TRUE) {

            M_First_LED_Board_Select();
            gCurrentLedGroupIndex = 0;
            M_TSPM_LED_Group_Clk_Set(groupCtrl);

        }
        else
        {
            boardIndexGap = groupCtrl - gCurrentLedGroupIndex;
            if(boardIndexGap == 1)
            {
                MCU_LED_GROUP_CLK_TSPM();
            }
            else
            {
                M_TSPM_LED_Group_Clk_Set(boardIndexGap);
            }
        }
        gCurrentLedGroupIndex = groupCtrl;
    }
    M_LED_CELL_CTRL_SET((uint8_t)cellCtrl);
    gCurrentTotalLedIdx = totalLedIndex;
    gCurrentLedBoardIndex = boardIndex;
}


uint16_t shiftClkCnt; //pdIdxGap
uint8_t pdGrpIdx, pdGrpIdxPrev;

inline __attribute__((always_inline))
void selectPDGroup(int16_t startTotalPdIdx)
{
    if (startTotalPdIdx == gCurrentStartTotalPdIdx) return;

    //calculate current pd group index
    if (gCurrentStartTotalPdIdx < Y_CELL_SIZE) {
        //y-axis
        pdGrpIdxPrev = gCurrentStartTotalPdIdx / CELLS_PER_CS;    // CELLS_PER_CS -> PD Group Num(NXP -> 10EA, GD -> 9 EA)
    } else {
        //x-axis, last PD group is not aligned!!
        pdGrpIdxPrev = (gCurrentStartTotalPdIdx + NUM_PD_NOT_ALIGNED) / CELLS_PER_CS; //nsmoon@210708
    }

    if (startTotalPdIdx < gCurrentStartTotalPdIdx) {      // PD Group Init
        pdGrpIdx = (X_CELL_SIZE + Y_CELL_SIZE + NUM_PD_NOT_ALIGNED) / CELLS_PER_CS; //nsmoon@210708
        shiftClkCnt = pdGrpIdx - pdGrpIdxPrev + 1;
        M_Disable_All_Pd(shiftClkCnt);
        gCurrentStartTotalPdIdx = 0; //nsmoon@210914

        if (startTotalPdIdx == -1) {
            // disable all PD CS
            gCurrentStartTotalPdIdx = -1;
            return;
        }
        M_First_PD_Gr_Select();
    }

    //calculate next pdGrpIdx
    if (startTotalPdIdx < Y_CELL_SIZE) {
        //y-axis
        pdGrpIdx = startTotalPdIdx / CELLS_PER_CS;
    } else {
        //x-axis, last PD group is not aligned!!
        pdGrpIdx = (startTotalPdIdx + NUM_PD_NOT_ALIGNED) / CELLS_PER_CS; //nsmoon@210708
    }
    shiftClkCnt = pdGrpIdx - pdGrpIdxPrev;
    //printf("\r\n\r\n [%s] -----------------------------------\r\n", __func__);
    //printf("\r\n\r\n [%s] shiftClkCnt: %d\r\n", __func__, shiftClkCnt);
    M_PD_SHIFT_CLK_GenPulse(shiftClkCnt); //nsmoon@210907
    gCurrentStartTotalPdIdx = startTotalPdIdx;
}


void selectPDGroupExt(int16_t startTotalPdIdx)
{
    selectPDGroup(startTotalPdIdx);
}

// global input : xScanResultData[][] or yScanResultData[][]
// 580nsec
//inline __attribute__((always_inline))
uint16_t
//__attribute__((optimize("O2")))
findStartScanSequenceIdx(axis_type_enum axisType, uint8_t pdIdxStart, uint8_t ledIdxMin, uint8_t ledIdxMax)
{
    uint16_t seqIdx = 0;
    uint8_t pdIdxStartInCsUnit;

    pdIdxStartInCsUnit = (pdIdxStart / CELLS_PER_CS);
    if (pdIdxStartInCsUnit > (ENABLED_CS_NUM - 1)) {
        pdIdxStartInCsUnit = (pdIdxStartInCsUnit - (ENABLED_CS_NUM - 1) ) * CELLS_PER_CS;
    } else {
        pdIdxStartInCsUnit = 0;
    }

    // PD_SIGNAL_OUT_NUM;

    if (axisType == X_AXIS) {
        if (pdIdxStart < X_CELL_SIZE) {
            for (seqIdx = 0; seqIdx < X_TOTAL_SCAN_STEPS; seqIdx++) {
                if (xAxisSequenceTbl[seqIdx][0] >= pdIdxStartInCsUnit) {
                    if ( xAxisSequenceTbl[seqIdx][1] >= ledIdxMin && xAxisSequenceTbl[seqIdx][1] <= ledIdxMax ) {
                        break;
                    }
                }
            }
        }
    } else { // Y-Axis
        if (pdIdxStart < Y_CELL_SIZE) {
            for (seqIdx = 0; seqIdx < Y_TOTAL_SCAN_STEPS; seqIdx++) {
                if (yAxisSequenceTbl[seqIdx][0] >= pdIdxStartInCsUnit) {
                    if ( yAxisSequenceTbl[seqIdx][1] >= ledIdxMin && yAxisSequenceTbl[seqIdx][1] <= ledIdxMax ) {
                        break;
                    }
                }
            }
        }
    }
#ifdef ENABLE_UART_CMD_PROCESS
    uint16_t seqIdxMax = (axisType == X_AXIS) ? X_TOTAL_SCAN_STEPS : Y_TOTAL_SCAN_STEPS;
    if (seqIdx >= seqIdxMax) {
        printf("ERROR! invalid seqIdx %d (%d %d %d/%d)\r\n", seqIdx, axisType, pdIdxStart, ledIdxMin, ledIdxMax);
        seqIdx = 0;
    }
#endif
    return seqIdx;
}


inline __attribute__((always_inline))
void xStoreAll_ADCResult(uint16_t seqIdx)
{
#if 1
    const uint8_t (*OffsetTbl)[PD_SIGNAL_OUT_NUM];
    const uint8_t (*PdTbl)[PD_SIGNAL_OUT_NUM];
    OffsetTbl = xUOffsetAtANOutTbl;
    PdTbl = xPdIdxAtANOutTbl;
    for (int i = 0; i < PD_SIGNAL_OUT_NUM; i++)
    {
        xScanResultData[PdTbl[seqIdx][i]][OffsetTbl[seqIdx][i]] = (uint8_t)(adc_value[i]);
    }
#else
    xScanResultData[xPdIdxAtANOutTbl[seqIdx][0]][xUOffsetAtANOutTbl[seqIdx][0]] = (uint8_t)(adc_value[0]);
    xScanResultData[xPdIdxAtANOutTbl[seqIdx][1]][xUOffsetAtANOutTbl[seqIdx][1]] = (uint8_t)(adc_value[1]);
    xScanResultData[xPdIdxAtANOutTbl[seqIdx][2]][xUOffsetAtANOutTbl[seqIdx][2]] = (uint8_t)(adc_value[2]);
    xScanResultData[xPdIdxAtANOutTbl[seqIdx][3]][xUOffsetAtANOutTbl[seqIdx][3]] = (uint8_t)(adc_value[3]);
    xScanResultData[xPdIdxAtANOutTbl[seqIdx][4]][xUOffsetAtANOutTbl[seqIdx][4]] = (uint8_t)(adc_value[4]);
    xScanResultData[xPdIdxAtANOutTbl[seqIdx][5]][xUOffsetAtANOutTbl[seqIdx][5]] = (uint8_t)(adc_value[5]);
    xScanResultData[xPdIdxAtANOutTbl[seqIdx][6]][xUOffsetAtANOutTbl[seqIdx][6]] = (uint8_t)(adc_value[6]);
    xScanResultData[xPdIdxAtANOutTbl[seqIdx][7]][xUOffsetAtANOutTbl[seqIdx][7]] = (uint8_t)(adc_value[7]);
    xScanResultData[xPdIdxAtANOutTbl[seqIdx][8]][xUOffsetAtANOutTbl[seqIdx][8]] = (uint8_t)(adc_value[8]);
    xScanResultData[xPdIdxAtANOutTbl[seqIdx][9]][xUOffsetAtANOutTbl[seqIdx][9]] = (uint8_t)(adc_value[9]);
#endif
}

inline  __attribute__((always_inline))
void yStoreAll_ADCResult(uint16_t seqIdx)
{
#if 1
    const uint8_t (*OffsetTbl)[PD_SIGNAL_OUT_NUM];
    const uint8_t (*PdTbl)[PD_SIGNAL_OUT_NUM];
    OffsetTbl = yUOffsetAtANOutTbl;
    PdTbl = yPdIdxAtANOutTbl;
    for (int i = 0; i < PD_SIGNAL_OUT_NUM; i++)
    {
        yScanResultData[PdTbl[seqIdx][i]][OffsetTbl[seqIdx][i]] = (uint8_t)(adc_value[i]);
    }
#else
    yScanResultData[yPdIdxAtANOutTbl[seqIdx][0]][yUOffsetAtANOutTbl[seqIdx][0]] = (uint8_t)(adc_value[0]);
    yScanResultData[yPdIdxAtANOutTbl[seqIdx][1]][yUOffsetAtANOutTbl[seqIdx][1]] = (uint8_t)(adc_value[1]);
    yScanResultData[yPdIdxAtANOutTbl[seqIdx][2]][yUOffsetAtANOutTbl[seqIdx][2]] = (uint8_t)(adc_value[2]);
    yScanResultData[yPdIdxAtANOutTbl[seqIdx][3]][yUOffsetAtANOutTbl[seqIdx][3]] = (uint8_t)(adc_value[3]);
    yScanResultData[yPdIdxAtANOutTbl[seqIdx][4]][yUOffsetAtANOutTbl[seqIdx][4]] = (uint8_t)(adc_value[4]);
    yScanResultData[yPdIdxAtANOutTbl[seqIdx][5]][yUOffsetAtANOutTbl[seqIdx][5]] = (uint8_t)(adc_value[5]);
    yScanResultData[yPdIdxAtANOutTbl[seqIdx][6]][yUOffsetAtANOutTbl[seqIdx][6]] = (uint8_t)(adc_value[6]);
    yScanResultData[yPdIdxAtANOutTbl[seqIdx][7]][yUOffsetAtANOutTbl[seqIdx][7]] = (uint8_t)(adc_value[7]);
    yScanResultData[yPdIdxAtANOutTbl[seqIdx][8]][yUOffsetAtANOutTbl[seqIdx][8]] = (uint8_t)(adc_value[8]);
    yScanResultData[yPdIdxAtANOutTbl[seqIdx][9]][yUOffsetAtANOutTbl[seqIdx][9]] = (uint8_t)(adc_value[9]);

#endif
}


void storeAnOutAdcResult(uint8_t adcGrNo)
{
    uint8_t anOutIdxBase;

    anOutIdxBase = adcGrNo*ADC_NUM;

    anOutAdcResult[anOutIdxBase + 0] = (uint8_t)(adc_value[0]>>4);
    anOutAdcResult[anOutIdxBase + 1] = (uint8_t)(adc_value[1]>>4);

    anOutAdcResult[anOutIdxBase + 2] = (uint8_t)(adc_value[2]>>4);
    anOutAdcResult[anOutIdxBase + 3] = (uint8_t)(adc_value[3]>>4);

    anOutAdcResult[anOutIdxBase + 4] = (uint8_t)(adc_value[4]>>4);
    anOutAdcResult[anOutIdxBase + 5] = (uint8_t)(adc_value[5]>>4);

    anOutAdcResult[anOutIdxBase + 6] = (uint8_t)(adc_value[6]>>4);
    anOutAdcResult[anOutIdxBase + 7] = (uint8_t)(adc_value[7]>>4);

    anOutAdcResult[anOutIdxBase + 8] = (uint8_t)(adc_value[8]>>4);
    anOutAdcResult[anOutIdxBase + 9] = (uint8_t)(adc_value[9]>>4);

}





#define LED_ON_TIME_FIX   100	//100	//10ns * 130 = 1.3us
#define LED_OFF_TIME_FIX  250	//180	//10ns * 130 = 1.3us
#define ADC_ENABLE_FULLSCAN
//#define DWT_ENABLE
#ifdef DWT_ENABLE
uint32_t diffCYC[10];
uint32_t prevCYC;
#endif

//#define LED_DAC_FIXED
#define LED_DAC_FIXED_VALUE		255

#if (SPM_MODE_ENABLE ==  TRUE)
//#define ADC_TEST_FUNC
#ifdef ADC_TEST_FUNC
ATTR_BACKEND_RAMFUNC
void adc_eoc_fuc(uint8_t mux, uint8_t startAdc)
{
	uint32_t cnt = 0;

	while(1)
	{
		//M_NOP_Delay_60nsec();
		if((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 1 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 1 )
		{

			if(++cnt > 1)
			{
				M_ADC_MUX_DATA_SET(mux);
				adc_value[startAdc++] = BOARD_ADC1_PERIPHERAL->R[0];
				adc_value[startAdc] = BOARD_ADC2_PERIPHERAL->R[0];

				break;
			}
		}
	}
}

ATTR_BACKEND_RAMFUNC
void ADC_Mux_conv(void)
{
	ADC_SamplesStart();

	adc_eoc_fuc(1,0);
	adc_eoc_fuc(2,2);
	adc_eoc_fuc(3,4);
	adc_eoc_fuc(4,6);
	adc_eoc_fuc(0,8);
	//M_ADC_MUX_DATA_SET(0);

	ADC_SamplesStop();
}
#endif





ATTR_BACKEND_RAMFUNC
int16_t scanAxisFull(axis_type_enum axisType, uint8_t bLedOn)

{
    //--------------------------------------------------------------------------
    //volatile uint16_t totalLedIdx, totalPdIdx;
    volatile uint8_t baseLedIdx, basePdIdx; //ledIdx, pdIdx
    volatile int16_t curScanSequenceIdx, nextScanSequenceIdx, prevScanSequenceIdx;
    //volatile uint8_t ledCurrentCtrl;
    uint8_t ledCurrentCtrl;
    volatile uint16_t ledOnTimeCtrl;
    //volatile uint8_t ledAmpGainCtrl, ledDacCtrl;
//    volatile uint8_t ledOnTime = 8, ledOffTime = 8;
    //uint8_t tmpLedOnTimeCtrl = 0;
    //volatile uint8_t pdStartANOutNo;
    //volatile uint8_t currentAdcGrIndex;
    uint8_t currentStartPdIdx;  // , currentLedIdx;
    uint8_t nextLedIdx, nextStartPdIdx;
    uint8_t pdShiftDelayUsec;
    // uint8_t PdShiftDelayFlag = 0;
    uint16_t firstSequence = 2; //10;//5; //TRUE; // kjs 210315 add
//  uint8_t  i = 0;

//    uint8_t adc_idx =0;
//   uint8_t AdcStoreTempBuf[9];
//  uint8_t testcnt =0;

    // LED_ON_TIME_SHORT //nsmoon@220330

    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    const uint8_t (*sequenceTbl)[2];
    int16_t totalScanSteps;
    uint8_t *ledCurrentTblIdx;
    //uint8_t *ledOnTimeIdx;
    uint8_t *ledDacIdx;
    // uint8_t ledCurrentFixed;    //YJ@220325

    if (axisType == X_AXIS) {
        DEBUG_TP2_LOW();
        gLedShiftOption = TRUE;
        sequenceTbl = xAxisSequenceTbl;
        totalScanSteps =X_TOTAL_SCAN_STEPS;
        ledCurrentTblIdx = xAxisLedCurrentTblIdx;
        ledDacIdx = xAxisDacIdx;

        pdShiftDelayUsec = xAxisPdShiftDelayUsec + xAxisExtLightDelayUsec;

        basePdIdx = Y_CELL_SIZE;
        baseLedIdx = 0;


    } else { // Y_AXIS
        DEBUG_TP2_HIGH();
        sequenceTbl = yAxisSequenceTbl;
        totalScanSteps = Y_TOTAL_SCAN_STEPS;
        ledCurrentTblIdx = yAxisLedCurrentTblIdx;
        ledDacIdx = yAxisDacIdx;

        pdShiftDelayUsec = yAxisPdShiftDelayUsec + yAxisExtLightDelayUsec;

        basePdIdx = 0;
        baseLedIdx = X_CELL_SIZE;
    }

#if (SCAN_INT_DISABLE_SHJ)          //YJ@220222
    //__builtin_disable_interrupts(); // kjsxy
    __disable_irq();
#endif


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // set 1st PD group in each Axis
    //--------------------------------------------------------------------------
    nextScanSequenceIdx = 0;
    nextStartPdIdx = sequenceTbl[nextScanSequenceIdx][0];
    currentStartPdIdx = nextStartPdIdx;

#ifdef ENABLE_UART_CMD_PROCESS
    if(fixedCurrentEnable)
    {
    	ledOnTimeCtrl = setValueDAC;
    	ledCurrentCtrl = LedSinkCurrentTbl[setADCSel].sinkCurrentControl;
    }
    else
    {
		ledOnTimeCtrl = ledDacIdx[nextScanSequenceIdx];
		ledCurrentCtrl = LedSinkCurrentTbl[ledCurrentTblIdx[nextScanSequenceIdx]].sinkCurrentControl;
    }
#else
    ledOnTimeCtrl = ledDacIdx[nextScanSequenceIdx];
    ledCurrentCtrl = LedSinkCurrentTbl[ledCurrentTblIdx[nextScanSequenceIdx]].sinkCurrentControl;
#endif

    M_PD_GAIN_CTRL_SET(ledCurrentCtrl);         //80ns
    M_DAC_DATA_SET(ledOnTimeCtrl);     // @hj check


    if (gCurrentStartTotalPdIdx < 0)
    {
        M_First_PD_Gr_Select();   //YJ@220929
        gCurrentStartTotalPdIdx = 0;
    }

    selectPDGroup((int16_t)(nextStartPdIdx + basePdIdx));

    //--------------------------------------------------------------------------
    // delay for first PD Shift
    //--------------------------------------------------------------------------

//   M_TSPM_Triger_Set();
    //M_Set_LedOff_Timer(LED_OFF_TIME_FIX);
    //--------------------------------------------------------------------------
    // select first LED
    //--------------------------------------------------------------------------
    nextLedIdx = sequenceTbl[nextScanSequenceIdx][1];
    //currentLedIdx = nextLedIdx;
#if 1 //must be checked
    //if (axisType == X_AXIS)
    {
        //M_LED_HGROUP_SHIFT_CLK_Low(); //YJ@220902a //YJ@220929
        //M_First_LED_Board_Select();   //YJ@220929
        gCurrentLedBoardIndex = 0;
        gCurrentLedGroupIndex = 0;
    }
    //else
#endif
    {
        gLedShift1stTime = TRUE;
        selectLED_PDshift((uint16_t)(nextLedIdx + baseLedIdx), axisType);
        gLedShift1stTime = FALSE;
    }

    //M_NOP_Delay_20usec();    //YJ@220929 -> Check
#if 0
    M_NOP_Delay_10usec();
#else
    CORETIMER_DelayUs(pdShiftDelayUsec);
//    M_NOP_Delay_1usec();
#endif

    prevScanSequenceIdx = curScanSequenceIdx = nextScanSequenceIdx; //YJ@220902

    //__disable_irq();
    //--------------------------------------------------------------------------
    // sequence zero's setting
    //--------------------------------------------------------------------------
    while (TRUE) {
        //__builtin_disable_interrupts(); // kjsxy

        __disable_irq();
#ifdef TIMER2_CH3_LAST
        while((BOARD_TMR2_PERIPHERAL->CHANNEL[BOARD_TMR2_CHANNEL_3_CHANNEL].CTRL & 0xe000) >0) {

        }
#else
//        //--------------------------------------------------------------------------
//        // 330ns
//        //--------------------------------------------------------------------------
//        while((BOARD_TMR3_PERIPHERAL->CHANNEL[BOARD_TMR3_CHANNEL_0_CHANNEL].CTRL & 0xe000) > 0) {
//
//        }
#endif

        //----------------------------------------------------------------------
        // Gain/Dac SelectLED_PDShift   == 380ns
        //----------------------------------------------------------------------
        //    M_Wait_LedOff_Timer_Expired();
        //    DEBUG_TP2_HIGH();
#ifdef ENABLE_UART_CMD_PROCESS
    if(fixedCurrentEnable)
    {
    	ledOnTimeCtrl = setValueDAC;
    	ledCurrentCtrl = LedSinkCurrentTbl[setADCSel].sinkCurrentControl;
    }
    else
    {
		ledOnTimeCtrl = ledDacIdx[nextScanSequenceIdx];
		ledCurrentCtrl = LedSinkCurrentTbl[ledCurrentTblIdx[nextScanSequenceIdx]].sinkCurrentControl;
    }
#else
    ledOnTimeCtrl = ledDacIdx[nextScanSequenceIdx];
    ledCurrentCtrl = LedSinkCurrentTbl[ledCurrentTblIdx[nextScanSequenceIdx]].sinkCurrentControl;
#endif
        selectLED_PDshift((uint16_t)(nextLedIdx + baseLedIdx), axisType);

        M_PD_GAIN_CTRL_SET(ledCurrentCtrl);         //80ns
        M_DAC_DATA_SET(ledOnTimeCtrl);     // @hj check
        //    DEBUG_TP2_LOW();
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        // variable update for the timing at sample & hold   == 45ns
        //----------------------------------------------------------------------
        prevScanSequenceIdx = curScanSequenceIdx;
        currentStartPdIdx = nextStartPdIdx;
        //currentLedIdx = nextLedIdx;
        curScanSequenceIdx = nextScanSequenceIdx;
        if (!firstSequence) {
          if (++nextScanSequenceIdx < totalScanSteps) {
              nextStartPdIdx = sequenceTbl[nextScanSequenceIdx][0];
              nextLedIdx = sequenceTbl[nextScanSequenceIdx][1];
          }
          M_TSPM_Triger_Set();
        }
        else {
          firstSequence--;
        }

        //----------------------------------------------------------------------
//    TRACSCAN("\r\n%d,%d,%d", currentStartPdIdx , nextLedIdx, prevScanSequenceIdx);
        // M_NOP_Delay_1usec();

//        M_Set_LedOff_Timer(LED_OFF_TIME_FIX);
//----------------------------------------------------------------------
// ADC Start/end Time   == 65" : 1.6us    75,85"  : 1.9us
//----------------------------------------------------------------------
//        DEBUG_TP2_HIGH();
#ifdef ADC_TEST_FUNC
        ADC_Mux_conv();
#else

        ADC_SamplesStart();

        DEBUG_TP1_LOW();
        DEBUG_TP1_HIGH();

        M_ADC_MUX_DATA_SET(1);
        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 0,1
        }
        adc_value[0] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[1] = BOARD_ADC2_PERIPHERAL->R[0];

        M_ADC_MUX_DATA_SET(2);
        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 2,3
        }
        adc_value[2] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[3] = BOARD_ADC2_PERIPHERAL->R[0];

        M_ADC_MUX_DATA_SET(3);
        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 4,5
        }
        adc_value[4] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[5] = BOARD_ADC2_PERIPHERAL->R[0];

        M_ADC_MUX_DATA_SET(4);
        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 6,7
        }
        adc_value[6] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[7] = BOARD_ADC2_PERIPHERAL->R[0];

#if (MODEL_TYPE == WMC_0750_NXP)||(MODEL_TYPE == WMC_0850_NXP)||(MODEL_TYPE == WMC_0850_NXP_VE)
            M_ADC_MUX_DATA_SET(5);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 8,9
            }

            adc_value[8] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[9] = BOARD_ADC2_PERIPHERAL->R[0];

            M_ADC_MUX_DATA_SET(0);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 8,9
            }

            adc_value[10] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[11] = BOARD_ADC2_PERIPHERAL->R[0];
#else
            M_ADC_MUX_DATA_SET(0);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 8,9
            }

            adc_value[8] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[9] = BOARD_ADC2_PERIPHERAL->R[0];
#endif

        ADC_SamplesStop();
#endif
//        DEBUG_TP2_LOW();
//----------------------------------------------------------------------
// ADC Result Save   == 100ns
//----------------------------------------------------------------------
        if (axisType == X_AXIS) {
            xStoreAll_ADCResult(prevScanSequenceIdx);
        }
        else {
            yStoreAll_ADCResult(prevScanSequenceIdx);
        }
//----------------------------------------------------------------------
        //--------------------------------------------------------------------------
		// 330ns
		//--------------------------------------------------------------------------
		while((BOARD_TMR3_PERIPHERAL->CHANNEL[BOARD_TMR3_CHANNEL_0_CHANNEL].CTRL & 0xe000) > 0) {

		}
//----------------------------------------------------------------------
//delay : 160ns
//----------------------------------------------------------------------
        if (nextScanSequenceIdx < totalScanSteps)     //YJ@220929 function
        {   // if not last scan sequence
            // generate PD shift clock if needed

            if (nextStartPdIdx != currentStartPdIdx)
            {
                uint8_t shiftClkCnt = (nextStartPdIdx - currentStartPdIdx) / CELLS_PER_CS; //nsmoon@210708
//                M_NOP_Delay_800nsec();		// 마지막 이상 신호 해결 Delay 원인 모름
                M_PD_SHIFT_CLK_GenPulse(shiftClkCnt);
                gCurrentStartTotalPdIdx = nextStartPdIdx + basePdIdx;
                __enable_irq();
				#if 0
					M_NOP_Delay_10usec();
				#else
					CORETIMER_DelayUs(pdShiftDelayUsec);
				#endif
				//__disable_irq();
            }

            //----------------------------------------------------------------------
            //----------------------------------------------------------------------
        }
        else
        {   // if last scan sequence

#ifdef TIMER2_CH3_LAST
            while((BOARD_TMR2_PERIPHERAL->CHANNEL[BOARD_TMR2_CHANNEL_3_CHANNEL].CTRL & 0xe000) >0) {

            }
#else
            while((BOARD_TMR3_PERIPHERAL->CHANNEL[BOARD_TMR3_CHANNEL_0_CHANNEL].CTRL & 0xe000) >0) {

            }
#endif
            //----------------------------------------------------------------------
            // ADC Start/end Time   == 1.6us
            //----------------------------------------------------------------------
            //    DEBUG_TP2_HIGH();
#ifdef ADC_TEST_FUNC
        ADC_Mux_conv();
#else
            ADC_SamplesStart();

            DEBUG_TP1_LOW();
            DEBUG_TP1_HIGH();

            M_ADC_MUX_DATA_SET(1);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 0,1
            }

            adc_value[0] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[1] = BOARD_ADC2_PERIPHERAL->R[0];
            //      M_NOP_Delay_40nsec();
            M_ADC_MUX_DATA_SET(2);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 2,3
            }

            adc_value[2] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[3] = BOARD_ADC2_PERIPHERAL->R[0];
            //      M_NOP_Delay_40nsec();
            M_ADC_MUX_DATA_SET(3);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 4,5
            }

            adc_value[4] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[5] = BOARD_ADC2_PERIPHERAL->R[0];

            M_ADC_MUX_DATA_SET(4);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 6,7
            }

            adc_value[6] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[7] = BOARD_ADC2_PERIPHERAL->R[0];

            //              M_NOP_Delay_40nsec();
#if (MODEL_TYPE == WMC_0750_NXP)||(MODEL_TYPE == WMC_0850_NXP)||(MODEL_TYPE == WMC_0850_NXP_VE)
            M_ADC_MUX_DATA_SET(5);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 8,9
            }

            adc_value[8] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[9] = BOARD_ADC2_PERIPHERAL->R[0];

            M_ADC_MUX_DATA_SET(0);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 8,9
            }

            adc_value[10] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[11] = BOARD_ADC2_PERIPHERAL->R[0];
#else
            M_ADC_MUX_DATA_SET(0);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 8,9
            }

            adc_value[8] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[9] = BOARD_ADC2_PERIPHERAL->R[0];
#endif


            ADC_SamplesStop();
#endif
            //        DEBUG_TP2_LOW();
            //----------------------------------------------------------------------
            // ADC Result Save   == 100ns
            //----------------------------------------------------------------------
            if (axisType == X_AXIS) {
                xStoreAll_ADCResult(curScanSequenceIdx);
            }
            else {
                yStoreAll_ADCResult(curScanSequenceIdx);
            }
            //----------------------------------------------------------------------

            //M_DebugTP2_Low();
            //----------------------------------------------------------------------
            break;  // test
        }
    } //while
    if (axisType == X_AXIS) {
        selectPDGroup(-1); // disable all PD CS
        //M_DebugTP2_Low();
    } else {
        selectPDGroup(Y_CELL_SIZE); // set PD CS to first X-Axis PD Gr
    }

    __enable_irq();

    return 0; //nsmoon@210720
}

#else	//IO_MODE
int16_t scanAxisFull(axis_type_enum axisType, uint8_t bLedOn)
{
    volatile uint8_t baseLedIdx, basePdIdx; //ledIdx, pdIdx
    volatile int16_t curScanSequenceIdx;
    volatile int16_t nextScanSequenceIdx;
    //volatile int16_t preScanSequenceIdx;
    volatile uint8_t ledCurrentCtrl;
    volatile uint16_t ledOnTimeCtrl;
//static uint8_t test;

    const uint8_t (*sequenceTbl)[2];

    int16_t totalScanSteps;
    uint8_t currentStartPdIdx;
    uint8_t nextLedIdx, nextStartPdIdx;
    uint8_t PdShiftDelayFlag = 0;
    uint8_t *ledCurrentTblIdx;
    uint8_t *ledDacIdx;
#ifdef DWT_ENABLE
    prevCYC = 0xff;
#endif
    // uint32_t regPrimask;
    uint8_t firstScan = 1;


//    setDelayScanTimer10Msec(1);
//        while (isDelayScanTimerExpired() == 0);
    //--------------------------------------------------------------------------
    // IRQ _DISABLE
    //--------------------------------------------------------------------------
    __disable_irq();
//     USB_DeviceIsrDisable();

    if (axisType == X_AXIS) {
        sequenceTbl = xAxisSequenceTbl;
        totalScanSteps =X_TOTAL_SCAN_STEPS;
        ledCurrentTblIdx = xAxisLedCurrentTblIdx;
        ledDacIdx = xAxisLedOnTimeIdx;            // Local variable = Global variable

        basePdIdx = Y_CELL_SIZE;
        baseLedIdx = 0;


    } else { // Y_AXIS
        sequenceTbl = yAxisSequenceTbl;
        totalScanSteps = Y_TOTAL_SCAN_STEPS;
        ledCurrentTblIdx = yAxisLedCurrentTblIdx;
        ledDacIdx = yAxisLedOnTimeIdx;

        basePdIdx = 0;
        baseLedIdx = X_CELL_SIZE;
    }
#if 1 //for test
    if (axisType == X_AXIS) {

        DEBUG_TP2_LOW();
    } else { // Y_AXIS
        DEBUG_TP2_HIGH();
    }
#endif

    //printf("\r\n **************************************************");
    //printf("\r\n [%s] 0", __func__);
    //printf("\r\n **************************************************");

// ------------------------------------------- Coupling Start High
    M_1stOpAmp_Coupling_SIG1_High(); //YJ@220817b
// -------------------------------------------

#if (ENABLE_UART_CMD_PROCESS && ENABLE_TRIGER)
    if (0 == Current_Triger_Pd && Current_Triger_Axis == axisType) {
        // M_DebugTP2_High();
    }
#endif

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // set 1st PD group in each Axis
    //--------------------------------------------------------------------------
    nextScanSequenceIdx = 0;
    nextStartPdIdx = sequenceTbl[nextScanSequenceIdx][0];
    currentStartPdIdx = nextStartPdIdx;

#ifdef LED_DAC_FIXED
    ledOnTimeCtrl = LED_DAC_FIXED_VALUE;
    ledCurrentCtrl =LedSinkCurrentTbl[ledCurrentTblIdx[nextScanSequenceIdx]].sinkCurrentControl;
#else
    ledOnTimeCtrl = ledDacIdx[nextScanSequenceIdx];
    ledCurrentCtrl = LedSinkCurrentTbl[ledCurrentTblIdx[nextScanSequenceIdx]].sinkCurrentControl;
#endif

    M_PD_GAIN_CTRL_SET(ledCurrentCtrl);         //80ns
    M_DAC_DATA_SET(ledOnTimeCtrl);     // @hj check


    if (gCurrentStartTotalPdIdx < 0)
    {
        M_First_PD_Gr_Select();
        gCurrentStartTotalPdIdx = 0;
    }

    selectPDGroup((int16_t)(nextStartPdIdx + basePdIdx));


    //--------------------------------------------------------------------------
    // delay for first PD Shift
    //--------------------------------------------------------------------------
    // if (pdShiftDelayUsec > 0)   //YJ@220817
    //core_timer_delay_us(pdShiftDelayUsec);  //YJ@220817
    // M_NOP_Delay_20usec();
    //Timer_Delay_us(20);
    //Timer_Delay_us(20);
    //Timer_Delay_us(20);
    M_NOP_Delay_20usec();
    //--------------------------------------------------------------------------
    // select first LED
    //--------------------------------------------------------------------------
    nextLedIdx = sequenceTbl[nextScanSequenceIdx][1];

    //must be checked
    M_LED_HGROUP_SHIFT_CLK_Low(); //YJ@220902a
    M_First_LED_Board_Select();
    gCurrentLedBoardIndex = 0;
    gCurrentLedGroupIndex = 0;
    selectLED((uint16_t)(nextLedIdx + baseLedIdx), axisType);



    M_1stOpAmp_Coupling_SIG1_Low(); //YJ@220817b                    // Coupling Low

#if ENABLE_TRACE_SCAN_AXIS //nsmoon@210906
    char axisChar = (axisType == X_AXIS) ? 'X' : 'Y';
    TRACE_UART("Start! scanAxisFull..(%c) %d %d\r\n", axisChar, M_Pd_Shift_Clk_Cnt, M_Pd_Shift_Clk_Cnt);
#endif

    //--------------------------------------------------------------------------

    //-----------------------------------------
    // reset sample and hold
    //M_2ndOpAmp_Input_On();
    //M_1stOpAmp_Input_Fixed_0d6V();
    //M_2ndOpAmp_Input_GND();
    //core_timer_delay_us(4);

    //M_2ndOpAmp_Input_Normal();
    //M_2ndOpAmp_Input_Off();

    //core_timer_delay_us(20);  //cki 210503 4->20
    //-----------------------------------------

    curScanSequenceIdx = nextScanSequenceIdx; //YJ@220902
    //M_PD_GAIN_CTRL_SET(0);
    //--------------------------------------------------------------------------
    // sequence zero's setting
    //--------------------------------------------------------------------------
#if 0//for test
    if (axisType == X_AXIS) {
        DEBUG_TP2_LOW();
    } else { // Y_AXIS
        DEBUG_TP2_HIGH();
    }
#endif

    while (TRUE) {
        //__builtin_disable_interrupts(); // kjsxy

        __disable_irq();
        //USB_DeviceIsrDisable();
        //regPrimask = DisableGlobalIRQ();

        if(PdShiftDelayFlag == TRUE) M_1stOpAmp_Coupling_SIG1_Low(); //YJ@220817b  org

        //----------------------------------------------------------------------
        // LED - ON
        //----------------------------------------------------------------------

        M_Set_LedOn_Timer(LED_ON_TIME_FIX); //nsmoon@210929
#if 0
        if (axisType == X_AXIS)
        {
            if(nextScanSequenceIdx == 5)
            {
                DEBUG_TP2_HIGH();
                //M_LED_CELL_Disable_High();
            }
        }
#endif
        if (bLedOn == TRUE) {
            if (firstScan) {
                M_LED_CELL_Disable_High();
            }
            else {
                M_LED_CELL_Enable_Low();
            }
        } else {
            M_LED_CELL_Disable_High();
        }

#if 0 //def ADC_ENABLE_FULLSCAN
        ADC_SamplesStart();
        M_NOP_Delay_60nsec();     // use adc start delay
        //M_NOP_Delay_100nsec();

//      M_NOP_Delay_40nsec();
        M_ADC_MUX_DATA_SET(4);

        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 6,7
        }

        adc_value[6] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[7] = BOARD_ADC2_PERIPHERAL->R[0];

//              M_NOP_Delay_40nsec();
        M_ADC_MUX_DATA_SET(0);
        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 8,9
        }

        adc_value[8] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[9] = BOARD_ADC2_PERIPHERAL->R[0];
        if (axisType == X_AXIS) {
            xStoreAll_ADCResult(curScanSequenceIdx);

        }
        else {
            yStoreAll_ADCResult(curScanSequenceIdx);
        }

        ADC_SamplesStop();

#else
        // M_NOP_Delay_300nsec();
#if 0
        if (axisType == X_AXIS) {
            xStoreAll_ADCResult(curScanSequenceIdx);

        }
        else {
            yStoreAll_ADCResult(curScanSequenceIdx);
        }
#endif
#endif

//    M_NOP_Delay_800nsec();

        M_LED_HGROUP_SHIFT_CLK_Low();


        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        M_Wait_LedOn_Timer_Expired(); // check and wait LED-On duty was expired


        M_2ndOpAmp_SW_EN1_On();
        M_NOP_Delay_100nsec();

        //----------------------------------------------------------------------
        // variable update for the timing at sample & hold   == 170ns
        //----------------------------------------------------------------------
        currentStartPdIdx = nextStartPdIdx;
        // preScanSequenceIdx = curScanSequenceIdx;
        curScanSequenceIdx = nextScanSequenceIdx;
        if (!firstScan) {
            if (++nextScanSequenceIdx < totalScanSteps) {
                nextStartPdIdx = sequenceTbl[nextScanSequenceIdx][0];
                nextLedIdx = sequenceTbl[nextScanSequenceIdx][1];
            }
        }
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        // LED - OFF
        //----------------------------------------------------------------------
        M_2ndOpAmp_SW_EN1_Off();// sample & hold state

        M_LED_CELL_Disable_High();
        //__enable_irq();
        //USB_DeviceIsrEnable();
        //EnableGlobalIRQ(regPrimask);

        // TRACE_YJ(" %d, %d,  %d", currentStartPdIdx , nextLedIdx, preScanSequenceIdx);
        M_Set_LedOff_Timer(LED_OFF_TIME_FIX);
#ifdef ADC_ENABLE_FULLSCAN
        ADC_SamplesStart();

        M_1stOpAmp_Coupling_SIG1_High();
        //M_NOP_Delay_40nsec();

        M_ADC_MUX_DATA_SET(1);
        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 0,1
        }

        adc_value[0] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[1] = BOARD_ADC2_PERIPHERAL->R[0];
//      M_NOP_Delay_40nsec();
        M_ADC_MUX_DATA_SET(2);
        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 2,3
        }

        adc_value[2] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[3] = BOARD_ADC2_PERIPHERAL->R[0];
        //      M_NOP_Delay_40nsec();
        M_ADC_MUX_DATA_SET(3);
        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 4,5
        }

        adc_value[4] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[5] = BOARD_ADC2_PERIPHERAL->R[0];
#if 1
        M_ADC_MUX_DATA_SET(4);
        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 6,7
        }

        adc_value[6] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[7] = BOARD_ADC2_PERIPHERAL->R[0];

        //              M_NOP_Delay_40nsec();
        M_ADC_MUX_DATA_SET(0);
        while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
            ;   // 8,9
        }

        adc_value[8] = BOARD_ADC1_PERIPHERAL->R[0];
        adc_value[9] = BOARD_ADC2_PERIPHERAL->R[0];
#endif


        ADC_SamplesStop();

#if 1
        if (firstScan) {
            firstScan--;
        }
        else
        {
            if (axisType == X_AXIS) {
                xStoreAll_ADCResult(curScanSequenceIdx);
            }
            else {
                yStoreAll_ADCResult(curScanSequenceIdx);
            }
        }
#else
        seqIdx =curScanSequenceIdx;
        if (axisType == X_AXIS) {
            //xScanResultData[xPdIdxAtANOutTbl[seqIdx][0]][xUOffsetAtANOutTbl[seqIdx][0]] = (uint8_t)(adc_value[0]);
            // xScanResultData[xPdIdxAtANOutTbl[seqIdx][1]][xUOffsetAtANOutTbl[seqIdx][1]] = (uint8_t)(adc_value[1]);
            //xScanResultData[xPdIdxAtANOutTbl[seqIdx][2]][xUOffsetAtANOutTbl[seqIdx][2]] = (uint8_t)(adc_value[2]);
            // xScanResultData[xPdIdxAtANOutTbl[seqIdx][3]][xUOffsetAtANOutTbl[seqIdx][3]] = (uint8_t)(adc_value[3]);
            seqIdx +=curScanSequenceIdx;
        }
        else
        {
            //yScanResultData[yPdIdxAtANOutTbl[seqIdx][0]][yUOffsetAtANOutTbl[seqIdx][0]] = (uint8_t)(adc_value[0]);
            //yScanResultData[yPdIdxAtANOutTbl[seqIdx][1]][yUOffsetAtANOutTbl[seqIdx][1]] = (uint8_t)(adc_value[1]);
            //yScanResultData[yPdIdxAtANOutTbl[seqIdx][2]][yUOffsetAtANOutTbl[seqIdx][2]] = (uint8_t)(adc_value[2]);
            //yScanResultData[yPdIdxAtANOutTbl[seqIdx][3]][yUOffsetAtANOutTbl[seqIdx][3]] = (uint8_t)(adc_value[3]);
            seqIdx +=curScanSequenceIdx;
        }

#endif
#else
        M_1stOpAmp_Coupling_SIG1_High();
        M_NOP_Delay_500nsec();
#endif
        //----------------------------------------------------------------------
        //delay : 160ns
        //----------------------------------------------------------------------
        //  ADC_CTL1(ADC0) |= (uint32_t)ADC_CTL1_SWRCST;    //60ns

        if (nextScanSequenceIdx < totalScanSteps) { // if not last scan sequence
            // generate PD shift clock if needed
            if (nextStartPdIdx != currentStartPdIdx) {
                PdShiftDelayFlag = TRUE;    //YJ@220920
                M_Set_LedOff_Timer(LED_ON_TIME_FIX + 1700);    //PD Shift time 18us offset
                //M_DebugTP2_High();
                uint8_t shiftClkCnt = (nextStartPdIdx - currentStartPdIdx) / CELLS_PER_CS; //nsmoon@210708
                M_1stOpAmp_Coupling_SIG1_High(); //YJ@220920a
                M_PD_SHIFT_CLK_GenPulse(shiftClkCnt);
                gCurrentStartTotalPdIdx = nextStartPdIdx + basePdIdx;   //Yj Check
                //__enable_irq();
                //Timer_Delay_us(20);
                //EnableGlobalIRQ(regPrimask);
                M_NOP_Delay_20usec();
            } else {
                PdShiftDelayFlag = FALSE;
                // M_Set_LedOff_Timer(LED_ON_TIME_FIX);
#ifdef DWT_ENABLE
                prevCYC = DWT->CYCCNT;
#endif
            }
            //----------------------------------------------------------------------
            //----------------------------------------------------------------------
//        M_1stOpAmp_Coupling_SIG1_High(); //YJ@220817b

            M_NOP_Delay_100nsec();


#ifdef LED_DAC_FIXED
            ledOnTimeCtrl = LED_DAC_FIXED_VALUE; ////ledDacIdx[nextScanSequenceIdx];
            ledCurrentCtrl = LedSinkCurrentTbl[ledCurrentTblIdx[nextScanSequenceIdx]].sinkCurrentControl;
#else
            ledOnTimeCtrl = ledDacIdx[nextScanSequenceIdx];
            ledCurrentCtrl = LedSinkCurrentTbl[ledCurrentTblIdx[nextScanSequenceIdx]].sinkCurrentControl;
#endif
            //TRACEBE("%d", ledOnTimeCtrl);
            M_PD_GAIN_CTRL_SET(ledCurrentCtrl);         //80ns
            M_DAC_DATA_SET(ledOnTimeCtrl);     // @hj check

            if(PdShiftDelayFlag == FALSE) M_1stOpAmp_Coupling_SIG1_Low(); //YJ@220817b  org

            selectLED(nextLedIdx + baseLedIdx, axisType); // take min 350nsec

            M_Wait_LedOff_Timer_Expired(); // check and wait LED-Off duty was expired //nsmoon@210916
#if 0 //for test
            if(nextScanSequenceIdx == 0)
            {
                test_diff = coreTimer_diff;
                test_Timeout = coreTimer_timeout;
            }
#endif
#ifdef DWT_ENABLE
            if(nextLedIdx < 6)
            {
                diffCYC[nextLedIdx] = DWT->CYCCNT - prevCYC;
            }
#endif
        } else {
            M_Set_LedOff_Timer(LED_OFF_TIME_FIX);
            ////////////////////////////////////////////////////////////////////////////////////////////////// 600ns
#if 0 //def ADC_ENABLE_FULLSCAN
            ADC_SamplesStart();
            M_NOP_Delay_60nsec();     // use adc start delay

            //      M_NOP_Delay_40nsec();
            M_ADC_MUX_DATA_SET(4);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 6,7
            }

            adc_value[6] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[7] = BOARD_ADC2_PERIPHERAL->R[0];

            //              M_NOP_Delay_40nsec();
            M_ADC_MUX_DATA_SET(0);
            while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                ;   // 8,9
            }

            adc_value[8] = BOARD_ADC1_PERIPHERAL->R[0];
            adc_value[9] = BOARD_ADC2_PERIPHERAL->R[0];

            ADC_SamplesStop();
#else
            M_NOP_Delay_500nsec();
#endif
            //----------------------------------------------------------------------
            // Store ADC value   == 200ns
            //----------------------------------------------------------------------
#if 0
            if (axisType == X_AXIS) {
                xStoreAll_ADCResult(curScanSequenceIdx);
            }
            else {
                yStoreAll_ADCResult(curScanSequenceIdx);
            }
#endif
            //////////////////////////////////////////////////////////////////////////////////////////////////
            M_Wait_LedOff_Timer_Expired(); // check and wait LED-Off duty was expired //nsmoon@210916

            break;  // test
        }

    }

    TRACE_FULL("\r\n%d	%d	%d", test_diff, test_Timeout, seqIdx);
    if (axisType == X_AXIS) {
        selectPDGroup(-1); // disable all PD CS
        /* for (int xx =0; xx < 9; xx++)
        {
        	TRACE_YJ("%d", xScanResultData[xPdIdxAtANOutTbl[0][xx]][xUOffsetAtANOutTbl[0][xx]]);
        }
         DbgConsole_Printf("\r\n");*/
    } else {

        selectPDGroup(Y_CELL_SIZE); // set PD CS to first X-Axis PD Gr
    }
    //USB_DeviceIsrEnable();


    __enable_irq();
    // EnableGlobalIRQ(regPrimask);

    return 0; //nsmoon@210720
}
#endif
#endif

int16_t averageNonBlockLevel(uint8_t cnt)
{
    int16_t pdIdx, totalCellSize;
    int16_t offset, maxOffset, ledIdx;
    int16_t lowLevelCnt;

    lowLevelCnt = 0;

    if (cnt == 0) return lowLevelCnt;

    //--------------------------------------------------------------------------
    // X-scan
    //--------------------------------------------------------------------------
    totalCellSize = X_CELL_SIZE;
    maxOffset = X_MAX_OFFSET;

    for (pdIdx = 0; pdIdx < totalCellSize; pdIdx++) {
        if (xCellStatus[pdIdx].isPdDead) continue;

        for (offset = - maxOffset; offset <= maxOffset; offset++)
        {
            ledIdx = offset + pdIdx;

            if ( ledIdx >= 0 && ledIdx < totalCellSize ) { // check if ledIdx is in the range
                if (xCellStatus[ledIdx].isLedDead) continue;

                xScanNonBlockLevel[pdIdx][offset + maxOffset] =
                    (xScanNonBlockLevel[pdIdx][offset + maxOffset] * (cnt - 1) + xScanResultData[pdIdx][offset + maxOffset]) / cnt;
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 1) //nsmoon@211213
                print_scan_threshold("X((", X_AXIS, ledIdx, pdIdx);
#endif
                if (xScanNonBlockLevel[pdIdx][offset + maxOffset] < MIN_THRESHOLD_LEVEL) {
                    lowLevelCnt++;
                    TRACE_AGC("X Low : pdIdx = %d, ledIdx = %d, level = %d\r\n",
                              pdIdx, ledIdx, xScanNonBlockLevel[pdIdx][offset + maxOffset]);
                }
            }
        }
    }

    //--------------------------------------------------------------------------
    // Y-scan
    //--------------------------------------------------------------------------
    totalCellSize = Y_CELL_SIZE;
    maxOffset = Y_MAX_OFFSET;

    for (pdIdx = 0; pdIdx < totalCellSize; pdIdx++) {
        if (yCellStatus[pdIdx].isPdDead) continue;

        for (offset = - maxOffset; offset <= maxOffset; offset++)
        {
            ledIdx = offset + pdIdx;

            if ( ledIdx >= 0 && ledIdx < totalCellSize ) { // check if ledIdx is in the range
                if (yCellStatus[ledIdx].isLedDead) continue;

                yScanNonBlockLevel[pdIdx][offset + maxOffset] =
                    (yScanNonBlockLevel[pdIdx][offset + maxOffset] * (cnt - 1) + yScanResultData[pdIdx][offset + maxOffset]) / cnt;
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 1) //nsmoon@211213
                print_scan_threshold("Y((", Y_AXIS, ledIdx, pdIdx);
#endif

                if (yScanNonBlockLevel[pdIdx][offset + maxOffset] < MIN_THRESHOLD_LEVEL) {
                    lowLevelCnt++;
                    TRACE_AGC("Y Low : pdIdx = %d, ledIdx = %d, level = %d\r\n",
                              pdIdx, ledIdx, yScanNonBlockLevel[pdIdx][offset + maxOffset]);
                }
            }
        }
    }

    return lowLevelCnt;
}

uint32_t Scan_Data_Process(void)
{
    uint8_t maxOffset, offsetByteSize; //i
    int16_t pdIdx, ledIdx, offset;
    uint8_t pdIdxMin, pdIdxMax, pdIdxStart, pdIdxEnd;
    uint8_t ledIdxMin, ledIdxMax, ledIdxStart, ledIdxEnd;
    uint8_t xPdIdxMin, xPdIdxMax, xLedIdxMin, xLedIdxMax;
    uint8_t yPdIdxMin, yPdIdxMax, yLedIdxMin, yLedIdxMax;
    uint8_t xPdScanWidth, xPartialEntryWidth;
    uint8_t byteIdx, bitIdx;
    uint16_t startIdxMain, startIdxSub, bufIdx;
    uint16_t totalBlockedLineCnt;

    gScanProcessCnt++; //nsmoon@210716

    //--------------------------------------------------------------------------
    Send_ExbBit_Data_to_USBCPU_Init();  //buffer clear

    //--------------------------------------------------------------------------
    // X-scan result
    //--------------------------------------------------------------------------
    if (scanStatus.isPartialScan) {
        pdIdxStart = xPartialScanData.pdIdxStart;
        pdIdxEnd = xPartialScanData.pdIdxEnd;
        ledIdxStart = xPartialScanData.ledIdxStart;
        ledIdxEnd = xPartialScanData.ledIdxEnd;
    } else {
        pdIdxStart = 0;
        pdIdxEnd = X_CELL_SIZE - 1;
        ledIdxStart = 0;
        ledIdxEnd = X_CELL_SIZE - 1;
    }
    pdIdxMin = pdIdxEnd;
    pdIdxMax = pdIdxStart;
    ledIdxMin = ledIdxEnd;
    ledIdxMax = ledIdxStart;

    maxOffset = X_MAX_OFFSET;
    offsetByteSize = EXB_X_OFFSET_BYTE_SIZE;
    startIdxMain = EXB_X_DATA_START;
    startIdxSub = EXB_X_SUB_DATA_START;
    xBlockedLineCnt = 0;
    xSubBlockedLineCnt = 0;

    for (pdIdx = pdIdxStart; pdIdx <= pdIdxEnd; pdIdx++)
    {
#if (SIDE_DATA_FIXED == TRUE)   //YJ@210719
        if(pdIdx == 0 || pdIdx ==X_CELL_SIZE - 1)
        {
            continue;
        }
#endif
        for (offset = - maxOffset; offset <= maxOffset; offset++)
        {
#if 1 //nsmoon@210716
            ledIdx = offset + pdIdx;
            if ( ledIdx < 0 && ledIdx >= X_CELL_SIZE ) {
                continue;
            }
#endif
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 1) //nsmoon@211213
            if (showCnt_test > 0 && scan_axis_test == X_AXIS) {
                print_scan_threshold("X{{", X_AXIS, ledIdx, pdIdx);
                showCnt_test--;
            }
#endif
            //if ( ledIdx >= ledIdxStart && ledIdx <= ledIdxEnd )
            {   // check if ledIdx is in the range
                if (xScanResultData[pdIdx][offset + maxOffset] < xScanThresholdData[pdIdx][offset + maxOffset]) {
                    //ledIdx = offset + pdIdx; //nsmoon@210716 move to above
                    byteIdx = (offset + maxOffset) / 8;
                    bitIdx = (offset + maxOffset) % 8;

                    bufIdx = startIdxMain + pdIdx * offsetByteSize + byteIdx;
                    Exb_Send_Buf[bufIdx] |= (1 << bitIdx);
                    //g_usb_in_buff[bufIdx] |= (1 << bitIdx);
                    xBlockedLineCnt++;

                    M_Update_MinMax(pdIdxMin, pdIdxMax, pdIdx);
                    M_Update_MinMax(ledIdxMin, ledIdxMax, ledIdx);

                    // sub-threshold process
                    if (scanStatus.requestSubThreshold == TRUE) {
                        if (xScanResultData[pdIdx][offset + maxOffset] < xScanSubThresholdData[pdIdx][offset + maxOffset]) {
                            bufIdx = startIdxSub + pdIdx * offsetByteSize + byteIdx;
                            Exb_Send_Buf[bufIdx] |= (1 << bitIdx);
                            //g_usb_in_buff[bufIdx] |= (1 << bitIdx);
                        }
                    }
                }

                if (xScanResultData[pdIdx][offset + maxOffset] < EXTL_LOW_SIG) {
                    xSubBlockedLineCnt++;
                }
            }
        }
    }
    xPdIdxMin = pdIdxMin;
    xPdIdxMax = pdIdxMax;
    xLedIdxMin = ledIdxMin;
    xLedIdxMax = ledIdxMax;
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Y-scan result
    //--------------------------------------------------------------------------
    if (scanStatus.isPartialScan) {
        pdIdxStart = yPartialScanData.pdIdxStart;
        pdIdxEnd = yPartialScanData.pdIdxEnd;
        ledIdxStart = yPartialScanData.ledIdxStart;
        ledIdxEnd = yPartialScanData.ledIdxEnd;
    } else {
        pdIdxStart = 0;
        pdIdxEnd = Y_CELL_SIZE - 1;
        ledIdxStart = 0;
        ledIdxEnd = Y_CELL_SIZE - 1;
    }
    pdIdxMin = pdIdxEnd;
    pdIdxMax = pdIdxStart;
    ledIdxMin = ledIdxEnd;
    ledIdxMax = ledIdxStart;

    maxOffset = Y_MAX_OFFSET;
    offsetByteSize = EXB_Y_OFFSET_BYTE_SIZE;
    startIdxMain = EXB_Y_DATA_START;
    startIdxSub = EXB_Y_SUB_DATA_START;
    yBlockedLineCnt = 0;
    ySubBlockedLineCnt = 0;

    for (pdIdx = pdIdxStart; pdIdx <= pdIdxEnd; pdIdx++)
    {
#if (SIDE_DATA_FIXED == TRUE)   //YJ@210719
        if (pdIdx == 0 || pdIdx == Y_CELL_SIZE - 1)
        {
            continue;
        }
#endif
        for (offset = - maxOffset; offset <= maxOffset; offset++)
        {
#if 1 //nsmoon@210716
            ledIdx = offset + pdIdx;
            if ( ledIdx < 0 && ledIdx >= Y_CELL_SIZE ) {
                continue;
            }
#endif
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 1) //nsmoon@211213
            if (showCnt_test > 0 && scan_axis_test == Y_AXIS) {
                print_scan_threshold("Y{{", Y_AXIS, ledIdx, pdIdx);
                showCnt_test--;
            }
#endif
            //if ( ledIdx >= ledIdxStart && ledIdx <= ledIdxEnd ) {
            {   // check if ledIdx is in the range
                if (yScanResultData[pdIdx][offset + maxOffset] < yScanThresholdData[pdIdx][offset + maxOffset]) { // ** x or y
                    //ledIdx = offset + pdIdx; //nsmoon@210716 move to above
                    byteIdx = (offset + maxOffset) / 8;
                    bitIdx = (offset + maxOffset) % 8;

                    bufIdx = startIdxMain + pdIdx * offsetByteSize + byteIdx;
                    Exb_Send_Buf[bufIdx] |= (1 << bitIdx);
                    // g_usb_in_buff[bufIdx] |= (1 << bitIdx);
                    yBlockedLineCnt++; // ** x or y

                    M_Update_MinMax(pdIdxMin, pdIdxMax, pdIdx);
                    M_Update_MinMax(ledIdxMin, ledIdxMax, ledIdx);

                    // sub-threshold process
                    if (scanStatus.requestSubThreshold == TRUE) {
                        if (yScanResultData[pdIdx][offset + maxOffset] < yScanSubThresholdData[pdIdx][offset + maxOffset]) { // ** x or y
                            bufIdx = startIdxSub + pdIdx * offsetByteSize + byteIdx;
                            Exb_Send_Buf[bufIdx] |= (1 << bitIdx);
                            //g_usb_in_buff[bufIdx] |= (1 << bitIdx);
                        }
                    }
                }

                if (yScanResultData[pdIdx][offset + maxOffset] < EXTL_LOW_SIG) { // ** x or y
                    ySubBlockedLineCnt++; // ** x or y
                }
            }
        }
    }
    yPdIdxMin = pdIdxMin;
    yPdIdxMax = pdIdxMax;
    yLedIdxMin = ledIdxMin;
    yLedIdxMax = ledIdxMax;
    //--------------------------------------------------------------------------

    //==========================================================================
    // Partial scan setting
    //--------------------------------------------------------------------------
    if (xBlockedLineCnt == 0) {
        xPdIdxMin = 0;
        xPdIdxMax = (X_CELL_SIZE-1);
        xLedIdxMin = 0;
        xLedIdxMax = (X_CELL_SIZE-1);
    }
    if (yBlockedLineCnt == 0) {
        yPdIdxMin = 0;
        yPdIdxMax = (Y_CELL_SIZE-1);
        yLedIdxMin = 0;
        yLedIdxMax = (Y_CELL_SIZE-1);
    }

    //--------------------------------------------------------------------------
    // add marginal area
    //--------------------------------------------------------------------------
    // X-Axis
    //--------------------------------------------------------------------------
    if(xPdIdxMin > PARTIAL_SEARCH_AREA_ADD_WIDTH) xPdIdxMin -= PARTIAL_SEARCH_AREA_ADD_WIDTH;
    else xPdIdxMin = 0;

    if(xPdIdxMax < ( (X_CELL_SIZE-1) - PARTIAL_SEARCH_AREA_ADD_WIDTH) ) xPdIdxMax += PARTIAL_SEARCH_AREA_ADD_WIDTH;
    else xPdIdxMax = (X_CELL_SIZE-1);

    if(xLedIdxMin > PARTIAL_SEARCH_AREA_ADD_WIDTH) xLedIdxMin -= PARTIAL_SEARCH_AREA_ADD_WIDTH;
    else xLedIdxMin = 0;

    if(xLedIdxMax < ( (X_CELL_SIZE-1) - PARTIAL_SEARCH_AREA_ADD_WIDTH)) xLedIdxMax += PARTIAL_SEARCH_AREA_ADD_WIDTH;
    else xLedIdxMax = (X_CELL_SIZE-1);

    // Y-Axis
    //--------------------------------------------------------------------------
    if(yPdIdxMin > PARTIAL_SEARCH_AREA_ADD_WIDTH) yPdIdxMin -= PARTIAL_SEARCH_AREA_ADD_WIDTH;
    else yPdIdxMin = 0;

    if(yPdIdxMax < ( (Y_CELL_SIZE-1) - PARTIAL_SEARCH_AREA_ADD_WIDTH) ) yPdIdxMax += PARTIAL_SEARCH_AREA_ADD_WIDTH;
    else yPdIdxMax = (Y_CELL_SIZE-1);

    if(yLedIdxMin > PARTIAL_SEARCH_AREA_ADD_WIDTH) yLedIdxMin -= PARTIAL_SEARCH_AREA_ADD_WIDTH;
    else yLedIdxMin = 0;

    if(yLedIdxMax < ( (Y_CELL_SIZE-1) - PARTIAL_SEARCH_AREA_ADD_WIDTH)) yLedIdxMax += PARTIAL_SEARCH_AREA_ADD_WIDTH;
    else yLedIdxMax = (Y_CELL_SIZE-1);
    //--------------------------------------------------------------------------

    xPdScanWidth = xPdIdxMax - xPdIdxMin + 1;
    xPartialEntryWidth = X_PARTIAL_ENTRY_WIDTH;
    //--------------------------------------------------------------------------
    if (scanStatus.isPartialScan == FALSE) { // if current scan is full
        partialScanExecCnt = 0;

        if ( ( xBlockedLineCnt > VALID_LINE_MIN_1ST && yBlockedLineCnt > VALID_LINE_MIN_2ND ) ||
                ( yBlockedLineCnt > VALID_LINE_MIN_1ST && xBlockedLineCnt > VALID_LINE_MIN_2ND ) ) {
            if (scanStatus.isFullScanForPartial == FALSE) {
                if (xPdScanWidth < xPartialEntryWidth) { // if partial scan range
                    partialScanEntryCnt++;
                } else {
                    partialScanEntryCnt = 0;
                }
                if (partialScanEntryCnt > FULL_TO_PARTIAL_CONDITION_CNT) {
                    scanStatus.isPartialScan = TRUE;
                }
            } else { // if full-scan during partial-scan
                if (xPdScanWidth < xPartialEntryWidth) { // if partial scan range
                    scanStatus.isPartialScan = TRUE;
                }
                scanStatus.isFullScanForPartial = FALSE;
            }
        } else {
            partialScanEntryCnt = 0;
        }
    } else { // if current scan is partial
        partialScanEntryCnt = 0;
        partialScanExecCnt++;

        if ( ( xBlockedLineCnt > VALID_LINE_MIN_1ST && yBlockedLineCnt > VALID_LINE_MIN_2ND ) ||
                ( yBlockedLineCnt > VALID_LINE_MIN_1ST && xBlockedLineCnt > VALID_LINE_MIN_2ND ) ) {
            if (partialScanExecCnt >= PARTIAL_TO_FULL_EXEC_CNT) {
                scanStatus.isFullScanForPartial = TRUE;
                scanStatus.isPartialScan = FALSE;
            } else {
                if (xPdScanWidth >= xPartialEntryWidth) { // if not partial scan range
                    scanStatus.isPartialScan = FALSE;
                }
            }
        } else {
            scanStatus.isPartialScan = FALSE;
        }
    }

    // if next scan is partial, set partial variable
    if (scanStatus.isPartialScan == TRUE) {
        xPartialScanData.pdIdxStart = xPdIdxMin;
        xPartialScanData.pdIdxEnd= xPdIdxMax;
        xPartialScanData.ledIdxStart = xLedIdxMin;
        xPartialScanData.ledIdxEnd = xLedIdxMax;

        yPartialScanData.pdIdxStart = yPdIdxMin;
        yPartialScanData.pdIdxEnd= yPdIdxMax;
        yPartialScanData.ledIdxStart = yLedIdxMin;
        yPartialScanData.ledIdxEnd = yLedIdxMax;
    }
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    totalBlockedLineCnt = xBlockedLineCnt + yBlockedLineCnt;


    if ( totalBlockedLineCnt < 5) totalBlockedLineCnt = 0;

    return totalBlockedLineCnt;
}

uint8_t isInSideArea(uint8_t pdIdx, uint8_t ledIdx, uint8_t cellSize, uint8_t cornerCellSize)
{
    uint8_t isSide = FALSE;
    uint8_t upperCornerCellSize;

    // lower side check
    if (pdIdx < cornerCellSize && ledIdx < cornerCellSize) isSide = TRUE;

    // upper side check
    if ( cornerCellSize < cellSize ) {
        upperCornerCellSize = cellSize - cornerCellSize;
        if (pdIdx >= upperCornerCellSize && ledIdx >= upperCornerCellSize) isSide = TRUE;
    }

    return isSide;
}

void setThresholdDataByAvgNonBlockLevel(uint8_t thresholdMode)
{
    int16_t pdIdx, totalCellSize;
    int16_t offset, maxOffset, ledIdx;
    int16_t cornerCellSize;
    int16_t thresholdDropPercent, centerThresholdDropPercent, cornerThresholdDropPercent;
#if (THRESHOLD_ZERO_COUNT_ENABLE) //nsmoon@211231
    uint8_t thresholdZeroCnt = 0;
#endif

    //--------------------------------------------------------------------------
    // X-scan threshold level setting
    //--------------------------------------------------------------------------
    totalCellSize = X_CELL_SIZE;
    maxOffset = X_MAX_OFFSET;
    cornerCellSize = X_CORNER_CELL_SIZE;

    if(thresholdMode == MAIN_THRESHOLD_MODE) {
        centerThresholdDropPercent = X_THRESHOLD_DROP_PERCENT;
        cornerThresholdDropPercent = X_CORNER_THRESHOLD_DROP_PERCENT;
    } else {
        if (scanStatus.requestSubThreshold == FALSE) return;

        centerThresholdDropPercent = subThresholdDropPercent;
        cornerThresholdDropPercent = subThresholdDropPercent;

        if ( cornerThresholdDropPercent >= X_CORNER_THRESHOLD_DROP_PERCENT) {
            cornerThresholdDropPercent -= X_CORNER_THRESHOLD_DROP_PERCENT;
        }
    }

    for (pdIdx = 0; pdIdx < totalCellSize; pdIdx++) {
        for (offset = - maxOffset; offset <= maxOffset; offset++)
        {
            ledIdx = offset + pdIdx;
            if ( ledIdx >= 0 && ledIdx < totalCellSize ) { // check if ledIdx is in the range
                if ( isInSideArea(pdIdx, ledIdx, totalCellSize, cornerCellSize)) {
                    thresholdDropPercent = cornerThresholdDropPercent;
                } else {
                    thresholdDropPercent = centerThresholdDropPercent;
                }

                if(thresholdMode == MAIN_THRESHOLD_MODE) {
                    if (xCellStatus[pdIdx].isPdDead || xCellStatus[ledIdx].isLedDead) {
                        xScanNonBlockLevel[pdIdx][offset + maxOffset] = 0;
                    }

                    xScanThresholdData[pdIdx][offset + maxOffset] =
                        (uint8_t)(xScanNonBlockLevel[pdIdx][offset + maxOffset] * (100 - thresholdDropPercent) / 100);
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 0) //nsmoon@211213
                    print_scan_threshold("X[[", X_AXIS, ledIdx, pdIdx);
#endif
#if (THRESHOLD_ZERO_COUNT_ENABLE) //nsmoon@211230
                    if (xCellStatus[pdIdx].isPdDead == 0 && xCellStatus[ledIdx].isLedDead == 0) {
                        if (xScanThresholdData[pdIdx][offset + maxOffset] == 0) {
                            if (thresholdZeroCnt < 0xFF) thresholdZeroCnt++;
                        }
                    }
#endif
                } else {
                    xScanSubThresholdData[pdIdx][offset + maxOffset] =
                        (uint8_t)(xScanNonBlockLevel[pdIdx][offset + maxOffset] * (100 - thresholdDropPercent) / 100);
                }
            }
        }
    }

    //--------------------------------------------------------------------------
    // Y-scan threshold level setting
    //--------------------------------------------------------------------------
    totalCellSize = Y_CELL_SIZE;
    maxOffset = Y_MAX_OFFSET;
    cornerCellSize = Y_CORNER_CELL_SIZE;
    if(thresholdMode == MAIN_THRESHOLD_MODE) {
        centerThresholdDropPercent = Y_THRESHOLD_DROP_PERCENT;
        cornerThresholdDropPercent = Y_CORNER_THRESHOLD_DROP_PERCENT;
    } else {
        centerThresholdDropPercent = subThresholdDropPercent;
        cornerThresholdDropPercent = subThresholdDropPercent;

        if ( cornerThresholdDropPercent >= Y_CORNER_THRESHOLD_DROP_PERCENT) {
            cornerThresholdDropPercent -= Y_CORNER_THRESHOLD_DROP_PERCENT;
        }
    }

    for (pdIdx = 0; pdIdx < totalCellSize; pdIdx++) {
        for (offset = - maxOffset; offset <= maxOffset; offset++)
        {
            ledIdx = offset + pdIdx;
            if ( ledIdx >= 0 && ledIdx < totalCellSize ) { // check if ledIdx is in the range
                if ( isInSideArea(pdIdx, ledIdx, totalCellSize, cornerCellSize)) {
                    thresholdDropPercent = cornerThresholdDropPercent;
                } else {
                    thresholdDropPercent = centerThresholdDropPercent;
                }

                if(thresholdMode == MAIN_THRESHOLD_MODE) {
                    if (yCellStatus[pdIdx].isPdDead || yCellStatus[ledIdx].isLedDead) {
                        yScanNonBlockLevel[pdIdx][offset + maxOffset] = 0;
                    }

                    yScanThresholdData[pdIdx][offset + maxOffset] =
                        (uint8_t)(yScanNonBlockLevel[pdIdx][offset + maxOffset] * (100 - thresholdDropPercent) / 100);
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 0) //nsmoon@211213
                    print_scan_threshold("Y[[", Y_AXIS, ledIdx, pdIdx);
#endif
#if (THRESHOLD_ZERO_COUNT_ENABLE) //nsmoon@211230
                    if (yCellStatus[pdIdx].isPdDead == 0 && yCellStatus[ledIdx].isLedDead == 0) {
                        if (yScanThresholdData[pdIdx][offset + maxOffset] == 0) {
                            if (thresholdZeroCnt < 0xFF) thresholdZeroCnt++;
                        }
                    }
#endif
                } else {
                    yScanSubThresholdData[pdIdx][offset + maxOffset] =
                        (uint8_t)(yScanNonBlockLevel[pdIdx][offset + maxOffset] * (100 - thresholdDropPercent) / 100);
                }
            }
        }
    }
#if FORCE_AGC_SET
#if (THRESHOLD_ZERO_COUNT_ENABLE) //nsmoon@211231
#define THRESHOLD_ZERO_COUNT_MAX    100
    if (thresholdZeroCnt > THRESHOLD_ZERO_COUNT_MAX) {
        TRACE_UART("thresholdZeroCnt=%d\r\n", thresholdZeroCnt);
        g_forcedagc.forced_saved = 1;
    }
#endif
#endif
}

uint8_t getADCResultByPdIdxAndOffset(axis_type_enum axisType, uint8_t pdIdx, int8_t offset)
{
    int16_t ledIdx;
    uint8_t pdIdxStart, pdIdxEnd, ledIdxStart, ledIdxEnd;
    uint8_t maxOffset;
    uint8_t adcResult;

    adcResult = 0;

    if (axisType == X_AXIS) {
        if (scanStatus.isPartialScan) {
            pdIdxStart = xPartialScanData.pdIdxStart;
            pdIdxEnd = xPartialScanData.pdIdxEnd;
            ledIdxStart = xPartialScanData.ledIdxStart;
            ledIdxEnd = xPartialScanData.ledIdxEnd;
        } else {
            pdIdxStart = 0;
            pdIdxEnd = X_CELL_SIZE - 1;
            ledIdxStart = 0;
            ledIdxEnd = X_CELL_SIZE - 1;
        }
        maxOffset = X_MAX_OFFSET;

        if ( pdIdx >= pdIdxStart && pdIdx <= pdIdxEnd) { // check boundary
            ledIdx = offset + pdIdx;
            if ( ledIdx >= ledIdxStart && ledIdx <= ledIdxEnd) { // check boundary
                adcResult = xScanResultData[pdIdx][offset + maxOffset];
            }
        }
    } else { // Y_AXIS
        if (scanStatus.isPartialScan) {
            pdIdxStart = yPartialScanData.pdIdxStart;
            pdIdxEnd = yPartialScanData.pdIdxEnd;
            ledIdxStart = yPartialScanData.ledIdxStart;
            ledIdxEnd = yPartialScanData.ledIdxEnd;
        } else {
            pdIdxStart = 0;
            pdIdxEnd = Y_CELL_SIZE - 1;
            ledIdxStart = 0;
            ledIdxEnd = Y_CELL_SIZE - 1;
        }
        maxOffset = Y_MAX_OFFSET;

        if ( pdIdx >= pdIdxStart && pdIdx <= pdIdxEnd) { // check boundary
            ledIdx = offset + pdIdx;
            if ( ledIdx >= ledIdxStart && ledIdx <= ledIdxEnd) { // check boundary
                adcResult = yScanResultData[pdIdx][offset + maxOffset];
            }
        }
    }

    return adcResult;
}

// global input :
//  scanStatus, xPartialScanData, yPartialScanData
//  xScanResultData, yScanResultData
// return ADC result
//
// cf) offset = ledIdx - pdIdx
uint8_t getADCResultByLedIdxAndOffset(axis_type_enum axisType, uint8_t ledIdx, int8_t offset)
{
    int16_t pdIdx;
    uint8_t pdIdxStart, pdIdxEnd, ledIdxStart, ledIdxEnd;
    uint8_t maxOffset;
    uint8_t adcResult;

    adcResult = 0;

    if (axisType == X_AXIS) {
        if (scanStatus.isPartialScan) {
            pdIdxStart = xPartialScanData.pdIdxStart;
            pdIdxEnd = xPartialScanData.pdIdxEnd;
            ledIdxStart = xPartialScanData.ledIdxStart;
            ledIdxEnd = xPartialScanData.ledIdxEnd;
        } else {
            pdIdxStart = 0;
            pdIdxEnd = X_CELL_SIZE - 1;
            ledIdxStart = 0;
            ledIdxEnd = X_CELL_SIZE - 1;
        }
        maxOffset = X_MAX_OFFSET;

        if ( ledIdx >= ledIdxStart && ledIdx <= ledIdxEnd) { // check boundary
            pdIdx = ledIdx - offset;
            if ( pdIdx >= pdIdxStart && pdIdx <= pdIdxEnd) { // check boundary
                adcResult = xScanResultData[pdIdx][offset + maxOffset];
            }
        }
    } else { // Y_AXIS
        if (scanStatus.isPartialScan) {
            pdIdxStart = yPartialScanData.pdIdxStart;
            pdIdxEnd = yPartialScanData.pdIdxEnd;
            ledIdxStart = yPartialScanData.ledIdxStart;
            ledIdxEnd = yPartialScanData.ledIdxEnd;
        } else {
            pdIdxStart = 0;
            pdIdxEnd = Y_CELL_SIZE - 1;
            ledIdxStart = 0;
            ledIdxEnd = Y_CELL_SIZE - 1;
        }
        maxOffset = Y_MAX_OFFSET;

        if ( ledIdx >= ledIdxStart && ledIdx <= ledIdxEnd) { // check boundary
            pdIdx = ledIdx - offset;
            if ( pdIdx >= pdIdxStart && pdIdx <= pdIdxEnd) { // check boundary
                adcResult = yScanResultData[pdIdx][offset + maxOffset];
            }
        }
    }

    return adcResult;
}

// global input :
//  scanStatus, xPartialScanData, yPartialScanData
//  xScanResultData, yScanResultData
// return ADC result
//
// cf) offset = ledIdx - pdIdx
uint8_t getADCResultByDeviceIdx(axis_type_enum axisType, uint8_t pdIdx, uint8_t ledIdx)
{
    int16_t offset;
    uint8_t pdIdxStart, pdIdxEnd, ledIdxStart, ledIdxEnd;
    uint8_t maxOffset;
    uint8_t adcResult;

    adcResult = 0;

    if (axisType == X_AXIS) {
        if (scanStatus.isPartialScan) {
            pdIdxStart = xPartialScanData.pdIdxStart;
            pdIdxEnd = xPartialScanData.pdIdxEnd;
            ledIdxStart = xPartialScanData.ledIdxStart;
            ledIdxEnd = xPartialScanData.ledIdxEnd;
        } else {
            pdIdxStart = 0;
            pdIdxEnd = X_CELL_SIZE - 1;
            ledIdxStart = 0;
            ledIdxEnd = X_CELL_SIZE - 1;
        }
        maxOffset = X_MAX_OFFSET;

        if ( pdIdx >= pdIdxStart && pdIdx <= pdIdxEnd) { // check pdIdx boundary
            if ( ledIdx >= ledIdxStart && ledIdx <= ledIdxEnd) { // check ledIdx boundary
                offset = ledIdx - pdIdx;
                if (offset >= - maxOffset && offset <= maxOffset) { // check offset boundary
                    adcResult = xScanResultData[pdIdx][offset + maxOffset];
                }
            }
        }
    } else { // Y_AXIS
        if (scanStatus.isPartialScan) {
            pdIdxStart = yPartialScanData.pdIdxStart;
            pdIdxEnd = yPartialScanData.pdIdxEnd;
            ledIdxStart = yPartialScanData.ledIdxStart;
            ledIdxEnd = yPartialScanData.ledIdxEnd;
        } else {
            pdIdxStart = 0;
            pdIdxEnd = Y_CELL_SIZE - 1;
            ledIdxStart = 0;
            ledIdxEnd = Y_CELL_SIZE - 1;
        }
        maxOffset = Y_MAX_OFFSET;

        if ( pdIdx >= pdIdxStart && pdIdx <= pdIdxEnd) { // check pdIdx boundary
            if ( ledIdx >= ledIdxStart && ledIdx <= ledIdxEnd) { // check ledIdx boundary
                offset = ledIdx - pdIdx;
                if (offset >= - maxOffset && offset <= maxOffset) { // check offset boundary
                    adcResult = yScanResultData[pdIdx][offset + maxOffset];
                }
            }
        }
    }

    return adcResult;
}

void software_reset(void)
{
    // perform a system unlock sequence
    //SYSKEY = 0x00000000;   //write invalid key to force lock
    //SYSKEY = 0xAA996655;   //write key1 to SYSKEY
    //SYSKEY = 0x556699AA;   //write key2 to SYSKEY
    // set SWRST bit to arm reset
    //RSWRSTSET = 1;
    // read RSWRST register to trigger reset
    //RSWRST;
    // prevent any unwanted code execution until reset occurs
    //while(1);
    NVIC_SystemReset();
}
#if 0
void setProcessExtLightTimerMsec(uint32_t val)
{
    endProcessExtLightTimer = (uint32_t)((val) * 50);
    //TIMER_CNT(TIMER3) = 0;
    startProcessExtLightTimer = GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL);
}

void clearProcessExtLightTimer(void)
{
    startProcessExtLightTimer =GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL);
}

int16_t isProcessExtLightTimerExpired(void)
{
    if ( (GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL) - startProcessExtLightTimer) >= endProcessExtLightTimer ) return 1;
    else return 0;
}
#else
void setProcessExtLightTimerMsec(uint32_t val)
{
	endProcessExtLightTimer = (uint32_t)(WAIT_TIME_10MS(val));	// 50
    //TIMER_CNT(TIMER3) = 0;
	startProcessExtLightTimer = GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL);
}

void clearProcessExtLightTimer(void)
{
	startProcessExtLightTimer =GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL);
}


int16_t isProcessExtLightTimerExpired(void)
{
    if ( GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL) - startProcessExtLightTimer >= endProcessExtLightTimer ) return 1;
    else return 0;
}





#endif
void setAgcControlTimerMsec(uint32_t val)
{
    endAgcCtrlTimer = (uint32_t)(WAIT_TIME_10MS(val));	// 50
    //TIMER_CNT(TIMER3) = 0;
    startAgcCtrlTimer = GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL);
}

void clearAgcControlTimer(void)
{
    startAgcCtrlTimer =GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL);
}


int16_t isAgcControlTimerExpired(void)
{
    if ( GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL) - startAgcCtrlTimer >= endAgcCtrlTimer ) return 1;
    else return 0;
}


void setDelayScanTimer10Msec(uint32_t val)
{
    endDelayscanTimer = (uint32_t)(WAIT_TIME_10MS(val));	// 50
    //TIMER_CNT(TIMER3) = 0;
    startDelayscanTimer = GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL);
}

void clearDelayScanTimer(void)
{
    startDelayscanTimer =GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL);
}


int16_t isDelayScanTimerExpired(void)
{
    if ( GPT_GetCurrentTimerCount(BOARD_GPT2_PERIPHERAL) - startDelayscanTimer >= endDelayscanTimer ) return 1;
    else return 0;
}

void setPDDelayTimerUsec(uint32_t val)
{
    endPdDelayTimer= (uint32_t)(WAIT_TIME_US(val));	// 50
    //TIMER_CNT(TIMER3) = 0;
    startPdDelayTimer = GPT_GetCurrentTimerCount(BOARD_GPT1_PERIPHERAL);
}

void set_DelayTimerUsec(uint32_t val)
{
    endPdDelayTimer= (uint32_t)(WAIT_TIME_US(val));	// 50
    //TIMER_CNT(TIMER3) = 0;
    startPdDelayTimer = GPT_GetCurrentTimerCount(BOARD_GPT1_PERIPHERAL);
}

void clearPDDelayTimer(void)
{
    startPdDelayTimer =GPT_GetCurrentTimerCount(BOARD_GPT1_PERIPHERAL);
}


int16_t isPDDelayTimerExpired(void)
{
    if ( GPT_GetCurrentTimerCount(BOARD_GPT1_PERIPHERAL) - startPdDelayTimer >= endPdDelayTimer ) return 1;
    else return 0;
}

int16_t isDelayTimerExpired(void)
{
    if ( GPT_GetCurrentTimerCount(BOARD_GPT1_PERIPHERAL) - startPdDelayTimer >= endPdDelayTimer ) return 1;
    else return 0;
}

void Timer_Delay_us(uint32_t val)
{
    setPDDelayTimerUsec(val);
    while(isPDDelayTimerExpired() == 0)
    {
        //WDOG_Reset_Refresh();	//2Sec
    }
}

#if (PROCESS_EXT_LIGHT_DISABLE == 0)

#if 0
int16_t checkExtLightNoise(uint8_t axisType)
{
    uint8_t currentStartPdIdx;
    int16_t pdIdxEnd;
    //cellStatus_t *cellStatus;
    uint8_t pdShiftDelayUsec;
    uint8_t adcResult[PD_SIGNAL_OUT_NUM];
    uint8_t i;
    uint8_t extLightNoistCnt;

    if (axisType == X_AXIS) {
        pdIdxEnd = X_CELL_SIZE - 1;
        //cellStatus = xCellStatus;
        pdShiftDelayUsec = xAxisPdShiftDelayUsec + xAxisExtLightDelayUsec;
    } else { // Y_AXIS
        pdIdxEnd = Y_CELL_SIZE - 1;
        //cellStatus = yCellStatus;
        pdShiftDelayUsec = yAxisPdShiftDelayUsec + yAxisExtLightDelayUsec;
    }
#if ENABLE_SCAN_HIGH_GAIN_DLT //nsmoon@220404
    if (scanGainHigh) {
        M_2ndOpAmp_Gain_High();
    }
#endif

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    // set 1st PD group in each Axis B'd : pd0 ~ pd14
    currentStartPdIdx = 0;

    if (axisType == X_AXIS) {
        selectPDGroup(currentStartPdIdx + Y_CELL_SIZE);
    } else {
        selectPDGroup(currentStartPdIdx);
    }
    if (pdShiftDelayUsec > 0) CORETIMER_DelayUs(pdShiftDelayUsec);

    //-----------------------------------------
    // reset sample and hold
    M_2ndOpAmp_Input_On();
    M_1stOpAmp_Input_Fixed_0d6V();
    M_2ndOpAmp_Input_GND();
    CORETIMER_DelayUs(4);

    M_2ndOpAmp_Input_Normal();
    M_2ndOpAmp_Input_Off();
    CORETIMER_DelayUs(20);	//cki 210503 4->20
    //-----------------------------------------

    extLightNoistCnt = 0;
    for (; currentStartPdIdx <= pdIdxEnd; currentStartPdIdx += PD_SIGNAL_OUT_NUM) {
        if (currentStartPdIdx != 0) {
#ifdef ENABLE_UART_CMD_PROCESS //nsmoon@210909
            if (PD_SIGNAL_OUT_NUM != (ENABLED_CS_NUM * CELLS_PER_CS)) {
                TRACE_UART("ERROR! invalid PD_SIGNAL_OUT_NUM %d (%d %d)", PD_SIGNAL_OUT_NUM, ENABLED_CS_NUM, CELLS_PER_CS);
            }
            if ((currentStartPdIdx % PD_SIGNAL_OUT_NUM) != 0) {
                TRACE_UART("ERROR! invalid currentStartPdIdx %d", currentStartPdIdx);
            }
#endif
            M_PD_SHIFT_CLK_GenPulse(ENABLED_CS_NUM); // (PD_SIGNAL_OUT_NUM/CELLS_PER_CS)
            gCurrentStartTotalPdIdx += PD_SIGNAL_OUT_NUM; //(ENABLED_CS_NUM * CELLS_PER_CS)
            if (pdShiftDelayUsec > 0) CORETIMER_DelayUs(pdShiftDelayUsec);
        }

        //----------------------------------------------------------------------
        // Sample and Hold External Light
        __builtin_disable_interrupts(); // kjsxy
#if ENABLE_LED_ON_TIME_FINE //20ns
        //M_Set_LedOn_Timer(75); //1.5usec=75*20ns
        M_Set_LedOn_Timer(LED_ON_TIME_INIT_X); //1.5usec=150*10ns
#else
        M_Set_LedOn_Timer(15); //1.5usec
#endif
        M_LED_Current_Off();
        M_NOP_Delay_200nsec();
        M_1stOpAmp_Input_Normal();

        M_NOP_Delay_200nsec();
        M_NOP_Delay_100nsec();
        M_2ndOpAmp_Input_On();

        M_Wait_LedOn_Timer_Expired(); // check and wait LED-On duty was expired

        M_2ndOpAmp_Input_Off(); // AN_OUT1 ~ AN_OUT15 sample & hold state
        M_LED_Current_Off();

        __builtin_enable_interrupts(); // kjsxy

        M_1stOpAmp_Input_Fixed_0d6V();
        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        // ANOUT_1 ~ ANOUT_5
        //----------------------------------------------------------------------
        M_ADC_GROUP_SELECT_AN_OUT_1To5(); // Set External MUX S/W to AN_OUT_1 ~ AN_OUT_5 => AN0 ~ AN4
        M_ADC_Input_Select_AN_0To4(); // AN0 ~ AN4 => ADC0 ~ ADC4
        M_ADC_Start(); // ADC converstion for current externally sampled & hold sig = ANOUT_1 ~ ANOUT_5
        M_WaitForADCCompleted();
        adcResult[0] = (uint8_t)(ADCDATA0 >> 4);
        adcResult[1] = (uint8_t)(ADCDATA1 >> 4);
        adcResult[2] = (uint8_t)(ADCDATA2 >> 4);
        adcResult[3] = (uint8_t)(ADCDATA3 >> 4);
        adcResult[4] = (uint8_t)(ADCDATA4 >> 4);

        //----------------------------------------------------------------------
        // ANOUT_6 ~ ANOUT_10
        //----------------------------------------------------------------------
        M_ADC_Input_Select_AN_45To49(); // AN45 ~ AN49 => ADC0 ~ ADC4
        M_ADC_Start(); // ADC converstion for current externally sampled & hold sig = ANOUT_6 ~ ANOUT_10
        M_WaitForADCCompleted();
        adcResult[5] = (uint8_t)(ADCDATA0 >> 4);
        adcResult[6] = (uint8_t)(ADCDATA1 >> 4);
        adcResult[7] = (uint8_t)(ADCDATA2 >> 4);
        adcResult[8] = (uint8_t)(ADCDATA3 >> 4);
        adcResult[9] = (uint8_t)(ADCDATA4 >> 4);

        //----------------------------------------------------------------------
        // ANOUT_11 ~ ANOUT_15
        //----------------------------------------------------------------------
        M_ADC_GROUP_SELECT_AN_OUT_11To15(); // Set External MUX S/W to AN_OUT_11 ~ AN_OUT_15 => AN0 ~ AN4
        M_ADC_Input_Select_AN_0To4(); // AN0 ~ AN4 => ADC0 ~ ADC4
        M_ADC_Start(); // ADC converstion for current externally sampled & hold sig = ANOUT_11 ~ ANOUT_15
        M_WaitForADCCompleted();
        adcResult[10] = (uint8_t)(ADCDATA0 >> 4);
        adcResult[11] = (uint8_t)(ADCDATA1 >> 4);
        adcResult[12] = (uint8_t)(ADCDATA2 >> 4);
        adcResult[13] = (uint8_t)(ADCDATA3 >> 4);
#if (PD_SIGNAL_OUT_NUM == 15)
        adcResult[14] = (uint8_t)(ADCDATA4 >> 4);
#endif

        for (i = 0; i < PD_SIGNAL_OUT_NUM; i++) {
            if (adcResult[i] > EXTL_MIN_LEVEL) {
                extLightNoistCnt++;
                TRACE_PROC_EXT_LIGHT("checkExtLightNoise() : axisType = %d, pdIdx = %d, adcResult = %d\r\n",
                        axisType, currentStartPdIdx+i, adcResult[i]);
                break;
            }
            else {
                if (adcResult[i] > 0) {
                    TRACE_PROC_EXT_LIGHT("\tcheckExtLightNoise() : axisType = %d, pdIdx = %d, adcResult = %d\r\n",
                            axisType, currentStartPdIdx+i, adcResult[i]);
                }
            }
        }

        if (extLightNoistCnt > 0) break;
    }

    if (extLightNoistCnt == 0) {
        TRACE_PROC_EXT_LIGHT("checkExtLightNoise() : axis = %d No noise\r\n", axisType);
    }

    if (axisType == X_AXIS) {
        selectPDGroup(-1); // disable all PD CS
    }

    return extLightNoistCnt;
}

#else
int16_t checkExtLightNoise(uint8_t axisType)
{
    volatile uint8_t basePdIdx; // pdIdx
    uint8_t currentStartPdIdx;
    int16_t pdIdxEnd;
    //cellStatus_t *cellStatus;
    uint8_t pdShiftDelayUsec;
    //uint8_t adcResult[PD_SIGNAL_OUT_NUM];
    uint32_t adcResult[PD_SIGNAL_OUT_NUM];
    uint8_t i;
    uint8_t extLightNoistCnt;

    if (axisType == X_AXIS) {
        pdIdxEnd = X_CELL_SIZE - 1;
        //cellStatus = xCellStatus;
        pdShiftDelayUsec = xAxisPdShiftDelayUsec + xAxisExtLightDelayUsec;
        basePdIdx = Y_CELL_SIZE;
    } else { // Y_AXIS
        pdIdxEnd = Y_CELL_SIZE - 1;
        //cellStatus = yCellStatus;
        pdShiftDelayUsec = yAxisPdShiftDelayUsec + yAxisExtLightDelayUsec;
        basePdIdx = 0;
    }
#if ENABLE_SCAN_HIGH_GAIN_DLT //nsmoon@220404
    if (scanGainHigh) {
        M_2ndOpAmp_Gain_High();
    }
#endif

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    // set 1st PD group in each Axis B'd : pd0 ~ pd14
    currentStartPdIdx = 0;
#if 0
    if (axisType == X_AXIS) {
        selectPDGroup(currentStartPdIdx + Y_CELL_SIZE);
    } else {
        selectPDGroup(currentStartPdIdx);
    }
    if (pdShiftDelayUsec > 0) CORETIMER_DelayUs(pdShiftDelayUsec);
#else
    if (axisType == Y_AXIS) {
    	M_First_PD_Gr_Select();
    	gCurrentStartTotalPdIdx = 0;
       selectPDGroup((int16_t)(basePdIdx));
    }
#endif
    extLightNoistCnt = 0;
    for (; currentStartPdIdx <= pdIdxEnd; currentStartPdIdx += PD_SIGNAL_OUT_NUM) {
        if (currentStartPdIdx != 0) {
#if ENABLE_UART_CMD_PROCESS //nsmoon@210909
            if (PD_SIGNAL_OUT_NUM != (ENABLED_CS_NUM * CELLS_PER_CS)) {
                TRACE_UART("ERROR! invalid PD_SIGNAL_OUT_NUM %d (%d %d)", PD_SIGNAL_OUT_NUM, ENABLED_CS_NUM, CELLS_PER_CS);
            }
            if ((currentStartPdIdx % PD_SIGNAL_OUT_NUM) != 0) {
                TRACE_UART("ERROR! invalid currentStartPdIdx %d", currentStartPdIdx);
            }
#endif
            M_PD_SHIFT_CLK_GenPulse(ENABLED_CS_NUM); // (PD_SIGNAL_OUT_NUM/CELLS_PER_CS)
            gCurrentStartTotalPdIdx += PD_SIGNAL_OUT_NUM; //(ENABLED_CS_NUM * CELLS_PER_CS)
            if (pdShiftDelayUsec > 0) CORETIMER_DelayUs(pdShiftDelayUsec);
        }

        //----------------------------------------------------------------------
        // Sample and Hold External Light

        //    TRACSCAN("\r\n%d,%d,%d", currentStartPdIdx , nextLedIdx, prevScanSequenceIdx);
                // M_NOP_Delay_1usec();
       // __disable_irq();
        //M_TSPM_Triger_Set();
        MCU_AN_SW_EN1_TSPM();
         Coupling_Sig_TSPM();

        while((BOARD_TMR3_PERIPHERAL->CHANNEL[BOARD_TMR3_CHANNEL_0_CHANNEL].CTRL & 0xe000) >0) {

                }
        //__enable_irq();
        //        M_Set_LedOff_Timer(LED_OFF_TIME_FIX);
        //----------------------------------------------------------------------
        // ADC Start/end Time   == 1.6us
        //----------------------------------------------------------------------
        //    DEBUG_TP2_HIGH();
                ADC_SamplesStart();
        #if 0
                M_NOP_Delay_60nsec();
        #else
                DEBUG_TP1_LOW();
                DEBUG_TP1_HIGH();
        #endif
                M_ADC_MUX_DATA_SET(1);
                while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                    ;   // 0,1
                }
                adcResult[0] = BOARD_ADC1_PERIPHERAL->R[0];
                adcResult[1] = BOARD_ADC2_PERIPHERAL->R[0];

                M_ADC_MUX_DATA_SET(2);
                while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                    ;   // 2,3
                }
                adcResult[2] = BOARD_ADC1_PERIPHERAL->R[0];
                adcResult[3] = BOARD_ADC2_PERIPHERAL->R[0];

                M_ADC_MUX_DATA_SET(3);
                while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                    ;   // 4,5
                }
                adcResult[4] = BOARD_ADC1_PERIPHERAL->R[0];
                adcResult[5] = BOARD_ADC2_PERIPHERAL->R[0];

                M_ADC_MUX_DATA_SET(4);
                while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                    ;   // 6,7
                }
                adcResult[6] = BOARD_ADC1_PERIPHERAL->R[0];
                adcResult[7] = BOARD_ADC2_PERIPHERAL->R[0];

                M_ADC_MUX_DATA_SET(0);
                while ((BOARD_ADC1_PERIPHERAL->HS & 1UL) == 0 && (BOARD_ADC2_PERIPHERAL->HS & 1UL) == 0 ) {
                    ;   // 8,9
                }
                adcResult[8] = BOARD_ADC1_PERIPHERAL->R[0];
                adcResult[9] = BOARD_ADC2_PERIPHERAL->R[0];

                ADC_SamplesStop();
        #endif
                //TRACE_PROC_EXT_LIGHT("\r\n adcResult:	");
        for (i = 0; i < PD_SIGNAL_OUT_NUM; i++) {
#if 1
            if (adcResult[i] > EXTL_MIN_LEVEL) {
                extLightNoistCnt++;
                TRACE_PROC_EXT_LIGHT("checkExtLightNoise() : axisType = %d, pdIdx = %d, adcResult = %d\r\n",
                        axisType, currentStartPdIdx+i, adcResult[i]);
                break;
            }
            else {
                if (adcResult[i] > 0) {
                    TRACE_PROC_EXT_LIGHT("\tcheckExtLightNoise() : axisType = %d, pdIdx = %d, adcResult = %d\r\n",
                            axisType, currentStartPdIdx+i, adcResult[i]);
                }
            }
#else
            //TRACE_PROC_EXT_LIGHT("%d	", adcResult[i]);
#endif

        }

        if (extLightNoistCnt > 0) break;
    }

    if (extLightNoistCnt == 0) {
        TRACE_PROC_EXT_LIGHT("checkExtLightNoise() : axis = %d No noise\r\n", axisType);
    }

    if (axisType == X_AXIS) {
        selectPDGroup(-1); // disable all PD CS
    }

    return extLightNoistCnt;
}


int16_t compensateExtLightNoise(uint8_t axis)
{
    uint8_t dir, prevDir, isNoise;
    uint8_t *pExtLightDelayUsec, prevExtLightDelayUsec;
    int16_t isDelayChanged = FALSE;

    if (axis == X_AXIS) {
        pExtLightDelayUsec = &xAxisExtLightDelayUsec;
    } else {
        pExtLightDelayUsec = &yAxisExtLightDelayUsec;
    }

    prevExtLightDelayUsec = *pExtLightDelayUsec;
    prevDir = EXT_LIGHT_DEC_DELAY;

    while(1) {
        isNoise = checkExtLightNoise(axis);

        if(isNoise != 0) { // if noise exist
            dir = EXT_LIGHT_INC_DELAY;

            if ( *pExtLightDelayUsec != EXT_LIGHT_SHIFT_DELAY_MAX) {
                *pExtLightDelayUsec += EXT_LIGHT_SHIFT_DELAY_STEP;
                if ( *pExtLightDelayUsec > EXT_LIGHT_SHIFT_DELAY_MAX) {
                    *pExtLightDelayUsec = EXT_LIGHT_SHIFT_DELAY_MAX;
                }
            } else {
                // need to take countermeasure for this situation
                break;
            }
        } else { // if no noise
            if ( prevDir == EXT_LIGHT_INC_DELAY) {
                break;
            }

            dir = EXT_LIGHT_DEC_DELAY;

            if (*pExtLightDelayUsec > EXT_LIGHT_SHIFT_DELAY_STEP) {
                *pExtLightDelayUsec -= EXT_LIGHT_SHIFT_DELAY_STEP;
            } else if (*pExtLightDelayUsec != 0) {
                *pExtLightDelayUsec = 0;
            } else {
                break;
            }
        }

        prevDir = dir;
        //CORETIMER_DelayMs(1); // kjs 210315 mask
    }

    //CORETIMER_DelayMs(1); // kjs 210315 mask

    if (*pExtLightDelayUsec != prevExtLightDelayUsec) {
        isDelayChanged = TRUE;
    }

    return isDelayChanged;
}

int16_t processExtLightNoise(void)
{
    int16_t yChanged, xChanged;

    if (scanStatus.isUsualExtLightNoiseCal == FALSE) return FALSE; // kjs 210315 add

    yChanged = xChanged = FALSE;

#if 0 // 0 for time chk, 1 for counter chk
    if (++procExtLightNoiseCnt > PROC_EXT_LIGHT_NOISE_ENTRY_CNT) {
#else
    if (isProcessExtLightTimerExpired()) {
#endif
        //TRACE_PROC_EXT_LIGHT("processExtLightNoise() : proc started\r\n");

        yChanged = compensateExtLightNoise(Y_AXIS);
        xChanged = compensateExtLightNoise(X_AXIS);
        selectPDGroup(-1); // disable all PD CS

        if (yChanged == TRUE || xChanged == TRUE) {
            scanStatus.isExtLightDelayChanged = TRUE;

            TRACE_PROC_EXT_LIGHT("processExtLightNoise() : ");
            if (xChanged) TRACE_PROC_EXT_LIGHT("xAxisExtLightDelayUsec = %d, ", xAxisExtLightDelayUsec);
            if (yChanged) TRACE_PROC_EXT_LIGHT("yAxisExtLightDelayUsec = %d, ", yAxisExtLightDelayUsec);
            TRACE_PROC_EXT_LIGHT("\n");
        }

#if 0 // 0 for time chk, 1 for counter chk
        if (yAxisExtLightDelayUsec == 0 && xAxisExtLightDelayUsec == 0) {
            procExtLightNoiseCnt = 0;
        } else {
            procExtLightNoiseCnt = PROC_EXT_LIGHT_NOISE_ENTRY_CNT - PROC_EXT_LIGHT_NOISE_ENTRY_CNT2;
        }
#else
        clearProcessExtLightTimer();
#endif
    }

    return (yChanged || xChanged);
}
#endif

uint16_t Led_CurrentTbl_Adj_DAC(uint8_t CurrentTblIdx, uint8_t axisType)
{
    uint16_t ledDacMinIdx;
    uint8_t axisDacMinVal;
    if(axisType == X_AXIS)axisDacMinVal = LED_ON_DAC_MIN_X;
    else axisDacMinVal = LED_ON_DAC_MIN_Y;
    switch ( CurrentTblIdx)
    {
    case 0  :
        ledDacMinIdx = axisDacMinVal;
        break;
    case 1  :
        ledDacMinIdx = axisDacMinVal + PD_GAIN_OFFSET_1;
        break;
    case 2  :
        ledDacMinIdx = axisDacMinVal + PD_GAIN_OFFSET_2;
        break;
    case 3  :
        ledDacMinIdx = axisDacMinVal + PD_GAIN_OFFSET_3;
        break;
    default :
        ledDacMinIdx = axisDacMinVal;
        break;
    }
    return ledDacMinIdx;
}

//------------------------------------------------------------------------------
// global input :
//  scanStatus, xPartialScanData, yPartialScanData
//  xScanResultData, yScanResultData
// return :  if val >= 0, val = changed sequence counter
//           else, -1 : max limit, -2 : min limit
// cf) offset = ledIdx - pdIdx
int16_t controlLEDLightIntensityWithTryCnt(uint8_t axisType, uint8_t tryCnt)
{
    int16_t offset;
    uint8_t pdIdxStart, pdIdxEnd, ledIdxStart, ledIdxEnd;
    uint8_t maxOffset;
    int16_t changedCnt; //errorKind;
    uint16_t scanSequenceIdx, totalScanSequenceStep;
    uint8_t ledIdx, pdIdx, pdIdxGrStart, i;
    uint8_t adcResult, validPdCnt, highPdCnt, lowPdCnt, minADCResult;
    // uint8_t *ledCurrentTblIdx, *ledOnTimeIdx, ledCurrentMaxIdx;
    uint8_t *ledCurrentTblIdx, *ledDacIdx, ledCurrentMaxIdx;
    cellStatus_t *cellStatus;
    // uint8_t *prevLedCurrentTblIdx;
    uint8_t pdSaturationRatio;
    uint8_t lowTolerance;
    //uint8_t curCurrentIdx, nextCurrentIdx;
#if LED_ON_TIME_SHORT //nsmoon@210929
    uint8_t changedCntHigh, changedCntLow; //kang@211013
    //uint16_t ledTimeMinIdx, ledTimeMaxIdx;
    uint16_t ledDacMinIdx, ledDacMaxIdx;
#endif
    int8_t *prevAxisLedCurrentCtrlDir;
#if DEBUG_LED_CUR_TBL_IDX //for test //nsmoon@220302;
    uint8_t axisTypeTest = X_AXIS;
    uint16_t scanSequenceIdxTest = 367;
    if (axisType == axisTypeTest && tryCnt == 0) {
        printf("\r\n");
    }
#endif
#if EDGE_SLOPE_AGC_ENABLE //nsmoon@220314
    uint8_t maxPdNo = (axisType == X_AXIS) ? (X_CELL_SIZE -1) : (Y_CELL_SIZE -1);
#endif

    changedCnt = 0;
    //errorKind = 0;

    if (axisType == X_AXIS) {
        if (scanStatus.isPartialScan) {
            pdIdxStart = xPartialScanData.pdIdxStart;
            pdIdxEnd = xPartialScanData.pdIdxEnd;
            ledIdxStart = xPartialScanData.ledIdxStart;
            ledIdxEnd = xPartialScanData.ledIdxEnd;
        } else {
            pdIdxStart = 0;
            pdIdxEnd = X_CELL_SIZE - 1;
            ledIdxStart = 0;
            ledIdxEnd = X_CELL_SIZE - 1;
        }
        maxOffset = X_MAX_OFFSET;
        totalScanSequenceStep = X_TOTAL_SCAN_STEPS;
        lowTolerance = PD_LEVEL_TOLERANCE_X;

        cellStatus = xCellStatus;
        ledCurrentTblIdx = xAxisLedCurrentTblIdx;
        // prevLedCurrentTblIdx = xAxisPrevLedCurrentTblIdx;
        prevAxisLedCurrentCtrlDir = prevXLedCurrentCtrlDir;
        ledDacIdx = xAxisDacIdx;
        ledCurrentMaxIdx = X_LED_CURRENT_MAX_INDEX;

#if LED_ON_TIME_SHORT //nsmoon@210929
        ledDacMinIdx = LED_ON_DAC_MIN_X;
        ledDacMaxIdx = LED_ON_DAC_MAX_X;
#endif
        scanSequenceIdx = findStartScanSequenceIdx(X_AXIS, pdIdxStart, ledIdxStart, ledIdxEnd);
    } else { // Y_AXIS
        if (scanStatus.isPartialScan) {
            pdIdxStart = yPartialScanData.pdIdxStart;
            pdIdxEnd = yPartialScanData.pdIdxEnd;
            ledIdxStart = yPartialScanData.ledIdxStart;
            ledIdxEnd = yPartialScanData.ledIdxEnd;
        } else {
            pdIdxStart = 0;
            pdIdxEnd = Y_CELL_SIZE - 1;
            ledIdxStart = 0;
            ledIdxEnd = Y_CELL_SIZE - 1;
        }
        maxOffset = Y_MAX_OFFSET;
        totalScanSequenceStep = Y_TOTAL_SCAN_STEPS;
        lowTolerance = PD_LEVEL_TOLERANCE_Y;

        cellStatus = yCellStatus;
        ledCurrentTblIdx = yAxisLedCurrentTblIdx;
        // prevLedCurrentTblIdx = yAxisPrevLedCurrentTblIdx;
        prevAxisLedCurrentCtrlDir = prevYLedCurrentCtrlDir;
        ledDacIdx = yAxisDacIdx;
        ledCurrentMaxIdx = Y_LED_CURRENT_MAX_INDEX;

#if LED_ON_TIME_SHORT //nsmoon@210929
#if ENABLE_LED_ON_TIME_FINE
        ledDacMinIdx = LED_ON_DAC_MIN_Y;
        ledDacMaxIdx = LED_ON_DAC_MAX_Y;
#else
        ledDacMinIdx = 0;
        ledDacMaxIdx = LED_ON_TIME_MAX_INDEX;
#endif
#endif
        scanSequenceIdx = findStartScanSequenceIdx(Y_AXIS, pdIdxStart, ledIdxStart, ledIdxEnd);
    }

    while (scanSequenceIdx < totalScanSequenceStep) {
        if (axisType == X_AXIS) {
            pdIdxGrStart = xAxisSequenceTbl[scanSequenceIdx][0];
            ledIdx = xAxisSequenceTbl[scanSequenceIdx][1];
        } else { // Y_AXIS
            pdIdxGrStart = yAxisSequenceTbl[scanSequenceIdx][0];
            ledIdx = yAxisSequenceTbl[scanSequenceIdx][1];
        }
        if (tryCnt == 0) {
            prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 0; //reset
        }

        // if pdIdxStart is boundary over, exit while()
        if (pdIdxGrStart > pdIdxEnd ) {
            break;
        }

        // if ledIdx is boundary over, continue.
        if ( ledIdx < ledIdxStart || ledIdx > ledIdxEnd ) {
            scanSequenceIdx++;
            continue;
        }

        // if LED cell is dead, skip.
        if (cellStatus[ledIdx].isLedDead) {
            scanSequenceIdx++;
            continue;
        }

        validPdCnt = 0;
        highPdCnt = 0;
        lowPdCnt = 0;
        minADCResult = 255; // set max value

#if DEBUG_LED_CUR_TBL_IDX //for test //nsmoon@220302
        if (axisType == axisTypeTest && tryCnt < 2 && scanSequenceIdx == scanSequenceIdxTest) {
            printf("#%d-%d-%d/%d\t", scanSequenceIdxTest, tryCnt, ledIdx, prevAxisLedCurrentCtrlDir[scanSequenceIdx]);
        }
#endif

        for (i = 0; i < PD_SIGNAL_OUT_NUM; i++)
        {
#if EDGE_SLOPE_AGC_ENABLE //nsmoon@220314
            pdIdx = (axisType == X_AXIS) ? xPdIdxAtANOutTbl[scanSequenceIdx][i] : yPdIdxAtANOutTbl[scanSequenceIdx][i];
            if ( pdIdx > maxPdNo ) {
                continue;
            }
#else
            pdIdx = pdIdxGrStart + i;
            if ( pdIdx > pdIdxEnd ) break;
#endif

            // if PD cell is dead, skip.
            if (cellStatus[pdIdx].isPdDead) {
                continue;
            }

            offset = ledIdx - pdIdx;
            if ( offset >= - maxOffset && offset <= maxOffset)
            {   // offset boundary check
                if (axisType == X_AXIS) {
                    adcResult = xScanResultData[pdIdx][offset + maxOffset];
#if DEBUG_LED_CUR_TBL_IDX //for test //nsmoon@220302
                    if (axisType == axisTypeTest && tryCnt < 2) {
                        //if (pdIdx == 102 && ledIdx == 114)
                        if (scanSequenceIdx == scanSequenceIdxTest)
                        {
                            // scanSequenceIdxTest = scanSequenceIdx;
                            printf("(%d-%d)", pdIdx, adcResult);
                        }
                    }
#endif
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 1) //nsmoon@211213
                    print_scan_threshold("X<<", X_AXIS, ledIdx, pdIdx);
#endif
                } else { // Y_AXIS
                    adcResult = yScanResultData[pdIdx][offset + maxOffset];
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 1) //nsmoon@211213
                    print_scan_threshold("Y<<", Y_AXIS, ledIdx, pdIdx);
#endif
                }

                validPdCnt++;

                // update minimum ADC result
                if (adcResult < minADCResult) {
                    minADCResult = adcResult;
                }

                if (adcResult < PD_LEVEL_LOW) {
                    //  count low level result
                    lowPdCnt++;
                }
                else if (adcResult > PD_LEVEL_HIGH) {
                    //  count high level result
                    highPdCnt++;
                }
            }
        }
#if DEBUG_LED_CUR_TBL_IDX //for test //nsmoon@220302
        if (axisType == axisTypeTest && tryCnt < 2) {
            if (scanSequenceIdx == scanSequenceIdxTest)
            {
                printf("@%d/%d", lowPdCnt, highPdCnt);
            }
        }
#endif

        if (validPdCnt != 0) {
#if LED_ON_TIME_SHORT //nsmoon@210929
            changedCntLow = changedCntHigh = 0; //kang@211013
#if (CAL_HIGH_TO_LOW == FALSE) //nsmoon@211230
            //------------------------------------------------------------------
            // need to increase light intensity
            //------------------------------------------------------------------
            if (lowPdCnt > 0) //kang@211013
            {

                if (ledDacIdx[scanSequenceIdx] < ledDacMaxIdx) {
                    prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 1;
                    changedCnt++;
                    changedCntLow++;
                    ledDacIdx[scanSequenceIdx] += LED_ON_DAC_UINT;
                    //TRACE_AGC_KANG("[%d/%d] UP TIME!!! Low PD COUNT %d led time %d \r\n",axisType,scanSequenceIdx, lowPdCnt,ledDacIdx[scanSequenceIdx]);
                } else {
                    if (ledCurrentTblIdx[scanSequenceIdx] < ledCurrentMaxIdx) { // adjust led current first
                        prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 1;
                        changedCnt++;
                        changedCntLow++;
                        ledCurrentTblIdx[scanSequenceIdx]++;
#if 1   //YJ@220915
                        ledDacMinIdx = Led_CurrentTbl_Adj_DAC(ledCurrentTblIdx[scanSequenceIdx], axisType);
#endif
                        ledDacIdx[scanSequenceIdx] = ledDacMinIdx;

                    }
                }

            } //if (lowPdCnt > 0)
#endif
            //------------------------------------------------------------------
            // need to decrease light intensity
            //------------------------------------------------------------------
            //if (highPdCnt > 0)
            if (changedCntLow == 0 && highPdCnt > 0) //nsmoon@211230
            {
                if (tryCnt > 0) {
                    if (prevAxisLedCurrentCtrlDir[scanSequenceIdx] == 1) {
                        scanSequenceIdx++;
                        continue;
                    }
                }
                if (minADCResult > (PD_LEVEL_LOW + lowTolerance)) { // if minimum pd level is over tolerance range
                    pdSaturationRatio = (highPdCnt * 100) / validPdCnt;
                    if (pdSaturationRatio > PD_LEVEL_SATURATION_RATIO) {
#if 1    //YJ@220915
                        ledDacMinIdx = Led_CurrentTbl_Adj_DAC(ledCurrentTblIdx[scanSequenceIdx], axisType);
#endif
                        if (ledDacIdx[scanSequenceIdx] > ledDacMinIdx) { // adjust led on time first
                            prevAxisLedCurrentCtrlDir[scanSequenceIdx] = -1;
                            changedCnt++;
                            changedCntHigh++;
                            ledDacIdx[scanSequenceIdx] -= LED_ON_DAC_UINT;
                            //TRACE_AGC_KANG("[%d/%d] DOWN TIME!! High PD COUNT %d led time %d \r\n",axisType,scanSequenceIdx, highPdCnt,ledDacIdx[scanSequenceIdx]);
                        }
                        else {
                            if (ledCurrentTblIdx[scanSequenceIdx] > 0) {
                                prevAxisLedCurrentCtrlDir[scanSequenceIdx] = -1;
                                changedCnt++;
                                changedCntHigh++;
                                ledCurrentTblIdx[scanSequenceIdx]--;
                                ledDacIdx[scanSequenceIdx] = ledDacMaxIdx;
                            }
                        }
                    }

                }
            } //if (highPdCnt > 0)
#if (CAL_HIGH_TO_LOW == TRUE)
            //------------------------------------------------------------------
            // need to increase light intensity
            //------------------------------------------------------------------
            //else if (lowPdCnt > 0) {
            if (changedCntHigh == 0 && lowPdCnt > 0) //kang@211013
            {
#if 1 //nsmoon@220330
                if (ledDacIdx[scanSequenceIdx] < ledDacMaxIdx) {
                    prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 1;
                    changedCnt++;
                    ledDacIdx[scanSequenceIdx]++;
                    //TRACE_AGC_KANG("[%d/%d] UP TIME!!! Low PD COUNT %d led time %d \r\n",axisType,scanSequenceIdx, lowPdCnt,ledDacIdx[scanSequenceIdx]);
                } else {
                    if (ledCurrentTblIdx[scanSequenceIdx] < ledCurrentMaxIdx) { // adjust led current first
                        prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 1;
                        changedCnt++;
                        ledCurrentTblIdx[scanSequenceIdx]++;
                        ledDacIdx[scanSequenceIdx] = ledDacMinIdx;
                    }
                }
#else
                if (ledDacIdx[scanSequenceIdx] < ledDacMaxIdx) {
                    prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 1;
                    ledDacIdx[scanSequenceIdx]++;
                    changedCnt++;
                    //TRACE_AGC_KANG("[%d/%d] UP TIME!!! Low PD COUNT %d led time %d \r\n",axisType,scanSequenceIdx, lowPdCnt,ledDacIdx[scanSequenceIdx]);
                } else {
                    if (ledCurrentTblIdx[scanSequenceIdx] < ledCurrentMaxIdx) { // adjust led current first
                        prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 1;
                        changedCnt++;
                        ledCurrentTblIdx[scanSequenceIdx]++;
                        if (ledCurrentTblIdx[scanSequenceIdx] > ledCurrentMaxIdx) {
                            ledCurrentTblIdx[scanSequenceIdx] = ledCurrentMaxIdx;
                        }
                    }
                    else {
                        //errorKind = LED_AGC_MAX_LIMIT_ERROR;
                    }
                }
#endif
            }
#endif
#else
            //------------------------------------------------------------------
            // need to increase light intensity
            //------------------------------------------------------------------
#if (CAL_HIGH_TO_LOW == FALSE)
            if (lowPdCnt > 0) {
                // ledCurrentTblIdx, ledDacIdx
                if (ledCurrentTblIdx[scanSequenceIdx] < ledCurrentMaxIdx) { // adjust led current first
                    prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 1;
                    changedCnt++;
                    ledCurrentTblIdx[scanSequenceIdx] += LED_CURRENT_CHANGE_STEP;
                    if (ledCurrentTblIdx[scanSequenceIdx] > ledCurrentMaxIdx) {
                        ledCurrentTblIdx[scanSequenceIdx] = ledCurrentMaxIdx;
                    }
                    // prevLedCurrentTblIdx[scanSequenceIdx] = ledCurrentTblIdx[scanSequenceIdx];
                    // printf("%3d Up %d\r\n", scanSequenceIdx, ledCurrentTblIdx[scanSequenceIdx]); // kjsxx
                } else { // adjust led on time
                    if (ledDacIdx[scanSequenceIdx] < (LED_ON_OFF_TIME_SIZE - 1) ) {
                        prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 1;
                        ledDacIdx[scanSequenceIdx]++;
                        changedCnt++;
                    } else {
                        //errorKind = LED_AGC_MAX_LIMIT_ERROR;
                    }
                    // printf("%3d UpT %d\r\n", scanSequenceIdx, ledDacIdx[scanSequenceIdx]); // kjsxx
                }
            }
            else
#endif
                //------------------------------------------------------------------
                // need to decrease light intensity
                //------------------------------------------------------------------
                if (highPdCnt > 0 && !lowPdCnt) {
                    if (tryCnt > 0) {
                        if (prevAxisLedCurrentCtrlDir[scanSequenceIdx] == 1 ||
                                prevAxisLedCurrentCtrlDir[scanSequenceIdx] == 0) {
                            minADCResult = 0; //invalidate
                        }
                    }
#if DEBUG_LED_CUR_TBL_IDX //for test //nsmoon@220302
                    if (axisType == axisTypeTest && tryCnt < 2) {
                        if (scanSequenceIdx == scanSequenceIdxTest)
                        {
                            printf("<%d/%d>", prevAxisLedCurrentCtrlDir[scanSequenceIdx], ledCurrentTblIdx[scanSequenceIdx]);
                        }
                    }
#endif
                    if ((int)minADCResult > (int)(PD_LEVEL_LOW + lowTolerance)) { // if minimum pd level is over tolerance range
                        pdSaturationRatio = (highPdCnt * 100) / validPdCnt;
                        if (ledDacIdx[scanSequenceIdx] > 0) { // adjust led on time first
                            if ( pdSaturationRatio > PD_LEVEL_SATURATION_RATIO) {
                                prevAxisLedCurrentCtrlDir[scanSequenceIdx] = -1;
                                changedCnt++;
                                ledDacIdx[scanSequenceIdx]--;
                            }
                        } else { // adjust led current
#if 0 // kjsxx 0
                            pdSaturationRatio = (highPdCnt * 100) / validPdCnt;
                            if ( highPdCnt == validPdCnt) { // if all PDs are over high level
                                changedCnt++;
                                if (ledCurrentTblIdx[scanSequenceIdx] > LED_CURRENT_CHANGE_STEP) {
                                    ledCurrentTblIdx[scanSequenceIdx] -= LED_CURRENT_CHANGE_STEP;
                                } else if (ledCurrentTblIdx[scanSequenceIdx] != 0) {
                                    ledCurrentTblIdx[scanSequenceIdx] = 0;
                                } else {
                                    errorKind = LED_AGC_MIN_LIMIT_ERROR;
                                }
                            } else if ( pdSaturationRatio > PD_LEVEL_SATURATION_RATIO) {
                                if (1) { // kjsxx : add condition
                                    changedCnt++;

                                    if (ledCurrentTblIdx[scanSequenceIdx] > LED_CURRENT_CHANGE_STEP) {
                                        ledCurrentTblIdx[scanSequenceIdx] -= LED_CURRENT_CHANGE_STEP;
                                    } else if (ledCurrentTblIdx[scanSequenceIdx] != 0) {
                                        ledCurrentTblIdx[scanSequenceIdx] = 0;
                                    } else {
                                        errorKind = LED_AGC_MIN_LIMIT_ERROR;
                                    }
                                }
                            }
#else
                            if ( pdSaturationRatio > PD_LEVEL_SATURATION_RATIO) {
//                            if ( (pdSaturationRatio == 100) ||
//                                 (ledCurrentTblIdx[scanSequenceIdx] != prevLedCurrentTblIdx[scanSequenceIdx]) )
                                {
                                    if (ledCurrentTblIdx[scanSequenceIdx] > LED_CURRENT_CHANGE_STEP) {
                                        prevAxisLedCurrentCtrlDir[scanSequenceIdx] = -1;
                                        changedCnt++;
                                        ledCurrentTblIdx[scanSequenceIdx] = ledCurrentTblIdx[scanSequenceIdx] - LED_CURRENT_CHANGE_STEP;
                                    } else if (ledCurrentTblIdx[scanSequenceIdx] != 0) {
                                        prevAxisLedCurrentCtrlDir[scanSequenceIdx] = -1;
                                        changedCnt++;
                                        ledCurrentTblIdx[scanSequenceIdx] = 0;
                                    } else {
                                        //errorKind = LED_AGC_MIN_LIMIT_ERROR;
                                    }
                                    // printf("%3d Dn %d\r\n", scanSequenceIdx, ledCurrentTblIdx[scanSequenceIdx]); // kjsxx
                                }
                            }
#endif
                        }
                    }
                }
#if (CAL_HIGH_TO_LOW)
#error CAL_HIGH_TO_LOW
                else if (lowPdCnt > 0) {
                    // ledCurrentTblIdx, ledDacIdx
                    if (ledCurrentTblIdx[scanSequenceIdx] < ledCurrentMaxIdx) { // adjust led current first
                        prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 1;
                        changedCnt++;
                        ledCurrentTblIdx[scanSequenceIdx] += LED_CURRENT_CHANGE_STEP;
                        if (ledCurrentTblIdx[scanSequenceIdx] > ledCurrentMaxIdx) {
                            ledCurrentTblIdx[scanSequenceIdx] = ledCurrentMaxIdx;
                        }
                        // prevLedCurrentTblIdx[scanSequenceIdx] = ledCurrentTblIdx[scanSequenceIdx];
                        // printf("%3d Up %d\r\n", scanSequenceIdx, ledCurrentTblIdx[scanSequenceIdx]); // kjsxx
                    } else { // adjust led on time
                        if (ledDacIdx[scanSequenceIdx] < (LED_ON_OFF_TIME_SIZE - 1) ) {
                            prevAxisLedCurrentCtrlDir[scanSequenceIdx] = 1;
                            ledDacIdx[scanSequenceIdx]++;
                            changedCnt++;
                        } else {
                            //errorKind = LED_AGC_MAX_LIMIT_ERROR;
                        }
                        // printf("%3d UpT %d\r\n", scanSequenceIdx, ledDacIdx[scanSequenceIdx]); // kjsxx
                    }
                }
#endif
            //prevLedCurrentTblIdx[scanSequenceIdx] = curCurrentIdx;
#endif
        }
#if DEBUG_LED_CUR_TBL_IDX //for test //nsmoon@220302
        if (axisType == axisTypeTest && tryCnt < 2) {
            if (scanSequenceIdx == scanSequenceIdxTest)
            {
                printf("[%d/%d]", prevAxisLedCurrentCtrlDir[scanSequenceIdx], ledCurrentTblIdx[scanSequenceIdx]);
            }
        }
#endif
        scanSequenceIdx++;
    } // while ()

#if 0
    if (axisType == X_AXIS) {
        TRACE_AGC("AGC @ X-Axis : ");
    } else {
        TRACE_AGC("AGC @ Y-Axis : ");
    }
    TRACE_AGC("changedCnt = %d, errorKind = %d\r\n", changedCnt, errorKind);
#endif

#if DEBUG_LED_CUR_TBL_IDX //for test //nsmoon@220302
    if (axisType == axisTypeTest) {
        printf("{%d/%d}\r\n", ledCurrentTblIdx[scanSequenceIdxTest], scanSequenceIdxTest);
    }
#endif

    return changedCnt;
}

//------------------------------------------------------------------------------
int16_t LED_Auto_Gain_ControlWithTryCnt(uint8_t tryCnt)
{
    int16_t xChangedSeqCnt, yChangedSeqCnt, totalChangedSeqCnt;
    yChangedSeqCnt = controlLEDLightIntensityWithTryCnt(Y_AXIS, tryCnt);
    xChangedSeqCnt = controlLEDLightIntensityWithTryCnt(X_AXIS, tryCnt);

    totalChangedSeqCnt = xChangedSeqCnt + yChangedSeqCnt;
    return totalChangedSeqCnt;
}

void setThresholdDataAxis(uint8_t axisType, uint8_t thresholdMode)
{
    int16_t pdIdx, totalCellSize;
    int16_t offset, maxOffset, ledIdx;
    int16_t cornerCellSize;
    int16_t thresholdDropPercent, centerThresholdDropPercent, cornerThresholdDropPercent;
#if (THRESHOLD_ZERO_COUNT_ENABLE) //nsmoon@211231
    uint8_t thresholdZeroCnt = 0;
#endif
    uint8_t offsetIdx;

    if (axisType == X_AXIS) {
        //--------------------------------------------------------------------------
        // X-scan threshold level setting
        //--------------------------------------------------------------------------
        totalCellSize = X_CELL_SIZE;
        maxOffset = X_MAX_OFFSET;
        cornerCellSize = X_CORNER_CELL_SIZE;

        if(thresholdMode == MAIN_THRESHOLD_MODE) {
            centerThresholdDropPercent = X_THRESHOLD_DROP_PERCENT;
            cornerThresholdDropPercent = X_CORNER_THRESHOLD_DROP_PERCENT;
        } else {
            if (scanStatus.requestSubThreshold == FALSE) return;

            centerThresholdDropPercent = subThresholdDropPercent;
            cornerThresholdDropPercent = subThresholdDropPercent;

            if ( cornerThresholdDropPercent >= X_CORNER_THRESHOLD_DROP_PERCENT) {
                cornerThresholdDropPercent -= X_CORNER_THRESHOLD_DROP_PERCENT;
            }
        }

        for (pdIdx = 0; pdIdx < totalCellSize; pdIdx++) {
            for (offset = - maxOffset; offset <= maxOffset; offset++)
            {
                ledIdx = offset + pdIdx;
                offsetIdx = offset + maxOffset;
                if ( ledIdx >= 0 && ledIdx < totalCellSize ) { // check if ledIdx is in the range
                    if ( isInSideArea(pdIdx, ledIdx, totalCellSize, cornerCellSize)) {
                        thresholdDropPercent = cornerThresholdDropPercent;
                    } else {
                        thresholdDropPercent = centerThresholdDropPercent;
                    }

                    if (xCellStatus[pdIdx].isPdDead || xCellStatus[ledIdx].isLedDead) {
                        xScanThresholdData[pdIdx][offsetIdx] = 0;
                        xScanSubThresholdData[pdIdx][offsetIdx] = 0;
                        continue;
                    }
                    if (xScanResultData[pdIdx][offsetIdx] < MIN_THRESHOLD_LEVEL) {
#if (THRESHOLD_ZERO_COUNT_ENABLE) //nsmoon@211230
                        if (thresholdMode == MAIN_THRESHOLD_MODE && thresholdZeroCnt < 0xFF) {
                            thresholdZeroCnt++;
                        }
#endif
                        //continue; //for test
                    }
#if(THRESHOLD_DROP_PERCENT_ENABLE == TRUE) //nsmoon@220330
                    else if (xScanResultData[pdIdx][offsetIdx] > PD_LEVEL_HIGH) {
                        if (thresholdMode == MAIN_THRESHOLD_MODE) {
                            thresholdDropPercent = THRESHOLD_DROP_PERCENT_SATURATION;
                        }
                    }
#endif

                    if(thresholdMode == MAIN_THRESHOLD_MODE) {
                        xScanThresholdData[pdIdx][offsetIdx] =
                            (uint8_t)(xScanResultData[pdIdx][offsetIdx] * (100 - thresholdDropPercent) / 100);
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 0) //nsmoon@211213
                        print_scan_threshold("X[[", X_AXIS, ledIdx, pdIdx);
#endif
                    } else {
                        xScanSubThresholdData[pdIdx][offsetIdx] =
                            (uint8_t)(xScanResultData[pdIdx][offsetIdx] * (100 - thresholdDropPercent) / 100);
                    }
                }
            }
        }
    }
    else {
        //--------------------------------------------------------------------------
        // Y-scan threshold level setting
        //--------------------------------------------------------------------------
        totalCellSize = Y_CELL_SIZE;
        maxOffset = Y_MAX_OFFSET;
        cornerCellSize = Y_CORNER_CELL_SIZE;
        if(thresholdMode == MAIN_THRESHOLD_MODE) {
            centerThresholdDropPercent = Y_THRESHOLD_DROP_PERCENT;
            cornerThresholdDropPercent = Y_CORNER_THRESHOLD_DROP_PERCENT;
        } else {
            centerThresholdDropPercent = subThresholdDropPercent;
            cornerThresholdDropPercent = subThresholdDropPercent;

            if ( cornerThresholdDropPercent >= Y_CORNER_THRESHOLD_DROP_PERCENT) {
                cornerThresholdDropPercent -= Y_CORNER_THRESHOLD_DROP_PERCENT;
            }
        }

        for (pdIdx = 0; pdIdx < totalCellSize; pdIdx++) {
            for (offset = - maxOffset; offset <= maxOffset; offset++)
            {
                ledIdx = offset + pdIdx;
                offsetIdx = offset + maxOffset;
                if ( ledIdx >= 0 && ledIdx < totalCellSize ) { // check if ledIdx is in the range
                    if ( isInSideArea(pdIdx, ledIdx, totalCellSize, cornerCellSize)) {
                        thresholdDropPercent = cornerThresholdDropPercent;
                    } else {
                        thresholdDropPercent = centerThresholdDropPercent;
                    }

                    if (yCellStatus[pdIdx].isPdDead || yCellStatus[ledIdx].isLedDead) {
                        yScanThresholdData[pdIdx][offsetIdx] = 0;
                        yScanSubThresholdData[pdIdx][offsetIdx] = 0;
                        continue;
                    }
                    if (yScanResultData[pdIdx][offsetIdx] < MIN_THRESHOLD_LEVEL) {
#if (THRESHOLD_ZERO_COUNT_ENABLE) //nsmoon@211230
                        if (thresholdMode == MAIN_THRESHOLD_MODE && thresholdZeroCnt < 0xFF) {
                            thresholdZeroCnt++;
                        }
#endif
                        //continue; //for test
                    }
#if(THRESHOLD_DROP_PERCENT_ENABLE == TRUE) //nsmoon@220330
                    else if (yScanResultData[pdIdx][offsetIdx] > PD_LEVEL_HIGH) {
                        if (thresholdMode == MAIN_THRESHOLD_MODE) {
                            thresholdDropPercent = THRESHOLD_DROP_PERCENT_SATURATION;
                        }
                    }
#endif

                    if(thresholdMode == MAIN_THRESHOLD_MODE) {
                        yScanThresholdData[pdIdx][offsetIdx] =
                            (uint8_t)(yScanResultData[pdIdx][offsetIdx] * (100 - thresholdDropPercent) / 100);
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 0) //nsmoon@211213
                        print_scan_threshold("Y[[", Y_AXIS, ledIdx, pdIdx);
#endif
                    } else {
                        yScanSubThresholdData[pdIdx][offsetIdx] =
                            (uint8_t)(yScanResultData[pdIdx][offsetIdx] * (100 - thresholdDropPercent) / 100);
                    }
                }
            }
        }
    }
#if FORCE_AGC_SET
#if (THRESHOLD_ZERO_COUNT_ENABLE) //nsmoon@211231
#define THRESHOLD_ZERO_COUNT_MAX    100
    if (thresholdZeroCnt > THRESHOLD_ZERO_COUNT_MAX) {
        TRACE_UART("thresholdZeroCnt=%d\r\n", thresholdZeroCnt);
        g_forcedagc.forced_saved = 1;
    }
#endif
#endif
}


//------------------------------------------------------------------------------
// Global Input :
//   xBlockedLineCnt, xSubBlockedLineCnt, yBlockedLineCnt, ySubBlockedLineCnt
// kjs 210315 change return value void to int16_t

int16_t Calibration_Process(void)
{
    uint32_t blockedLineCnt, gainChangedSeqCnt, lowNonBlockLevelCnt;
    uint8_t thresholdAvgCnt;
    int16_t ledAGCTryCnt;
    uint16_t totalGainChanged; // kjs 210315 add
    uint8_t agc_cnt, agc_cnt_max; //YJ@210719
    int16_t yChangedSeqCnt, xChangedSeqCnt;

    if (scanStatus.isUsualAgcCal == FALSE) return FALSE; // kjs 210315 add

    totalGainChanged = 0; // kjs 210315 add
    blockedLineCnt = xBlockedLineCnt + yBlockedLineCnt;

    if (scanStatus.isExtLightDelayChanged == TRUE) {
        //if ( (xSubBlockedLineCnt + ySubBlockedLineCnt) == 0 )
        if ( (xSubBlockedLineCnt + ySubBlockedLineCnt) < EXT_LIGHT_LED_AGC_ENTRY_LINE_CNT  )
        {
            TRACE_CAL_PROC("Calibration_Process() : AGC Gain Start by Ext Light\r\n");
            scanStatus.isExtLightDelayChanged = FALSE;

            for (ledAGCTryCnt = 0; ledAGCTryCnt < EXT_LIGHT_LED_AGC_TRY_CNT; ledAGCTryCnt++) {

                gainChangedSeqCnt = LED_Auto_Gain_ControlWithTryCnt(ledAGCTryCnt); //LED_Auto_Gain_Control();

                totalGainChanged += gainChangedSeqCnt; // kjs 210315 add

                //if (gainChangedSeqCnt == 0) break;

                //scanAxis(Y_AXIS, TRUE, 0, Y_CELL_SIZE-1, 0, Y_CELL_SIZE-1);
                //scanAxis(X_AXIS, TRUE, 0, X_CELL_SIZE-1, 0, X_CELL_SIZE-1);
                scanAxisFull(Y_AXIS, TRUE);
                scanAxisFull(X_AXIS, TRUE);
                //CORETIMER_DelayMs(1); // kjs 210315 add //YJ@220915
            }
            //if (ledAGCTryCnt > 0)
            if (totalGainChanged > 0) // kjs 210315
            {
#if 1 //nsmoon@211216 move to below
                for (thresholdAvgCnt = 1; thresholdAvgCnt <= RUN_THRESHOLD_AVG_CNT; thresholdAvgCnt++) {
                    //scanAxis(Y_AXIS, TRUE, 0, Y_CELL_SIZE-1, 0, Y_CELL_SIZE-1);
                    //scanAxis(X_AXIS, TRUE, 0, X_CELL_SIZE-1, 0, X_CELL_SIZE-1);
                    scanAxisFull(Y_AXIS, TRUE);
                    scanAxisFull(X_AXIS, TRUE);
                    //CORETIMER_DelayMs(1); // kjs 210315 add   //YJ@220915

                    lowNonBlockLevelCnt = averageNonBlockLevel(thresholdAvgCnt);

                    if (lowNonBlockLevelCnt > 0) {
                        break;
                    }
                }

                setThresholdDataByAvgNonBlockLevel(MAIN_THRESHOLD_MODE);
                if (scanStatus.requestSubThreshold) {
                    setThresholdDataByAvgNonBlockLevel(SUB_THRESHOLD_MODE);
                }
#endif
                noTouchObjectCnt = 0;

                TRACE_CAL_PROC("Calibration_Process() : AGC Gain Changed by Ext Light\r\n");
            }
        } else {
            TRACE_CAL_PROC("Calibration_Process() : Ext Light But No AGC, xSubBlockedLineCnt = %d, ySubBlockedLineCnt = %d\r\n",
                           xSubBlockedLineCnt, ySubBlockedLineCnt);
        }
    } else {
#if (FORCE_AGC_SET == TRUE)     //YJ@210719
        if(g_forcedagc.forced_agc)
        {
        	g_forcedagc.forced_saved = 1; //kang@211029
        }

        if (blockedLineCnt < NO_TOUCH_OBJECT_LINE_CNT || g_forcedagc.forced_saved > 0)
        {
            if (isAgcControlTimerExpired() || g_forcedagc.forced_saved > 0) //nsmoon@220207A
            {
            	if (g_forcedagc.forced_saved > 0) {
                    //TRACE_UART("***forced_saved***\r\n");
                	TRACE_ERROR("***forced_saved***\r\n");
                    g_forcedagc.forced_agc = g_forcedagc.forced_saved = 0; //reset
                    agc_cnt_max = FORCED_THRESHOLD_AVG_CNT;
#if (ENABLE_UART_CMD_PROCESS == DEBUG_UART_MOME)
                    showCnt_test = 5; //nsmoon@211213
#endif
                }
                else {
                    agc_cnt_max = 2;
#if (ENABLE_UART_CMD_PROCESS == DEBUG_UART_MOME)
                    showCnt_test = 1; //nsmoon@211213
#endif
                }
#else
        if (blockedLineCnt < NO_TOUCH_OBJECT_LINE_CNT)
        {
            if (isAgcControlTimerExpired())
            {
                agc_cnt_max = 2;
#if (ENABLE_UART_CMD_PROCESS == DEBUG_UART_MOME)
                showCnt_test = 1; //nsmoon@211213
#endif
#endif
                TRACE_CAL_PROC("Calibration_Process() : AGC Started\r\n");
                for (agc_cnt = 0 ; agc_cnt < agc_cnt_max; agc_cnt++) {
                    yChangedSeqCnt = controlLEDLightIntensityWithTryCnt(Y_AXIS, agc_cnt);
                    xChangedSeqCnt = controlLEDLightIntensityWithTryCnt(X_AXIS, agc_cnt);
#if 0 //(ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 1)
                    print_scan_threshold("Y^^", Y_AXIS, ledIdx_test, pdIdx_test);
                    print_scan_threshold("X^^", X_AXIS, ledIdx_test, pdIdx_test);
#endif
                    if (!yChangedSeqCnt && !xChangedSeqCnt) {
                        break;
                    }
                    totalGainChanged += (yChangedSeqCnt + xChangedSeqCnt); // kjs 210315 add

                    scanAxisFull(Y_AXIS, TRUE);
                    scanAxisFull(X_AXIS, TRUE);

                    //  CORETIMER_DelayMs(3);  // kjs 210315        1    //YJ@220317

                    if (yChangedSeqCnt) {
                        setThresholdDataAxis(Y_AXIS, MAIN_THRESHOLD_MODE);
                        if (scanStatus.requestSubThreshold) {
                            setThresholdDataAxis(Y_AXIS, SUB_THRESHOLD_MODE);
                        }
                    }
                    if (xChangedSeqCnt) {
                        setThresholdDataAxis(X_AXIS, MAIN_THRESHOLD_MODE);
                        if (scanStatus.requestSubThreshold) {
                            setThresholdDataAxis(X_AXIS, SUB_THRESHOLD_MODE);
                        }
                    }
                } //for (agc_cnt = 0 ; agc_cnt < agc_cnt_max; agc_cnt++)
#if (ENABLE_UART_CMD_PROCESS && ENABLE_CAL_DEBUG_PRINT > 1)
                if (totalGainChanged) {
                    TRACE_CAL_PROC("Calibration_Process() : AGC Gain Changed\r\n");
                }
#endif
#if 0 // 0 for time chk, 1 for counter chk
                noTouchObjectCnt = 0;
#else
                clearAgcControlTimer();
#endif
            }
        }
        else {
#if 0 // 0 for time chk, 1 for counter chk
            noTouchObjectCnt = 0;
#else
            clearAgcControlTimer();
#endif
        }
    }

    // 210315 add if
    if (totalGainChanged > 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

uint16_t decideDeadCell(void)
{
    uint8_t maxOffset;
    int16_t pdIdx, ledIdx, offset, i;
    uint8_t pdIdxStart, pdIdxEnd;
    uint8_t ledIdxStart, ledIdxEnd;
    uint16_t totalDeadCellCnt;

    // common use for X-Axis and Y-Axis
    uint8_t deadLineCntForPd[X_CELL_SIZE];
    uint8_t deadLineCntForLed[X_CELL_SIZE];
    uint8_t validLineCnt[X_CELL_SIZE];
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // clear status and variable
    //--------------------------------------------------------------------------
    // clear x cell status
    for(i = 0; i < X_CELL_SIZE; i++)
    {
        xCellStatus[i].isPdDead = FALSE;
        xCellStatus[i].isLedDead = FALSE;
    }

    // clear y cell status
    for(i = 0; i < Y_CELL_SIZE; i++)
    {
        yCellStatus[i].isPdDead = FALSE;
        yCellStatus[i].isLedDead = FALSE;
    }

    xPdDeadCellCnt = 0;
    xLedDeadCellCnt = 0;
    yPdDeadCellCnt = 0;
    yLedDeadCellCnt = 0;
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // X-Axis
    //--------------------------------------------------------------------------
    pdIdxStart = 0;
    pdIdxEnd = X_CELL_SIZE - 1;
    ledIdxStart = 0;
    ledIdxEnd = X_CELL_SIZE - 1;
    maxOffset = X_MAX_OFFSET;
    memset(deadLineCntForPd, 0, sizeof(deadLineCntForPd));
    memset(deadLineCntForLed, 0, sizeof(deadLineCntForLed));
    memset(validLineCnt, 0, sizeof(validLineCnt));

    // find dead cells
    for (pdIdx = pdIdxStart; pdIdx <= pdIdxEnd; pdIdx++)
    {
        for (offset = - maxOffset; offset <= maxOffset; offset++)
        {
            ledIdx = offset + pdIdx;
            if ( ledIdx >= ledIdxStart && ledIdx <= ledIdxEnd ) // check if ledIdx is in the range
            {
                validLineCnt[pdIdx]++;

                if (xScanResultData[pdIdx][offset + maxOffset] < PD_DEAD_CELL_LEVEL) {
                    deadLineCntForPd[pdIdx]++;
                    deadLineCntForLed[ledIdx]++;
                }
            }
        }
    }

    // decide dead cell
    for (pdIdx = pdIdxStart; pdIdx <= pdIdxEnd; pdIdx++)
    {
        if ( ((deadLineCntForPd[pdIdx] * 100) / validLineCnt[pdIdx]) >= DEAD_CELL_RATIO ) {
            xCellStatus[pdIdx].isPdDead = TRUE;
            xPdDeadCellCnt++;
        }

        if ( ((deadLineCntForLed[pdIdx] * 100) / validLineCnt[pdIdx]) >= DEAD_CELL_RATIO ) {
            xCellStatus[pdIdx].isLedDead = TRUE;
            xLedDeadCellCnt++;
        }
    }


    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Y-Axis
    //--------------------------------------------------------------------------
    pdIdxStart = 0;
    pdIdxEnd = Y_CELL_SIZE - 1;
    ledIdxStart = 0;
    ledIdxEnd = Y_CELL_SIZE - 1;
    maxOffset = Y_MAX_OFFSET;

    memset(deadLineCntForPd, 0, sizeof(deadLineCntForPd));
    memset(deadLineCntForLed, 0, sizeof(deadLineCntForLed));
    memset(validLineCnt, 0, sizeof(validLineCnt));

    // find dead cells
    for (pdIdx = pdIdxStart; pdIdx <= pdIdxEnd; pdIdx++)
    {
        for (offset = - maxOffset; offset <= maxOffset; offset++)
        {
            ledIdx = offset + pdIdx;
            if ( ledIdx >= ledIdxStart && ledIdx <= ledIdxEnd ) // check if ledIdx is in the range
            {
                validLineCnt[pdIdx]++;

                if (yScanResultData[pdIdx][offset + maxOffset] < PD_DEAD_CELL_LEVEL) {
                    deadLineCntForPd[pdIdx]++;
                    deadLineCntForLed[ledIdx]++;
                }
            }
        }
    }

    // decide dead cell
    for (pdIdx = pdIdxStart; pdIdx <= pdIdxEnd; pdIdx++)
    {
        if ( ((deadLineCntForPd[pdIdx] * 100) / validLineCnt[pdIdx]) >= DEAD_CELL_RATIO ) {
            yCellStatus[pdIdx].isPdDead = TRUE;
            yPdDeadCellCnt++;
        }

        if ( ((deadLineCntForLed[pdIdx] * 100) / validLineCnt[pdIdx]) >= DEAD_CELL_RATIO ) {
            yCellStatus[pdIdx].isLedDead = TRUE;
            yLedDeadCellCnt++;
        }
    }
    //--------------------------------------------------------------------------
#if (DEAD_CELL_FIXED ==  TRUE)
    yLedDeadCellCnt = 2;
    yPdDeadCellCnt = 2;
    xLedDeadCellCnt = 2;
    xPdDeadCellCnt = 2;
    yCellStatus[0].isLedDead = TRUE;
    yCellStatus[0].isPdDead = TRUE;
    yCellStatus[Y_CELL_SIZE - 1].isLedDead = TRUE;
    yCellStatus[Y_CELL_SIZE - 1].isPdDead = TRUE;
    xCellStatus[0].isLedDead = TRUE;
    xCellStatus[0].isPdDead = TRUE;
    xCellStatus[X_CELL_SIZE - 1].isLedDead = TRUE;
    xCellStatus[X_CELL_SIZE - 1].isPdDead = TRUE;
#endif
    totalDeadCellCnt = xPdDeadCellCnt + xLedDeadCellCnt + yPdDeadCellCnt + yLedDeadCellCnt;

    if (totalDeadCellCnt > 0) {
        TRACE_DEAD_CELL("\n\r");
        TRACE_DEAD_CELL("xPdDeadCellCnt = %d, xLedDeadCellCnt = %d\r\n", xPdDeadCellCnt, xLedDeadCellCnt);
        TRACE_DEAD_CELL("yPdDeadCellCnt = %d, yLedDeadCellCnt = %d\r\n", yPdDeadCellCnt, yLedDeadCellCnt);
    }

    return totalDeadCellCnt;
}

//------------------------------------------------------------------------------

void scanYAxisAuto(void)
{
    uint8_t pdIdxStart, pdIdxEnd;
    uint8_t ledIdxStart, ledIdxEnd;

    if (scanStatus.isPartialScan) {
        pdIdxStart = yPartialScanData.pdIdxStart;
        pdIdxEnd = yPartialScanData.pdIdxEnd;
        ledIdxStart = yPartialScanData.ledIdxStart;
        ledIdxEnd = yPartialScanData.ledIdxEnd;
        scanAxis(Y_AXIS, TRUE, pdIdxStart, pdIdxEnd, ledIdxStart, ledIdxEnd);
    } else {
        scanAxisFull(Y_AXIS, TRUE);
    }
}

void scanXAxisAuto(void)
{
    uint8_t pdIdxStart, pdIdxEnd;
    uint8_t ledIdxStart, ledIdxEnd;

    if (scanStatus.isPartialScan) {
        pdIdxStart = xPartialScanData.pdIdxStart;
        pdIdxEnd = xPartialScanData.pdIdxEnd;
        ledIdxStart = xPartialScanData.ledIdxStart;
        ledIdxEnd = xPartialScanData.ledIdxEnd;
        scanAxis(X_AXIS, TRUE, pdIdxStart, pdIdxEnd, ledIdxStart, ledIdxEnd);
    } else {
        scanAxisFull(X_AXIS, TRUE);
    }
}

//------------------------------------------------------------------------------

static int16_t agcTryCnt;
static int16_t thresholdAvgCnt;
uint8_t scanTaskPowerOnInitCnt;
uint32_t totalBlockedLineCnt;

SCAN_STATUS_T Scan_Process(void)
{
    int16_t gainChangedSeqCnt, lowNonBlockLevelCnt;
    SCAN_STATUS_T scanStatus= SCAN_NOT_INITIALIZED;

    switch(scanTaskData.state)
    {
    case SCAN_STATE_INIT:
    {

        scanTaskPowerOnInitCnt++; // kjs 210414 add
        initializeScanHW();
        Scan_Initialize();
        copySequenceTbl();
        // dltInit();
        TRACE_SCAN_TASK("SCAN_STATE_INIT..\r\n");
        // setScanStateXferTimerMsec(100); // 100msec time-out setting
#if POWER_ON_CAL_PD_SHIFT == TRUE
        calPdShiftOKCnt = 0;
        calPdShiftTryCnt = 0;
#endif
        scanStatus = SCAN_NOT_INITIALIZED;
        scanTaskData.state = SCAN_STATE_POWER_ON_CAL_PD_SHIFT;
        // gPacketManage.gSendPacket = 0;
    }
    break;

    case SCAN_STATE_POWER_ON_CAL_PD_SHIFT:
    {
        TRACE_SCAN_TASK("SCAN_STATE_POWER_ON_CAL_PD_SHIFT..\r\n");
#if 0 //USE_TEST_BOARD == TRUE // for using test b'd
        scanTaskData.state = SCAN_STATE_MAIN_TASK;
        break;
#endif

#if POWER_ON_CAL_PD_SHIFT == TRUE //
        pdShiftChangedCnt = calibratePdShiftNoise(0);
        CORETIMER_DelayMs(1);

        if (pdShiftChangedCnt == 0) {
            calPdShiftOKCnt++;
        } else {
            calPdShiftOKCnt = 0;
        }

        if (calPdShiftOKCnt >= CAL_PD_SHIFT_OK_CNT || ++calPdShiftTryCnt >=  CAL_PD_SHIFT_MAX_TRY_CNT) {
            agcTryCnt = 0;
            thresholdAvgCnt = 0;
            scanTaskData.state = SCAN_STATE_POWER_ON_EXT_LIGHT_NOISE_PROC;
        }
#else
        scanTaskData.state = SCAN_STATE_POWER_ON_EXT_LIGHT_NOISE_PROC;
#endif
    }
    break;

    case SCAN_STATE_POWER_ON_EXT_LIGHT_NOISE_PROC:
        TRACE_SCAN_TASK("SCAN_STATE_POWER_ON_EXT_LIGHT_NOISE_PROC..\r\n");
#if POWER_ON_EXT_LIGHT_NOISE_CONTROL == TRUE
        compensateExtLightNoise(Y_AXIS);
        compensateExtLightNoise(X_AXIS);
        selectPDGroupExt(-1); // disable all PD CS
#endif

        agcTryCnt = 0;
        thresholdAvgCnt = 0;
        scanTaskData.state = SCAN_STATE_POWER_ON_AGC;

        break;

    case SCAN_STATE_POWER_ON_AGC: // LED AGC Process
    {
        TRACE_SCAN_TASK("SCAN_STATE_POWER_ON_AGC..\r\n");
#if (POWER_ON_LED_AGC_CONTROL == FALSE)
        scanTaskData.state = SCAN_STATE_SET_THRESHOLD_LEVEL;
        break;
#endif


        TRACE_SCAN_TASK("SCAN_STATE_POWER_ON_AGC..SCAN START\r\n");
        scanAxisFull(Y_AXIS, TRUE);
        scanAxisFull(X_AXIS, TRUE);
        TRACE_SCAN_TASK("SCAN_STATE_POWER_ON_AGC..SCAN OK\r\n");

#if ENABLE_DEAD_CELL_CHK == TRUE
        decideDeadCell();
#endif
#if 0//def WITH_GD_USB		//YJ@220906
        scanTaskData.state = SCAN_STATE_SET_THRESHOLD_LEVEL;
#else
#if AGC_TOGGLE_PREVENTION_ENABLE
        gainChangedSeqCnt = LED_Auto_Gain_ControlWithTryCnt(agcTryCnt);
#else
        gainChangedSeqCnt = LED_Auto_Gain_Control();
#endif
        agcTryCnt++;

        if (gainChangedSeqCnt == 0) {
#if ENABLE_DISPLAY_AGC_RESULT == TRUE
            printf("LED AGC Completed @ cnt = %d\r\n", agcTryCnt);
            displayLedAgcResult();
#endif

            scanTaskData.state = SCAN_STATE_SET_THRESHOLD_LEVEL;
        }

        if (agcTryCnt > LED_AGC_POWER_ON_MAX_TRY_CNT) {
#if ENABLE_DISPLAY_AGC_RESULT == TRUE
            printf("LED AGC Max Try Cnt !!!, remained sequence cnt = %d\r\n", gainChangedSeqCnt);
            displayLedAgcResult();
#endif

            scanTaskData.state = SCAN_STATE_SET_THRESHOLD_LEVEL;
        }
#endif
    }
    break;

    case SCAN_STATE_SET_THRESHOLD_LEVEL:
    {
        TRACE_SCAN_TASK("SCAN_STATE_SET_THRESHOLD_LEVEL..\r\n");
        scanAxisFull(Y_AXIS, TRUE);
        scanAxisFull(X_AXIS, TRUE);

#if ENABLE_DEAD_CELL_CHK == TRUE
        decideDeadCell();
#endif

#if 0 // kjs 0 to use average non-block level
        updateThresholdData(MAIN_THRESHOLD_MODE);
        scanTaskData.state = SCAN_STATE_MAIN_TASK;
#else
        lowNonBlockLevelCnt = averageNonBlockLevel(++thresholdAvgCnt);

        // kjs 210414 add
        if (scanTaskPowerOnInitCnt >= SCAN_TASK_POWER_ON_INIT_FORCED_EXIT_CNT) {
            if (lowNonBlockLevelCnt > 0) lowNonBlockLevelCnt = 0;
        }

        if (lowNonBlockLevelCnt > 0) {
            scanTaskData.state = SCAN_STATE_INIT;
            TRACE_SCAN_TASK("\r\nScan_Task() : Retry SCAN_STATE_INIT, lowNonBlockLevelCnt = %d\r\n", lowNonBlockLevelCnt);
        } else {
            if (thresholdAvgCnt >= THRESHOLD_AVG_CNT) {
                setThresholdDataByAvgNonBlockLevel(MAIN_THRESHOLD_MODE);
                setThresholdDataByAvgNonBlockLevel(SUB_THRESHOLD_MODE); //OSY BACKEND 로부터 받아서 설정 했어야 하나, N65는 이미 SUB THRETHOLD를 알고 있기에 현 위치에 적용
                scanTaskData.state = SCAN_STATE_CHK_NO_OBJECT;
            }
        }
#endif
    }
    break;

    case SCAN_STATE_CHK_NO_OBJECT:
    {
        TRACE_SCAN_TASK("SCAN_STATE_CHK_NO_OBJECT..\r\n");

        scanAxisFull(Y_AXIS, TRUE);
        scanAxisFull(X_AXIS, TRUE);

        totalBlockedLineCnt = Scan_Data_Process();

        // kjs 210414 add
        if (scanTaskPowerOnInitCnt >= SCAN_TASK_POWER_ON_INIT_FORCED_EXIT_CNT) {
            if (totalBlockedLineCnt > 0) totalBlockedLineCnt = 0;
        }

        if (totalBlockedLineCnt == 0) { // if not exist any touch objects
            scanTaskData.state = SCAN_STATE_MAIN_TASK;

#if ENABLE_DISPLAY_DEAD_CELL == TRUE
            displayDeadCellInormation();
#endif

#if USUAL_EXT_LIGHT_NOISE_CONTROL == TRUE
            clearProcessExtLightTimer(); // kjs 210310
#endif
        } else { // if exist touch objects
            scanTaskData.state = SCAN_STATE_INIT;
            TRACE_SCAN_TASK("\r\nScan_Task() : Retry SCAN_STATE_INIT, Touch exist\r\n");
        }
    }
    break;

    case SCAN_STATE_MAIN_TASK:
    {
        TRACE_SCAN_TASK("SCAN_STATE_MAIN_TASK..\r\n");
#if USUAL_EXT_LIGHT_NOISE_CONTROL == TRUE
        processExtLightNoise();
#endif

#if (USUAL_LED_AGC_CONTROL) //nsmoon@220420
        Calibration_Process();
#endif
#if ENABLE_SCAN_HIGH_GAIN_DLT //nsmoon@220420
        if (scanGainHighDetected) {
            scanGainHighDetected = 0; //reset
            receivedDataBuffer[1] = 0; //reset
            scanGainHigh = 1;
            g_forcedagc.forced_agc = 1;
            //printf("scanGainHigh*******\r\n");
            break;
        }
#endif
        //DEBUG_TP2_HIGH();
        scanAxisFull(Y_AXIS, TRUE);
        //DEBUG_TP2_LOW();
        scanAxisFull(X_AXIS, TRUE);
#if 0
        ////////////////////////////////////
        // scan routine
        ////////////////////////////////////
#if (ENABLE_UART_CMD_PROCESS == FALSE && ENABLE_TRIGER)
        //M_Disable_All_Pd(); //FIXME!! for test
//        M_DebugTP1_Low();
#endif
        scanYAxisAuto();
#if (ENABLE_UART_CMD_PROCESS == FALSE && ENABLE_TRIGER)
//        M_DebugTP1_High();
#endif
        //requestUSBRead();
        scanXAxisAuto();
#if (ENABLE_UART_CMD_PROCESS == FALSE && ENABLE_TRIGER)
        //M_DebugTP2_Low();
        //M_DebugTP2_Low();
#endif
        ////////////////////////////////////
#endif

#if (SIDE_DATA_FIXED == TRUE)
        Del_Side_data(0); //YJ@210716
#endif
#if (DEBUG_USB_OFF == FALSE)
        if (appData.epDataWritePending == true) break; // kjs 210315 add if
#endif
        totalBlockedLineCnt = Scan_Data_Process();
        //nObject_Status = Scan_Data_Process();
        scanStatus = SCAN_INIT_COMPLETE;
    }
    break;

    case SCAN_STATE_ERROR:
        printf("ERROR! : SCAN_STATE_ERROR \r\n");
        break;

    default:
        break;
    }
    return scanStatus;
}


//------------------------------------------------------------------------------
#ifdef ENABLE_UART_CMD_PROCESS
void display_led_agc(uint8_t axisType, uint8_t pdIdx0)
{
    uint16_t i, maxScanStep, pdIdx, xx;
    uint8_t ledCurrentTblIdx, ledDacIdx;
    uint8_t *axisLedCurrentTblIdx;
    uint8_t *axisLedOnTimeIdx;
    const uint8_t (*sequenceTbl)[2];
    uint8_t pd, led; //maxCell;

    if (axisType == X_AXIS) {
        maxScanStep = X_TOTAL_SCAN_STEPS;
        axisLedCurrentTblIdx = &xAxisLedCurrentTblIdx[0];
        axisLedOnTimeIdx = &xAxisDacIdx[0];
        sequenceTbl = xAxisSequenceTbl;
        //maxCell = X_CELL_SIZE;
        TRACE_UART("X ");
    }
    else {
        maxScanStep = Y_TOTAL_SCAN_STEPS;
        axisLedCurrentTblIdx = &yAxisLedCurrentTblIdx[0];
        axisLedOnTimeIdx = &yAxisDacIdx[0];
        sequenceTbl = yAxisSequenceTbl;
        //maxCell = Y_CELL_SIZE;
        TRACE_UART("Y ");
    }
    pdIdx = pdIdx0; //findStartScanSequenceIdx(axisType, pdIdx0, 0, maxCell);
    //--------------------------------------------------------------------------
    // Init LED current table index & LED On time
    TRACE_UART("AGC Result %d\r\n", pdIdx);
    TRACE_UART("-------------------------------------------------------------------------");
    TRACE_UART("\r\n");
    xx =0;
    for(i = 0; i < maxScanStep; i++)
    {
        pd = sequenceTbl[i][0];
        led = sequenceTbl[i][1];
        if (pd == pdIdx || pd == pdIdx-CELLS_PER_CS){
            ledCurrentTblIdx = axisLedCurrentTblIdx[i];
            ledDacIdx = axisLedOnTimeIdx[i];
            TRACE_UART("(,%d,%d,%d,%2d,%2d,)\r\n", xx++,i, led,  ledCurrentTblIdx, ledDacIdx);
        }
    }
    TRACE_UART("\r\n");
    TRACE_UART("-------------------------------------------------------------------------\r\n\r\n");
}

#include "fsl_wdog.h"
#define MY_WDOG_BASE		WDOG1

void displayLedAgcResult(void)
{
    int i;
    uint8_t ledCurrentTblIdx, ledDacIdx;
#if (DEBUG_VCOM_ENABLE == DEBUG_VCOM_MODE)
    nr_option = 1;
#endif
    //--------------------------------------------------------------------------
    // Init LED current table index & LED On time
    TRACE_UART("X-Axis AGC Result\r\n");
    TRACE_UART("-------------------------------------------------------------------------");
    for(i = 0; i < X_TOTAL_SCAN_STEPS; i++)
    {
    	WDOG_Refresh(MY_WDOG_BASE);
    	ledCurrentTblIdx = xAxisLedCurrentTblIdx[i];
        ledDacIdx = xAxisDacIdx[i];

        if ((i % PD_SIGNAL_OUT_NUM) == 0) {
        	TRACE_UART("\r\n%3d : ", i);
        }

        TRACE_UART("(%2d, %2d), ", ledCurrentTblIdx, ledDacIdx);
    }

    TRACE_UART("\r\n\r\n");
    TRACE_UART("Y-Axis AGC Result\r\n");
    TRACE_UART("-------------------------------------------------------------------------");
    for(i = 0; i < Y_TOTAL_SCAN_STEPS; i++)
    {
    	WDOG_Refresh(MY_WDOG_BASE);
    	ledCurrentTblIdx = yAxisLedCurrentTblIdx[i];
        ledDacIdx = yAxisDacIdx[i];

        if ((i % PD_SIGNAL_OUT_NUM) == 0) {
        	TRACE_UART("\r\n%3d : ", i);
        }

        TRACE_UART("(%2d, %2d), ", ledCurrentTblIdx, ledDacIdx);
    }
    TRACE_UART("\r\n");
    TRACE_UART("-------------------------------------------------------------------------\r\n\r\n");
#if (DEBUG_VCOM_ENABLE == DEBUG_VCOM_MODE)
    nr_option = 0;
#endif
}
#endif



/*******************************************************************************
 End of File
 */
