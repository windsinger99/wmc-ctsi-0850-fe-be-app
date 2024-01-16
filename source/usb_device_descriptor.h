/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __USB_DEVICE_DESCRIPTOR_H__
#define __USB_DEVICE_DESCRIPTOR_H__

#include "backend.h"
#include "usb.h"
#include "usb_phy.h"
#include "usb_device_config.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_hid.h"
#include "usb_device_ch9.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define USB_DEVICE_SPECIFIC_BCD_VERSION (0x0200U)
#define USB_DEVICE_DEMO_BCD_VERSION (0x0101U)

#define USB_DEVICE_VID	0x1FC9

#if (MODEL_SPT == MODEL_CTKS_750_V140)
 #ifdef FRONTEND_0512_ENABLE
  #define USB_DEVICE_PID	0x0761//FE New Version YJ@210428
 #else
  #define USB_DEVICE_PID	0x0751
 #endif
#elif(MODEL_SPT == MODEL_CTSK_N750_V100)
 #define USB_DEVICE_PID		0x0771
#elif (MODEL_SPT == MODEL_CTSK_850_V100)
 #ifdef FRONTEND_0512_ENABLE
  #define USB_DEVICE_PID	0x0760//FE New Version YJ@210428
 #else
  #define USB_DEVICE_PID	0x0750
 #endif
#elif(MODEL_SPT == MODEL_CTSK_N850_V100)
 #define USB_DEVICE_PID		0x750	//0x0770	//YJ@231017 change PID
#elif (MODEL_SPT == MODEL_CTSK_650_V100)
 #define USB_DEVICE_PID	0x0753
#elif (MODEL_SPT == MODEL_CTSK_N650_V100)		//YJ@230523
 #define USB_DEVICE_PID	0x0763
#elif (MODEL_SPT == MODEL_CTSK_550_V100)
 #define USB_DEVICE_PID	0x0754
#endif

#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
#define USB_DEVICE_CLASS (0xEF)
#define USB_DEVICE_SUBCLASS (0x02)
#define USB_DEVICE_PROTOCOL (0x01)
#else
#define USB_DEVICE_CLASS (0x00U)
#define USB_DEVICE_SUBCLASS (0x00U)
#define USB_DEVICE_PROTOCOL (0x00U)
#endif

#define USB_DEVICE_MAX_POWER (250) //(0x32U) //nsmoon@210609

#define USB_DESCRIPTOR_LENGTH_STRING0 (sizeof(g_UsbDeviceString0))
#define USB_DESCRIPTOR_LENGTH_STRING1 (sizeof(g_UsbDeviceString1))
#define USB_DESCRIPTOR_LENGTH_STRING2 (sizeof(g_UsbDeviceString2))
#define USB_DESCRIPTOR_LENGTH_STRING3 (sizeof(g_UsbDeviceString3))
#define USB_DESCRIPTOR_LENGTH_STRING4 (sizeof(g_UsbDeviceString4))

#define USB_DEVICE_CONFIGURATION_COUNT (1U)
#define USB_DEVICE_STRING_COUNT (7U) //(5U) //nsmoon@230321 5=>7
#define USB_DEVICE_LANGUAGE_COUNT (1U)

#define USB_COMPOSITE_CONFIGURE_INDEX (1U)

#if (USB_DEVICE_CONFIG_HID > 0) && (USB_DEVICE_CONFIG_CDC_ACM > 0)
 #define USB_COMPOSITE_INTERFACE_COUNT (USB_HID_GENERIC_INTERFACE_COUNT + USB_HID_MOUSE_INTERFACE_COUNT + USB_CDC_VCOM_INTERFACE_COUNT)
 #define USB_COMPOSITE_CLASS_COUNT (3)
