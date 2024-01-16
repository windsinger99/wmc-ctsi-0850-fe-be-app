#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "macro.h"
#include "scan.h"
#include "scan_table.h"
#include "app.h"
#include "uartCommand.h"

#if (ENABLE_UART_CMD_PROCESS == DEBUG_VCOM_MODE)

char HisBuff[HIS_MAX][DEBUG_BUF_MAX] = { {0,}, };
int HisCount = 0;
int HisIndex = 0;

uint16_t setValueDAC, getValueDAC;
uint16_t setADCSel, getADCSel;
uint8_t vcom_mode, nr_option;
uint8_t testFlag, testblockedlinecnt;

static const char *delim = " \f\n\r\t\v";

typedef struct {
	char *CmdStr;
	int (*func)(int argc, char **argv);

} TCommand;

void vcom_init(void)
{
    getValueDAC = setValueDAC = LED_ON_DAC_INIT_Y;
    getADCSel = setADCSel = 0;
    dltThresholdEnbale = 0;
    nr_option = 0;
    vcom_mode = VCOM_SHELL_MODE;
}

int vcom_delay(uint32_t cnt)
{
    volatile uint32_t i = 0;
    uint32_t cntTotal = cnt * 1000;
    for (i = 0; i < cntTotal; ++i)
    {
        __asm("NOP"); /* delay */
    }
    return 0;
}

