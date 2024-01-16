/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __APP_H__
#define __APP_H__
//#include "usb_host_config.h"
//#include "usb_host.h"
#include "fsl_device_registers.h"
#include "backend.h"
#include "dlt.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if ((defined USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI))
#error FIXME
#define CONTROLLER_ID kUSB_ControllerKhci0
#endif /* USB_HOST_CONFIG_KHCI */
#if ((defined USB_HOST_CONFIG_EHCI) && (USB_HOST_CONFIG_EHCI))
#define HOST_CONTROLLER_ID kUSB_ControllerEhci1 //kUSB_ControllerEhci0
#endif /* USB_HOST_CONFIG_EHCI */
#if ((defined USB_HOST_CONFIG_OHCI) && (USB_HOST_CONFIG_OHCI))
#error FIXME
#define CONTROLLER_ID kUSB_ControllerOhci0
#endif /* USB_HOST_CONFIG_OHCI */
#if ((defined USB_HOST_CONFIG_IP3516HS) && (USB_HOST_CONFIG_IP3516HS))
#error FIXME
#define CONTROLLER_ID kUSB_ControllerIp3516Hs0
#endif /* USB_HOST_CONFIG_IP3516HS */

#if defined(__GIC_PRIO_BITS)
#define USB_HOST_INTERRUPT_PRIORITY (25U)
#elif defined(__NVIC_PRIO_BITS) && (__NVIC_PRIO_BITS >= 3)
#define USB_HOST_INTERRUPT_PRIORITY (6U)
#else
#define USB_HOST_INTERRUPT_PRIORITY (3U)
#endif

#if 0 //nsmoon@200715
#define FRONTEND_RESET_VAL(a)	//(GPIO_PinWrite(BOARD_FRONT_RESET_GPIO, BOARD_FRONT_RESET_GPIO_PIN, (a)))
#endif


#define BACKEND_OK		0
#define BACKEND_ERR		1

#define BRUSH_DELAY_REPORT_ENABLE
#define BRUSH_DELAY_SIZE	5

#define USB_NORCMD_LENGHT	10
typedef enum _usb_device_normal_cmd
{
	USB_REQ_OFST = 0x80,
	USB_REQ_LINE = 0x81,
	USB_REQ_LINE_ADC0 = 0x82,
    USB_REQ_SEN_LIFE_TEST = 0x83,
    USB_REQ_LINE_THRESHOLD_DATA = 0x84,
	USB_NORCMD_RESET = 0x0A,
	USB_NORCMD_DLT = 0x11, //change to DLT
	USB_NORCMD_NORMAL = 0x12, //change to NORMAL
	USB_NORCMD_DRAW_STAT = 0x13, //drawing mode
	USB_NORCMD_DRAW_PEN_MARKER = 0x14,
	USB_NORCMD_DRAW_BRUSH = 0x15,
	USB_NORCMD_READ_VER = 0x30, //nsmoon@200706
#if ENABLE_SCAN_HIGH_GAIN_DLT //nsmoon@220404
    USB_NORCMD_GAIN_HIGH = 0xA3,
#endif
	USB_NORCMD_NONE = 0xFF,
}  usb_device_normal_cmd_t;

#define USB_NORRES_LENGHT	5
#define MAX_VER_STRING		5 //include '\0'  //nsmoon@200706

typedef enum _usb_device_normal_res
{
	USB_RES_OK = 0x00,
	USB_RES_ERR = 0xFF,
}  usb_device_normal_res_t;

typedef enum _usb_host_app_state
{
    kStatus_DEV_Idle = 0, /*!< there is no device attach/detach */
    kStatus_DEV_Attached, /*!< device is attached */
    kStatus_DEV_Detached, /*!< device is detached */
} usb_host_app_state_t;

#define APP_OK		0
#define APP_ERR		1

typedef enum
{
    APP_STATE_INIT=0, /* Application's state machine's initial state. */
    APP_STATE_NORMAL,
#if(MODEL_SPT == MODEL_CTSK_N650_V100)||(MODEL_SPT == MODEL_CTSK_N750_V100)||(MODEL_SPT == MODEL_CTSK_N850_V100)
	APP_STATE_SCAN,
	APP_STATE_BACKEND,
#endif
    APP_STATE_DLT,
    APP_STATE_DLT_READY,
	APP_STATE_DO_RESET,
    APP_STATE_ERROR /* Application error state */
} app_state_t;


typedef enum
{
    BACKEND_STATE_INIT=0,
	BACKEND_STATE_LINE_DATA,
    BACKEND_STATE_ERROR /* Application error state */
} BACKEND_STATE_T;

typedef enum
{
	APP_MODE_BACKEND = 0,
	APP_MODE_DLT,
	APP_MODE_WAIT_DLT_CMD,
	APP_MODE_RESET,
	APP_MODE_ERROR
} app_mode_t;


typedef enum
{
    APP_DRAW_PEN_MARKER=0,
    APP_DRAW_BRUSH,
    APP_DRAW_ERROR
} touch_mode_t;

typedef enum
{
    APP_CMD_STATE_CMD=0,
    APP_CMD_STATE_RES,
    APP_CMD_STATE_ERROR
} app_cmd_state_t;


typedef struct {
    uint16_t rmFrameCnt, rmFrameCntPrev;
    uint8_t forced_agc, forced_saved;
//    uint8_t fdltmode;
} forcedagc;

typedef struct {
	uint8_t lastSend;
	uint8_t condition;
	int bufCnt;
	uint8_t brushIdx;
	uint16_t xCord[BRUSH_DELAY_SIZE];
	uint16_t yCord[BRUSH_DELAY_SIZE];
	uint16_t xSize[BRUSH_DELAY_SIZE];
	uint16_t ySize[BRUSH_DELAY_SIZE];
	uint16_t pressure[BRUSH_DELAY_SIZE];
	uint8_t reSend[BRUSH_DELAY_SIZE];
} brush_last_send;

extern forcedagc g_forcedagc;
extern backend_config_t g_back_conf;
extern uint8_t g_usb_in_buff[EXB_XFER_BUF_SIZE];		//MAX_USB_IN_BUF_SIZE
extern app_state_t appRunState;
extern uint8_t DltParam[USB_PARAM_LEN];
extern uint8_t LineThresholdValue;
extern touch_mode_t touchMode;
#ifdef WMC_MODE_CHANGE //YJ@230207
extern touch_mode_t touchModePrev;
#endif
extern float s_sensor_zero_x, s_sensor_zero_y, s_sensor_end_x, s_sensor_end_y;
extern float s_aarea_zero_x, s_aarea_zero_y, s_aarea_end_x, s_aarea_end_y;
extern float s_logical_max_x, s_logical_max_y;
extern uint8_t verReadFlag;	//YJ@211221
extern brush_last_send brushLastSend;
extern uint32_t s_get_frame_span(void);
extern void s_frontend_Reset();
extern void DoSoftReset(int cnt);	//YJ@211206
#if 1 //nsmoon@220207
#define FAGC_EXPIRED_TIME	20000 //2sec
extern void s_set_fagc_time(void);
extern int s_is_expired_fagc_time(uint32_t time100us);
extern int normal_dlt_send_data(uint8_t *txbuff, int len);
extern int backend_process_line_data(void);
extern uint16_t USB_CalculateCrc(uint8_t *data, uint32_t len);
#endif
#endif /* __APP_H__ */
