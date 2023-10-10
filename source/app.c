/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "backend.h"
//#include "usb_host_config.h"
//#include "usb_host.h"
#include "fsl_device_registers.h"
//#include "usb_host_vendor.h"
#include "board.h"
//#include "host_vendor_generic.h"
#include "fsl_common.h"
#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include "fsl_sysmpu.h"
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

//#if ((!USB_HOST_CONFIG_KHCI) && (!USB_HOST_CONFIG_EHCI) && (!USB_HOST_CONFIG_OHCI) && (!USB_HOST_CONFIG_IP3516HS))
//#error Please enable USB_HOST_CONFIG_KHCI, USB_HOST_CONFIG_EHCI, USB_HOST_CONFIG_OHCI, or USB_HOST_CONFIG_IP3516HS in file usb_host_config.
//#endif

#include "peripherals.h"
#include "pin_mux.h"
#include "usb_phy.h"
#include "clock_config.h"

#include "composite.h"
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
#include "hid_generic.h"
#include "hid_mouse.h"
#endif
#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
#include "virtual_com.h"
#endif

#if (MODEL_SPT == MODEL_CTSK_N650_V100)||(MODEL_SPT == MODEL_CTSK_N750_V100)||(MODEL_SPT == MODEL_CTSK_N850_V100)
#include "app.h"
#include "Scan.h"
#include "dlt.h"
#include "pitch_table.h"
#include "gpio.h"
#include "macro.h"
#include "adc.h"
//#include <test_data_1p.h>
#endif

#include "backend_postp.h"
#include "speed_test_float.h" //nsmoon@210205

#include "uartCommand.h"


/*******************************************************************************
* Definitions
******************************************************************************/
#if 1 //gpt, nsmoon@190114
#include "fsl_gpt.h"
#define GPT_TIME_SCALE_100US	7500 //7200
#define GPT_TIME_SCALE_10US	    750 //nsmoon@220124

#define GPT_IRQ_ID GPT1_IRQn
#define MY_GPT GPT1
#define MY_GPT_IRQHandler GPT1_IRQHandler

/* Select IPG Clock as PERCLK_CLK clock source */
#define MY_GPT_CLOCK_SOURCE_SELECT (0U)
/* Clock divider for PERCLK_CLK clock source */
#define MY_GPT_CLOCK_DIVIDER_SELECT (14U)
/* Get source clock for GPT driver (GPT prescaler = 0) */
#define MY_GPT_CLK_FREQ (CLOCK_GetFreq(kCLOCK_IpgClk) / (MY_GPT_CLOCK_DIVIDER_SELECT + 1U))

gpt_config_t gpt_config;
#endif //gpt

//#define TEST_POINT1_HIGH GPIO_PinWrite(DEBUG_TP_GPIO, IRT_DEBUG_PIN1_POS, HIGH);
//#define TEST_POINT1_LOW GPIO_PinWrite(DEBUG_TP_GPIO, IRT_DEBUG_PIN1_POS, LOW);

uint8_t DltParam[USB_PARAM_LEN];
uint8_t LineThresholdValue;

#if 1 //nsmoon@200706
uint8_t verStrBe[MAX_VER_STRING];
#if (MODEL_SPT == MODEL_CTSK_N650_V100)					//YJ@230222
const uint8_t verStrFe[MAX_VER_STRING] = {'3','0'} ;
#elif (MODEL_SPT == MODEL_CTSK_N750_V100)					//YJ@230222
const uint8_t verStrFe[MAX_VER_STRING] = {'2','0'} ;
#elif (MODEL_SPT == MODEL_CTSK_N850_V100)					//YJ@230222
const uint8_t verStrFe[MAX_VER_STRING] = {'1','0'} ;
#else
uint8_t verStrFe[MAX_VER_STRING];
#endif
#endif

uint8_t usbCmdBuf[USB_NORCMD_LENGHT];

float s_sensor_zero_x, s_sensor_zero_y, s_sensor_end_x, s_sensor_end_y;
float s_aarea_zero_x, s_aarea_zero_y, s_aarea_end_x, s_aarea_end_y;
float s_logical_max_x, s_logical_max_y;

uint8_t verReadFlag;	//YJ@211221

#ifdef WAIT_FOR_READ_VERSION //nsmoon@211222
#define FRAME_COUNT_MAX	   5000  //about 5sec
uint32_t frameCount;
#endif
uint8_t contTouch, contTouch2, contError; //nsmoon@211231

#if ENABLE_SCAN_HIGH_GAIN_DLT //nsmoon@220420
uint8_t scanGainHigh;
#endif

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t g_usb_in_buff[EXB_XFER_BUF_SIZE];
backend_config_t g_back_conf;
forcedagc g_forcedagc;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief host callback function.
 *
 * device attach/detach callback function.
 *
 * @param deviceHandle         device handle.
 * @param configurationHandle  attached device's configuration descriptor information.
 * @param eventCode            callback event code, please reference to enumeration host_event_t.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_NotSupported         The application don't support the configuration.
 */
#if (MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode);
#endif

/*!
 * @brief app initialization.
 */
#if (MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
static usb_status_t USB_HostApplicationInit(void);
#endif

#if 1 //nsmoon@190123
ATTR_BACKEND_RAM3  DEF_DATA_INFO s_output_buffer;
ATTR_BACKEND_RAM3  DEF_DATA_INFO2 s_output_buffer2;
ATTR_BACKEND_RAM3  DEF_PD_INFO s_input_buffer;
ATTR_BACKEND_RAM2  DEF_PD_INFO blocked_info;
ATTR_BACKEND_RAM3  DEF_PD s_hor_touch_pd[MAX_HOR_TOUCH_PD];
ATTR_BACKEND_RAM3  DEF_PD s_ver_touch_pd[MAX_VER_TOUCH_PD];
#ifdef FRONTEND_LINE_THRESHOLD
ATTR_BACKEND_RAM3 uint8_t s_hor_line_threshold[MAX_HOR_LINE_THRESHOLD];
ATTR_BACKEND_RAM3 uint8_t s_ver_line_threshold[MAX_VER_LINE_THRESHOLD];
#endif
ATTR_BACKEND_RAM3 static DEF_TOUCH_OUT s_touch_output[ALLOWABLE_TOUCH_IO];
//ATTR_BACKEND_RAM3  static uint8_t s_usb_out_buff[MAX_USB_OUT_BUF_SIZE];
#if 0 //nsmoon@210603
ATTR_BACKEND_RAM3  static int8_t s_offset_table_x[MAX_NUM_SLOPES_DLT_X];
ATTR_BACKEND_RAM3  static int8_t s_offset_table_y[MAX_NUM_SLOPES_DLT_Y];
#endif
#if 1 // (ENABLE_DEBUG_MESSAGE == 1)
ATTR_BACKEND_RAM3  __attribute__((aligned(4))) float s_led_pos_x[MAX_NUM_PD_X_DLT];
ATTR_BACKEND_RAM3  __attribute__((aligned(4))) float s_pd_pos_x[MAX_NUM_PD_X_DLT];
ATTR_BACKEND_RAM3  __attribute__((aligned(4))) float s_led_pos_y[MAX_NUM_PD_Y_DLT];
ATTR_BACKEND_RAM3  __attribute__((aligned(4))) float s_pd_pos_y[MAX_NUM_PD_Y_DLT];
#endif
//ATTR_BACKEND_RAM3  static uint64_t s_shiftTbl64[MAX_SHIFT_TBL];
//static void initShiftTbl64(void);
//static void get_slope_mask(axis_t axis, uint16_t slope_idx, slope_mask_t *sm);
//static void get_slope_val(axis_t axis, int stIdx, int pd, unsigned char *pRcv, slope_mask_t *slopeVal);

#if 0 //for test
#define APP_TIME_LEN 5
#define APP_TIME_STR_LEN 80
uint32_t curTime, prevTime;
uint32_t app_time[APP_TIME_LEN];
char app_time_str[APP_TIME_STR_LEN];
#define APP_TIME_INIT()	{prevTime=DWT->CYCCNT;}
#define APP_TIME_ADD(a)	{ \
	curTime=DWT->CYCCNT; \
	if ((a) < APP_TIME_LEN)	app_time[(a)]=(curTime-prevTime); \
	prevTime=DWT->CYCCNT; \
}
#define APP_TIME_PRINT() { \
	int i, idx; \
	app_time_str[0] = '\0'; \
	for (i =0; i < APP_TIME_LEN; i++) { \
		idx = strlen(app_time_str); \
		snprintf(&app_time_str[idx], (APP_TIME_STR_LEN-idx), "%ld,", app_time[i]/6); \
    } \
	DbgConsole_Printf("@@@,%s,\r\n", app_time_str); \
}
#else
#define APP_TIME_INIT()
#define APP_TIME_ADD(a)
#define APP_TIME_PRINT()
#endif
//#define BACKEND_TEST_DATA //for test
#ifdef BACKEND_TEST_DATA
#include <test_data_4p.h>
ATTR_BACKEND_RAM3  DEF_PD s_hor_touch_pd2[MAX_TEST_DATA_X];
ATTR_BACKEND_RAM3  DEF_PD s_ver_touch_pd2[MAX_TEST_DATA_Y];

static void Make_Test_Data(void)
{
    uint8_t *pRcv = &Exb_Send_Buf[0];
    int i, offset, byteIdx, bitIdx, bufIdx, stIdx;

    memset(pRcv, 0 , EXB_XFER_BUF_SIZE); //clear
    Send_ExbBit_Data_to_USBCPU_Init();  //buffer clear
    stIdx = PD_DATA_IDX2;
    for (i = 0; i < MAX_TEST_DATA_X; i++) {
        if (test_data_x[i][0] < 0 || test_data_x[i][1] < 0) {
            break;
        }
        offset = test_data_x[i][1] - test_data_x[i][0]; //led-pd
        byteIdx = (offset + X_MAX_OFFSET) / 8;
        bitIdx = (offset + X_MAX_OFFSET) % 8;
        bufIdx = stIdx + test_data_x[i][0] * MAX_SLOPE_BYTE_X + byteIdx;
        pRcv[bufIdx] |= (1 << bitIdx);
    }

    stIdx += g_back_conf.numPdX * g_back_conf.slope_byte_x;
    for (i = 0; i < MAX_TEST_DATA_Y; i++) {
        if (test_data_y[i][0] < 0 || test_data_y[i][1] < 0) {
            break;
        }
        offset = test_data_y[i][1] - test_data_y[i][0]; //led-pd
        byteIdx = (offset + Y_MAX_OFFSET) / 8;
        bitIdx = (offset + Y_MAX_OFFSET) % 8;
        bufIdx = stIdx + test_data_y[i][0] * MAX_SLOPE_BYTE_Y + byteIdx;
        pRcv[bufIdx] |= (1 << bitIdx);
    }

#ifdef FRONTEND_LINE_THRESHOLD
    stIdx += g_back_conf.numPdY * g_back_conf.slope_byte_y;
    for (i = 0; i < MAX_TEST_DATA_X; i++) {
        if (test_data_x[i][0] < 0 || test_data_x[i][1] < 0) {
            break;
        }
        offset = test_data_x[i][1] - test_data_x[i][0]; //led-pd
        byteIdx = (offset + X_MAX_OFFSET) / 8;
        bitIdx = (offset + X_MAX_OFFSET) % 8;
        bufIdx = stIdx + test_data_x[i][0] * MAX_SLOPE_BYTE_X + byteIdx;
        pRcv[bufIdx] |= (1 << bitIdx);
    }

    stIdx += g_back_conf.numPdX * g_back_conf.slope_byte_x;
    for (i = 0; i < MAX_TEST_DATA_Y; i++) {
        if (test_data_y[i][0] < 0 || test_data_y[i][1] < 0) {
            break;
        }
        offset = test_data_y[i][1] - test_data_y[i][0]; //led-pd
        byteIdx = (offset + Y_MAX_OFFSET) / 8;
        bitIdx = (offset + Y_MAX_OFFSET) % 8;
        bufIdx = stIdx + test_data_y[i][0] * MAX_SLOPE_BYTE_Y + byteIdx;
        pRcv[bufIdx] |= (1 << bitIdx);
    }
#endif
}
#endif //BACKEND_TEST_DATA

static const uint16_t crc_table[16] =
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

uint16_t USB_CalculateCrc(uint8_t *data, uint32_t len)
{
	uint32_t i;
	uint16_t crc = 0;

	while (len--)
	{
		i = (crc >> 12) ^ (*data >> 4);
		crc = crc_table[i & 0x0F] ^ (crc << 4);
		i = (crc >> 12) ^ (*data >> 0);
		crc = crc_table[i & 0x0F] ^ (crc << 4);
		data++;
	}

	return (crc & 0xFFFF);
}

