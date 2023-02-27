/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_DESCRIPTOR_H__
#define __USB_DEVICE_DESCRIPTOR_H__
#include "backend_postp.h" //nsmoon@201027
#include "backend.h"		//YJ@220117
#include "usb_device_class.h"		// /usb/device/class/
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define USB_DEVICE_SPECIFIC_BCD_VERSION (0x0200U)
#define USB_DEVICE_DEMO_BCD_VERSION (0x0101U)

#define USB_DEVICE_VID	0x1FC9

#if ((MODEL_SPT == MODEL_CTKS_750_V140)||(MODEL_SPT == MODEL_CTSK_N750_V100))
#ifdef FRONTEND_0512_ENABLE
#define USB_DEVICE_PID	0x0761//FE New Version YJ@210428
#else
#define USB_DEVICE_PID	0x0751
#endif
#elif ((MODEL_SPT == MODEL_CTSK_850_V100)||(MODEL_SPT == MODEL_CTSK_N850_V100))
#ifdef FRONTEND_0512_ENABLE
#define USB_DEVICE_PID	0x0760//FE New Version YJ@210428
#else
#define USB_DEVICE_PID	0x0750
#endif
#elif ((MODEL_SPT == MODEL_CTSK_650_V100)||(MODEL_SPT == MODEL_CTSK_N650_V100))
#define USB_DEVICE_PID	0x0753
#elif (MODEL_SPT == MODEL_CTSK_550_V100)
#define USB_DEVICE_PID	0x0754
#endif



#define USB_DEVICE_CLASS (0x00U)
#define USB_DEVICE_SUBCLASS (0x00U)
#define USB_DEVICE_PROTOCOL (0x00U)

#define USB_DEVICE_MAX_POWER (250) //(0x32U) //nsmoon@210609

#define USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL (sizeof(g_UsbDeviceConfigurationDescriptor))
#define USB_DESCRIPTOR_LENGTH_HID_MOUSE_REPORT (sizeof(g_UsbDeviceHidMouseReportDescriptor))
#define USB_DESCRIPTOR_LENGTH_HID_GENERIC_REPORT (sizeof(g_UsbDeviceHidGenericReportDescriptor))
#define USB_DESCRIPTOR_LENGTH_HID (9U)
#define USB_DESCRIPTOR_LENGTH_STRING0 (sizeof(g_UsbDeviceString0))
#define USB_DESCRIPTOR_LENGTH_STRING1 (sizeof(g_UsbDeviceString1))
#define USB_DESCRIPTOR_LENGTH_STRING2 (sizeof(g_UsbDeviceString2))
#define USB_DESCRIPTOR_LENGTH_STRING3 (sizeof(g_UsbDeviceString3))
#define USB_DESCRIPTOR_LENGTH_STRING4 (sizeof(g_UsbDeviceString4))

#define USB_DEVICE_CONFIGURATION_COUNT (1U)
#define USB_DEVICE_STRING_COUNT (5U)
#define USB_DEVICE_LANGUAGE_COUNT (1U)

#define USB_COMPOSITE_CONFIGURE_INDEX (1U)

////////////////////////////////////////////////////////
// mouse
#define USB_HID_MOUSE_INTERFACE_COUNT (1U)
#define USB_HID_MOUSE_INTERFACE_INDEX (1U)
#define USB_HID_MOUSE_IN_BUFFER_LENGTH (8U)
#define USB_HID_MOUSE_ENDPOINT_COUNT (1U)
#define USB_HID_MOUSE_ENDPOINT_IN (3U)

#define USB_HID_MOUSE_CLASS (0x03U)
#define USB_HID_MOUSE_SUBCLASS (0x00U) //(0x01U)
#define USB_HID_MOUSE_PROTOCOL (0x02U)

#define USB_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE (8U)
#define HS_HID_MOUSE_INTERRUPT_IN_INTERVAL (0x01U) /* 2^(1-1)=125*1=125us */
#define FS_HID_MOUSE_INTERRUPT_IN_INTERVAL (0x01U) /* 2^(1-1)=1ms */

#define USB_HID_MOUSE_REPORT_LENGTH  (0x05U) //(0x04U) //nsmoon@21061
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// digitizer
#define USB_HID_GENERIC_CLASS (0x03U)
#define USB_HID_GENERIC_SUBCLASS (0x00U)
#define USB_HID_GENERIC_PROTOCOL (0x00U)

#define USB_HID_GENERIC_INTERFACE_COUNT (1U)
#define USB_HID_GENERIC_INTERFACE_INDEX (0U)
#define USB_HID_GENERIC_IN_BUFFER_LENGTH (64U)
#define USB_HID_GENERIC_OUT_BUFFER_LENGTH (64U)
#define USB_HID_GENERIC_ENDPOINT_COUNT (2U)
#define USB_HID_GENERIC_ENDPOINT_IN (1U)
#define USB_HID_GENERIC_ENDPOINT_OUT (2U)

