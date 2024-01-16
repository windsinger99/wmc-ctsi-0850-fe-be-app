/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __USB_DEVICE_HID_MOUSE_H__
#define __USB_DEVICE_HID_MOUSE_H__

#include "usb_device_config.h"

#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct _usb_device_hid_mouse_struct
{
    uint8_t *buffer;
    uint8_t idleRate;
} usb_device_hid_mouse_struct_t;

/*******************************************************************************
 * API
 ******************************************************************************/
extern usb_status_t USB_DeviceHidMouseInit(usb_device_composite_struct_t *deviceComposite);
extern usb_status_t USB_DeviceHidMouseCallback(class_handle_t handle, uint32_t event, void *param);
extern usb_status_t USB_DeviceHidMouseSetConfigure(class_handle_t handle, uint8_t configure);
extern usb_status_t USB_DeviceHidMouseSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting);
extern int USB_SendMouseReport(uint8_t *transmitDataBuffer, int len);
extern uint8_t *USB_GetMouseBuffer(uint8_t *len);
#endif /*(USB_DEVICE_CONFIG_HID > 0)*/
#endif /* __USB_DEVICE_HID_MOUSE_H__ */