static int setPdData(axis_t axis, int pd, int offSetIdx, int pdIdx);
#if (MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
static int backend_process_ofst_data(void);
#endif
#endif

#ifdef FRONTEND_FORCED_AGC
#define BPLD_NOR_MODE	0
#define BPLD_DLT_MODE	1

#if (MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
static int backend_process_line_data(int dltMode);
#else
//static int backend_process_line_data(void);
int backend_process_line_data(void);
#endif
#else
static int backend_process_line_data(void);
#endif

#if (DEAD_CELL_TBL == 1) //nsmoon@190829
DEF_DEADCELL_INFO deadcell_info; //nsmoon@190829
ATTR_BACKEND_RAM3  uint8_t deadCellHorLed[MAX_DEADCELL_SIZE_X];
ATTR_BACKEND_RAM3  uint8_t deadCellHorPd[MAX_DEADCELL_SIZE_X];
ATTR_BACKEND_RAM3  uint8_t deadCellVerLed[MAX_DEADCELL_SIZE_Y];
ATTR_BACKEND_RAM3  uint8_t deadCellVerPd[MAX_DEADCELL_SIZE_Y];
#endif

#if (MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
#if (SECOND_OFST_TBL == 1) //nsmoon@190624
static uint8_t sensor_table_x2[MAX_NUM_2ND_SENSOR_X];
static uint8_t sensor_table_y2[MAX_NUM_2ND_SENSOR_Y];
#endif
#endif

#if 1 //nsmoon@191022
SCAN_STATUS_T scan_status;
app_state_t appRunState;
BACKEND_STATE_T backendRunState;
static app_mode_t appMode, appCurrMode, appNextMode;
touch_mode_t touchMode;
#ifdef WMC_MODE_CHANGE //YJ@230207
touch_mode_t touchModePrev;
#endif
//static app_cmd_state_t appCmdState;
#endif

brush_last_send brushLastSend;

#ifdef NORMAL_RESET_DELAY //nsmoon@200810
#define RESET_DELAY_DOT_CNT			10	//1000
#define MAX_RESET_DELAY_CNT			2	//3
static int s_reset_delay;
static unsigned int s_reset_delay_dot;
#else
uint32_t do_reset_cnt; //DL-TEST@200604
#endif

#define DEV_INIT_DELAY    20000 //nsmoon@220420
#if(MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
static int s_devinit_delay; //nsmoon@201211
#endif

ATTR_BACKEND_RAM3	uint32_t debug_count;

void BOARD_InitHardware(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief USB host generic instance global variable */
//static usb_host_handle g_HostHandle;

extern void USB_DeviceEhciIsrFunction(void *deviceHandle); //nsmoon@190110
extern void USB_DeviceEhciGetSofIntCnt(uint32_t *cnt); //nsmoon@201211

//static app_data_t s_appData;

#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
extern void USB_DeviceCdcVcomTask(void);
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

#if 1 //nsmoon@191025
#include "fsl_wdog.h"
#define MY_WDOG_BASE		WDOG1
#define MY_WDOG_IRQHandler	WDOG1_IRQHandler
#define TRACE_WDOG(...)		//usb_echo(__VA_ARGS__);
#define TRACE_APP(...)		//DEBUG_PRINTF(__VA_ARGS__);
#define TRACE_DBG(...)		//DBG_PRINTF(__VA_ARGS__);

static wdog_config_t wdog_config;

void MY_WDOG_IRQHandler(void)
{
    WDOG_ClearInterruptStatus(MY_WDOG_BASE, kWDOG_InterruptFlag);
    /* User code. User can do urgent case before timeout reset.
     * IE. user can backup the ram data or ram log to flash.
     * the period is set by config.interruptTimeValue, user need to
     * check the period between interrupt and timeout.
     */
}

void MY_WDOG_system_reset_reason(void)
{
    TRACE_WDOG("System reset by:");
    uint16_t resetFlag = 0U;
    resetFlag = WDOG_GetStatusFlags(MY_WDOG_BASE);
    DEBUG_PRINTF("<%X\r\n", resetFlag);

    switch (resetFlag & (kWDOG_PowerOnResetFlag | kWDOG_TimeoutResetFlag | kWDOG_SoftwareResetFlag))
    {
    case kWDOG_PowerOnResetFlag:
        TRACE_WDOG(" Power On Reset!\r\n");
        break;
    case kWDOG_TimeoutResetFlag:
        TRACE_WDOG(" Time Out Reset!\r\n");
        break;
    case kWDOG_SoftwareResetFlag:
        TRACE_WDOG(" Software Reset!\r\n");
        break;
    default:
        TRACE_WDOG(" Error status!\r\n");
        break;
    }
}

#if (_DEBUG == 1)
#define MY_WDOG_TIMEOUT		0x64U  /* Timeout value is 15.6 sec. */
#else
#define MY_WDOG_TIMEOUT		0xFU  /* Timeout value is 2.5 sec. */
#endif
void MY_WDOG_reset_init()
{
    //PRINTF("System reset by WDOG timeout.\r\n");
    /*
     * wdogConfig->enableWdog = true;
     * wdogConfig->workMode.enableWait = true;
     * wdogConfig->workMode.enableStop = false;
     * wdogConfig->workMode.enableDebug = false;
     * wdogConfig->enableInterrupt = false;
     * wdogConfig->enablePowerdown = false;
     * wdogConfig->resetExtension = flase;
     * wdogConfig->timeoutValue = 0xFFU;
     * wdogConfig->interruptTimeValue = 0x04u;
     */
    WDOG_GetDefaultConfig(&wdog_config);
    wdog_config.timeoutValue = MY_WDOG_TIMEOUT;
    WDOG_Init(MY_WDOG_BASE, &wdog_config);
    TRACE_WDOG("...wdog Init done\r\n");
}

void MY_WDOG_soft_reset()
{
    WDOG_Type *base = (WDOG_Type *)MY_WDOG_BASE;
    base->WCR &= ~WDOG_WCR_SRS_MASK;
}
#endif //1

#if 0 //nsmoon@201108
void USB_OTG1_IRQHandler(void)
{

    if (HOST_CONTROLLER_ID == kUSB_ControllerEhci0) {
        USB_HostEhciIsrFunction(g_HostHandle);
    }
    else {
        USB_DeviceEhciIsrFunction(USB_Get_Device_Handle());
    }
}

void USB_OTG2_IRQHandler(void)
{
    if (HOST_CONTROLLER_ID == kUSB_ControllerEhci1) {
        USB_HostEhciIsrFunction(g_HostHandle);
    }
    else {
        USB_DeviceEhciIsrFunction(USB_Get_Device_Handle());
    }
}
#else
void USB_OTG1_IRQHandler(void)
{
    USB_DeviceEhciIsrFunction(USB_Get_Device_Handle());
}

void USB_OTG2_IRQHandler(void)
{
    USB_DeviceEhciIsrFunction(USB_Get_Device_Handle());
}
#endif
#if 0
void USB_HostClockInit(void)
{
    usb_phy_config_struct_t phyConfig = {
        BOARD_USB_PHY_D_CAL,
        BOARD_USB_PHY_TXCAL45DP,
        BOARD_USB_PHY_TXCAL45DM,
    };

    if (HOST_CONTROLLER_ID == kUSB_ControllerEhci0)
    {
        CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);
    }
    else
    {
        CLOCK_EnableUsbhs1PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs1Clock(kCLOCK_Usb480M, 480000000U);
    }
    USB_EhciPhyInit(HOST_CONTROLLER_ID, BOARD_XTAL0_CLK_HZ, &phyConfig);
}

void USB_HostIsrEnable(void)
{
    uint8_t irqNumber;

    uint8_t usbHOSTEhciIrq[] = USBHS_IRQS;
    irqNumber = usbHOSTEhciIrq[HOST_CONTROLLER_ID - kUSB_ControllerEhci0];
    /* USB_HOST_CONFIG_EHCI */

    /* Install isr, set priority, and enable IRQ. */
#if defined(__GIC_PRIO_BITS)
    GIC_SetPriority((IRQn_Type)irqNumber, USB_HOST_INTERRUPT_PRIORITY);
#else
    NVIC_SetPriority((IRQn_Type)irqNumber, USB_HOST_INTERRUPT_PRIORITY);
#endif
    EnableIRQ((IRQn_Type)irqNumber);
}

void USB_HostTaskFn(void *param)
{
    USB_HostEhciTaskFunction(param);
}
#endif
/*!
 * @brief USB isr function.
 */
#if (MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
static usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode)
{
    usb_status_t status = kStatus_USB_Success;

    switch (eventCode)
    {
    case kUSB_HostEventAttach:
        status = USB_HostVendorGenericEvent(deviceHandle, configurationHandle, eventCode);
        break;

    case kUSB_HostEventNotSupported:
        TRACE("device not supported.");
        break;

    case kUSB_HostEventEnumerationDone:
        status = USB_HostVendorGenericEvent(deviceHandle, configurationHandle, eventCode);
        break;

    case kUSB_HostEventDetach:
        status = USB_HostVendorGenericEvent(deviceHandle, configurationHandle, eventCode);
        break;

    default:
        break;
    }
    return status;
}
#endif

int MY_delay(uint32_t cnt)
{
    volatile uint32_t i = 0;
    uint32_t cntTotal = cnt * 1000;
    for (i = 0; i < cntTotal; ++i)
    {
        __asm("NOP"); /* delay */
    }
    return 0;
}

int delay_test(uint32_t cnt)
{
    volatile uint32_t i = 0;
    volatile float a=1.1f, b=1.2f, c=0;
    uint32_t cntTotal = cnt * 1000;
    for (i = 0; i < cntTotal; ++i)
    {
        __asm("NOP"); /* delay */ //1.6*1000
        c += a * b;
    }
    return c;
}
#if(MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
#define MAX_HOST_INIT_CNT	50000	//1000	//bug-fix	YJ211005
static usb_status_t USB_HostApplicationInit(void)
{
    usb_status_t status = kStatus_USB_Success;

    USB_HostClockInit();

#if ((defined FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT))
    SYSMPU_Enable(SYSMPU, 0);
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

    status = USB_HostInit(HOST_CONTROLLER_ID, &g_HostHandle, USB_HostEvent);
    if (status != kStatus_USB_Success)
    {
        TRACE_ERROR("ERROR! host init error");
        return status;
    }
#ifdef USB_HOST_DELAYED_INT_ENABLE
    uint32_t cnt = 0;
    while(g_HostHandle == NULL)
    {
        if(cnt++ > 0xF000000)		//YJ@211215
        {
            TRACE_ERROR("ERROR! g_HostHandle null");
            MY_WDOG_soft_reset();
        }
    }
    USB_HostEhciIntEnable(g_HostHandle); //nsmoon@211201
#endif
    USB_HostIsrEnable();
    USB_HostVendorGenericInit();

#if	0 //remove unexpected interrupt //nsmoon@201108 //nsmoon@211201 not-used
    unsigned int cnt = 0;
    while (cnt < MAX_HOST_INIT_CNT) {		//bug-fix	YJ211005
        USB_HostTaskFn(g_HostHandle);
        USB_HostVendorGenericTask();
        if (g_HostVendorGeneric.deviceState == kStatus_DEV_Attached) {
            TRACE_ERROR("attached...%d\r\n", cnt);
            break;
        }
        cnt++;
    }
    if (cnt == MAX_HOST_INIT_CNT) {
        TRACE_ERROR("ERROR! USB_HostApplicationInit...timeout!!\r\n");
    }
#else
    usb_debug("host init done...\r\n");
#endif

    return status;
}
#endif

#if 1 //USE_FW_KEY_VALIDATION //nsmoon@191010
//#include "fsl_ocotp.h"
#include "aes.h"
#define MY_OCOTP_FREQ_HZ		(CLOCK_GetFreq(kCLOCK_IpgClk))
#define MY_OCOTP_ADDR_ST		0x18
#define MY_OCOTP_ADDR_ED		0x1F

#define EFUSE_KEY_LEN			0x20
#define FLASH_SECTOR_SIZE		0x1000	/*4KB*/
#define FLASH_BASE_ADDR			FlexSPI2_AMBA_BASE

#define MY_FLASH_SIZE			0x400000 /*4MB*/
#define BOOTLOADER_EEPROM_BASE_ADDRESS  (FLASH_BASE_ADDR + MY_FLASH_SIZE - FLASH_SECTOR_SIZE)

//header0_t size
#define HD_0_SIZE				(64)
#define HD_MODEL_NAME_SIZE		(24)
#define HD_KEY0_SIZE			(16)
#define HD_KEY1_SIZE			(16)

//header1_t size
#define HD_1_SIZE				(64)
#define HD_FW_VERSION_SIZE		(64 - 2) //crc