#elif (USB_DEVICE_CONFIG_HID > 0) && (USB_DEVICE_CONFIG_CDC_ACM == 0)
 #define USB_COMPOSITE_INTERFACE_COUNT (USB_HID_GENERIC_INTERFACE_COUNT + USB_HID_MOUSE_INTERFACE_COUNT)
 #define USB_COMPOSITE_CLASS_COUNT (2)
#elif (USB_DEVICE_CONFIG_HID == 0) && (USB_DEVICE_CONFIG_CDC_ACM > 0)
 #define USB_COMPOSITE_INTERFACE_COUNT (USB_CDC_VCOM_INTERFACE_COUNT)
 #define USB_COMPOSITE_CLASS_COUNT (1)
#else
 #error "ERROR! not defined USB_DEVICE_CONFIG_HID or USB_DEVICE_CONFIG_CDC_ACM"
#endif

#define USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL (sizeof(g_UsbDeviceConfigurationDescriptor))

#define USB_DEVICE_16bitTo8bitArrange(a)	(a&0xFF),((a>>8)&0xFF)
#define USB_DEVICE_32bitTo8bitArrange(a)	(a&0xFF),((a>>8)&0xFF),((a>>16)&0xFF),((a>>24)&0xFF)

#define USB_DEVICE_EP0_BUFFER_SIZE			64

#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
////////////////////////////////////////////////////////
// mouse
////////////////////////////////////////////////////////
#define USB_HID_MOUSE_INTERFACE_COUNT (1U)
#define USB_HID_MOUSE_INTERFACE_INDEX (1U)
#define USB_HID_MOUSE_INTERFACE_ALTERNATE_COUNT (1U)
#define USB_HID_MOUSE_INTERFACE_ALTERNATE_0 (0U)
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
////////////////////////////////////////////////////////
#define USB_HID_GENERIC_CLASS (0x03U)
#define USB_HID_GENERIC_SUBCLASS (0x00U)
#define USB_HID_GENERIC_PROTOCOL (0x00U)

#define USB_HID_GENERIC_INTERFACE_COUNT (1U)
#define USB_HID_GENERIC_INTERFACE_INDEX (0U)
#define USB_HID_GENERIC_INTERFACE_ALTERNATE_COUNT (1U)
#define USB_HID_GENERIC_INTERFACE_ALTERNATE_0 (0U)
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
#define USB_DESCRIPTOR_LENGTH_HID_MOUSE_REPORT (sizeof(g_UsbDeviceHidMouseReportDescriptor))
#define USB_DESCRIPTOR_LENGTH_HID_GENERIC_REPORT (sizeof(g_UsbDeviceHidGenericReportDescriptor))
#define USB_DESCRIPTOR_LENGTH_HID (9U)

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

#if 1 //def TP_20_TEST
#define MAX_VALID_CONTACT_POINTS            (uint8_t)ALLOWABLE_TOUCH_BE //(uint8_t)0x14
#else
#define MAX_VALID_CONTACT_POINTS            (uint8_t)0x0a
#endif

#define MAX_TOUCH_DATA_SIZE					60			  // 6byte, object (10)
#define MAX_TOUCH_DATA_SIZE_WITH_SIZE		50            // 10bytes * 5
#endif
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// CDC VCOM
////////////////////////////////////////////////////////
#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
/* Communication Class SubClass Codes */
#define USB_CDC_DIRECT_LINE_CONTROL_MODEL (0x01)
#define USB_CDC_ABSTRACT_CONTROL_MODEL (0x02)
#define USB_CDC_TELEPHONE_CONTROL_MODEL (0x03)
#define USB_CDC_MULTI_CHANNEL_CONTROL_MODEL (0x04)
#define USB_CDC_CAPI_CONTROL_MOPDEL (0x05)
#define USB_CDC_ETHERNET_NETWORKING_CONTROL_MODEL (0x06)
#define USB_CDC_ATM_NETWORKING_CONTROL_MODEL (0x07)
#define USB_CDC_WIRELESS_HANDSET_CONTROL_MODEL (0x08)
#define USB_CDC_DEVICE_MANAGEMENT (0x09)
#define USB_CDC_MOBILE_DIRECT_LINE_MODEL (0x0A)
#define USB_CDC_OBEX (0x0B)
#define USB_CDC_ETHERNET_EMULATION_MODEL (0x0C)