#define USB_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE (64U)
#define HS_HID_GENERIC_INTERRUPT_OUT_INTERVAL (0x04U) /* 2^(4-1)=125*8=1ms */
#define FS_HID_GENERIC_INTERRUPT_OUT_INTERVAL (0x01U) /* 2^(1-1)=1ms */

#define USB_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE (64U)
#define HS_HID_GENERIC_INTERRUPT_IN_INTERVAL (0x01U) /* 2^(1-1)=125*1=125us */
#define FS_HID_GENERIC_INTERRUPT_IN_INTERVAL (0x01U) /* 2^(1-1)=1ms */

//#define USB_HID_GENERIC_REPORT_LENGTH (64U) //not-used
////////////////////////////////////////////////////////

#define USB_COMPOSITE_INTERFACE_COUNT (USB_HID_GENERIC_INTERFACE_COUNT + USB_HID_MOUSE_INTERFACE_COUNT)

#define MULTI_TOUCH_DATA_REPORT_ID			(uint8_t)0x01
#define VALID_CONTACTS_FEATURE_REPORT_ID	(uint8_t)0x02
#define DEVICE_MODE_FEATURE_REPORT_ID		(uint8_t)0x03
#define MOUSE_DATA_REPORT_ID				(uint8_t)0x04
#define SINGLE_TOUCH_DATA_REPORT_ID			(uint8_t)0x05
#define DIAGNOSTIC_REPORT_ID                (uint8_t)0x06

#define X_LOGICAL_MAX                       TEST_LOGICAL_X_MAX
#define Y_LOGICAL_MAX                       TEST_LOGICAL_Y_MAX

#define X_LOGICAL_MIN                       0
#define X_PHYSICAL_MAX                      TEST_PHYSICAL_X_MAX
#define X_PHYSICAL_MIN                      0

#define Y_LOGICAL_MIN                       0
#define Y_PHYSICAL_MAX                      TEST_PHYSICAL_Y_MAX
#define Y_PHYSICAL_MIN                      0

#define USB_DEVICE_16bitTo8bitArrange(a)	(a&0xFF),((a>>8)&0xFF)
#define USB_DEVICE_32bitTo8bitArrange(a)	(a&0xFF),((a>>8)&0xFF),((a>>16)&0xFF),((a>>24)&0xFF)

#if 1 //def TP_20_TEST
#define MAX_VALID_CONTACT_POINTS            (uint8_t)ALLOWABLE_TOUCH_BE //(uint8_t)0x14
#else
#define MAX_VALID_CONTACT_POINTS            (uint8_t)0x0a
#endif
#define USB_DEVICE_EP0_BUFFER_SIZE			64

#define MAX_TOUCH_DATA_SIZE					60			  // 6byte, object (10)
#define MAX_TOUCH_DATA_SIZE_WITH_SIZE		50            // 10bytes * 5

/*******************************************************************************
 * API
 ******************************************************************************/

/* Configure the device according to the USB speed. */
extern usb_status_t USB_DeviceSetSpeed(usb_device_handle handle, uint8_t speed);
#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
/* Get device qualifier descriptor request */
usb_status_t USB_DeviceGetDeviceQualifierDescriptor(
    usb_device_handle handle, usb_device_get_device_qualifier_descriptor_struct_t *deviceQualifierDescriptor);
#endif
/* Get device descriptor request */
usb_status_t USB_DeviceGetDeviceDescriptor(usb_device_handle handle,
                                           usb_device_get_device_descriptor_struct_t *deviceDescriptor);

/* Get device configuration descriptor request */
usb_status_t USB_DeviceGetConfigurationDescriptor(usb_device_handle handle,
											usb_device_get_configuration_descriptor_struct_t *configurationDescriptor);

/* Get device string descriptor request */
usb_status_t USB_DeviceGetStringDescriptor(usb_device_handle handle,
                                           usb_device_get_string_descriptor_struct_t *stringDescriptor);

/* Get hid descriptor request */
usb_status_t USB_DeviceGetHidDescriptor(usb_device_handle handle,
                                        usb_device_get_hid_descriptor_struct_t *hidDescriptor);

/* Get hid report descriptor request */
usb_status_t USB_DeviceGetHidReportDescriptor(usb_device_handle handle,
                                              usb_device_get_hid_report_descriptor_struct_t *hidReportDescriptor);

/* Get hid physical descriptor request */
usb_status_t USB_DeviceGetHidPhysicalDescriptor(usb_device_handle handle,
                                                usb_device_get_hid_physical_descriptor_struct_t *hidPhysicalDescriptor);

#endif /* __USB_DEVICE_DESCRIPTOR_H__ */