//header2 size
#define HD_2_SIZE				(64)
#define HD_SN_SIZE				(64 - 1 - 2) //cmd+crc
//#define HD_SIZE_TOT				(HD_0_SIZE + HD_1_SIZE + HD_2_SIZE)

#if 1 //nsmoon@230503
//header3 size
#define HD_3_SIZE				(64)
#define HD_FE_EEPROM_SIZE		(64-1) //cmd
#define HD_SIZE_TOT				(HD_0_SIZE + HD_1_SIZE + HD_2_SIZE + HD_3_SIZE)
#endif

typedef union {
    uint8_t all[HD_1_SIZE];
    struct {
        uint8_t fwVersion[HD_FW_VERSION_SIZE];
        uint16_t hd01_crc;
    } ind;
} header1_t; //64-bytes, eeprom block no-1

static header1_t header1;

#if 0
typedef union {
    uint8_t all[HD_0_SIZE];
    struct {
        uint16_t code_crc;
        uint16_t code_misc;
        uint32_t code_size;
        uint8_t modelName[HD_MODEL_NAME_SIZE];
        uint8_t key0[HD_KEY0_SIZE];
        uint8_t key1[HD_KEY1_SIZE];
    } ind;
} header0_t; //64-bytes, eeprom block no-1

static header0_t header0;

static uint8_t tmpKey0[HD_KEY0_SIZE];
static uint8_t tmpKey1[HD_KEY1_SIZE];
#define MAX_MATCHED_NUM		2
static uint8_t matched[MAX_MATCHED_NUM];


static void get_header0(void)
{
    uint8_t *eeprom = (uint8_t *)BOOTLOADER_EEPROM_BASE_ADDRESS;
    //TRACE("eeprom= %x %x", (uint32_t)eeprom, __base_PROGRAM_FLASH);

    header0_t *hd0 = &header0;
    memcpy(&header0.all[0], eeprom, HD_0_SIZE);
    encrypt_ecb(&tmpKey0[0], &hd0->all[0], HD_KEY0_SIZE);
    encrypt_ecb(&tmpKey1[0], &hd0->all[HD_KEY0_SIZE], HD_KEY1_SIZE);

    memset(&matched[0], 0, MAX_MATCHED_NUM);
    if (memcmp(&tmpKey0[0], &hd0->ind.key0[0], HD_KEY0_SIZE) == 0) {
        matched[0] = 1; //matched
        TRACE("KEY0 is matched!!!!");
    }
    if (memcmp(&tmpKey1[0], &hd0->ind.key1[0], HD_KEY1_SIZE) == 0) {
        matched[1] = 1; //matched
        TRACE("KEY1 is matched!!!!");
    }
}
#endif
static void get_header1(void)
{
    uint8_t *eeprom = (uint8_t *)BOOTLOADER_EEPROM_BASE_ADDRESS + HD_0_SIZE;
    memcpy(&header1.all[0], eeprom, HD_1_SIZE);
#if 0
    memcpy((void*) &frontend_eeprom[0], eepromFE, FE_EEPROM_SIZE); //nsmoon@210908 bug-fix MY_EEPROM_SIZE=>FE_EEPROM_SIZE
    show_frontend_fwinfo();
#else
    get_ver_string();
#endif
}


#if USE_FW_KEY_VALIDATION
static uint8_t mcuKey[EFUSE_KEY_LEN];

__attribute__((section(".bss.$RAM2")))
static uint8_t is_key_matched(uint32_t keyNo)
{
    return matched[(keyNo % MAX_MATCHED_NUM)];
}

void MY_OCOTP_init()
{
#if 0 //for test
    OCOTP_Init(OCOTP, MY_OCOTP_FREQ_HZ);
    memset(&matched[0], 1, MAX_MATCHED_NUM);
    //OCOTP_Deinit(OCOTP);
#else
    /* Initializes OCOTP controller. */
    OCOTP_Init(OCOTP, MY_OCOTP_FREQ_HZ);

#if 0 //(_DEBUG == 1) || defined(DEBUG)
    /* Get the OCOTP controller version. */
    uint32_t version = OCOTP_GetVersion(OCOTP);
    TRACENR("OCOTP controller version: 0x%08X\r\n", version);
#endif

    uint32_t addr, value;
#if 0 //for test
    for (addr = 0; addr <= 0xF; addr++) { //3F
        value = OCOTP_ReadFuseShadowRegister(OCOTP, addr);
        TRACENR("0x%02X: 0x%08X\r\n", addr, value);
    }
#endif
#if 0 //for test
    TRACENR("0x%08X : 0x%08X\r\n", &OCOTP->CTRL, OCOTP->CTRL);
    TRACENR("0x%08X : 0x%08X\r\n", &OCOTP->OTPMK0, OCOTP->OTPMK0);
    TRACENR("0x%08X : 0x%08X\r\n", &OCOTP->SRK0, OCOTP->SRK0);
    TRACENR("0x%08X : 0x%08X\r\n", &OCOTP->SRK1, OCOTP->SRK1);
    TRACENR("0x%08X : 0x%08X\r\n", &OCOTP->SRK2, OCOTP->SRK2);
    TRACENR("0x%08X : 0x%08X\r\n", &OCOTP->SRK3, OCOTP->SRK3);
    TRACENR("0x%08X : 0x%08X\r\n", &SNVS->HPSR, SNVS->HPSR);
    TRACENR("0x%08X : 0x%08X\r\n", &DCP->STAT, DCP->STAT);
#endif

    int idx = 0;
    for (addr = MY_OCOTP_ADDR_ST; addr <= MY_OCOTP_ADDR_ED; addr++) {
        value = OCOTP_ReadFuseShadowRegister(OCOTP, addr);
        TRACENR("0x%02X: 0x%08X\r\n", addr, value);
        memcpy(&mcuKey[idx], &value, 4);
        idx += 4;
    }
    encrypt_key_copy(&mcuKey[0], EFUSE_KEY_LEN);
    get_header0();
    get_header1(); //nsmoon@200706
    //OCOTP_Deinit(OCOTP);
#endif
}
#endif //USE_FW_KEY_VALIDATION
#endif //1

#if 0 //nsmoon@190123
static void initShiftTbl64(void)
{
    int i;
    uint64_t sm = 1;

    for (i = 0; i < MAX_SHIFT_TBL; i++)
    {
        s_shiftTbl64[i] = sm << i;
    }
}
#endif

#if 1 //nsmoon@190123

void backend_process_ofst_data_debug_print(void)
{

    TRACE("g_back_conf.numPdX,,numPdY=%d, %d", g_back_conf.numPdX, g_back_conf.numPdY);
    TRACE("g_back_conf.numOfsX,slope_byte_x,slope_byte_y=(%d, %d)(%d, %d)", g_back_conf.numOfsX, g_back_conf.numOfsY, g_back_conf.slope_byte_x, g_back_conf.slope_byte_y);
    TRACE("s_sensor_end_x,sensor_end_y=%f, %f", s_sensor_end_x, s_sensor_end_y);
    TRACE("s_pd_pos_x,s_pd_pos_y=(%f, %f)(%f, %f)", s_pd_pos_x[0], s_pd_pos_y[0], s_pd_pos_x[g_back_conf.numPdX - 1], s_pd_pos_y[g_back_conf.numPdY - 1]);
    TRACE("s_led_pos_x,s_led_pos_y=(%f, %f)(%f, %f)", s_led_pos_x[0], s_led_pos_y[0], s_led_pos_x[g_back_conf.numPdX - 1], s_led_pos_y[g_back_conf.numPdY - 1]);
}

static int setPdData(axis_t axis, int pd, int offSetIdx, int pdIdx)
{
    int led, len = 0; //maxPd

    if (axis == ENUM_HOR_X) {
        if (offSetIdx >= MAX_NUM_SLOPES_DLT_X) {
            TRACE_ERROR("ERROR! invalid offSetIdx %d", offSetIdx);
            return 0;
        }
#if 1 //nsmoon@210603
        int slopeVal = offSetIdx - g_back_conf.maxOfsX;
        led = pd + slopeVal;
#else
        led = pd + s_offset_table_x[offSetIdx];
#endif
        if (led >= 0 && led < g_back_conf.numPdX)
        {
            if (pdIdx < MAX_HOR_TOUCH_PD) {
                s_input_buffer.hor_touch_pd[pdIdx].pd = (uint8_t)pd;
                s_input_buffer.hor_touch_pd[pdIdx].led = (uint8_t)offSetIdx; //slope index
                len++;
            }
            else {
                TRACE_ERROR("ERROR! invalid hor len %d", pdIdx);
            }
        }
        else {
            TRACE_ERROR("ERROR! draw_pd_data...invalid X led %d", led);
        }
    }
    else {
        //y-axis
        if (offSetIdx >= MAX_NUM_SLOPES_DLT_Y) {
            TRACE_ERROR("ERROR! invalid offSetIdx %d", offSetIdx);
            return 0;
        }
#if 1 //nsmoon@210603
        int slopeVal = offSetIdx - g_back_conf.maxOfsY;
        led = pd + slopeVal;
#else
        led = pd + s_offset_table_y[offSetIdx];
#endif
        if (led >= 0 && led < g_back_conf.numPdY)
        {
            if (pdIdx < MAX_VER_TOUCH_PD) {
                s_input_buffer.ver_touch_pd[pdIdx].pd = (uint8_t)pd;
                s_input_buffer.ver_touch_pd[pdIdx].led = (uint8_t)offSetIdx; //slope index
                // TRACE(" %d, %d", pd, offSetIdx);
                len++;
            }
            else {
                TRACE_ERROR("ERROR! invalid ver len %d", pdIdx);
            }
        }
        else {
            TRACE_ERROR("ERROR! draw_pd_data...invalid Y led %d", led);
        }
    }
    return len;
}

#endif

#if 0 //for test
static uint8_t bit_mask_8[MAX_SHIFT_TBL] =
{
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
};
#endif