/* Communication Class Protocol Codes */
#define USB_CDC_NO_CLASS_SPECIFIC_PROTOCOL (0x00) /*also for Data Class Protocol Code */
#define USB_CDC_AT_250_PROTOCOL (0x01)
#define USB_CDC_AT_PCCA_101_PROTOCOL (0x02)
#define USB_CDC_AT_PCCA_101_ANNEX_O (0x03)
#define USB_CDC_AT_GSM_7_07 (0x04)
#define USB_CDC_AT_3GPP_27_007 (0x05)
#define USB_CDC_AT_TIA_CDMA (0x06)
#define USB_CDC_ETHERNET_EMULATION_PROTOCOL (0x07)
#define USB_CDC_EXTERNAL_PROTOCOL (0xFE)
#define USB_CDC_VENDOR_SPECIFIC (0xFF) /*also for Data Class Protocol Code */

/* Data Class Protocol Codes */
#define USB_CDC_PYHSICAL_INTERFACE_PROTOCOL (0x30)
#define USB_CDC_HDLC_PROTOCOL (0x31)
#define USB_CDC_TRANSPARENT_PROTOCOL (0x32)
#define USB_CDC_MANAGEMENT_PROTOCOL (0x50)
#define USB_CDC_DATA_LINK_Q931_PROTOCOL (0x51)
#define USB_CDC_DATA_LINK_Q921_PROTOCOL (0x52)
#define USB_CDC_DATA_COMPRESSION_V42BIS (0x90)
#define USB_CDC_EURO_ISDN_PROTOCOL (0x91)
#define USB_CDC_RATE_ADAPTION_ISDN_V24 (0x92)
#define USB_CDC_CAPI_COMMANDS (0x93)
#define USB_CDC_HOST_BASED_DRIVER (0xFD)
#define USB_CDC_UNIT_FUNCTIONAL (0xFE)

/* Descriptor SubType in Communications Class Functional Descriptors */
#define USB_CDC_HEADER_FUNC_DESC (0x00)
#define USB_CDC_CALL_MANAGEMENT_FUNC_DESC (0x01)
#define USB_CDC_ABSTRACT_CONTROL_FUNC_DESC (0x02)
#define USB_CDC_DIRECT_LINE_FUNC_DESC (0x03)
#define USB_CDC_TELEPHONE_RINGER_FUNC_DESC (0x04)
#define USB_CDC_TELEPHONE_REPORT_FUNC_DESC (0x05)
#define USB_CDC_UNION_FUNC_DESC (0x06)
#define USB_CDC_COUNTRY_SELECT_FUNC_DESC (0x07)
#define USB_CDC_TELEPHONE_MODES_FUNC_DESC (0x08)
#define USB_CDC_TERMINAL_FUNC_DESC (0x09)
#define USB_CDC_NETWORK_CHANNEL_FUNC_DESC (0x0A)
#define USB_CDC_PROTOCOL_UNIT_FUNC_DESC (0x0B)
#define USB_CDC_EXTENSION_UNIT_FUNC_DESC (0x0C)
#define USB_CDC_MULTI_CHANNEL_FUNC_DESC (0x0D)
#define USB_CDC_CAPI_CONTROL_FUNC_DESC (0x0E)
#define USB_CDC_ETHERNET_NETWORKING_FUNC_DESC (0x0F)
#define USB_CDC_ATM_NETWORKING_FUNC_DESC (0x10)
#define USB_CDC_WIRELESS_CONTROL_FUNC_DESC (0x11)
#define USB_CDC_MOBILE_DIRECT_LINE_FUNC_DESC (0x12)
#define USB_CDC_MDLM_DETAIL_FUNC_DESC (0x13)
#define USB_CDC_DEVICE_MANAGEMENT_FUNC_DESC (0x14)
#define USB_CDC_OBEX_FUNC_DESC (0x15)
#define USB_CDC_COMMAND_SET_FUNC_DESC (0x16)
#define USB_CDC_COMMAND_SET_DETAIL_FUNC_DESC (0x17)
#define USB_CDC_TELEPHONE_CONTROL_FUNC_DESC (0x18)
#define USB_CDC_OBEX_SERVICE_ID_FUNC_DESC (0x19)