void Current_adj_mode(uint8_t val)
{
	switch (val)
	{
	case '=' :
           if (setValueDAC >= 255) {
               setValueDAC = 255;
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
           if (setADCSel >= LED_SINK_CONTROL_STEPS-1) {
               setADCSel = LED_SINK_CONTROL_STEPS-1;
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

       case 0x1B :
    	   vcom_mode = VCOM_SHELL_MODE;
    	   fixedCurrentEnable = 0;
    	   scanStatus.isUsualAgcCal = TRUE;
    	   TRACE_MENU("\r\n>>/*************\033[1;35m DAC MODE END \033[0m*************/\r\n");
    	   TRACE_VCOM("\r\n>> AGC ON -> DAC_ADJ_MODE_OFF\r\n");
    	   break;

       default :
    	   TRACE_VCOM("up down key -> DAC : - =, CURRENT : _ +, EXIT : ESC ");
           break;
       } //switch

       if(setValueDAC !=getValueDAC || setADCSel  != getADCSel)
       {
    	   fixedCurrentEnable = 1;
    	   getValueDAC = setValueDAC;
           getADCSel = setADCSel;
           TRACE_VCOM("\r\n>> DAC value : %d, PD GAIN: %d ", setValueDAC, setADCSel);
       }
}

void display_menu(void)
{
	int i = 0;

	TRACE_MENU("\r\n/***************\033[1;32m Command List \033[0m***************/");
	TRACE_MENU("\r\n %d. ?		:	Command List print", i++);
	TRACE_MENU("\r\n %d. TEST	:	Test print ex) test 222 555", i++);
	TRACE_MENU("\r\n %d. AGC		:	AGC on/off 1 = on, 0 = off", i++);
	TRACE_MENU("\r\n %d. AGCD	:	AGC Calibration data Display", i++);
	TRACE_MENU("\r\n %d. DAC		:	Fixed Current mode ", i++);
	TRACE_MENU("\r\n %d. THD		:	DLT Display Threshold	1 = on, 0 = off ", i++);
	TRACE_MENU("\r\n %d. FOC		:	Forced AGC", i++);
	TRACE_MENU("\r\n %d. TIME	:	GPT2 Timer,  Value X 10ms, T1", i++);
	TRACE_MENU("\r\n %d. FLAG	:	Timer test delay flag", i++);
	TRACE_MENU("\r\n %d. BLOCK	:	Block line cnt test", i++);
	TRACE_MENU("\r\n ex) cmd xx xx ... enter  -> xx max = 255");
	TRACE_MENU("\r\n/********************************************/\r\n");
}

int CmdContents(int argc, char **argv)
{
	argv = argv;
	if(argc > 0)
	{
		display_menu();
	}
	return 0;
}

int testprint(int argc, char **argv)
{
	int i = 0;
	int j = 0;

	if(argc > 1)
	{
		i = (uint8_t)atoi(argv[1]);
		j = (uint8_t)atoi(argv[2]);
		TRACE_VCOM( "\r\n>>Test argv 1: %d, argv 2: %d", i, j);
	}
	return 0;
}

int AGC_onoff_test(int argc, char **argv)
{
	int i = 0;
	i = (uint8_t)atoi(argv[1]);

	if (i == 0 )
	{
		scanStatus.isUsualAgcCal = FALSE;
		TRACE_VCOM("\r\n>>AGC OFF\r\n");
	}
	else if(i == 1 )
	{
		scanStatus.isUsualAgcCal = TRUE;
		TRACE_VCOM("\r\n>>AGC ON\r\n");
	}
	else
	{
		TRACE_VCOM(WRONG);
	}
	return 0;
}

int DAC_test_mode(int argc, char **argv)
{
	argv = argv;
	vcom_mode = VCOM_DAC_MODE;
	fixedCurrentEnable = 1;
	scanStatus.isUsualAgcCal = FALSE;
	TRACE_MENU("\r\n>>AGC OFF -> DAC_ADJ_MODE_ON\r\n");
	TRACE_MENU("DAC up down		: -  =");
	TRACE_MENU("CURRENT up down	: _  +");
	TRACE_MENU("EXIT key		: ESC ");
	TRACE_MENU("\r\n>>/*************\033[1;34m DAC MODE START \033[0m*************/\r\n");

	return 0;
}

int dlt_TH_display(int argc, char **argv)
{
	int i = 0;
	i = (uint8_t)atoi(argv[1]);

	if (i == 0 )
	{
		dltThresholdEnbale = 0;
		TRACE_VCOM("\r\n>>DLT Display Threshold level OFF\r\n");
	}
	else if(i == 1 )
	{
		dltThresholdEnbale = 1;
		TRACE_VCOM("\r\n>>DLT Display Threshold level ON\r\n");
	}
	else
	{
		TRACE_VCOM(WRONG);
	}
	return 0;
}
int AGC_display(int argc, char **argv)
{
	argv = argv;
	displayLedAgcResult();
	return 0;
}

int forcedAgc_test(int argc, char **argv)
{
	argv = argv;
	if (scanStatus.isUsualAgcCal == FALSE)
	{
		scanStatus.isUsualAgcCal = TRUE;
		fixedCurrentEnable = 0;
		TRACE_MENU("\r\nAGC ON, Fixed Current OFF");
	}

	g_forcedagc.forced_agc = 1;
	TRACE_VCOM("\r\n*Forced agc Start*\r\n");
	return 0;
}


int Timertest(int argc, char **argv)
{
	int i = 0;	//, j = 0 ;
	i = (uint16_t)atoi(argv[1]);

	if(i > 0)
	{
		setTestTimerMsec(i);
		TRACE_VCOM("\r\nTimer Set (%d) \r\n", i);
	}
	return 0;
}

int Flagtest(int argc, char **argv)
{
	int i = 0;
	i = (uint8_t)atoi(argv[1]);
	if(i > 0)
	{
		testFlag = i;
		TRACE_VCOM("\r\nFlag Set (%d) \r\n", i);
	}
	else testFlag = 0;

	return 0;
}

int Blocklinetest(int argc, char **argv)
{
	int i = 0;
	i = (uint8_t)atoi(argv[1]);
	testblockedlinecnt = i;
	TRACE_VCOM("\r\nBlocked Line cnt (%d) \r\n", i );

	return 0;
}



TCommand Cmds[CMD_MAX] = 	{
							{	"?"						,	CmdContents							},
							{	"TEST"					,	testprint							},
							{	"AGC"					,	AGC_onoff_test						},
							{	"AGCD"					,	AGC_display							},
							{	"DAC"					,	DAC_test_mode						},
							{	"THD"					,	dlt_TH_display						},
							{	"FOC"					,	forcedAgc_test						},
							{	"TIME"					,	Timertest							},
							{	"FLAG"					,	Flagtest							},
							{	"BLOCK"					,	Blocklinetest							},
							{	"NULL"					,	NULL								}
};


int his_append(char *s)
{
	int loop;

	for( loop = 0; loop < HIS_MAX; loop++ )
	{
		if( strcmp( s, HisBuff[loop] ) == 0 )
		{
			HisIndex = 0;
			if( HisCount ) HisIndex = HisCount-1;
			return HisCount;
		}
	}

	if( HisCount < HIS_MAX )
	{
		strcpy( HisBuff[HisCount], s );
		HisCount++;
	}
	else
	{
		for( loop = 1; loop < HIS_MAX ; loop++ )
		{
			strcpy( HisBuff[loop-1], HisBuff[loop] );
		}
		strcpy( HisBuff[HIS_MAX-1], s );
		HisIndex = HisCount-1;
	}

	HisIndex = 0;
	if( HisCount ) HisIndex = HisCount-1;

	return HisCount;
}

int gets_his(char *s, uint8_t *vcomRecvBuf, uint32_t revsize)
{
	char  *fp;
	char  c;
	int cnt = 0;

	fp = s;

	for(cnt=0; cnt < revsize; cnt++)
	{
		c = vcomRecvBuf[cnt];

		if( c == '\r' ) {
			*s = 0;
			if( strlen( fp ) )
				his_append( fp );
			break;
		}
		*s++ = c;
	}

	return cnt;
}

static int parse_args(char *cmdline, char **argv)
{
	char *tok;
	int argc = 0;

	argv[argc] = NULL;

	for (tok = strtok(cmdline, delim); tok; tok = strtok(NULL, delim))
	{
		argv[argc++] = tok;
	}

	return argc;
}

void UpperStr(char *Str )
{
   while( *Str ){ *Str = toupper( *Str ); Str++; }
}

void shellTask(uint8_t *vcomRecvBuf, uint32_t revsize )
{
	char ReadBuffer[256];	//512
	char *argv[128];			//128
	int argc;
	int cmdlp;
	int cnt;

	memset( ReadBuffer, 0 , sizeof( ReadBuffer ) );
	gets_his( ReadBuffer , vcomRecvBuf, revsize);

	argc = parse_args( ReadBuffer, argv );

	if(argc)
	{
		UpperStr( argv[0] );
		cmdlp = 0;

		for(cnt=0; cnt < CMD_MAX; cnt++)		//cmds size
		{
			if( strcmp( argv[0], Cmds[cmdlp].CmdStr ) == 0 )
			{

				Cmds[cmdlp].func( argc, argv );
				break;
			}

			if( strcmp( "NULL", Cmds[cmdlp].CmdStr ) == 0 )
			{
				TRACE_VCOM(WRONG);
				break;
			}
			cmdlp++;
		}
	}
}
#elif (ENABLE_UART_CMD_PROCESS == DEBUG_UART_MOME)
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
    getValueDAC = setValueDAC = LED_ON_DAC_INIT_Y;
    getADCSel = setADCSel = 0;
    dltThresholdEnbale = 0;
    Scan_Initialize();
    TRACE_UART("\r\n***Please press 123 to enter UART command Mode***\r\n\n");
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
        if (rcvByte == '\r')
        {
        	TRACE_UART("\n");
       	}
        else
        {
        	TRACE_UART("%c", rcvByte);
        }
        //TRACE_UART("%c (%02x)\r\n", rcvByte, rcvByte);
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
        TRACE_UART("%c", rcvChar);
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



void print_scan_threshold(const char *dbgStr, uint8_t axis, int16_t ledIdx, int16_t pdIdx)
{
    if (!getPdLedIdx_test_enable) return; //nsmoon@211230

    if (showCnt_test > 0) {
        uint8_t max_cell = (axis == X_AXIS) ? X_CELL_SIZE : Y_CELL_SIZE;
        if (pdIdx >= max_cell || ledIdx >= max_cell) {
            TRACE_UART("ERROR! invalid ledIdx.. %d/%d/%d", axis, ledIdx, pdIdx);
            return;
        }
        max_cell = (scan_axis_test == X_AXIS) ? X_CELL_SIZE : Y_CELL_SIZE;
        if (pdIdx >= max_cell || ledIdx_test >= max_cell) {
            TRACE_UART("ERROR! invalid ledIdx_test.. %d/%d/%d", scan_axis_test, ledIdx_test, pdIdx_test);
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
                TRACE_UART("ERROR! print_scan_threshold..invalid offsetIdx.. %d/%d/%d %d/%d", axis, ledIdx, pdIdx, offset, offsetIdx);
                return;
            }
#if 0
            if (scan_axis_test == X_AXIS) {
                TRACE_UART("%s %d/%d %d/%d/%d %d/%d\r\n", dbgStr, ledIdx, pdIdx, xScanThresholdData[pdIdx][offsetIdx], xScanResultData[pdIdx][offsetIdx], xScanNonBlockLevel[pdIdx][offsetIdx], tmpSeqIdx, xAxisLedOnTimeIdx[tmpSeqIdx]);
            }
            else {
                TRACE_UART("%s %d/%d %d/%d/%d %d/%d\r\n", dbgStr, ledIdx, pdIdx, yScanThresholdData[pdIdx][offsetIdx], yScanResultData[pdIdx][offsetIdx], yScanNonBlockLevel[pdIdx][offsetIdx], tmpSeqIdx, yAxisLedOnTimeIdx[tmpSeqIdx]);
            }
#else
            if (scan_axis_test == X_AXIS) {
			   TRACE_UART("%s %d/%d %d/%d/%d %d/%d\r\n", dbgStr, ledIdx, pdIdx, xScanThresholdData[pdIdx][offsetIdx], xScanResultData[pdIdx][offsetIdx], xScanNonBlockLevel[pdIdx][offsetIdx], tmpSeqIdx, xAxisDacIdx[tmpSeqIdx]);
		   }
		   else {
			   TRACE_UART("%s %d/%d %d/%d/%d %d/%d\r\n", dbgStr, ledIdx, pdIdx, yScanThresholdData[pdIdx][offsetIdx], yScanResultData[pdIdx][offsetIdx], yScanNonBlockLevel[pdIdx][offsetIdx], tmpSeqIdx, yAxisDacIdx[tmpSeqIdx]);
		   }
#endif
        }
    }
}



#if 0 //nsmoon@211213
static void getPdLedIdx_test(void)
{
    uint8_t rcvUint8;
    TRACE_UART("Axis(0=X, 1=Y): ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
    scan_axis_test = (rcvUint8)? Y_AXIS : X_AXIS; //0:x-axis
    TRACE_UART("LED Index: ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
    ledIdx_test = rcvUint8;
    TRACE_UART("PD Index: ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
    pdIdx_test = rcvUint8;
#if 0
    TRACE_UART("SCAN Delay: ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
    scan_delay_test = rcvUint8;
#endif
}

#if 0
static void scan_full_test(void)
{
    int cnt = 30;
    TRACE_UART("scan_full_test..(%d)\r\n", cnt);
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
            TRACE_UART("X==%d/%d/%d %d\r\n", ledIdx_test, pdIdx_test, xScanResultData[pdIdx_test][offsetIdx], scan_delay_test);
        }
        else {
            int16_t offsetIdx = (ledIdx_test - pdIdx_test) + Y_MAX_OFFSET;
            TRACE_UART("Y==%d/%d/%d %d\r\n", ledIdx_test, pdIdx_test, yScanResultData[pdIdx_test][offsetIdx], scan_delay_test);
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

#define DISPLAY_SCAN_RESULT_DATA() { \
    TRACE_UART(" %c,%03d,%03d, ", axisChar, seqNo, onTimeIdx); \
    TRACE_UART("%03d,%03d,%03d, ", ledIdx, pdIdx, offsetIdx); \
    if (pdIdx == maxCellSize || offsetIdx == offsetIdxMax) { \
        TRACE_UART("---,---,--- "); \
    } \
    else { \
        if (axis == X_AXIS) { \
            TRACE_UART("%03d,%03d,%03d, ", xScanResultData[pdIdx][offsetIdx], xScanNonBlockLevel[pdIdx][offsetIdx], xScanThresholdData[pdIdx][offsetIdx]); \
        } \
        else { \
            TRACE_UART("%03d,%03d,%03d, ", yScanResultData[pdIdx][offsetIdx], yScanNonBlockLevel[pdIdx][offsetIdx], yScanThresholdData[pdIdx][offsetIdx]); \
        } \
    } \
    TRACE_UART("\r\n"); \
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
        //onTimeIdx = xAxisLedOnTimeIdx[seqNo];
        onTimeIdx = xAxisDacIdx[seqNo];				//YJ@230314

    }
    else {
        axisChar = 'Y';
        offsetIdxMax = Y_TOTAL_OFFSET;
        maxCellSize = Y_CELL_SIZE;
        //maxOffset = Y_MAX_OFFSET;
        ledIdx = yAxisSequenceTbl[seqNo][1];
        //onTimeIdx = yAxisLedOnTimeIdx[seqNo];
        onTimeIdx = yAxisDacIdx[seqNo];				//YJ@230314
    }
    //TRACE_UART("Axis:%c SeqNo:%d OnTime:%d\r\n", axisChar, seqNo, onTimeIdx);
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
                    TRACE_UART("%02d,%02d, ", diffLevel, thresholdData);
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
                    TRACE_UART("%02d,%02d,", thLevel, thDiff);
                    DISPLAY_SCAN_RESULT_DATA();
                }
#endif
            }
        }
    }
    //TRACE_UART("\r\n");
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
    TRACE_UART("..done (%d)\r\n", cnt);
}
#endif