int backend_process_line_data(void)
{
    // usb_host_vendor_generic_instance_t *genericHostInstance = &g_HostVendorGeneric;
    uint8_t *pRcv = &Exb_Send_Buf[0];//&g_usb_in_buff[0];
    DEF_DATA_INFO *OutBuf = &s_output_buffer;
    DEF_DATA_INFO2 *OutBuf2 = &s_output_buffer2;
    DEF_PD_INFO *InBuf = &s_input_buffer;
    next_scan_t  nextScan;
    int i, j, k, m, pd, len;
    int slope_byte, stIdx, bufIdx;
    axis_t axis;
    int len_line_data_x = g_back_conf.numPdX * g_back_conf.slope_byte_x;
    int len_line_data_y = g_back_conf.numPdY * g_back_conf.slope_byte_y;
#ifdef TIMER_CNT_CHECK
    uint32_t test_dwt, test_gpt1, test_gpt2;
#endif

#if defined(WAIT_FOR_READ_VERSION) && USB_DEVICE_SOF_ENABLE //nsmoon@211222
    if (verReadFlag == 0) {
        usb_echo("-");
        USB_DeviceEhciGetSofIntCnt(&frameCount);
        if (frameCount > FRAME_COUNT_MAX) {
            verReadFlag = 1;
        }
        return APP_OK;
    }
#endif
    //init input buffer
    s_input_buffer.hor_len = 0;
    s_input_buffer.ver_len = 0;
    s_input_buffer.hor_touch_pd = &s_hor_touch_pd[0];
    s_input_buffer.ver_touch_pd = &s_ver_touch_pd[0];
#ifdef FRONTEND_LINE_THRESHOLD
    s_input_buffer.threshold_x = &s_hor_line_threshold[0];
    s_input_buffer.threshold_y = &s_ver_line_threshold[0];
#endif

    //init output buffer
    s_output_buffer.len = 0;
    s_output_buffer.buf = &s_touch_output[0];

    stIdx = PD_DATA_IDX2;
    len = 0;
    for (m = 0; m < (g_back_conf.numPdX + g_back_conf.numPdY); m++) {
        if (m == g_back_conf.numPdX) {
            //x-axis done, save and reset
            s_input_buffer.hor_len = len;
            stIdx = PD_DATA_IDX2 + len_line_data_x;
            len = 0;
        }

        if (m < g_back_conf.numPdX) {
            //x-axis
            pd = m;
            axis = ENUM_HOR_X;
            slope_byte = g_back_conf.slope_byte_x;
        }
        else {
            //y-xis
            pd = m - g_back_conf.numPdX;
            axis = ENUM_VER_Y;
            slope_byte = g_back_conf.slope_byte_y;
        }
        bufIdx = stIdx + (pd * slope_byte);

        for (i = 0; i < slope_byte; i++) {
            uint8_t tmp8 = pRcv[bufIdx + i];
            if (tmp8) {
                //TRACE("pd,i: %d %d %02x", pd, i, tmp8);
                for (j = 0; j < MAX_SHIFT_TBL_BYTE; j++) {
                    //if (tmp8 & bit_mask_8[j])
                    if (tmp8 & (0x1 << j))
                    {
                        k = (i * 8) + j;
                        //TRACE("j,k: %d %d", j, k);
                        if (setPdData(axis, pd, k, len)) {
                            len++;
                        }
                    }
                }
            }
        }
    }
    s_input_buffer.ver_len = len;

#ifdef FRONTEND_LINE_THRESHOLD
    //1st line-threshold-X
    stIdx += len_line_data_y;
    //TRACE("1)stIdx=%d", stIdx);
    int thCntX = 0, tIdxX = 0;
    for (pd = 0; pd < g_back_conf.numPdX; pd++) {
        bufIdx = stIdx + (pd * g_back_conf.slope_byte_x);
        for (i = 0; i < g_back_conf.slope_byte_x; i++) {
            if (i < MAX_HOR_LINE_THRESHOLD) {
                s_hor_line_threshold[tIdxX++] = pRcv[bufIdx + i];
                //TRACE(" %d, %d", tIdxX-1, s_hor_line_threshold[tIdxX-1]);
                if (pRcv[bufIdx + i] != 0) {
                    thCntX++;
                }
            }
            else {
                TRACE_ERROR("ERROR! invalid i");
                break;
            }
        }
    }
    s_input_buffer.threshold_x_cnt = thCntX;

    //1st line-threshold-Y
    stIdx += len_line_data_x;
    //TRACE("2)stIdx=%d", stIdx);
    int thCntY = 0, tIdxY = 0;
    for (pd = 0; pd < g_back_conf.numPdY; pd++) {
        bufIdx = stIdx + (pd * g_back_conf.slope_byte_y);
        for (i = 0; i < g_back_conf.slope_byte_y; i++) {
            if (i < MAX_VER_LINE_THRESHOLD) {
                s_ver_line_threshold[tIdxY++] = pRcv[bufIdx + i];
                if (pRcv[bufIdx + i] != 0) {
                    thCntY++;
                }
            }
            else {
                TRACE_ERROR("ERROR! invalid i");
                break;
            }
        }
    }
    s_input_buffer.threshold_y_cnt = thCntY;
    stIdx += len_line_data_y;
#endif

#if 0//1		//YJ211005 for DLT //YJ@220207 not used
#define MAX_LINE_VAL4DLT	1500
    if(dltMode == BPLD_DLT_MODE)
    {
        if ((s_input_buffer.ver_len+s_input_buffer.hor_len) > MAX_LINE_VAL4DLT)
        {
            TRACE("MaxLineData : %d > %d", s_input_buffer.ver_len+s_input_buffer.hor_len, MAX_LINE_VAL4DLT);
            g_forcedagc.rmFrameCnt = 0;	//0xFF;
            return APP_OK;
        }
    }
    //if(thCntY+thCntX > 0)TRACE("T....MaxLineData : %d > %d", (thCntY+thCntX), MAX_LINE_VAL4DLT);
#endif

    //nextScan.currScan = nextScan.nextScan; //not-used
#if (BRUSH_MODE_ENABLE == 1)
    nextScan.brushMode = (touchMode == APP_DRAW_BRUSH) ? 1 : 0; //nsmoon@200403
#endif
    OutBuf->len = 0;

#if 1	//shj	for Brush, simultaneous release & tip switch
    OutBuf->outcnt = 0;
    OutBuf->reSend = 0;
#endif
    OutBuf2->touch_count = 0;
#if 0 //for test
    TRACE("S");
    MY_delay(10000);
    TRACE("E");
#else
#if 0 //for test
    if(InBuf->hor_len || InBuf->ver_len)
    {
        TRACE_ERROR("$ %d %d",InBuf->hor_len, InBuf->ver_len);
    }
#endif
    //DEBUG_LED_ON;
#ifdef TIMER_CNT_CHECK
    test_dwt = DWT->CYCCNT;
    test_gpt1 = GPT1_PERIPHERAL->CNT;
    test_gpt2 = GPT2_PERIPHERAL->CNT;
#endif
#if 1 //for test
    APP_TIME_ADD(2);
    nextScan.numTouch = s_prevTouchCntPen;
    if (BG_call_backend2((DEF_PD_INFO *)InBuf, (DEF_DATA_INFO2 *)OutBuf2, (next_scan_t *)&nextScan) != NO_BACKEND_ERROR)
    {
        TRACE_ERROR("(&)");
        //g_HostVendorGeneric.rmFrameCnt ++;	//= 4;	//YJ@211201
#if 1
#define MAX_CONT_ERROR_CNT	200 //100
//        if (contError < 0xFF)
//        {
//        	contError++;
//        	 TRACE_ERROR("@ %d",contError);
//        }
//        if (contError > MAX_CONT_ERROR_CNT) {
        	//contError = 0;
            if (g_forcedagc.rmFrameCnt < 0xFF) {
            	g_forcedagc.rmFrameCnt += 2;		//5;
            }
        //}
#endif
        //return APP_ERR;
    }
    APP_TIME_ADD(3);
#endif //1
#ifdef TIMER_CNT_CHECK
    TRACEYJ("\r\n%d, %d, %d", test_dwt, test_gpt1, test_gpt2);
#endif
    //  TRACE_ERROR("(@@)");
     //return APP_ERR;		// for test YJ@221128
   // contError = 0;
#if 0 //for test
    if(OutBuf2->touch_count)
    {
        TRACE_ERROR("$$ %d",OutBuf2->touch_count);
    }
#endif
    //DEBUG_LED_OFF;
#ifdef FRONTEND_FORCED_AGC
#define FRAME_REMAINED_LINE_MAX_X 5		//YJ@211102
#define FRAME_REMAINED_LINE_MAX_Y 20
    //if (OutBuf2->touch_data_edge.x == 0 )
    if((OutBuf2->touch_count == 0) && (OutBuf2->touch_data_edge.x == 0))
    {
        if ((nextScan.x1 + nextScan.y1) >= FRAME_REMAINED_LINE_MAX_X)
            //if (nextScan.x1 > FRAME_REMAINED_LINE_MAX_X || nextScan.y1 > FRAME_REMAINED_LINE_MAX_X)
        {
            TRACEYJ("## %d", nextScan.x1 + nextScan.y1);
            TRACEYJ("g_HostVendorGeneric.rmFrameCnt= %d (%d,%d)", g_forcedagc.rmFrameCnt, nextScan.x1 , nextScan.y1);
            if (g_forcedagc.rmFrameCnt < 0xFF) {
                //g_HostVendorGeneric.rmFrameCnt++;
            	g_forcedagc.rmFrameCnt += 1;
            }
        }
        else {
#if 0
        	g_forcedagc.rmFrameCnt =0;
#else
            if(g_forcedagc.rmFrameCnt > 0)
            {
            	g_forcedagc.rmFrameCnt--;
            }
#endif
        }
        contTouch = contTouch2 = 0; //nsmoon@211231
    }

#if 1  //nsmoon@211231
    else if((OutBuf2->touch_count !=0) ) {
#define MAX_CONT_TOUCH_CNT	100
        if (contTouch < 0xFF) contTouch++;
        if (contTouch > MAX_CONT_TOUCH_CNT) {
            if ((nextScan.x1 + nextScan.y1) >= FRAME_REMAINED_LINE_MAX_X) {
                if (g_forcedagc.rmFrameCnt < 0xFF) {
                	g_forcedagc.rmFrameCnt += 1; 	//2;	//YJ@230817
                }
            }
            else {
                if(g_forcedagc.rmFrameCnt > 0) {
                	g_forcedagc.rmFrameCnt--;
                }
            }
        }
    }
#if 1
    //else if(OutBuf2->touch_data_edge.x != 0 )
    else if(OutBuf2->touch_data_edge.x != 0 && OutBuf2->touch_data_edge.y != 0) //YJ@220207
    {
        if(g_forcedagc.rmFrameCnt > 0) {
        	g_forcedagc.rmFrameCnt--;
        }
    }
#endif
#if 0
    else if(OutBuf2->touch_data_edge.x != 0 )
    {
#define MAX_CONT_TOUCH_CNT2	1000
        if (contTouch2 < 0xFF) contTouch2++;
        if (contTouch2 > MAX_CONT_TOUCH_CNT) {
            if ((nextScan.x1 + nextScan.y1) >= FRAME_REMAINED_LINE_MAX_X) {
                if (g_forcedagc.rmFrameCnt < 0xFF) {
                	g_forcedagc.rmFrameCnt += 2;
                }
            }
            else {
                if(g_forcedagc.rmFrameCnt > 0) {
                	g_forcedagc.rmFrameCnt--;
                }
            }
        }
    }


#endif
//Forced agc result
if (g_forcedagc.rmFrameCnt >= REMAINED_LINE_FRAME_CNT_MAX) {
	if (s_is_expired_fagc_time(FAGC_EXPIRED_TIME)) { //nsmoon@220207
		g_forcedagc.rmFrameCnt = 0;
		g_forcedagc.forced_agc = TRUE;
		s_set_fagc_time(); //nsmoon@220207
		TRACE_ERROR("\r\nforcedagc be..");
		 return APP_ERR;
	}
}
#endif //1
#endif
#if 0 //ENABLE_DEBUG_MESSAGE //nsmoon@211231
    if (g_forcedagc.rmFrameCnt != 0) {
        if (g_forcedagc.rmFrameCnt != g_forcedagc.rmFrameCntPrev) {
        	g_forcedagc.rmFrameCntPrev = g_forcedagc.rmFrameCnt;
            TRACE("nextScan.x1,y1= (%d,%d) (%d,%d) %d", (int)nextScan.x1 , (int)nextScan.y1, OutBuf2->touch_count, (int)OutBuf2->touch_data_edge.x, (int)g_forcedagc.rmFrameCnt);
        }
    }
#endif

    if (appMode != APP_MODE_BACKEND) { //nsmoon@230117
      return APP_ERR;
    }

    s_touch_count = OutBuf2->touch_count;
    s_touch_data = OutBuf2->touch_data;
    s_touch_size = OutBuf2->touch_size;
    s_touch_lineCnt = OutBuf2->touch_line_cnt;
    s_multi_fine = OutBuf2->multi_fine;
    //TRACE("\r\n %d, %f, %f, %f, %f", s_touch_count, s_touch_data->x, s_touch_data->y, s_touch_size->xSize, s_touch_size->ySize);
    //TRACE_ERROR("%d", s_touch_count);
    // return APP_ERR;		// for test YJ@221128

    s_touch_point_tracking();
#ifdef WMC_MODE_CHANGE //YJ@230207
    if(touchModePrev == APP_DRAW_PEN_MARKER)
#else
    if(touchMode == APP_DRAW_PEN_MARKER)
#endif
    {
#ifdef SMOOTH_FILTER_AVG//for test
        s_smooth_filter_avg();
#endif
        s_smooth_filter();
        s_coordinates_conversion(OutBuf);
#ifdef HOR_EDGE_TOUCH_ENABLE
        //TRACE("touch_data_edge= %0.1f, %0.1f", OutBuf2->touch_data_edge.x, OutBuf2->touch_data_edge.y);
        s_wheel_delta = s_cal_hor_scroll(OutBuf2->touch_data_edge);
        //TRACE("s_wheel_delta= %d", s_wheel_delta);
#endif
    }
    else
    {
#ifdef BRUSH_DELAY_REPORT_ENABLE
		if (s_smooth_filter_brush_report_delay() == APP_OK) {
			s_coordinates_conversion_brush_report_delay(OutBuf);
		} else {
			return APP_ERR;
		}
#else
		s_smooth_filter_brush();
		s_coordinates_conversion_brush(OutBuf);
#endif
    }
#endif

#if 0 //for testing
    if (s_input_buffer.hor_len || s_input_buffer.ver_len) {
        TRACE("H/V: %d/%d %d", s_input_buffer.hor_len, s_input_buffer.ver_len, OutBuf->len);
    }
#endif
#if 0 //for testing
    for (int i = 0; i < OutBuf->len; i++) {
        TRACE("[%d]%d,%d", i, OutBuf->buf[i].status, OutBuf->buf[i].contactID);
        TRACE("(%d,%d)(%d,%d)", OutBuf->buf[i].xCord, OutBuf->buf[i].yCord, OutBuf->buf[i].xSize, OutBuf->buf[i].ySize);
    }
#endif
    return APP_OK;
}

static uint32_t s_belib_time; //nsmoon@220124
static uint32_t s_fagc_time; //nsmoon@220207

#if 1 //gpt, nsmoon@190114
static uint32_t s_prevTime, s_prevTime2, s_prevTime3, s_prevTime4;

