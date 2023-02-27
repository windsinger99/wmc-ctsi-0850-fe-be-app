#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "macro.h"
#include "scan.h"
#include "scan_table.h"
#include "app.h"

#if (ENABLE_UART_CMD_PROCESS == TRUE)
#define READ_STR_MAX_BYTES  80
uint32_t uartScanSeqTestCnt;
int debug_print_on;
static unsigned char readStrBuffer[READ_STR_MAX_BYTES+1];
static int16_t axisType, seqNo;
static uint8_t bLedOn;

#define UART_MODE_ON_CNT 3
static uint8_t uartCmdModeCnt;

uint8_t ledIdx_test, pdIdx_test, showCnt_test, scan_delay_test, scan_axis_test; //nsmoon@211213
static uint8_t getPdLedIdx_test_enable; //nsmoon@211230

uint16_t setValueDAC, getValueDAC;
uint16_t setADCSel, getADCSel;

int16_t uartScanTest(void);
int16_t uartScanSeqTest(void);



uint16_t _mon_getc(int canblock)
{
#if 0 //not tested
    volatile int c = 0;
    while (UART2_Read((void*)&c, 1) != true);
    return c;
#endif
    return 0;
}

void _mon_putc(uint32_t c)
{
#if 0 //not tested
    uint8_t size = 0;

    do {
        size = UART2_Write( (void *)*&c, 1);
    } while(size != 1);
#endif
}



void uartCmdProcessInit(void)
{
    showCnt_test = 0; //nsmoon@211213
    getPdLedIdx_test_enable = 0; //nsmoon@211230
    scan_delay_test = 0;
    scan_axis_test = 0;
    debug_print_on = 0; //off
    readStrBuffer[0] = '\0';
    axisType = X_AXIS;
    seqNo = 0;
    bLedOn = 0; //off
    uartCmdModeCnt = 0;
    setValueDAC = LED_ON_DAC_INIT_Y;
    setADCSel = 0;
    Scan_Initialize();
    printf("\r\n***Please press 123 to enter UART command Mode***\r\n\n");
}

static uint8_t UART2_ReadByte(void)
{
    return 0;
}

static bool UART2_ReceiverIsReady(void)
{
    return false;
}

void uartReceiveHandler(void)
{
    // int rcvByte;
    uint8_t rcvByte;

    if (UART2_ReceiverIsReady() == true) {
        rcvByte = (uint8_t)UART2_ReadByte();
        if (rcvByte == '\r') printf("\n");
        else printf("%c", rcvByte);
        //printf("%c (%02x)\r\n", rcvByte, rcvByte);
    }
}