static void getThreshold_wSeqNo(void)
{
    uint8_t rcvUint8, axis;
    uint16_t seqNo, seqNoMax;
    int rcvUint;
    TRACE_UART("Axis (0=X, 1=Y): ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
    axis = (rcvUint8)? Y_AXIS : X_AXIS; //0:x-axis
    TRACE_UART("Seq No: ");
    readStr();
    sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint);
    seqNo = (uint16_t)rcvUint;
    seqNoMax = (axis == X_AXIS) ? X_TOTAL_SCAN_STEPS : Y_TOTAL_SCAN_STEPS;
    if (seqNo >= seqNoMax) {
        TRACE_UART("ERROR! invalid seqNo %d\r\n", seqNo);
        return;
    }
    displayScanResult(axis, seqNo, 1);
}
#endif

static void DAC_getValue(void)
{
    uint8_t rxDacValue;

    rxDacValue = dac_data.VAL;

    TRACE_UART("DAC Current Value: 0x%04x \r\n", rxDacValue);

}

int16_t uartDACTest(void)
{
    int32_t commandNo;
    uint16_t setValueDAC, getValueDAC;
    //uint16_t rcvValue;
    //uint8_t rcvChar;

    while(1) {
        TRACE_UART("\r\n");
        TRACE_UART("----------------------------------------------------------------\r\n");
        TRACE_UART("Uart DAC Test\r\n");
        TRACE_UART("----------------------------------------------------------------\r\n");
        TRACE_UART(" 1) Read DAC Value \r\n");
        TRACE_UART(" =) increase DAC Value \r\n");
        TRACE_UART(" -) decrease DAC value \r\n");
        TRACE_UART(" Esc) Exit\r\n");
        TRACE_UART("----------------------------------------------------------------\r\n");
        TRACE_UART("\r\n");


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
            TRACE_UART("increase>> Set DAC value : 0x%04x \r\n", setValueDAC);
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
            TRACE_UART("decrease>> set DAC value : 0x%04x \r\n", setValueDAC);
            //rcvValue = setValueDAC;
            break;
        default:
            break;
        }

        if (commandNo >= '0' && commandNo <= '9') {
            continue;
        } else if (commandNo == 0x1B) { // if key is ESC
            TRACE_UART("\r\nGood Bye !!!\r\n");
            break;
        }

    }

    return 0;

}