#if 0
const gpt_config_t GPT_1_config = {
    .clockSource = kGPT_ClockSource_Periph,
    .divider = 1,
    .enableFreeRun = true,
    .enableRunInWait = false,
    .enableRunInStop = false,
    .enableRunInDoze = false,
    .enableRunInDbg = false,
    .enableMode = true
};

void GPT_1_init(void) {
    /* GPT device and channels initialization */
    GPT_Init(GPT_1_PERIPHERAL, &GPT_1_config);
    GPT_SetOscClockDivider(GPT_1_PERIPHERAL, 1);
    /* Enable GPT interrupt sources */
    GPT_EnableInterrupts(GPT_1_PERIPHERAL, 0);
    /* Start the GPT timer */
    GPT_StartTimer(GPT_1_PERIPHERAL);
}
#endif

#if 1
const gpt_config_t GPT_1_config = {
    .clockSource = kGPT_ClockSource_Periph,
    .divider = 1,
    .enableFreeRun = true,
    .enableRunInWait = false,
    .enableRunInStop = false,
    .enableRunInDoze = false,
    .enableRunInDbg = false,
    .enableMode = true
};
#endif

static void MY_GTP_init(void)
{
#if 0
    /*Clock setting for GPT*/
    CLOCK_SetMux(kCLOCK_PerclkMux, MY_GPT_CLOCK_SOURCE_SELECT);
    CLOCK_SetMux(kCLOCK_PerclkDiv, MY_GPT_CLOCK_DIVIDER_SELECT);

    /*GPT timer is setup */
    GPT_GetDefaultConfig(&gpt_config);
    gpt_config.enableRunInStop = false;
    gpt_config.enableRunInWait = false;
    gpt_config.enableRunInDoze = false;
    gpt_config.enableRunInDbg = false;
    gpt_config.enableFreeRun = true;
    gpt_config.enableMode = true;

    /* Initialize GPT module */
    GPT_Init(MY_GPT, &gpt_config);

    /*Divide GPT clock source frequency by 2 inside GPT module */
    GPT_SetClockDivider(MY_GPT, 1000); //1000, FIXME

    /* Start Timer */
    TRACE("Starting GPT Timer... %d", MY_GPT_CLK_FREQ);

    GPT_StartTimer(MY_GPT);
#endif
    /* GPT device and channels initialization */
    GPT_Init(MY_GPT, &GPT_1_config);
    GPT_SetOscClockDivider(MY_GPT, 1);
    /* Enable GPT interrupt sources */
    GPT_EnableInterrupts(MY_GPT, 0);
    /* Start the GPT timer */
    GPT_StartTimer(MY_GPT);

    s_prevTime = 0;
    s_prevTime2 = 0;
    s_belib_time = 0; //nsmoon@220124
    s_fagc_time = 0; //nsmoon@220207
}


uint32_t s_get_frame_span(void)
{
    uint32_t diffTime;
    uint32_t curTime = GPT_GetCurrentTimerCount(MY_GPT);
    if (curTime > s_prevTime) {
        diffTime = curTime - s_prevTime;
    }
    else {
        diffTime = (0xFFFFFFFF - s_prevTime + 1) + curTime;
    }
    s_prevTime = curTime;
    diffTime = diffTime / GPT_TIME_SCALE_100US;
    //TRACE("s_get_frame_span: %d(%x) %d(%x) %d(%x)", diffTime, diffTime, curTime, curTime, s_prevTime, s_prevTime);
    return (diffTime);
}

uint32_t s_get_time_diff(void)
{
    uint32_t diffTime;
    uint32_t curTime = GPT_GetCurrentTimerCount(MY_GPT);
    if (curTime > s_prevTime2) {
        diffTime = curTime - s_prevTime2;
    }
    else {
        diffTime = (0xFFFFFFFF - s_prevTime2 + 1) + curTime;
    }
    //TRACE("s_get_time_diff: %d(%x) %d(%x) %d(%x)", diffTime, diffTime, curTime, curTime, s_prevTime2, s_prevTime2);
    s_prevTime2 = curTime;
    return (diffTime / GPT_TIME_SCALE_100US);
}

#endif //gtp

uint32_t s_wheel_get_time_diff(void)
{
    uint32_t diffTime;
    uint32_t curTime = GPT_GetCurrentTimerCount(MY_GPT);
    if (curTime > s_prevTime3) {
        diffTime = curTime - s_prevTime3;
    }
    else {
        diffTime = (0xFFFFFFFF - s_prevTime3 + 1) + curTime;
    }
    //TRACE("s_get_time_diff: %d(%x) %d(%x) %d(%x)", diffTime, diffTime, curTime, curTime, s_prevTime2, s_prevTime2);
    s_prevTime3 = curTime;
    return (diffTime / GPT_TIME_SCALE_100US);
}


uint32_t s_Send_get_time_diff(void)
{
    uint32_t diffTime;
    uint32_t curTime = GPT_GetCurrentTimerCount(MY_GPT);
    if (curTime > s_prevTime4) {
        diffTime = curTime - s_prevTime4;
    }
    else {
        diffTime = (0xFFFFFFFF - s_prevTime4 + 1) + curTime;
    }
    //TRACE("s_get_time_diff: %d(%x) %d(%x) %d(%x)", diffTime, diffTime, curTime, curTime, s_prevTime2, s_prevTime2);
    s_prevTime4 = curTime;
    return (diffTime / GPT_TIME_SCALE_100US);
}



#if 1 //nsmoon@220124
uint32_t s_belib_time_diff(void)
{
    uint32_t curTime = GPT_GetCurrentTimerCount(MY_GPT);
    uint32_t diffTime = curTime - s_belib_time;
    //TRACE("s_get_time_diff: 0x%x 0x%x 0x%x", diffTime, s_belib_time, curTime);
    s_belib_time = curTime;
    return (diffTime / GPT_TIME_SCALE_10US);
}
#endif

#ifdef FRONTEND_FORCED_AGC
void s_set_fagc_time(void)
{
    s_fagc_time = GPT_GetCurrentTimerCount(MY_GPT);
}

int s_is_expired_fagc_time(uint32_t time100us)
{
    uint32_t curTime = GPT_GetCurrentTimerCount(MY_GPT);
    uint32_t diffTime = curTime - s_fagc_time;
    //TRACE("s_is_expired_fagc_time: 0x%x 0x%x 0x%x", diffTime, s_fagc_time, curTime);
    if ((diffTime / GPT_TIME_SCALE_100US) > time100us) {
        return 1; //expired
    }
    return 0;
}
#endif

static int SendTouch_Report(DEF_DATA_INFO *data_out, uint8_t reSend, uint8_t reSendStep)
{
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
    usb_device_hid_generic_struct_t *genericDeviceInstance = &g_UsbDeviceHidGeneric;
    uint8_t *transmitDataBuffer = (uint8_t *)&genericDeviceInstance->buffTx[0];
    DEF_TOUCH_OUT *Data_Buf = data_out->buf;
    int tpCnt = data_out->len;
    int i, j, txIdx, bufIdx, len;
    uint8_t contact_status;

    bufIdx = 0;

#if 1 //def TP_20_TEST
    int loopCnt = (int)((tpCnt + (TP_PER_PACKET - 1)) / TP_PER_PACKET);
#else
    int loopCnt = (tpCnt > TP_PER_PACKET) ? 2 : 1;
#endif

    /* Prepare the USB module to send the data packet to the host */
    for (i = 0; i < loopCnt; i++)
    {
        memset(transmitDataBuffer, 0x00, USB_HID_GENERIC_IN_BUFFER_LENGTH);
        for(j=0; j<5; j++)
        {
            transmitDataBuffer[2+(j*10)] = 0xff;
        }
        transmitDataBuffer[0] = MULTI_TOUCH_DATA_REPORT_ID;

        if (i == 0)
        {
            transmitDataBuffer[MAX_TOUCH_DATA_SIZE_WITH_SIZE + 1] = tpCnt;
            len = MIN(tpCnt, TP_PER_PACKET);
        }
#if 1 //def TP_20_TEST
        else if (j == (loopCnt - 1)) {
            len = tpCnt - (j * TP_PER_PACKET);
        }
        else {
            len = TP_PER_PACKET;
        }
#else
        else {
            len = tpCnt - TP_PER_PACKET;
        }
#endif

        for(j = 0; j<len; j++)
        {
            txIdx = (j * BYTES_PER_POS);
#if 1 //def TP_20_TEST
            bufIdx = (i * TP_PER_PACKET) + j;
#else
            bufIdx = (i == 0) ? j : (TP_PER_PACKET + j);
#endif

            contact_status = Data_Buf[bufIdx].status & 0x03;

            if(reSend)
            {
                if(reSendStep == 0)
                {
                    if(Data_Buf[bufIdx].ID_staus == NEW_ID)
                    {
                        contact_status = 0x00;
                    }
                }
                else
                {
                    if(Data_Buf[bufIdx].ID_staus == NEW_ID)
                    {
                        contact_status = 0x03;
                    }
                }
            }


#if (FLIP2_COMPATIBLE_DESCRIPTOR == 0U)
            transmitDataBuffer[txIdx+1] = contact_status;//Data_Buf[bufIdx].status & 0x03;//touch_status_data;//Data_Buf[bufIdx].status & 0x03;      //0:status
            transmitDataBuffer[txIdx+2] = Data_Buf[bufIdx].contactID;          //1:id
            transmitDataBuffer[txIdx+3] = LOBYTE_16(Data_Buf[bufIdx].xCord);   //2:x-low
            transmitDataBuffer[txIdx+4] = HIBYTE_16(Data_Buf[bufIdx].xCord);   //3:x-high
            transmitDataBuffer[txIdx+5] = LOBYTE_16(Data_Buf[bufIdx].yCord);   //4:y-low
            transmitDataBuffer[txIdx+6] = HIBYTE_16(Data_Buf[bufIdx].yCord);   //5:y-high
            transmitDataBuffer[txIdx+7] = LOBYTE_16(Data_Buf[bufIdx].xSize);   //6:x-low
            transmitDataBuffer[txIdx+8] = HIBYTE_16(Data_Buf[bufIdx].xSize);   //7:x-high
            transmitDataBuffer[txIdx+9] = LOBYTE_16(Data_Buf[bufIdx].ySize);   //8:y-low
            transmitDataBuffer[txIdx+10] = HIBYTE_16(Data_Buf[bufIdx].ySize);   //9:y-high
#else
            if(Data_Buf[bufIdx].xSize >= 255) Data_Buf[bufIdx].xSize = 255;
            if(Data_Buf[bufIdx].ySize >= 255) Data_Buf[bufIdx].ySize = 255;

            transmitDataBuffer[txIdx+1] = contact_status;//Data_Buf[bufIdx].status & 0x03;
            transmitDataBuffer[txIdx+2] = Data_Buf[bufIdx].contactID;
            transmitDataBuffer[txIdx+3] = LOBYTE_16(Data_Buf[bufIdx].xCord);
            transmitDataBuffer[txIdx+4] = HIBYTE_16(Data_Buf[bufIdx].xCord);
            transmitDataBuffer[txIdx+5] = LOBYTE_16(Data_Buf[bufIdx].yCord);
            transmitDataBuffer[txIdx+6] = HIBYTE_16(Data_Buf[bufIdx].yCord);
            transmitDataBuffer[txIdx+7] = LOBYTE_16(Data_Buf[bufIdx].xSize);
            transmitDataBuffer[txIdx+8] = LOBYTE_16(Data_Buf[bufIdx].ySize);
            transmitDataBuffer[txIdx+9] = 0x00;	//LOBYTE_16(0x00);//(0x02);
            transmitDataBuffer[txIdx+10] =0x04;	// LOBYTE_16(0x20);		//YJ@210824	//ppt signpen
#endif
        }

        if (USB_SendDigitizerReport(transmitDataBuffer, USB_HID_GENERIC_IN_BUFFER_LENGTH) != USB_OK) {
            TRACE("*1");
            return USB_ERR;
        }
    }
#endif
    return USB_OK;
}

