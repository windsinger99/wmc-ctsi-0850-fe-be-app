/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016, 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _HOST_HID_GENERIC_H_
#define _HOST_HID_GENERIC_H_
#include "usb_host_config.h"
#include "usb_host.h"
#include "usb_host_hid.h"
#include "usb_phy.h"
#include "clock_config.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if ((defined USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI))
#define HOST_CONTROLLER_ID kUSB_ControllerKhci0
#endif /* USB_HOST_CONFIG_KHCI */
#if ((defined USB_HOST_CONFIG_EHCI) && (USB_HOST_CONFIG_EHCI))
#define HOST_CONTROLLER_ID kUSB_ControllerEhci1 //kUSB_ControllerEhci0
#endif /* USB_HOST_CONFIG_EHCI */
#if ((defined USB_HOST_CONFIG_OHCI) && (USB_HOST_CONFIG_OHCI))
#define HOST_CONTROLLER_ID kUSB_ControllerOhci0
#endif /* USB_HOST_CONFIG_OHCI */
#if ((defined USB_HOST_CONFIG_IP3516HS) && (USB_HOST_CONFIG_IP3516HS))
#define HOST_CONTROLLER_ID kUSB_ControllerIp3516Hs0
#endif /* USB_HOST_CONFIG_IP3516HS */

#if defined(__GIC_PRIO_BITS)
#define USB_HOST_INTERRUPT_PRIORITY (25U)
#elif defined(__NVIC_PRIO_BITS) && (__NVIC_PRIO_BITS >= 3)
#define USB_HOST_INTERRUPT_PRIORITY (6U)
#else
#define USB_HOST_INTERRUPT_PRIORITY (3U)
#endif

#define FRONTEND_VID_HEX	0x04D8
#define FRONTEND_PID_HEX	0x0053 //0x0320
#define FRONTEND_BT_PID_HEX	0x003C //bootloader pid

/*! @brief buffer for receiving report descriptor and data */
#define HID_GENERIC_IN_BUFFER_SIZE (64U) //(100U)
/*! @brief buffer for sending data */
#define HID_GENERIC_OUT_BUFFER_SIZE (64U) //(8U)

#define USB_HOST_OK		0
#define USB_HOST_EMPTY	1  //receive buff empty
#define USB_HOST_DETACH	2
#define USB_HOST_ERR	99

/*! @brief host app device attach/detach status */
typedef enum _usb_host_app_state
{
    kStatus_DEV_Idle = 0, /*!< there is no device attach/detach */
    kStatus_DEV_Attached, /*!< device is attached */
    kStatus_DEV_Detached, /*!< device is detached */
} usb_host_app_state_t;

/*! @brief host app run status */
typedef enum _usb_host_hid_generic_run_state
{
    kUSB_HostHidRunIdle = 0,                /*!< idle */
    kUSB_HostHidRunSetInterface,            /*!< execute set interface code */
    kUSB_HostHidRunWaitSetInterface,        /*!< wait set interface done */
    kUSB_HostHidRunSetInterfaceDone,        /*!< set interface is done, execute next step */
    kUSB_HostHidRunWaitSetIdle,             /*!< wait set idle done */
    kUSB_HostHidRunSetIdleDone,             /*!< set idle is done, execute next step */
    kUSB_HostHidRunWaitGetReportDescriptor, /*!< wait get report descriptor done */
    kUSB_HostHidRunGetReportDescriptorDone, /*!< get report descriptor is done, execute next step */
    kUSB_HostHidRunWaitSetProtocol,         /*!< wait set protocol done */
    kUSB_HostHidRunSetProtocolDone,         /*!< set protocol is done, execute next step */
    kUSB_HostHidRunWaitGetProtocol,         /*!< wait set protocol done */
    kUSB_HostHidRunGetProtocolDone,         /*!< set protocol is done, execute next step */
    kUSB_HostHidRunWaitDataReceived,        /*!< wait interrupt in data */
    kUSB_HostHidRunDataReceived,            /*!< interrupt in data received */
    kUSB_HostHidRunPrimeDataReceive,        /*!< prime interrupt in receive */
    kUSB_HostHidRunWaitDataTransmitted,     /*!< wait interrupt in data */
    kUSB_HostHidRunDataTransmitted,         /*!< interrupt in data transmitted */
    kUSB_HostHidRunDataTransmitting,
    kUSB_HostHidRunWaitDataReceiving,
} usb_host_hid_generic_run_state_t;

/*! @brief USB host hid generic instance structure */
typedef struct _usb_host_hid_generic_instance
{
    usb_host_configuration_handle configHandle; /*!< the hid generic's configuration handle */
    usb_device_handle deviceHandle;             /*!< the hid generic's device handle */
    usb_host_class_handle classHandle;          /*!< the hid generic's class handle */
    usb_host_interface_handle interfaceHandle;  /*!< the hid generic's interface handle */
    uint8_t deviceState;                        /*!< device attach/detach status */
    uint8_t prevState;                          /*!< device attach/detach previous status */
    uint8_t runState;                           /*!< hid generic application run status */
    uint8_t
        runWaitState; /*!< hid generic application wait status, go to next run status when the wait status success */
    uint16_t inMaxPacketSize;  /*!< Interrupt in max packet size */
    uint16_t outMaxPacketSize; /*!< Interrupt out max packet size */
    uint8_t *genericInBuffer;
    //uint8_t *genericInBufferSav;
    uint8_t *genericOutBuffer;
    uint16_t sendIndex; /*!< data sending position */
    uint8_t rxDone;
#if 0 //ndef FRONTEND_RESET_ENABLE
    uint8_t softReset;
#endif
    uint8_t modeChange;
    uint32_t hostWaitCnt;
} usb_host_hid_generic_instance_t;

/*******************************************************************************
 * API
 ******************************************************************************/
extern usb_host_handle g_HostHandle;
extern usb_host_hid_generic_instance_t g_HostHidGeneric;

extern void USB_HostClockInit(void);
extern void USB_HostIsrEnable(void);
extern void USB_HostApplicationInit(void);
extern void USB_HostApplicationDeinit(void);
extern void USB_HostTaskFn(void *param);
extern usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode);
extern void USB_HostHidGenericTask(void *param);
extern usb_status_t USB_HostHidGenericEvent(usb_device_handle deviceHandle,
                                            usb_host_configuration_handle configurationHandle,
                                            uint32_t eventCode);
extern int USB_HostCmd(int cmd);
extern int USB_HostSendPacket(uint8_t *buff, int len);
extern int USB_HostRecvPacket(uint8_t *buff);
extern int USB_HostIsReady();
extern void USB_host_delay(uint32_t maxCnt);
#endif /* _HOST_HID_GENERIC_H_ */