static void uartCmdProcessMenu(void)
{
    TRACE_UART("\r\n");
    TRACE_UART("-----------------------------------------------\r\n");
    TRACE_UART("UART Command Process\r\n");
    TRACE_UART("-----------------------------------------------\r\n");
    TRACE_UART(" h) Display This Menu\r\n");
    TRACE_UART(" a) AGC On/Off + Fixed Current mode\r\n");
   // TRACE_UART(" e) ExtLightNoiseCal On/Off\r\n");
    //TRACE_UART(" s) Scan Test\r\n");
    //TRACE_UART(" t) Scan Seq Test\r\n");
    //TRACE_UART(" f) Fixed Current Mode\r\n");
    //TRACE_UART(" j) Decrease PD number Triger\r\n");
    //TRACE_UART(" k) Increase PD number Triger\r\n");
    //TRACE_UART(" L) Toggle Axis Triger\r\n");
    TRACE_UART("   -) Decrease DAC Index\r\n");
    TRACE_UART("   =) Increase DAC Index\r\n");
    TRACE_UART("   _) Decrease PD Gain Index\r\n");
    TRACE_UART("   +) Increase PD Gain Index\r\n");
    TRACE_UART(" b) DLT Threshold Display\r\n");
    TRACE_UART(" d) Display AGC Result (PD gain, Dac)\r\n");
    //TRACE_UART("    [) Decrease X On Time\r\n");
    //TRACE_UART("    ]) Increase X On Time\r\n");
    //TRACE_UART("    {) Decrease Y On Time\r\n");
    //TRACE_UART("    }) Increase Y On Time\r\n");
    //TRACE_UART(" m) Scan Result All\r\n");
    //TRACE_UART(" n) Scan Result (SeqNo)\r\n");
    //TRACE_UART(" p) Scan Result (pdIdx,ledIdx)\r\n");
    //TRACE_UART(" P) Scan Result Display On/Off\r\n");
    TRACE_UART(" c) Forced AGC On\r\n");
    //TRACE_UART(" D) DAC Voltage Value control\r\n");

    //TRACE_UART(" =) increase DAC Value \r\n");
    //TRACE_UART(" -) decrease DAC Value \r\n");
    TRACE_UART("-----------------------------------------------\r\n");
    TRACE_UART("\r\n");
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
    TRACE_UART("Exit UART Command Process~~\r\n\r\n");
    uartCmdModeCnt = 0;
    fixedCurrentEnable = 0;
#if (WATCHDOG_ENABLE)
    WDT_Enable();
#endif
}