static int SendBrush_Report(DEF_DATA_INFO *data_out)
{
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
    usb_device_hid_generic_struct_t *genericDeviceInstance = &g_UsbDeviceHidGeneric;
    uint8_t *transmitDataBuffer = (uint8_t *)&genericDeviceInstance->buffTx[0];
    DEF_TOUCH_OUT *Data_Buf = data_out->buf;
    uint8_t resend = Data_Buf->nReportSendCnt;
    uint8_t i, contact_status;

    //resend = 0;

    memset(transmitDataBuffer, 0x00, USB_HID_GENERIC_IN_BUFFER_LENGTH);

    if(Data_Buf[0].xSize == 0 && Data_Buf[0].ySize == 0 && Data_Buf[0].pressure == 0) {
    	Data_Buf[0].status = TOUCH_SIG_UP;
    }

    contact_status = Data_Buf[0].status & 0x03;

    for(i=0; i<5; i++)
    {
        transmitDataBuffer[2+(i*10)] = 0xff;
    }

#if 0
    if((Data_Buf[0].status & 0x03) == 0x00)
    {
        resend = 1;	// Fixed for One Brush ID Mode
        contact_status = 0x03;
    }
#else
    if(resend == 1)
    {
        if((Data_Buf[0].status & 0x03) == 0x00)
        {
            contact_status = 0x03;
        }
    }
#endif

    transmitDataBuffer[0] = MULTI_TOUCH_DATA_REPORT_ID;
    transmitDataBuffer[MAX_TOUCH_DATA_SIZE_WITH_SIZE + 1] = 1;

    transmitDataBuffer[1] = contact_status;//Data_Buf[bufIdx].status & 0x03;
    transmitDataBuffer[2] = 0x00;
    transmitDataBuffer[3] = LOBYTE_16(Data_Buf[0].xCord);
    transmitDataBuffer[4] = HIBYTE_16(Data_Buf[0].xCord);
    transmitDataBuffer[5] = LOBYTE_16(Data_Buf[0].yCord);
    transmitDataBuffer[6] = HIBYTE_16(Data_Buf[0].yCord);
    transmitDataBuffer[7] = LOBYTE_16(Data_Buf[0].xSize);
    transmitDataBuffer[8] = LOBYTE_16(Data_Buf[0].ySize);
    transmitDataBuffer[9] = LOBYTE_16(Data_Buf[0].pressure);
    transmitDataBuffer[10] = HIBYTE_16(Data_Buf[0].pressure);

    //if (USB_SendDigitizerReport(transmitDataBuffer, USB_HID_GENERIC_IN_BUFFER_LENGTH) != USB_OK) {
    if (USB_SendDigitizerReport(transmitDataBuffer, 52) != USB_OK) {
        TRACE("*2");
        return USB_ERR;
    }

    if(resend)
    {
        MY_delay(80);

        transmitDataBuffer[1] = 0x00;
        transmitDataBuffer[7] = 0x00;
        transmitDataBuffer[8] = 0x00;
        transmitDataBuffer[9] = 0x00;
        transmitDataBuffer[10] = 0x00;
        //if (USB_SendDigitizerReport(transmitDataBuffer, USB_HID_GENERIC_IN_BUFFER_LENGTH) != USB_OK) {
        if (USB_SendDigitizerReport(transmitDataBuffer, 52) != USB_OK) {
            TRACE("*3");
            return USB_ERR;
        }
    }
#endif
    return USB_OK;
}

static int SendDigitizerReport(DEF_DATA_INFO *data_out)
{
    //DEF_TOUCH_OUT *Data_Buf = data_out->buf;
    int tpCnt = data_out->len;
    uint8_t reSendReport = data_out->reSend;

    if (USB_Device_ready() == 0) {
        TRACE_ERROR("ERROR! USB device is not configured yet");
        return USB_ERR;
    }
    if (tpCnt > ALLOWABLE_TOUCH_IO) {
        TRACE_ERROR("ERROR! invalid tpCnt %d", tpCnt);
        return USB_ERR;
    }
    if (tpCnt == 0) {
        TRACE("$");
        return USB_OK;
    }

    int ret = 0;

#ifdef WMC_MODE_CHANGE //YJ@230207
    if(touchModePrev == APP_DRAW_PEN_MARKER)
#else
    if(touchMode == APP_DRAW_PEN_MARKER)
#endif
    {
        ret = SendTouch_Report(data_out, reSendReport, 0);
        if(reSendReport)
        {
            MY_delay(80);
            ret = SendTouch_Report(data_out, reSendReport, 1);
        }
    }
    else	//APP_DRAW_BRUSH
    {
#ifdef BRUSH_DELAY_REPORT_ENABLE
    	uint32_t Send_time = 0;

    	if (brushLastSend.lastSend) {
    		brushLastSend.lastSend = 0;

    		if (brushLastSend.condition) {
    			int i = 0;
    			int cnt = brushLastSend.condition;

    			brushLastSend.condition = 0;

    			for(i = 0; i < cnt; i++) {
    				data_out->buf[brushLastSend.bufCnt].xCord = brushLastSend.xCord[i];
					data_out->buf[brushLastSend.bufCnt].yCord = brushLastSend.yCord[i];
					data_out->buf[brushLastSend.bufCnt].xSize = brushLastSend.xSize[i];
					data_out->buf[brushLastSend.bufCnt].ySize = brushLastSend.ySize[i];
					data_out->buf[brushLastSend.bufCnt].pressure = brushLastSend.pressure[i]<15?15:brushLastSend.pressure[i];

					if (i == (cnt-1)) {
						data_out->buf[brushLastSend.bufCnt].nReportSendCnt = 1;
						data_out->buf[brushLastSend.bufCnt].status = TOUCH_SIG_UP;
					} else {
						data_out->buf[brushLastSend.bufCnt].nReportSendCnt = 0;
						data_out->buf[brushLastSend.bufCnt].status = TOUCH_SIG_DOWN;

						ret = SendBrush_Report(data_out);

						Send_time = s_Send_get_time_diff();
						while(Send_time < 60)		//60	100us
						{
							MY_delay(30);
							Send_time  += s_Send_get_time_diff();
						}
					}
				}

    		} else {

				int i;

				data_out->buf[brushLastSend.bufCnt].nReportSendCnt = 0;
				data_out->buf[brushLastSend.bufCnt].status = TOUCH_SIG_DOWN;

				ret = SendBrush_Report(data_out);


				Send_time = s_Send_get_time_diff();
				while(Send_time < 60)		//60	100us
				{
					MY_delay(30);
					Send_time  += s_Send_get_time_diff();
				}

				for (i = 0; i < BRUSH_DELAY_SIZE; i++) {

					data_out->buf[brushLastSend.bufCnt].xCord = brushLastSend.xCord[i];
					data_out->buf[brushLastSend.bufCnt].yCord = brushLastSend.yCord[i];
					data_out->buf[brushLastSend.bufCnt].xSize = brushLastSend.xSize[i];
					data_out->buf[brushLastSend.bufCnt].ySize = brushLastSend.ySize[i];
					data_out->buf[brushLastSend.bufCnt].pressure = brushLastSend.pressure[i];//brushLastSend.pressure[i]<200?200:brushLastSend.pressure[i];

					if (i == (BRUSH_DELAY_SIZE-2)) {
						data_out->buf[brushLastSend.bufCnt].nReportSendCnt = 1;
						data_out->buf[brushLastSend.bufCnt].status = TOUCH_SIG_UP;
						break;
					} else {
						data_out->buf[brushLastSend.bufCnt].nReportSendCnt = 0;
						data_out->buf[brushLastSend.bufCnt].status = TOUCH_SIG_DOWN;
						ret = SendBrush_Report(data_out);
						Send_time = s_Send_get_time_diff();
						while(Send_time < 60)		//60	100us
						{
							MY_delay(30);
							Send_time  += s_Send_get_time_diff();
						}
					}
				}
    		}
    	}
#endif
        ret = SendBrush_Report(data_out);
    }
    return ret;
}

#if 1 //for test
typedef enum
{
    /* All data from or to the buffer was transferred successfully. */
    DATASTREAM_BUFFER_EVENT_COMPLETE,
    /* There was an error while processing the buffer transfer request. */
    DATASTREAM_BUFFER_EVENT_ERROR,
    /* Data transfer aborted (Applicable in DMA mode) */
    DATASTREAM_BUFFER_EVENT_ABORT
} DATASTREAM_BUFFER_EVENT;


uint8_t *normalCmdProcess(void)
{
    uint8_t *ret = NULL;
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
    usb_device_hid_generic_struct_t *genericDeviceInstance = &g_UsbDeviceHidGeneric;

    uint16_t crc_cal = USB_CalculateCrc(&genericDeviceInstance->buffRx[0], USB_NORCMD_LENGHT);
    uint16_t crc_get = genericDeviceInstance->buffRx[USB_NORCMD_LENGHT+1];
    crc_get = (crc_get << 8) | genericDeviceInstance->buffRx[USB_NORCMD_LENGHT];

    //memset(&usbCmdBuf[0], 0 , sizeof(usbCmdBuf)); //nsmoon@201224
    usbCmdBuf[0] = 0xFF;

    if (crc_cal == crc_get)
    {
        if (genericDeviceInstance->RxCmd == DIAGNOSTIC_REPORT_ID)
        {
            memcpy(&usbCmdBuf[0], &genericDeviceInstance->buffRx[1], sizeof(usbCmdBuf));
            //TRACE("usbCmdBuf[0]= %x", usbCmdBuf[0]);
            return &usbCmdBuf[0];
        }
        TRACE_ERROR("usbcmd<%d,%d>\r\n", genericDeviceInstance->buffRx[0], genericDeviceInstance->buffRx[1]);
    }
    else
    {
        TRACE_ERROR("ERROR! invalid crc %04x, %04x\r\n", crc_cal, crc_get);
    }
    TRACE_ERROR("usbcmd(%d,%d)\r\n", genericDeviceInstance->buffRx[0], genericDeviceInstance->buffRx[1]);
#endif

    return ret;
}


#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
#if 1 //nsmoon@200706
static void get_fw_version(char *src, char *dst, int len)
{
    int i, found, cnt;

    cnt = found = 0;
    for (i = 0; i <= strlen(src); i++)
    {
        if (src[i] == '_') {
            found++;
            if (found == 2) {
                if (cnt >= len) {
                    cnt = len - 1;
                }
                dst[cnt] = '\0';
                break;
            }
            continue;
        }
        if (found) {
            if (cnt < len) {
                dst[cnt++] = src[i];
            }
        }
    }
}

static void getVerString(uint8_t *txBuff, int idx)
{
    usb_device_hid_generic_struct_t *genericDeviceInstance = &g_UsbDeviceHidGeneric;
    uint8_t *transmitDataBuffer = (uint8_t *)&genericDeviceInstance->buffTxDiag[0];
    char *verStr = (char *)&txBuff[idx];

    memset(verStrBe, 0xFF, MAX_VER_STRING); //reset
#if (MODEL_SPT == MODEL_CTSK_N650_V100)		//YJ@230223	for new boot
    get_fw_version((char *)header1.ind.fwVersion, (char *)verStrBe, 3);
    TRACE("verStrBe= [%s][%s]", verStrBe, header1.ind.fwVersion);
    //memcpy((void *)&verStr[0], (void *)&verStrFe[0],  2);

	memcpy((void *)&verStr[0], (void *)&frontend_fwVersion[0],  2);
    memcpy((void *)&verStr[2], (void *)&verStrBe[0],  2);
#else
    get_fw_version((char *)header1.ind.fwVersion, (char *)verStrBe, 3);
    TRACE("verStrBe= [%s][%s]", verStrBe, header1.ind.fwVersion);
    memcpy((void *)&verStr[0], (void *)&verStrFe[0],  2);
    memcpy((void *)&verStr[2], (void *)&verStrBe[0],  2);
#endif
    verStr[4] = '\0';
   // TRACE_ERROR("verStr= [%s], F[%s], B[%s]", verStr, verStrFe, verStrBe);		//YJ0114
    int len = idx + 4;
    uint16_t crc_cal = USB_CalculateCrc(&txBuff[0], len);
    transmitDataBuffer[len] = (uint8_t)crc_cal;
    transmitDataBuffer[len + 1] = (uint8_t)(crc_cal >> 8);
}
#endif
#endif

#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
static int normalSendRes(usb_device_normal_res_t res, uint8_t param)
{
    usb_device_hid_generic_struct_t *genericDeviceInstance = &g_UsbDeviceHidGeneric;
    uint8_t *transmitDataBuffer = (uint8_t *)&genericDeviceInstance->buffTxDiag[0];

    //memset(transmitDataBuffer, 0, USB_HID_GENERIC_IN_BUFFER_LENGTH);
    transmitDataBuffer[0] = DIAGNOSTIC_REPORT_ID;
    transmitDataBuffer[1] = (uint8_t)res;
    transmitDataBuffer[2] = param;

#if 1 //nsmoon@200706
    if (param == USB_NORCMD_READ_VER) {
        getVerString(&transmitDataBuffer[0], 3);
    }
    else
#endif
    {
        uint16_t crc_cal = USB_CalculateCrc(&transmitDataBuffer[0], (USB_NORRES_LENGHT-2));
        transmitDataBuffer[3] = (uint8_t)crc_cal;
        transmitDataBuffer[4] = (uint8_t)(crc_cal >> 8);
    }

    if (USB_SendDigitizerReport(transmitDataBuffer, USB_HID_GENERIC_IN_BUFFER_LENGTH) != USB_OK) {
        TRACE("*4");
        return USB_ERR;
    }
    return USB_OK;
}
#endif