static int kbhit(void)
{
    if (UART2_ReceiverIsReady() == true)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int16_t readStr(void)
{
    int nChars = 0;
    unsigned char rcvChar;

    while(nChars <= READ_STR_MAX_BYTES) {
        rcvChar = _mon_getc(1);
        _mon_putc(rcvChar);
        printf("%c", rcvChar);
        if (rcvChar == '\b') {
            if (nChars) nChars--;
        } else {
            readStrBuffer[nChars++] = rcvChar;
        }
        if (rcvChar == '\r') {
            _mon_putc('\n');
            break;
        }
    }

    return nChars;
}

#if 1 //nsmoon@211213
static void getPdLedIdx_test(void)
{
    uint8_t rcvUint8;
    printf("Axis(0=X, 1=Y): ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
    scan_axis_test = (rcvUint8)? Y_AXIS : X_AXIS; //0:x-axis
    printf("LED Index: ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
    ledIdx_test = rcvUint8;
    printf("PD Index: ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
    pdIdx_test = rcvUint8;
#if 0
    printf("SCAN Delay: ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
    scan_delay_test = rcvUint8;
#endif
}

#if 0
static void scan_full_test(void)
{
    int cnt = 30;
    printf("scan_full_test..(%d)\r\n", cnt);
    while(cnt-- > 0) {
        scanAxisFull(Y_AXIS, TRUE);
        scanAxisFull(X_AXIS, TRUE);
        CORETIMER_DelayMs(1); // kjs 210315
        if (pdIdx_test >= X_CELL_SIZE) pdIdx_test = 0;
        if (ledIdx_test >= X_CELL_SIZE) ledIdx_test = 0;
        if (scan_delay_test >= 10) scan_delay_test = 0;
        CORETIMER_DelayMs(scan_delay_test);
        if (scan_axis_test == X_AXIS) {
            int16_t offsetIdx = (ledIdx_test - pdIdx_test) + X_MAX_OFFSET;
            printf("X==%d/%d/%d %d\r\n", ledIdx_test, pdIdx_test, xScanResultData[pdIdx_test][offsetIdx], scan_delay_test);
        }
        else {
            int16_t offsetIdx = (ledIdx_test - pdIdx_test) + Y_MAX_OFFSET;
            printf("Y==%d/%d/%d %d\r\n", ledIdx_test, pdIdx_test, yScanResultData[pdIdx_test][offsetIdx], scan_delay_test);
        }
    }
}
#endif
#if 0 //nsmoon@211230
static uint8_t getZeroThresholdData(uint8_t axis, uint8_t seqNo)
{
    uint8_t i, pdIdx, ledIdx, offsetIdx;
    uint8_t deadCellPd, deadCellLed, cnt = 0;
    uint8_t thresholdData;

    for (i = 0; i < PD_SIGNAL_OUT_NUM; i++) {
        if (axis == X_AXIS) {
            pdIdx = xPdIdxAtANOutTbl[seqNo][i];
            offsetIdx = xUOffsetAtANOutTbl[seqNo][i];
            if (pdIdx == X_CELL_SIZE || offsetIdx == X_TOTAL_OFFSET) {
                continue;
            }
            ledIdx = offsetIdx + X_MAX_OFFSET + pdIdx;
            deadCellPd = xCellStatus[pdIdx].isPdDead;
            deadCellLed = xCellStatus[ledIdx].isLedDead;
            thresholdData = xScanThresholdData[pdIdx][offsetIdx];
        }
        else {
            pdIdx = yPdIdxAtANOutTbl[seqNo][i];
            offsetIdx = yUOffsetAtANOutTbl[seqNo][i];
            if (pdIdx == Y_CELL_SIZE || offsetIdx == Y_TOTAL_OFFSET) {
                continue;
            }
            ledIdx = offsetIdx + Y_MAX_OFFSET + pdIdx;
            deadCellPd = yCellStatus[pdIdx].isPdDead;
            deadCellLed = yCellStatus[ledIdx].isLedDead;
            thresholdData = yScanThresholdData[pdIdx][offsetIdx];
        }
        if (deadCellPd || deadCellLed) {
            continue;
        }
        if (thresholdData == 0) {
            cnt++;
        }
    }
    return cnt;
}

#define MAX_ZERO_THRESHOLD_CNT    10
static uint8_t getZeroThresholdDataAll(void)
{
    uint8_t i, cnt = 0;
    for (i = 0; i < X_TOTAL_SCAN_STEPS; i++) {
        cnt += getZeroThresholdData(X_AXIS, i);
        if (cnt >= MAX_ZERO_THRESHOLD_CNT) {
            break;
        }
    }
    for (i = 0; i < Y_TOTAL_SCAN_STEPS; i++) {
        cnt += getZeroThresholdData(Y_AXIS, i);
        if (cnt >= MAX_ZERO_THRESHOLD_CNT) {
            break;
        }
    }
    return cnt;
}
#endif

#define INVALID_OFST_IDX 0xFF
void print_scan_threshold(const char *dbgStr, uint8_t axis, int16_t ledIdx, int16_t pdIdx)
{
    if (!getPdLedIdx_test_enable) return; //nsmoon@211230

    if (showCnt_test > 0) {
        uint8_t max_cell = (axis == X_AXIS) ? X_CELL_SIZE : Y_CELL_SIZE;
        if (pdIdx >= max_cell || ledIdx >= max_cell) {
            printf("ERROR! invalid ledIdx.. %d/%d/%d", axis, ledIdx, pdIdx);
            return;
        }
        max_cell = (scan_axis_test == X_AXIS) ? X_CELL_SIZE : Y_CELL_SIZE;
        if (pdIdx >= max_cell || ledIdx_test >= max_cell) {
            printf("ERROR! invalid ledIdx_test.. %d/%d/%d", scan_axis_test, ledIdx_test, pdIdx_test);
            ledIdx_test = pdIdx_test = 0;
            return;
        }
        if (axis == (uint8_t)scan_axis_test && ledIdx == (int16_t)ledIdx_test && pdIdx == (int16_t)pdIdx_test) {
            uint16_t tmpSeqIdx = findStartScanSequenceIdx(scan_axis_test, pdIdx, ledIdx, ledIdx);
            int16_t offset = ledIdx_test - pdIdx_test;
            int16_t maxOffset = (scan_axis_test == X_AXIS) ? X_MAX_OFFSET : Y_MAX_OFFSET;
            int16_t offsetIdxMax = (scan_axis_test == X_AXIS) ? X_TOTAL_OFFSET : Y_TOTAL_OFFSET;
            int16_t offsetIdx = offset + maxOffset;
            if (offsetIdx < 0 || offsetIdx >= offsetIdxMax) {
                printf("ERROR! print_scan_threshold..invalid offsetIdx.. %d/%d/%d %d/%d", axis, ledIdx, pdIdx, offset, offsetIdx);
                return;
            }
            if (scan_axis_test == X_AXIS) {
                printf("%s %d/%d %d/%d/%d %d/%d\r\n", dbgStr, ledIdx, pdIdx, xScanThresholdData[pdIdx][offsetIdx], xScanResultData[pdIdx][offsetIdx], xScanNonBlockLevel[pdIdx][offsetIdx], tmpSeqIdx, xAxisLedOnTimeIdx[tmpSeqIdx]);
            }
            else {
                printf("%s %d/%d %d/%d/%d %d/%d\r\n", dbgStr, ledIdx, pdIdx, yScanThresholdData[pdIdx][offsetIdx], yScanResultData[pdIdx][offsetIdx], yScanNonBlockLevel[pdIdx][offsetIdx], tmpSeqIdx, yAxisLedOnTimeIdx[tmpSeqIdx]);
            }
        }
    }
}

#define DISPLAY_SCAN_RESULT_DATA() { \
    printf(" %c,%03d,%03d, ", axisChar, seqNo, onTimeIdx); \
    printf("%03d,%03d,%03d, ", ledIdx, pdIdx, offsetIdx); \
    if (pdIdx == maxCellSize || offsetIdx == offsetIdxMax) { \
        printf("---,---,--- "); \
    } \
    else { \
        if (axis == X_AXIS) { \
            printf("%03d,%03d,%03d, ", xScanResultData[pdIdx][offsetIdx], xScanNonBlockLevel[pdIdx][offsetIdx], xScanThresholdData[pdIdx][offsetIdx]); \
        } \
        else { \
            printf("%03d,%03d,%03d, ", yScanResultData[pdIdx][offsetIdx], yScanNonBlockLevel[pdIdx][offsetIdx], yScanThresholdData[pdIdx][offsetIdx]); \
        } \
    } \
    printf("\r\n"); \
}

static int displayScanResult(uint8_t axis, uint16_t seqNo, uint8_t dispAll)
{
    uint8_t i, maxCellSize, pdIdx, ledIdx; //maxOffset;
    uint8_t offsetIdx, offsetIdxMax, onTimeIdx; //offset

    char axisChar;
    int cnt = 0;

    if (axis == X_AXIS) {
        axisChar = 'X';
        offsetIdxMax = X_TOTAL_OFFSET;
        maxCellSize = X_CELL_SIZE;
        //maxOffset = X_MAX_OFFSET;
        ledIdx = xAxisSequenceTbl[seqNo][1];
        onTimeIdx = xAxisLedOnTimeIdx[seqNo];
    }
    else {
        axisChar = 'Y';
        offsetIdxMax = Y_TOTAL_OFFSET;
        maxCellSize = Y_CELL_SIZE;
        //maxOffset = Y_MAX_OFFSET;
        ledIdx = yAxisSequenceTbl[seqNo][1];
        onTimeIdx = yAxisLedOnTimeIdx[seqNo];
    }
    //printf("Axis:%c SeqNo:%d OnTime:%d\r\n", axisChar, seqNo, onTimeIdx);
    for (i = 0; i < PD_SIGNAL_OUT_NUM; i++) {
        if (axis == X_AXIS) {
            pdIdx = xPdIdxAtANOutTbl[seqNo][i];
            offsetIdx = xUOffsetAtANOutTbl[seqNo][i];
        }
        else {
            pdIdx = yPdIdxAtANOutTbl[seqNo][i];
            offsetIdx = yUOffsetAtANOutTbl[seqNo][i];
        }
        if (dispAll) {
            DISPLAY_SCAN_RESULT_DATA();
        }
        else {
            if (pdIdx != maxCellSize && offsetIdx != offsetIdxMax) {
#if 1 //for test
#define LEVEL_CHECK_MARGIN     10
#define THRESHOLD_CHECK_MIN    40
                int16_t diffLevel;
                uint8_t scanData, thresholdData;
                if (axis == X_AXIS) {
                    scanData = xScanResultData[pdIdx][offsetIdx];
                    thresholdData = xScanThresholdData[pdIdx][offsetIdx];
                }
                else {
                    scanData = yScanResultData[pdIdx][offsetIdx];
                    thresholdData = yScanThresholdData[pdIdx][offsetIdx];
                }
                diffLevel = (int16_t)(scanData - thresholdData);
                if (diffLevel < LEVEL_CHECK_MARGIN || thresholdData < THRESHOLD_CHECK_MIN) {
                    cnt++;
                    printf("%02d,%02d, ", diffLevel, thresholdData);
                    DISPLAY_SCAN_RESULT_DATA();
                }
#endif
#if 0 //for test
#define THRESHOLD_CHECK_MARGIN 5
                uint8_t centerThresholdDropPercent, cornerThresholdDropPercent, cornerCellSize;
                int thLevel, thresholdDropPercent, thDiff;
                if (axis == X_AXIS) {
                    cornerCellSize = X_CORNER_CELL_SIZE;
                    centerThresholdDropPercent = X_THRESHOLD_DROP_PERCENT;
                    cornerThresholdDropPercent = X_CORNER_THRESHOLD_DROP_PERCENT;
                }
                else {
                    cornerCellSize = Y_CORNER_CELL_SIZE;
                    centerThresholdDropPercent = Y_THRESHOLD_DROP_PERCENT;
                    cornerThresholdDropPercent = Y_CORNER_THRESHOLD_DROP_PERCENT;
                }
                if (isInSideArea(pdIdx, ledIdx, maxCellSize, cornerCellSize)) {
                    thresholdDropPercent = cornerThresholdDropPercent;
                } else {
                    thresholdDropPercent = centerThresholdDropPercent;
                }
                if (axis == X_AXIS) {
                    thLevel = (int)(xScanResultData[pdIdx][offsetIdx] * (100 - thresholdDropPercent)) / 100;
                    thDiff = thLevel - xScanThresholdData[pdIdx][offsetIdx];
                }
                else {
                    thLevel = (int)(yScanResultData[pdIdx][offsetIdx] * (100 - thresholdDropPercent)) / 100;
                    thDiff = thLevel - yScanThresholdData[pdIdx][offsetIdx];
                }
                if (M_ABS(thDiff) > THRESHOLD_CHECK_MARGIN) {
                    cnt++;
                    printf("%02d,%02d,", thLevel, thDiff);
                    DISPLAY_SCAN_RESULT_DATA();
                }
#endif
            }
        }
    }
    //printf("\r\n");
    return cnt;
}

#if 0
static void displayScanResultAll(void)
{
    int i, cnt;
    for (i = 0; i < X_TOTAL_SCAN_STEPS; i++) {
        cnt = displayScanResult(X_AXIS, i, 0);
    }
    for (i = 0; i < Y_TOTAL_SCAN_STEPS; i++) {
        int cntTmp = displayScanResult(Y_AXIS, i, 0);
        cnt += cntTmp;
    }
    printf("..done (%d)\r\n", cnt);
}
#endif

static void getThreshold_wSeqNo(void)
{
    uint8_t rcvUint8, axis;
    uint16_t seqNo, seqNoMax;
    int rcvUint;
    printf("Axis (0=X, 1=Y): ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
    axis = (rcvUint8)? Y_AXIS : X_AXIS; //0:x-axis
    printf("Seq No: ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint);
    seqNo = (uint16_t)rcvUint;
    seqNoMax = (axis == X_AXIS) ? X_TOTAL_SCAN_STEPS : Y_TOTAL_SCAN_STEPS;
    if (seqNo >= seqNoMax) {
        printf("ERROR! invalid seqNo %d\r\n", seqNo);
        return;
    }
    displayScanResult(axis, seqNo, 1);
}
#endif

static void DAC_getValue(void)
{
    uint8_t rxDacValue;

    rxDacValue = dac_data.VAL;

    printf("DAC Current Value: 0x%04x \r\n", rxDacValue);

}

int16_t uartDACTest(void)
{
    int32_t commandNo;
    uint16_t setValueDAC, getValueDAC;
    //uint16_t rcvValue;
    //uint8_t rcvChar;

    while(1) {
        printf("\r\n");
        printf("----------------------------------------------------------------\r\n");
        printf("Uart DAC Test\r\n");
        printf("----------------------------------------------------------------\r\n");
        printf(" 1) Read DAC Value \r\n");
        printf(" =) increase DAC Value \r\n");
        printf(" -) decrease DAC value \r\n");
        printf(" Esc) Exit\r\n");
        printf("----------------------------------------------------------------\r\n");
        printf("\r\n");


        commandNo = _mon_getc(1);

        switch(commandNo) {
        case '1' :
            DAC_getValue();
            break;
        case '2' :
            getValueDAC = dac_data.VAL;
            if (getValueDAC >= 0xff8) {
                setValueDAC = 0x0fff;
            } else {
                setValueDAC = getValueDAC + 8;
            }
            dac_data_set(setValueDAC);
            printf("increase>> Set DAC value : 0x%04x \r\n", setValueDAC);
            //rcvValue = setValueDAC;
            break;
        case '3' :
            getValueDAC = dac_data.VAL;
            if (getValueDAC >= 0x08) {
                setValueDAC = getValueDAC - 8;
            } else {
                setValueDAC = 0;
            }
            dac_data_set(setValueDAC);
            printf("decrease>> set DAC value : 0x%04x \r\n", setValueDAC);
            //rcvValue = setValueDAC;
            break;
        default:
            break;
        }

        if (commandNo >= '0' && commandNo <= '9') {
            continue;
        } else if (commandNo == 0x1B) { // if key is ESC
            printf("\r\nGood Bye !!!\r\n");
            break;
        }

    }

    return 0;

}

static void uartCmdProcessMenu(void)
{
    printf("\r\n");
    printf("-----------------------------------------------\r\n");
    printf("UART Command Process\r\n");
    printf("-----------------------------------------------\r\n");
    printf(" h) Display This Menu\r\n");
    printf(" a) AGC On/Off\r\n");
    printf(" e) ExtLightNoiseCal On/Off\r\n");
    printf(" d) Display AGC Result (currIdx, onTime)\r\n");
    printf(" s) Scan Test\r\n");
    printf(" t) Scan Seq Test\r\n");
    printf(" f) Fixed Current Mode\r\n");
    printf(" j) Decrease PD number Triger\r\n");
    printf(" k) Increase PD number Triger\r\n");
    printf(" L) Toggle Axis Triger\r\n");
    printf("    -) Decrease X Current Index\r\n");
    printf("    =) Increase X Current Index\r\n");
    printf("    _) Decrease Y Current Index\r\n");
    printf("    +) Increase Y Current Index\r\n");
    printf("    [) Decrease X On Time\r\n");
    printf("    ]) Increase X On Time\r\n");
    printf("    {) Decrease Y On Time\r\n");
    printf("    }) Increase Y On Time\r\n");
    printf(" m) Scan Result All\r\n");
    printf(" n) Scan Result (SeqNo)\r\n");
    printf(" p) Scan Result (pdIdx,ledIdx)\r\n");
    printf(" P) Scan Result Display On/Off\r\n");
    printf(" c) Forced AGC On\r\n");
    printf(" D) DAC Voltage Value control\r\n");

    printf(" =) increase DAC Value \r\n");
    printf(" -) decrease DAC Value \r\n");
    printf("-----------------------------------------------\r\n");
    printf("\r\n");
}

static uint8_t chkUartCmdMode(char key)
{
    uint8_t mached = 0;
    if (uartCmdModeCnt < UART_MODE_ON_CNT) {
        switch (uartCmdModeCnt) {
        case 0:
            if (key == '1') mached = 1;
            break;
        case 1:
            if (key == '2') mached = 1;
            break;
        case 2:
            if (key == '3') mached = 1;
            break;
        }
        if (mached) {
            uartCmdModeCnt++;
            if (uartCmdModeCnt == UART_MODE_ON_CNT) {
                uartCmdProcessMenu();
            }
        }
        else {
            uartCmdModeCnt = 0;
        }
        return 0; //not-mached
    }
    return 1; //mached
}

static void uartCmdESC(void)
{
    printf("Exit UART Command Process~~\r\n\r\n");
    uartCmdModeCnt = 0;
    fixedCurrentEnable = 0;
#if (WATCHDOG_ENABLE)
    WDT_Enable();
#endif
}

#if 0
static void uartPDGainCtrl(uint8_t gain)
{
    switch (gain) {
    case 0:
        MCU_ADC_GROUP_SELECT_0_Clear();
        MCU_ADC_GROUP_SELECT_1_Clear();
        break;
    case 1:
        MCU_ADC_GROUP_SELECT_0_Set();
        MCU_ADC_GROUP_SELECT_1_Clear();
        break;
    case 2:
        MCU_ADC_GROUP_SELECT_0_Clear();
        MCU_ADC_GROUP_SELECT_1_Set();
        break;
    case 3:
        MCU_ADC_GROUP_SELECT_0_Set();
        MCU_ADC_GROUP_SELECT_1_Set();
        break;

    }
}
#endif

uint16_t uartCmdProcess(void)
{
    char key;
    if(kStatus_LPUART_FlagCannotClearManually != LPUART_GetStatusFlags(LPUART1_PERIPHERAL))
    {

        key = getchar();
        if (chkUartCmdMode(key) == 0)
        {
            return 0;
        }

#if (WATCHDOG_ENABLE)
        WDT_Disable();
#endif

        switch(key) {
        case 0x1B : // if key is ESC
            uartCmdESC();
            break;
        case 'h' :
        case 'H' :
            uartCmdProcessMenu();
            break;
#if 1 // function off
#if 1 //nsmoon@211213
        case 'n' :
        case 'N' :
            //scan_full_test();
            getThreshold_wSeqNo();
            break;
        case 'p' :
            getPdLedIdx_test();
            break;
        case 'P' :
            if (getPdLedIdx_test_enable) {
                getPdLedIdx_test_enable = 0;
                printf("\r\ngetPdLedIdx_test OFF\r\n");
            }
            else {
                getPdLedIdx_test_enable = 1;
                printf("\r\ngetPdLedIdx_test ON\r\n");
            }
            break;
#endif
        case 'a' :
        case 'A' :
            if (scanStatus.isUsualAgcCal == TRUE) {
                scanStatus.isUsualAgcCal = FALSE;
                fixedCurrentEnable = 1;
                printf("\r\nAGC OFF\r\n");
            } else {
                scanStatus.isUsualAgcCal = TRUE;
                fixedCurrentEnable = 0;
                printf("\r\nAGC ON\r\n");
            }
            break;
#if FORCE_AGC_SET
        case 'c' :
        case 'C' :
            //g_forcedagc.forced_agc = 1;
        	g_forcedagc.forced_saved =1;
            break;
#endif
        case 'e' :
        case 'E' :
            if (scanStatus.isUsualExtLightNoiseCal == TRUE) {
                scanStatus.isUsualExtLightNoiseCal = FALSE;
                printf("\r\nExtLightNoiseCal OFF\r\n");
            } else {
                scanStatus.isUsualExtLightNoiseCal = TRUE;
                printf("\r\nExtLightNoiseCal ON\r\n");
            }
            break;
        case 'd' : // if key is 'd'
        case 'D' : // if key is 'D'
            // Display sense information related to current mode
            //uartCmdDisplayInformation();
            // printf("D command\r\n");
            // kjs 210414 add
            //  displayLedAgcResult();
            break;
        case 'm' :
        case 'M' :
            // displayScanResultAll();
            break;
#if 0 //ENABLE_TRIGER
        case 'j' :
        case 'J' :
            if (Current_Triger_Pd > CELLS_PER_CS) {
                Current_Triger_Pd -= CELLS_PER_CS;
            }
            else {
                Current_Triger_Pd = 0;
            }
            printf("Current Triger PD number is (%d) \r\n", Current_Triger_Pd);
            break;
        case 'k' : // if key is 'k'
        case 'K' : // if key is 'K'
            if (Current_Triger_Pd < (X_CELL_SIZE - CELLS_PER_CS - 1)) {
                Current_Triger_Pd += CELLS_PER_CS;
            }
            else {
                Current_Triger_Pd = X_CELL_SIZE - 1;
            }
            printf("Current Triger PD number is (%d) \r\n", Current_Triger_Pd);
            break;
        case 'l' : // if key is 'l'
        case 'L' : // if key is 'L'
            if (Current_Triger_Axis == Y_AXIS) {
                Current_Triger_Axis = X_AXIS;
            }
            else {
                Current_Triger_Axis = Y_AXIS;
            }
            char axis_char = (Current_Triger_Axis == Y_AXIS) ? 'Y' : 'X';
            printf("Current Triger AXIS is (%c) \r\n", axis_char);
            break;
        case 'u' : // if key is 'u'
        case 'U' : // if key is 'U'
            if ( Usb_Off_Test == 0) {
                Usb_Off_Test = 1;
            }
            else {
                Usb_Off_Test = 0;
            }
            char usb_char = (Usb_Off_Test == 0) ? 'O' : 'X';
            printf("USB status is (%c) \r\n", usb_char);
            break;
#endif
#if 0
        case 'p' : // if key is 'p'
        case 'P' : // if key is 'P'
            // parameter settings
            //uartCmdParameters();
            printf("P command\r\n");
            break;
#endif
        case 's' : // if key is 's'
        case 'S' : // if key is 'S'
            // Scan Test
            uartScanTest();
            break;
#if 1 //nsmoon@210719
        case 't' : // if key is 't'
        case 'T' : // if key is 'T'
            // Scan Test
            uartScanSeqTest();
            break;
#endif
#if 0
        case 'v' : // if key is 'v'
        case 'V' : // if key is 'V'
            // Display F/W version information
//        printf("\n\n");
//        printf("Model Name       : %s\n", ModelName);
//        printf("Firmware Version : %s\n", FirmwareVersion);
//        printf("Compile time is %s, %s\n\n", __TIME__, __DATE__);
            break;
#endif
#if 0 // 1 //nsmoon@210915
        case 'f' :
        case 'F' :
            if (fixedCurrentEnable) {
                fixedCurrentEnable = 0;
                g_forcedagc.forced_agc = 1;
                printf("Fixed Current OFF\r\n");
            } else {
                fixedCurrentEnable = 1;
                printf("Fixed Current ON (%d,%d) (%d,%d)\r\n", Current_Test_index_X, Led_On_Time_Test_X, Current_Test_index_Y, Led_On_Time_Test_Y);
            }
            break;

        case '_' :
            if (fixedCurrentEnable) {
                if(Current_Test_index_Y > 0) {
                    Current_Test_index_Y--;
                }
            }
            break;
        case '-' :
            if (fixedCurrentEnable) {
                if(Current_Test_index_X > 0) {
                    Current_Test_index_X--;
                }
            }
            break;

        case '+' :
            if (fixedCurrentEnable) {
                if(Current_Test_index_Y < LED_SINK_CONTROL_STEPS-1) {
                    Current_Test_index_Y++;
                }
            }
            break;
        case '=' :
            if (fixedCurrentEnable) {
                if(Current_Test_index_X < LED_SINK_CONTROL_STEPS-1) {
                    Current_Test_index_X++;
                }
            }
            break;
        case '{' :
            if (fixedCurrentEnable) {
                if (Led_On_Time_Test_Y > LED_ON_DAC_MIN_Y) {
                    Led_On_Time_Test_Y--;
                }
            }
            break;
        case '[' :
            if (fixedCurrentEnable) {
                if (Led_On_Time_Test_X > LED_ON_DAC_MIN_X) {
                    Led_On_Time_Test_X--;
                }
            }
            break;
        case '}' :
            if (fixedCurrentEnable) {
                if (Led_On_Time_Test_Y < LED_ON_DAC_MAX_Y) {
                    Led_On_Time_Test_Y++;
                }
            }
            break;
        case ']' :
            if (fixedCurrentEnable) {
                if (Led_On_Time_Test_X < LED_ON_DAC_MAX_X) {
                    Led_On_Time_Test_X++;
                }
            }
            break;
#endif
#endif  // function off

#if 0 //for test
        case '1' : // if key is '1'
        {
            display_led_agc(X_AXIS, 77);    //uint8_t axisType, uint8_t pdIdx0
        }
        break;
#endif

#if 0
        case '=' :
            getValueDAC = dac_output_value_get(DAC1);
            if (getValueDAC >= 4095) {
                setValueDAC = 4095;
            } else {
                setValueDAC = getValueDAC + 40;
            }
            dac_data_set(DAC1, DAC_ALIGN_12B_R, setValueDAC);
            //printf("increase>> Set DAC value : %d \r\n", setValueDAC);
            //rcvValue = setValueDAC;
            break;
        case '-' :
            getValueDAC = dac_output_value_get(DAC1);
            if (getValueDAC >= 40) {
                setValueDAC = getValueDAC - 40;
            } else {
                setValueDAC = 15;
            }
            dac_data_set(DAC1, DAC_ALIGN_12B_R, setValueDAC);
            //printf("decrease>> set DAC value : %d \r\n", setValueDAC);
            break;
        case '+' :
            if (getADCSel >= 4) {
                setADCSel = 4;
            } else {
                setADCSel = ++getADCSel;
            }
            uartPDGainCtrl(setADCSel);
            //printf("increase>> Set DAC value : 0x%04x \r\n", setValueDAC);
            //rcvValue = setValueDAC;
            break;
        case '_' :
            if (getADCSel > 0) {
                setADCSel = --getADCSel;
            } else {
                setADCSel = 0;
            }
            uartPDGainCtrl(setADCSel);
            break;
#else
        case '=' :
            if (setValueDAC >= LED_ON_DAC_MAX_X) {
                setValueDAC = LED_ON_DAC_MAX_X;
            } else {
                setValueDAC ++;
            }
            break;
        case '-' :
            if (setValueDAC <= 0) {
                setValueDAC = 0;
            } else {
                setValueDAC--;
            }

            break;
        case '+' :
            if (setADCSel >= LED_SINK_CONTROL_STEPS) {
                setADCSel = LED_SINK_CONTROL_STEPS;
            } else {
                setADCSel++;
            }

            break;
        case '_' :
            if (setADCSel <= 0) {
                setADCSel = 0;
            } else {
                setADCSel --;
            }
            break;


#endif
        default :
            //uartCmdProcessMenu();
            break;
        } //switch

        if(setValueDAC !=getValueDAC || setADCSel  != getADCSel)
        {
            getValueDAC = setValueDAC;
            getADCSel = setADCSel;
            printf(">> DAC value : %d, PD GAIN: %d \r\n", setValueDAC, setADCSel);
        }
#if 0

        if (fixedCurrentEnable) {
            if (key == '_' || key == '-' || key == '+' || key == '=' ||
                    key == '{' || key == '[' || key == '}' || key == ']') {
                printf("Fixed Current ON (%d,%d) (%d,%d)\r\n", Current_Test_index_X, Led_On_Time_Test_X, Current_Test_index_Y, Led_On_Time_Test_Y);
            }
        }

#endif
    }


    return 0;
}

int16_t uartScanTest(void)
{
    int32_t commandNo;
    int16_t pdIdxMin, pdIdxMax; //ledIdxMin, ledIdxMax;
    int16_t ledIdx, pdStartIdx, loopDelayMsec, maxOffset, cellSize;
    char    rcvChar;
    uint8_t rcvUint8;

    axisType = Y_AXIS; //X_AXIS;
    bLedOn = TRUE;
    ledIdx = 0;
    pdStartIdx = 0;
    loopDelayMsec = 5;

    if (axisType == X_AXIS) {
        maxOffset = X_MAX_OFFSET;
        cellSize = X_CELL_SIZE;
    } else {
        maxOffset = Y_MAX_OFFSET;
        cellSize = Y_CELL_SIZE;
    }

    pdIdxMin = ledIdx - maxOffset;
    if (pdIdxMin < 0) pdIdxMin = 0;
    pdIdxMax = ledIdx + maxOffset;
    if (pdIdxMax >= cellSize) pdIdxMax = cellSize - 1;

    while(1) {
        printf("\r\n");
        printf("----------------------------------------------------------------\r\n");
        printf("Uart Scan Test\r\n");
        printf("----------------------------------------------------------------\r\n");
        printf(" Press 1 ~ 5 to change parameter or s to start scan\r\n");
        printf(" 1) Axis = %s\r\n", axisType == X_AXIS ? "X" : "Y");
        printf(" 2) LED index No = %d\r\n", ledIdx);
        printf(" 3) PD Start index No = %d\r\n", pdStartIdx);
        printf(" 4) LED On/Off = %s\r\n", bLedOn == TRUE ? "On" : "Off");
        printf(" 5) Loop delay = %d [msec]\r\n", loopDelayMsec);
        printf(" s) Start scan\r\n");
        printf(" Esc) Exit\r\n");
        printf("----------------------------------------------------------------\r\n");
        printf("\r\n");
        //printf("Enter command number : ");

        commandNo = _mon_getc(1);

        switch(commandNo) {
        case '1' :
            printf("Enter Axis Type [ x / y ] = ");
            readStr();
            sscanf((const char *)readStrBuffer, "%c", &rcvChar);
            axisType = rcvChar == 'x' ? X_AXIS : Y_AXIS;

            if (axisType == X_AXIS) {
                maxOffset = X_MAX_OFFSET;
                cellSize = X_CELL_SIZE;
            } else {
                maxOffset = Y_MAX_OFFSET;
                cellSize = Y_CELL_SIZE;
            }

            if (ledIdx >= cellSize) ledIdx = cellSize - 1;

            pdIdxMin = ledIdx - maxOffset;
            if (pdIdxMin < 0) pdIdxMin = 0;
            pdIdxMax = ledIdx + maxOffset;
            if (pdIdxMax >= cellSize) pdIdxMax = cellSize - 1;

            if (pdStartIdx < pdIdxMin) pdStartIdx = pdIdxMin;
            if (pdStartIdx > pdIdxMax) pdStartIdx = pdIdxMax;

            break;

        case '2' :
            printf("Enter LED Index No in the axis = ( 0 ~ %d ) = ", cellSize - 1);
            readStr();
            sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
            ledIdx = rcvUint8;

            if (ledIdx >= cellSize) ledIdx = cellSize - 1;

            pdIdxMin = ledIdx - maxOffset;
            if (pdIdxMin < 0) pdIdxMin = 0;
            pdIdxMax = ledIdx + maxOffset;
            if (pdIdxMax >= cellSize) pdIdxMax = cellSize - 1;

            if (pdStartIdx < pdIdxMin) pdStartIdx = pdIdxMin;
            if (pdStartIdx > pdIdxMax) pdStartIdx = pdIdxMin;
            break;

        case '3' :
            printf("Enter PD Start Index No in the axis ( %d ~ %d ) = ", pdIdxMin, pdIdxMax);
            readStr();
            sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
            if (rcvUint8 < pdIdxMin) rcvUint8 = pdIdxMin;
            if (rcvUint8 > pdIdxMax) rcvUint8 = pdIdxMax;
            pdStartIdx = (rcvUint8/3) * 3;
            break;

        case '4' :
            printf("Enter LED On/Off [ 1 for On / 0 for Off ] = ");
            readStr();
            sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
            bLedOn = rcvUint8 == 1 ? TRUE : FALSE;
            break;

        case '5' :
            printf("Enter Loop Delay [msec] = ");
            readStr();
            sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
            loopDelayMsec = rcvUint8;
            if (loopDelayMsec == 0) loopDelayMsec = 1;
            break;
        }

        if (commandNo >= '0' && commandNo <= '9') {
            continue;
        } else if (commandNo == 0x1B) { // if key is ESC
            printf("\r\nGood Bye !!!\r\n");
            break;
        }

        if (commandNo == 's') {
            printf("\t-----> Now scan testing, press ESC key to quit !!!\r\n");
            selectPDGroupExt(-1);
            //gCurrentStartTotalPdIdx = -1; //nsmoon@210914 move to selectPDGroup()
            while(1) {
                debug_print_on = 1;
                //M_DebugTP2_High();
                //scanAxis(axisType, bLedOn, pdStartIdx, pdStartIdx + 14, ledIdx, ledIdx);
                //M_DebugTP2_Low();
                debug_print_on = 0;
                //core_timer_delay_ms(loopDelayMsec);

                if (kbhit()) { // if key pressed
                    rcvChar = UART2_ReadByte();
                    if (rcvChar == 0x1B) { // if key is ESC
                        break;
                    }
                }
            }
        }
    }

    return 0;
}

#if 1 //nsmoon@210719
#define LED_ON_STR_LEN 4
#define uartScanSeqTest_printParam() { \
    printf("\r\n"); \
    if (axisType == X_AXIS) { \
        maxSeqNo = X_TOTAL_SCAN_STEPS-1; \
        sequenceTbl = xAxisSequenceTbl; \
        axisChar = 'X'; \
    } \
    else { \
        maxSeqNo = Y_TOTAL_SCAN_STEPS-1; \
        sequenceTbl = yAxisSequenceTbl; \
        axisChar = 'Y'; \
    } \
    if (bLedOn) { \
        snprintf(ledOnStr, LED_ON_STR_LEN, "%s", "ON"); \
    } \
    else { \
        snprintf(ledOnStr, LED_ON_STR_LEN, "%s", "OFF"); \
    } \
    pdIdx = sequenceTbl[seqNo][0]; \
    ledIdx = sequenceTbl[seqNo][1]; \
    printf("%s %c seq=%d pd=%d led=%d currIdx=%d onTime=%d\r\n", \
        ledOnStr, axisChar, seqNo, pdIdx, ledIdx, \
        currentTestIndex, ledOnTimeTest); \
}

static void uartScanSeqTestMenu(void)
{
    printf("\r\n");
    printf("-------------------------------\r\n");
    printf("UART Scan Seq Test\r\n");
    printf("-------------------------------\r\n");
    printf(" H) Display This Menu\r\n");
    printf(" A) Axis Toggle\r\n");
    printf(" L) LED On/Off\r\n");
    printf(" D) Display ADC Value\r\n");
    printf(" 1,2) Sequence No +,-\r\n");
    printf(" 3,4) Sequence No +10,-10\r\n");
    printf(" 5,6) Sequence No +50,-50\r\n");
    printf(" +,-) Led Current Index\r\n");
    printf(" [,]) Led On Time\r\n");
    printf(" Esc) Exit\r\n");
    printf("-------------------------------\r\n");
    printf("gCurrentStartTotalPdIdx=%d\r\n", gCurrentStartTotalPdIdx);
}

int16_t uartScanSeqTest(void)
{
    //int16_t loopDelayMsec = 5;
    int16_t maxSeqNo = 0;
    int16_t pdIdx, ledIdx;
    const uint8_t (*sequenceTbl)[2];
    char rcvChar, axisChar;
    char ledOnStr[LED_ON_STR_LEN] = { 0 };
    uint8_t currentTestIndex = 0, ledOnTimeTest = 0;
    uint8_t currentTestIndexMin, ledOnTimeTestMin;
    uint8_t currentTestIndexMax, ledOnTimeTestMax;
    uint8_t fixedCurrentEnable_sav = fixedCurrentEnable; //save

    if (axisType == X_AXIS) {
        currentTestIndex = Current_Test_index_X;
        ledOnTimeTest = Led_On_Time_Test_X;
        currentTestIndexMin = 0;//X_LED_CURRENT_MIN_INDEX;
        currentTestIndexMax = 0; //X_LED_CURRENT_MAX_INDEX;
        ledOnTimeTestMin = LED_ON_DAC_MIN_X;
        ledOnTimeTestMax = LED_ON_DAC_MAX_X;
    }
    else {
        currentTestIndex = Current_Test_index_Y;
        ledOnTimeTest = Led_On_Time_Test_Y;
        currentTestIndexMin = 0; //Y_LED_CURRENT_MIN_INDEX;
        currentTestIndexMax = 0; //Y_LED_CURRENT_MAX_INDEX;
        ledOnTimeTestMin = LED_ON_DAC_MIN_Y;
        ledOnTimeTestMax = LED_ON_DAC_MAX_Y;
    }

    uartScanSeqTestMenu();
    uartScanSeqTest_printParam();
    printf("\r\n");

    uartScanSeqTestCnt = 0;
    debug_print_on = 1;
    gCurrentStartTotalPdIdx = 0; //must be not -1
    selectPDGroupExt(-1);
    debug_print_on = 0;
    fixedCurrentEnable = 1;

    while (1)
    {
        uartScanSeqTestCnt++;
        //M_DebugTP2_High();
        //scanAxisTestSeqNo(axisType, bLedOn, seqNo);
        //M_DebugTP2_Low();
        //core_timer_delay_ms(loopDelayMsec);

        if (kbhit()) { // if key pressed
            int16_t newSeqNo = seqNo;
            int16_t newAxisType = axisType;
            uint8_t newLedOn = bLedOn;
            uint8_t newCurrentTestIndex = currentTestIndex;
            uint8_t newLedOnTimeTest = ledOnTimeTest;

            rcvChar = UART2_ReadByte();
            if (rcvChar == 0x1B) { // if key is ESC
                printf("\r\nGood Bye !!!\r\n");
                break;
            }
            if (rcvChar == 'h' || rcvChar == 'H') {
                uartScanSeqTestMenu();
                debug_print_on = 1;
            }
            else if (rcvChar == 'a' || rcvChar == 'A') {
                newAxisType = (axisType == X_AXIS) ? Y_AXIS : X_AXIS;
            }
            else if (rcvChar == 'l' || rcvChar == 'L') {
                newLedOn = (bLedOn) ? 0 : 1;
            }
            else if (rcvChar == 'd' || rcvChar == 'D') {
                debug_print_on = (debug_print_on) ? 0 : 1;
            }
            else if (rcvChar == '1') {
                newSeqNo++;
            }
            else if (rcvChar == '2') {
                newSeqNo--;
            }
            else if (rcvChar == '3') {
                newSeqNo += 10;
            }
            else if (rcvChar == '4') {
                newSeqNo -= 10;
            }
            else if (rcvChar == '5') {
                newSeqNo += 50;
            }
            else if (rcvChar == '6') {
                newSeqNo -= 50;
            }
            else if (rcvChar == '_' || rcvChar == '-') {
                if (fixedCurrentEnable && newCurrentTestIndex > currentTestIndexMin) {
                    newCurrentTestIndex--;
                }
            }
            else if (rcvChar == '+' || rcvChar == '=') {
                if (fixedCurrentEnable && newCurrentTestIndex < currentTestIndexMax) {
                    newCurrentTestIndex++;
                }
            }
            else if (rcvChar == '{' || rcvChar == '[') {
                if (fixedCurrentEnable && newLedOnTimeTest > ledOnTimeTestMin) {
                    newLedOnTimeTest--;
                }
            }
            else if (rcvChar == '}' || rcvChar == ']') {
                if (fixedCurrentEnable && newLedOnTimeTest < ledOnTimeTestMax) {
                    newLedOnTimeTest++;
                }
            }

            if (newAxisType != axisType || newSeqNo != seqNo ||
                    newLedOn != bLedOn || debug_print_on ||
                    newCurrentTestIndex != currentTestIndex ||
                    newLedOnTimeTest != ledOnTimeTest) {
                if (newSeqNo < 0 || newSeqNo > maxSeqNo) {
                    newSeqNo = 0;
                }
                debug_print_on = 1;
                axisType = newAxisType;
                seqNo = newSeqNo;
                bLedOn = newLedOn;
                currentTestIndex = newCurrentTestIndex;
                ledOnTimeTest = newLedOnTimeTest;
                uartScanSeqTest_printParam();

                if (axisType == X_AXIS) {
                    Current_Test_index_X = currentTestIndex;
                    Led_On_Time_Test_X = ledOnTimeTest;
                }
                else {
                    Current_Test_index_Y = currentTestIndex;
                    Led_On_Time_Test_Y = ledOnTimeTest;
                }
            }
        }
    } //while

    fixedCurrentEnable = fixedCurrentEnable_sav;
    if (fixedCurrentEnable) {
        printf("Fixed Current ON (%d,%d) (%d,%d)\r\n", Current_Test_index_X, Led_On_Time_Test_X, Current_Test_index_Y, Led_On_Time_Test_Y);
    }
    else {
        printf("Fixed Current OFF\r\n");
    }

    return 0;
}
#endif //1
#endif