#define USB_DESCRIPTOR_LENGTH_CDC_HEADER_FUNC (5)
#define USB_DESCRIPTOR_LENGTH_CDC_CALL_MANAG (5)
#define USB_DESCRIPTOR_LENGTH_CDC_ABSTRACT (4)
#define USB_DESCRIPTOR_LENGTH_CDC_UNION_FUNC (5)

/* Configuration, interface and endpoint. */
#define USB_CDC_VCOM_CIC_CLASS (0x02)
#define USB_CDC_VCOM_CIC_SUBCLASS (0x02)
#define USB_CDC_VCOM_CIC_PROTOCOL (0x00)
#define USB_CDC_VCOM_DIC_CLASS (0x0A)
#define USB_CDC_VCOM_DIC_SUBCLASS (0x00)
#define USB_CDC_VCOM_DIC_PROTOCOL (0x00)

#define USB_CDC_VCOM_INTERFACE_COUNT (2)
#if (USB_DEVICE_CONFIG_HID > 0)
#define USB_CDC_VCOM_CIC_INTERFACE_INDEX (2)
#define USB_CDC_VCOM_DIC_INTERFACE_INDEX (3)
#else
#define USB_CDC_VCOM_CIC_INTERFACE_INDEX (0)
#define USB_CDC_VCOM_DIC_INTERFACE_INDEX (1)
#endif
#define USB_CDC_VCOM_CIC_INTERFACE_ALTERNATE_COUNT (1)
#define USB_CDC_VCOM_DIC_INTERFACE_ALTERNATE_COUNT (1)
#define USB_CDC_VCOM_CIC_INTERFACE_ALTERNATE_0 (0)
#define USB_CDC_VCOM_DIC_INTERFACE_ALTERNATE_0 (0)
#define USB_CDC_VCOM_CIC_ENDPOINT_COUNT (1)
#define USB_CDC_VCOM_CIC_INTERRUPT_IN_ENDPOINT (4)
#define USB_CDC_VCOM_DIC_ENDPOINT_COUNT (2)
#define USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT (5)
#define USB_CDC_VCOM_DIC_BULK_OUT_ENDPOINT (6)

/* Packet size. */
#define HS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE (16)
#define FS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE (16)
#define HS_CDC_VCOM_INTERRUPT_IN_INTERVAL (0x07) /* 2^(7-1) = 8ms */
#define FS_CDC_VCOM_INTERRUPT_IN_INTERVAL (0x08)

#define HS_CDC_VCOM_BULK_IN_PACKET_SIZE (512)
#define FS_CDC_VCOM_BULK_IN_PACKET_SIZE (64)
#define HS_CDC_VCOM_BULK_OUT_PACKET_SIZE (512)
#define FS_CDC_VCOM_BULK_OUT_PACKET_SIZE (64)

#define USB_DESCRIPTOR_TYPE_CDC_CS_INTERFACE (0x24)
#define USB_DESCRIPTOR_TYPE_CDC_CS_ENDPOINT (0x25)
#endif

#define USB_CDC_VCOM_REPORT_DESCRIPTOR_LENGTH (33)
#if (USB_DEVICE_CONFIG_CDC_ACM > 0)
#define USB_IAD_DESC_SIZE (8)
#else
#define USB_IAD_DESC_SIZE (0)
#endif
/////////////////////////////////////////////////////////////////////////////

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