int normal_dlt_send_data(uint8_t *txbuff, int len)
{
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
    usb_device_hid_generic_struct_t *genericDeviceInstance = &g_UsbDeviceHidGeneric;
    //uint8_t *pRcv = &g_usb_in_buff[0];
    uint8_t *transmitDataBuffer = (uint8_t *)&genericDeviceInstance->buffTxDiag[0];
    int pLen = (USB_HID_GENERIC_IN_BUFFER_LENGTH - 1); //exclude header
    int idx = 0;

    int totalPacketSize = len;//USB_get_NormaltotalPacketSize();
    if (totalPacketSize < 10) {
        TRACE_ERROR("ERROR! invalid totalPacketSize: %d", totalPacketSize);
        return USB_ERR;
    }
    //TRACE("normal_send_data..%d", totalPacketSize);

    for (idx = 0; idx < totalPacketSize; idx += pLen) {
        //TRACE("#3");
        transmitDataBuffer[0] = DIAGNOSTIC_REPORT_ID;
        memcpy((uint8_t *)&transmitDataBuffer[1], (uint8_t *)&txbuff[idx], pLen);
        if (USB_SendDigitizerReport(transmitDataBuffer, USB_HID_GENERIC_IN_BUFFER_LENGTH) != USB_OK) {
            //	TRACE("*");
            return USB_ERR;
        }
    }
#endif
    return USB_OK;
}

#endif

#ifdef HOR_EDGE_TOUCH_ENABLE
static int SendWheelReport()
{
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
    int cnt = 0;
    uint8_t transmitDataBufferLength;
    uint8_t *transmitDataBuffer = USB_GetMouseBuffer(&transmitDataBufferLength);

    if (USB_Device_ready() == 0) {
        TRACE_ERROR("ERROR! SendWheelReport..USB device is not configured yet");
        return USB_ERR;
    }
    if (transmitDataBufferLength < 5) {
        TRACE_ERROR("ERROR! SendWheelReport..invalid transmitDataBufferLength %d", transmitDataBuffer);
        return USB_ERR;
    }
    transmitDataBuffer[cnt++] = 0; //0:button
    transmitDataBuffer[cnt++] = 0; //1:x
    transmitDataBuffer[cnt++] = 0; //2:y
    transmitDataBuffer[cnt++] = (int8_t)(s_wheel_delta); //slide
    transmitDataBuffer[cnt++] = 0;	//(int8_t)(s_wheel_delta); //wheel
    if (USB_SendMouseReport(transmitDataBuffer, cnt) != USB_OK) {
        TRACE("*6");
        return USB_ERR;
    }
#endif
    return USB_OK;
}
#endif

#ifdef NORMAL_RESET_DELAY //nsmoon@200807
void DoSoftReset(int cnt)
//static void DoSoftReset(int cnt)
{
    s_reset_delay = cnt;
}

#if (MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
static void check_reset_delay(void)
{
    if (s_reset_delay > 0) {
        if ((s_reset_delay_dot++ % RESET_DELAY_DOT_CNT) == 0) {
            TRACE(".");
            s_reset_delay--;
        }
    }
    else if (s_reset_delay == 0) {
#ifdef FRONTEND_RESET_ENABLE //nsmoon@200824, for old bootloader
        s_frontend_Reset();
#endif
        MY_WDOG_soft_reset();
        while(1);
    }
}
#else
static int check_reset_delay(void)
{
	int ret = 0;
    if (s_reset_delay > 0) {
        if ((s_reset_delay_dot++ % RESET_DELAY_DOT_CNT) == 0) {
            TRACE(".");
            s_reset_delay--;
        }
    }
    else if (s_reset_delay == 0) {
    	ret = 1;
    }

    return ret;
}



#endif
#endif

#if (MODEL_SPT == MODEL_CTSK_N650_V100)||(MODEL_SPT == MODEL_CTSK_N750_V100)||(MODEL_SPT == MODEL_CTSK_N850_V100)
uint32_t wheel_Send_time = 0;
uint32_t Digitizer_Send_time = 0;

void APP_Init(void)
{
    appRunState = APP_STATE_INIT;
    backendRunState = BACKEND_STATE_INIT;
    appCurrMode = APP_MODE_BACKEND;
    appNextMode = APP_MODE_BACKEND;
    appMode = APP_MODE_BACKEND;

    //appScanSleep = 0;

    //GPT1_Time_Var_Init();

    Scan_Initialize();

}

int backendInit(void)
{
    int ret = 0;
    DEF_TP_LAYOUT_INFO tp_layout_info;
    //TRACE("backendInit...");

    s_logical_max_x = TEST_LOGICAL_X_MAX;
    s_logical_max_y = TEST_LOGICAL_Y_MAX;

    s_sensor_zero_x = sensor_zero_x__;
    s_sensor_end_x = sensor_end_x__;
    s_sensor_zero_y = sensor_zero_y__;
    s_sensor_end_y = sensor_end_y__;
    s_aarea_zero_x = aarea_zero_x__;
    s_aarea_end_x = aarea_end_x__;
    s_aarea_zero_y = aarea_zero_y__;
    s_aarea_end_y = aarea_end_y__;

    tp_layout_info.sensor_zero_x = sensor_zero_x__;
    tp_layout_info.sensor_end_x = sensor_end_x__;
    tp_layout_info.sensor_zero_y = sensor_zero_y__;
    tp_layout_info.sensor_end_y = sensor_end_y__;
    tp_layout_info.aarea_zero_x = aarea_zero_x__;
    tp_layout_info.aarea_end_x = aarea_end_x__;
    tp_layout_info.aarea_zero_y = aarea_zero_y__;
    tp_layout_info.aarea_end_y = aarea_end_y__;
#if 0 //for test
    memcpy((void *)&s_led_pos_x[0], (void *)&sensor_XLED_Pos__[0], sizeof(float) * MAX_NUM_PD_X_DLT);
    memcpy((void *)&s_pd_pos_x[0], (void *)&sensor_XPD_Pos__[0], sizeof(float) * MAX_NUM_PD_X_DLT);
    memcpy((void *)&s_led_pos_y[0], (void *)&sensor_YLED_Pos__[0], sizeof(float) * MAX_NUM_PD_Y_DLT);
    memcpy((void *)&s_pd_pos_y[0], (void *)&sensor_YPD_Pos__[0], sizeof(float) * MAX_NUM_PD_Y_DLT);
//    int i = 0;
//    for( i =0; i < MAX_NUM_PD_X_DLT; i++)
//    {
//    	s_led_pos_x[i] = sensor_XLED_Pos__[i];
//    	s_pd_pos_x[i]= sensor_XPD_Pos__[i];
//    }
//    for( i =0; i < MAX_NUM_PD_Y_DLT; i++)
//    {
//    	s_led_pos_y[i]= sensor_YLED_Pos__[i];
//    	s_pd_pos_y[i]= sensor_YPD_Pos__[i];
//    }
    tp_layout_info.pd_pos_x = (float *)&s_pd_pos_x[0];
    tp_layout_info.pd_pos_y = (float *)&s_pd_pos_y[0];
    tp_layout_info.led_pos_x = (float *)&s_led_pos_x[0];
    tp_layout_info.led_pos_y = (float *)&s_led_pos_y[0];

//	int i;
//	for (i = 0; i < MAX_NUM_PD_X_DLT; i++) {
//		if (s_pd_pos_x[i] != sensor_XPD_Pos__[i]) {
//			TRACE_ERROR("1: %d\r\n", i);
//		}
//	}
//
//	for (i = 0; i < MAX_NUM_PD_Y_DLT; i++) {
//		if (s_pd_pos_y[i] != sensor_YPD_Pos__[i]) {
//			TRACE_ERROR("2: %d\r\n", i);
//		}
//	}

#else
    tp_layout_info.pd_pos_x = (float *)&sensor_XPD_Pos__[0];
    tp_layout_info.pd_pos_y = (float *)&sensor_YPD_Pos__[0];
    tp_layout_info.led_pos_x = (float *)&sensor_XLED_Pos__[0];
    tp_layout_info.led_pos_y = (float *)&sensor_YLED_Pos__[0];
#endif
    tp_layout_info.hor_pd_num = X_CELL_SIZE;
    tp_layout_info.ver_pd_num = Y_CELL_SIZE;
    tp_layout_info.maxOfstValX = X_MAX_OFFSET;
    tp_layout_info.maxOfstValY = Y_MAX_OFFSET;
    tp_layout_info.maxOfstValX2 = 0; //reserved
    tp_layout_info.maxOfstValY2 = 0; //reserved
    tp_layout_info.senTblX2.len = 0; //reserved
    tp_layout_info.senTblY2.len = 0; //reserved
    tp_layout_info.logical_x_size = s_logical_max_x;
    tp_layout_info.logical_y_size = s_logical_max_y;

    g_back_conf.numPdX = X_CELL_SIZE;
    g_back_conf.numPdY = Y_CELL_SIZE;
    g_back_conf.numOfsX = X_TOTAL_OFFSET;
    g_back_conf.numOfsY = Y_TOTAL_OFFSET;
    g_back_conf.maxOfsX = X_MAX_OFFSET;
    g_back_conf.maxOfsY = Y_MAX_OFFSET;
    g_back_conf.slope_byte_x = ((g_back_conf.numOfsX - 1) / 8 + 1);
    g_back_conf.slope_byte_y = ((g_back_conf.numOfsY - 1) / 8 + 1);

//#if (TEST_DEAD_CELL_TBL == 1U)//for test
    deadcell_info.hor_pd_len = 0;
    deadcell_info.hor_led_len = 0;
    deadcell_info.ver_pd_len = 0;
    deadcell_info.ver_led_len = 0;
#if 0
    deadcell_info.hor_dead_pd[0] = 10;
    deadcell_info.hor_dead_led[0] = 11;
    deadcell_info.ver_dead_pd[0] = 20;
    deadcell_info.ver_dead_led[0] = 21;
#endif
//#endif

    touchMode = APP_DRAW_PEN_MARKER;						 //YJ@230207
#if 0 //for test
    TRACE("tp_layout_info.hor_pd_num = %d %d", (int)tp_layout_info.hor_pd_num, (int)tp_layout_info.ver_pd_num);
    TRACE("tp_layout_info.maxOfstValX = %d %d", (int)tp_layout_info.maxOfstValX, (int)tp_layout_info.maxOfstValY);
#endif

    s_init_post_processing();
    tp_init_post_processing();

#if 1//(DEAD_CELL_TBL == 1) //shj@190925
    ret = BG_init_backend(&tp_layout_info, &deadcell_info, 0);
#else
    BG_init_backend(&tp_layout_info, 0, 0);
#endif
    //return BACKEND_OK;
    return ret;
}


uint32_t diff;
uint32_t diff_2;