uint16_t uartCmdProcess(void)
{
    char key;
#if 0
    if(kStatus_LPUART_FlagCannotClearManually != LPUART_GetStatusFlags(LPUART1_PERIPHERAL))
    {
        key = getchar();
#else
	key =  DbgConsole_Getchar();	//YJ@230314
	if(key)
	{
#endif
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
#if 0 //nsmoon@211213
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
                TRACE_UART("\r\ngetPdLedIdx_test OFF\r\n");
            }
            else {
                getPdLedIdx_test_enable = 1;
                TRACE_UART("\r\ngetPdLedIdx_test ON\r\n");
            }
            break;
#endif
        case 'a' :
        case 'A' :
            if (scanStatus.isUsualAgcCal == TRUE) {
				scanStatus.isUsualAgcCal = FALSE;
                fixedCurrentEnable = 1;
                TRACE_UART("\r\nAGC OFF\r\n");
            } else {
                scanStatus.isUsualAgcCal = TRUE;
                fixedCurrentEnable = 0;
                TRACE_UART("\r\nAGC ON\r\n");
            }
            break;
        case 'b' :
        case 'B' :
		   if (dltThresholdEnbale == 0) {
			   dltThresholdEnbale = 1;
			   TRACE_UART("\r\nDLT THRESHOLD DISPLAY ON\r\n");
		   } else {
			   dltThresholdEnbale = 0;
			   TRACE_UART("\r\nDLT THRESHOLD DISPLAY OFF\r\n");
		   }
		   break;


#if FORCE_AGC_SET
        case 'c' :
        case 'C' :
            //g_forcedagc.forced_agc = 1;
        	g_forcedagc.forced_saved =1;
            break;
#endif
        case 'd' : // if key is 'd'
        case 'D' : // if key is 'D'
            // Display sense information related to current mode
            //uartCmdDisplayInformation();
            // TRACE_UART("D command\r\n");
            // kjs 210414 add
              displayLedAgcResult();
            break;
#if 0
        case 'e' :
        case 'E' :
            if (scanStatus.isUsualExtLightNoiseCal == TRUE) {
                scanStatus.isUsualExtLightNoiseCal = FALSE;
                TRACE_UART("\r\nExtLightNoiseCal OFF\r\n");
            } else {
                scanStatus.isUsualExtLightNoiseCal = TRUE;
                TRACE_UART("\r\nExtLightNoiseCal ON\r\n");
            }
            break;


        case 'm' :
        case 'M' :
            // displayScanResultAll();
            break;
 //ENABLE_TRIGER
        case 'j' :
        case 'J' :
            if (Current_Triger_Pd > CELLS_PER_CS) {
                Current_Triger_Pd -= CELLS_PER_CS;
            }
            else {
                Current_Triger_Pd = 0;
            }
            TRACE_UART("Current Triger PD number is (%d) \r\n", Current_Triger_Pd);
            break;
        case 'k' : // if key is 'k'
        case 'K' : // if key is 'K'
            if (Current_Triger_Pd < (X_CELL_SIZE - CELLS_PER_CS - 1)) {
                Current_Triger_Pd += CELLS_PER_CS;
            }
            else {
                Current_Triger_Pd = X_CELL_SIZE - 1;
            }
            TRACE_UART("Current Triger PD number is (%d) \r\n", Current_Triger_Pd);
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
            TRACE_UART("Current Triger AXIS is (%c) \r\n", axis_char);
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
            TRACE_UART("USB status is (%c) \r\n", usb_char);
            break;
#endif
#if 0
        case 'p' : // if key is 'p'
        case 'P' : // if key is 'P'
            // parameter settings
            //uartCmdParameters();
            TRACE_UART("P command\r\n");
            break;
//#endif
        case 's' : // if key is 's'
        case 'S' : // if key is 'S'
            // Scan Test
            uartScanTest();
            break;
//#if 1 //nsmoon@210719
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
//        TRACE_UART("\n\n");
//        TRACE_UART("Model Name       : %s\n", ModelName);
//        TRACE_UART("Firmware Version : %s\n", FirmwareVersion);
//        TRACE_UART("Compile time is %s, %s\n\n", __TIME__, __DATE__);
            break;
#endif
#if 0 // 1 //nsmoon@210915
        case 'f' :
        case 'F' :
            if (fixedCurrentEnable) {
                fixedCurrentEnable = 0;
                g_forcedagc.forced_agc = 1;
                TRACE_UART("Fixed Current OFF\r\n");
            } else {
                fixedCurrentEnable = 1;
                TRACE_UART("Fixed Current ON (%d,%d) (%d,%d)\r\n", Current_Test_index_X, Led_On_Time_Test_X, Current_Test_index_Y, Led_On_Time_Test_Y);
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
            //TRACE_UART("increase>> Set DAC value : %d \r\n", setValueDAC);
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
            //TRACE_UART("decrease>> set DAC value : %d \r\n", setValueDAC);
            break;
        case '+' :
            if (getADCSel >= 4) {
                setADCSel = 4;
            } else {
                setADCSel = ++getADCSel;
            }
            uartPDGainCtrl(setADCSel);
            //TRACE_UART("increase>> Set DAC value : 0x%04x \r\n", setValueDAC);
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
            if (setValueDAC >= 255) {
                setValueDAC = 255;
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
            if (setADCSel >= LED_SINK_CONTROL_STEPS-1) {
                setADCSel = LED_SINK_CONTROL_STEPS-1;
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
        	fixedCurrentEnable = 1;
        	getValueDAC = setValueDAC;
            getADCSel = setADCSel;
            TRACE_UART(">> DAC value : %d, PD GAIN: %d \r\n", setValueDAC, setADCSel);
        }
#if 0

        if (fixedCurrentEnable) {
            if (key == '_' || key == '-' || key == '+' || key == '=' ||
                    key == '{' || key == '[' || key == '}' || key == ']') {
                TRACE_UART("Fixed Current ON (%d,%d) (%d,%d)\r\n", Current_Test_index_X, Led_On_Time_Test_X, Current_Test_index_Y, Led_On_Time_Test_Y);
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
        TRACE_UART("\r\n");
        TRACE_UART("----------------------------------------------------------------\r\n");
        TRACE_UART("Uart Scan Test\r\n");
        TRACE_UART("----------------------------------------------------------------\r\n");
        TRACE_UART(" Press 1 ~ 5 to change parameter or s to start scan\r\n");
        TRACE_UART(" 1) Axis = %s\r\n", axisType == X_AXIS ? "X" : "Y");
        TRACE_UART(" 2) LED index No = %d\r\n", ledIdx);
        TRACE_UART(" 3) PD Start index No = %d\r\n", pdStartIdx);
        TRACE_UART(" 4) LED On/Off = %s\r\n", bLedOn == TRUE ? "On" : "Off");
        TRACE_UART(" 5) Loop delay = %d [msec]\r\n", loopDelayMsec);
        TRACE_UART(" s) Start scan\r\n");
        TRACE_UART(" Esc) Exit\r\n");
        TRACE_UART("----------------------------------------------------------------\r\n");
        TRACE_UART("\r\n");
        //TRACE_UART("Enter command number : ");

        commandNo = _mon_getc(1);

        switch(commandNo) {
        case '1' :
            TRACE_UART("Enter Axis Type [ x / y ] = ");
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
            TRACE_UART("Enter LED Index No in the axis = ( 0 ~ %d ) = ", cellSize - 1);
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
            TRACE_UART("Enter PD Start Index No in the axis ( %d ~ %d ) = ", pdIdxMin, pdIdxMax);
            readStr();
            sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
            if (rcvUint8 < pdIdxMin) rcvUint8 = pdIdxMin;
            if (rcvUint8 > pdIdxMax) rcvUint8 = pdIdxMax;
            pdStartIdx = (rcvUint8/3) * 3;
            break;

        case '4' :
            TRACE_UART("Enter LED On/Off [ 1 for On / 0 for Off ] = ");
            readStr();
            sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
            bLedOn = rcvUint8 == 1 ? TRUE : FALSE;
            break;

        case '5' :
            TRACE_UART("Enter Loop Delay [msec] = ");
            readStr();
            sscanf((const char *)readStrBuffer, "%d", (int *)&rcvUint8);
            loopDelayMsec = rcvUint8;
            if (loopDelayMsec == 0) loopDelayMsec = 1;
            break;
        }

        if (commandNo >= '0' && commandNo <= '9') {
            continue;
        } else if (commandNo == 0x1B) { // if key is ESC
            TRACE_UART("\r\nGood Bye !!!\r\n");
            break;
        }

        if (commandNo == 's') {
            TRACE_UART("\t-----> Now scan testing, press ESC key to quit !!!\r\n");
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
    TRACE_UART("\r\n"); \
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
    TRACE_UART("%s %c seq=%d pd=%d led=%d currIdx=%d onTime=%d\r\n", \
        ledOnStr, axisChar, seqNo, pdIdx, ledIdx, \
        currentTestIndex, ledOnTimeTest); \
}

static void uartScanSeqTestMenu(void)
{
    TRACE_UART("\r\n");
    TRACE_UART("-------------------------------\r\n");
    TRACE_UART("UART Scan Seq Test\r\n");
    TRACE_UART("-------------------------------\r\n");
    TRACE_UART(" H) Display This Menu\r\n");
    TRACE_UART(" A) Axis Toggle\r\n");
    TRACE_UART(" L) LED On/Off\r\n");
    TRACE_UART(" D) Display ADC Value\r\n");
    TRACE_UART(" 1,2) Sequence No +,-\r\n");
    TRACE_UART(" 3,4) Sequence No +10,-10\r\n");
    TRACE_UART(" 5,6) Sequence No +50,-50\r\n");
    TRACE_UART(" +,-) Led Current Index\r\n");
    TRACE_UART(" [,]) Led On Time\r\n");
    TRACE_UART(" Esc) Exit\r\n");
    TRACE_UART("-------------------------------\r\n");
    TRACE_UART("gCurrentStartTotalPdIdx=%d\r\n", gCurrentStartTotalPdIdx);
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
    TRACE_UART("\r\n");

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
                TRACE_UART("\r\nGood Bye !!!\r\n");
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
        TRACE_UART("Fixed Current ON (%d,%d) (%d,%d)\r\n", Current_Test_index_X, Led_On_Time_Test_X, Current_Test_index_Y, Led_On_Time_Test_Y);
    }
    else {
        TRACE_UART("Fixed Current OFF\r\n");
    }

    return 0;
}
#endif //1
#endif