uint8_t backendProcess(void)
{
    uint8_t ret = 0;
    switch(backendRunState)
    {
    case BACKEND_STATE_INIT:
        backendRunState = BACKEND_STATE_LINE_DATA;
        break;

    case BACKEND_STATE_LINE_DATA:
        //TRACEBE("z");
        if(backend_process_line_data() == BACKEND_OK)
        {
            //while(isDelayScanTimerExpired() ==0);
            //setDelayScanTimer10Msec(100);
            //delay_test(5000);
//        	if(g_forcedagc.fdltmode == TRUE)
//          {
//                g_forcedagc.fdltmode = FALSE;
//            break;
//          }
#if 1
#ifdef HOR_EDGE_TOUCH_ENABLE
            if (s_output_buffer.len == 0 && s_wheel_delta != 0)
            {

#ifdef WHEEL_REPORT_TIME_FIX
                wheel_Send_time +=s_wheel_get_time_diff();
                while(wheel_Send_time < 60)		//60	100us
                {
                    MY_delay(30);
                    wheel_Send_time  += s_wheel_get_time_diff();
                }
#endif
                //TRACE("SendWheelReport %d\r\n", s_wheel_delta);
                SendWheelReport();
            }
            else

#endif
            {
                ret = s_output_buffer.len;
                //TRACE("BackEnd Line Data %d\r\n", ret);
                if(ret != 0)
                {
#if REPORT_TIME_FIX
                    Digitizer_Send_time +=s_Send_get_time_diff();
                    while(Digitizer_Send_time < 60)		//60	100us
                    {
                        MY_delay(30);
                        Digitizer_Send_time  += s_Send_get_time_diff();
                    }
#endif
                    //TRACE("BackEnd Line Data %d\r\n", ret);
                    SendDigitizerReport((DEF_DATA_INFO *)&s_output_buffer);

#if (ONEPOINT_6MSFIX_REPORT)
                    while(isDelayTimerExpired() == 0)
                    {

                    }
#endif
                }
            }
#endif
        }
        else
        {
        	TRACE_ERROR("BackEnd Line Data Error!!!\r\n");
        }
        break;

    default:
        break;
    }
//    M_NOP_Delay_1usec();
    return ret;
}
app_mode_t ReceiveCmdParser(void)
{
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
    usb_device_hid_generic_struct_t *genericDeviceInstance = &g_UsbDeviceHidGeneric;
    //usb_hid_generic_struct_t *hidDeviceInstance = &s_UsbDeviceHidGeneric;
    usb_device_normal_cmd_t rcvCmd;
    uint8_t *rcvUSBBuf;
    //TRACE_RCP("=%d/%d/%d/%d", appRunState, appMode, appScanSleep, hidDeviceInstance->DataRecv);

#ifdef WMC_MODE_CHANGE //YJ@230207
        touchModePrev = touchMode;
#endif

    if (genericDeviceInstance->DataRecv == 0)
    {
        if(appCurrMode == APP_MODE_DLT)
        {
            appCurrMode = APP_MODE_WAIT_DLT_CMD;
        }

        return appCurrMode;
    }
    TRACE_RCP("a");
    genericDeviceInstance->DataRecv = 0;
    rcvUSBBuf = normalCmdProcess();
    if(rcvUSBBuf == NULL) return appCurrMode;

    rcvCmd = rcvUSBBuf[0];
    TRACE_RCP("rcvCmd=0x%x", rcvCmd);

    switch(rcvCmd)
    {
    case USB_REQ_OFST:
    case USB_REQ_LINE:
    case USB_REQ_LINE_ADC0:
    case USB_REQ_LINE_THRESHOLD_DATA:
        dltParmInit(rcvCmd, rcvUSBBuf);
        appNextMode = APP_MODE_DLT;
        TRACE_RCP("USB_CMD_DLT %d",rcvCmd );
        break;

    case USB_NORCMD_DLT:
        TRACE_RCP("USB_NORCMD_DLT");
        normalSendRes(USB_RES_OK, 0);
#if 0  //nsmoon@201222
        USB_DeviceApplicationDeInit();
        USB_DeviceApplicationInitSpeed(USB_SPEED_HIGH);
#endif
        appNextMode = APP_MODE_WAIT_DLT_CMD;
        break;
        /**********************************************************/
    case USB_NORCMD_NONE:
    case USB_NORCMD_NORMAL:
        normalSendRes(USB_RES_OK, 0);
        appNextMode = APP_MODE_BACKEND;
        break;

    case USB_NORCMD_RESET:
        TRACE_RCP("USB_NORCMD_RESET");
        DoSoftReset(MAX_RESET_DELAY_CNT); //nsmoon@201222
        normalSendRes(USB_RES_OK, 0);
        //appNextMode = APP_MODE_RESET;							//YJ@230222
        break;

#if 1 //nsmoon@201222
    case USB_NORCMD_READ_VER:
        TRACE_RCP("USB_NORCMD_READ_VER");
        normalSendRes(USB_RES_OK, USB_NORCMD_READ_VER);
        break;
#endif

#if ENABLE_SCAN_HIGH_GAIN_DLT //nsmoon@220404
    case USB_NORCMD_GAIN_HIGH:
        scanGainHigh = rcvUSBBuf[2];
        TRACE_RCP("USB_NORCMD_GAIN_HIGH %d", scanGainHigh);
        normalSendRes(USB_RES_OK, 0);
        break;
#endif
    case USB_NORCMD_DRAW_STAT:
        TRACE_RCP("USB_NORCMD_DRAW_STAT");
        normalSendRes(USB_RES_OK, (uint8_t)touchMode);
        break;

    case USB_NORCMD_DRAW_PEN_MARKER :
        TRACE_RCP("USB_NORCMD_DRAW_PEN_MARKER");
        touchMode = APP_DRAW_PEN_MARKER;
        normalSendRes(USB_RES_OK, (uint8_t)touchMode);
        break;

    case USB_NORCMD_DRAW_BRUSH :
        TRACE_RCP("USB_NORCMD_DRAW_BRUSH");
        touchMode = APP_DRAW_BRUSH;
        normalSendRes(USB_RES_OK, (uint8_t)touchMode);
        break;

    default:
        TRACE_RCP("USB_NORCMD_UNKNOWN(%x)", rcvCmd);
        normalSendRes(USB_RES_OK, 0);
        //appNextMode = APP_MODE_ERROR;
        break;
    }

    appCurrMode = appNextMode;
#endif
    return appNextMode;
}

void APP_Tasks(void)
{
//  uint8_t appScanSleep;


    //setDelayScanTimer10Msec(1);
    //while (isDelayScanTimerExpired() == 0);
#if 1 //nsmoon@201222, move to check_reset_delay
    int ret;
    ret = check_reset_delay();
    if (ret) {
        if (ret == 1) {
            appRunState = APP_STATE_DO_RESET;
        }
        else {
            return; //idle
        }
    }
#endif
    switch (appRunState)
    {
    case APP_STATE_INIT:
    {
        APP_Init();
        debug_count = 0;
        backendInit();
        s_set_fagc_time();
//        appScanSleep = 1;
        //dltInit();
        //if(USBDeviceConnected())
        //if (USB_HostVendorIsReadyToRead() == USB_HOST_RDY)
        //{
        appRunState = APP_STATE_SCAN;
        //}
        appMode = APP_MODE_BACKEND;
    }
    break;

    case APP_STATE_SCAN:
    {
        /*if(!appScanSleep)
        {
            Scan_Start_Time();
            Scan_Wait_Time(WAIT_TIME_US(50000));
        }*/
        //DbgConsole_Printf("appMode=%d\r\n", appMode);
        if(appMode == APP_MODE_RESET)
        {
            appRunState = APP_STATE_DO_RESET; //nsmoon@201222, move to check_reset_delay
            //break;
        }
        else if (appMode != APP_MODE_WAIT_DLT_CMD)
        {
            APP_TIME_INIT();
#if (ONEPOINT_6MSFIX_REPORT)
        	set_DelayTimerUsec(6000);	// 6ms
#endif
            scan_status = Scan_Process();
            APP_TIME_ADD(0);
#ifdef BACKEND_TEST_DATA
            Make_Test_Data();
#endif
            APP_TIME_ADD(1);
            if(scan_status == SCAN_INIT_COMPLETE)
            {
                //TRACE("APP_MODE_BACKEND..\r\n");
                backendProcess();
                APP_TIME_ADD(4);
                APP_TIME_PRINT();

                if (appMode == APP_MODE_DLT)
                {
                    //TRACE("$$APP_MODE_DLT..........a");
                    dltProcess();
                   // g_forcedagc.fdltmode = TRUE;
                }
            }
        }

		M_NOP_Delay_2usec();


    }
    break;

#if 1
    case APP_STATE_DO_RESET:
    {
        //WDOG_Software_Reset();
        MY_WDOG_soft_reset();
        while(1);					//YJ@230222 
    }
    break;
#endif

    default:
        TRACE_ERROR("ERROR! unknown appRunState %d", appRunState);
        appRunState = APP_STATE_SCAN;
        break;
    }

    appMode = ReceiveCmdParser();
#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
    USB_DeviceCdcVcomTask();
#endif
}

#endif

void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void test_mode_func(void)
{
#if 1
    scanAxisFull(Y_AXIS, TRUE);
    scanAxisFull(X_AXIS, TRUE);
    Scan_Data_Process();
#if 1
	uint8_t buf[64];
	uint8_t cnt = 0;
    buf[cnt++] = 0x06;
    for (uint8_t i = cnt; i < 63; i++)
    {
    	buf[cnt++] = 0xFF;
    }
    backend_process_line_data();
  //  USB_SendDigitizerReport(buf, cnt) ;
//#else
   //dltProcess();
    //backendProcess();
   // if(backend_process_line_data() == BACKEND_OK)
    {
    	//backend_process_line_data();
    	if (s_output_buffer.len == 0 && s_wheel_delta != 0) SendWheelReport();
    	else if(s_output_buffer.len > 0) SendDigitizerReport((DEF_DATA_INFO *)&s_output_buffer);
        else USB_SendDigitizerReport(buf, cnt) ;
    }
#endif
    ReceiveCmdParser();
    if(s_reset_delay > 0)
    {
    	MY_WDOG_soft_reset();
         while(1);					//YJ@230222
    }
#else
    scanAxisFull(Y_AXIS, TRUE);
    scanAxisFull(X_AXIS, TRUE);
    Scan_Data_Process();
    dltProcess();
    ReceiveCmdParser();
#endif
}
int main(void)
{
#if (MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
    usb_host_vendor_generic_instance_t *genericHostInstance = &g_HostVendorGeneric;
    usb_device_hid_generic_struct_t *genericDeviceInstance = &g_UsbDeviceHidGeneric;
#endif

#ifdef WHEEL_REPORT_TIME_FIX
    //uint32_t wheel_Send_time = 0;
#endif

    BOARD_ConfigMPU();
    BOARD_InitPins(); //nsmoon@190116
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
#if (MODEL_SPT != MODEL_CTSK_N650_V100)&&(MODEL_SPT != MODEL_CTSK_N750_V100)&&(MODEL_SPT != MODEL_CTSK_N850_V100)
    USB_HostApplicationInit();
#endif
    //USB_DeviceApplicationInit(USB_SPEED_FULL); //nsmoon@201211, full_speed
    USB_DeviceApplicationInit();

#if (MODEL_SPT == MODEL_CTSK_N650_V100)||(MODEL_SPT == MODEL_CTSK_N750_V100)||(MODEL_SPT == MODEL_CTSK_N850_V100)
    /* Initialize components */
    BOARD_InitBootPeripherals();
    MY_GTP_init(); //nsmoon@190114
    DWT_Init();
#else
    MY_GTP_init(); //nsmoon@190114
    MY_GPIO_init(); //nsmoon@190116
#endif
#if 0
#if USE_FW_KEY_VALIDATION
    MY_OCOTP_init();
#else
    get_header0();
    //get_header1(); //nsmoon@200706
#endif
#endif
    get_header1(); //nsmoon@200706

    MY_WDOG_system_reset_reason();
    MY_WDOG_reset_init();

    appRunState = APP_STATE_INIT;
    //appRunSubState = APP_SUB_STATE_CMD;

#ifdef NORMAL_RESET_DELAY //nsmoon@200810
    s_reset_delay = -1;
    s_reset_delay_dot = 0;
#endif
#if (ENABLE_DEBUG_MESSAGE == 1) //for test, nsmoon@201203
    uint32_t freq = CLOCK_GetFreq(kCLOCK_CpuClk);
    TRACE("Build @ %s %s (%d)", __DATE__, __TIME__, freq);
#endif
#if 0 //for test //nsmoon@210430
    next_scan_t nextScan;
    TRACE("sizeof(next_scan_t)= %d", sizeof(next_scan_t)); //14
    TRACE("sizeof(nextScan)= %d", sizeof(nextScan)); //14
    TRACE("sizeof(nextScan.currScan)= %d", sizeof(nextScan.currScan)); //1
    TRACE("sizeof(nextScan.nextScan)= %d", sizeof(nextScan.nextScan)); //1
    TRACE("sizeof(nextScan.brushMode)= %d", sizeof(nextScan.brushMode)); //1
    TRACE("sizeof(nextScan.numTouch)= %d", sizeof(nextScan.numTouch)); //2
    TRACE("sizeof(nextScan.x1)= %d", sizeof(nextScan.x1)); //2
#endif
#ifdef SPEED_TEST_FLOAT //nsmoon@210205
    executeSpeedTest_init();
    while (1) {
        WDOG_Refresh(MY_WDOG_BASE);
        executeSpeedTest();
    }
#endif
#if defined(WAIT_FOR_READ_VERSION) //nsmoon@211222
    frameCount = 0;
    verReadFlag = 0;
#endif
    contError = contTouch = contTouch2= 0; //nsmoon@211231
#if ENABLE_SCAN_HIGH_GAIN_DLT //nsmoon@220420
    scanGainHigh = 0; //init = 0;
#endif
    Scan_Initialize();
#if (ENABLE_UART_CMD_PROCESS == DEBUG_UART_MOME)
 //   uartCmdProcessInit();
#elif(ENABLE_UART_CMD_PROCESS == DEBUG_VCOM_MODE)
    vcom_init();
    //display_menu();
#endif
    while (1)
    {
        WDOG_Refresh(MY_WDOG_BASE);

        APP_Tasks();

        //test_mode_func();

#if (ENABLE_UART_CMD_PROCESS == DEBUG_UART_MOME)
		  //uartCmdProcess();
#endif

    } //while (1)
}

/* end of file */
