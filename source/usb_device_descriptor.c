/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "composite.h"

#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
#include "hid_mouse.h"
#include "hid_generic.h"
#include "backend_postp.h"
#endif
#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
#include "virtual_com.h"
#endif
#include "usb_device_descriptor.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
/* hid generic endpoint information */
usb_device_endpoint_struct_t g_UsbDeviceHidGenericEndpoints[USB_HID_GENERIC_ENDPOINT_COUNT] = {
    /* HID generic interrupt IN pipe */
    {
        USB_HID_GENERIC_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
        USB_ENDPOINT_INTERRUPT,
        USB_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE,
    },
    /* HID generic interrupt OUT pipe */
    {
        USB_HID_GENERIC_ENDPOINT_OUT | (USB_OUT << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
        USB_ENDPOINT_INTERRUPT,
        USB_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE,
    },
};

/* HID generic interface information */
usb_device_interface_struct_t g_UsbDeviceHidGenericInterface[] = {{
    0U, /* The alternate setting of the interface */
    {
        USB_HID_GENERIC_ENDPOINT_COUNT, /* Endpoint count */
        g_UsbDeviceHidGenericEndpoints, /* Endpoints handle */
    },
    NULL,
}};

usb_device_interfaces_struct_t g_UsbDeviceHidGenericInterfaces[USB_HID_GENERIC_INTERFACE_COUNT] = {
    {
        USB_HID_GENERIC_CLASS,           /* HID generic class code */
        USB_HID_GENERIC_SUBCLASS,        /* HID generic subclass code */
        USB_HID_GENERIC_PROTOCOL,        /* HID generic protocol code */
        USB_HID_GENERIC_INTERFACE_INDEX, /* The interface number of the HID generic */
        g_UsbDeviceHidGenericInterface,  /* Interfaces handle */
        sizeof(g_UsbDeviceHidGenericInterface) / sizeof(usb_device_interface_struct_t),
    },
};

usb_device_interface_list_t g_UsbDeviceHidGenericInterfaceList[USB_DEVICE_CONFIGURATION_COUNT] = {
    {
        USB_HID_GENERIC_INTERFACE_COUNT, /* The interface count of the HID generic */
        g_UsbDeviceHidGenericInterfaces, /* The interfaces handle */
    },
};

usb_device_class_struct_t g_UsbDeviceHidGenericConfig = {
    g_UsbDeviceHidGenericInterfaceList, /* The interface list of the HID generic */
    kUSB_DeviceClassTypeHid,            /* The HID class type */
    USB_DEVICE_CONFIGURATION_COUNT,     /* The configuration count */
};

/* hid mouse endpoint information */
usb_device_endpoint_struct_t g_UsbDeviceHidMouseEndpoints[USB_HID_MOUSE_ENDPOINT_COUNT] = {
    /* HID mouse interrupt IN pipe */
    {
        USB_HID_MOUSE_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
        USB_ENDPOINT_INTERRUPT,
        USB_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE,
    },
};

/* HID mouse interface information */
usb_device_interface_struct_t g_UsbDeviceHidMouseInterface[] = {{
    0U, /* The alternate setting of the interface */
    {
        USB_HID_MOUSE_ENDPOINT_COUNT, /* Endpoint count */
        g_UsbDeviceHidMouseEndpoints, /* Endpoints handle */
    },
    NULL,
}};

usb_device_interfaces_struct_t g_UsbDeviceHidMouseInterfaces[USB_HID_MOUSE_INTERFACE_COUNT] = {
    {
        USB_HID_MOUSE_CLASS,           /* HID mouse class code */
        USB_HID_MOUSE_SUBCLASS,        /* HID mouse subclass code */
        USB_HID_MOUSE_PROTOCOL,        /* HID mouse protocol code */
        USB_HID_MOUSE_INTERFACE_INDEX, /* The interface number of the HID mouse */
        g_UsbDeviceHidMouseInterface,  /* Interfaces handle */
        sizeof(g_UsbDeviceHidMouseInterface) / sizeof(usb_device_interface_struct_t),
    },
};

usb_device_interface_list_t g_UsbDeviceHidMouseInterfaceList[USB_DEVICE_CONFIGURATION_COUNT] = {
    {
        USB_HID_MOUSE_INTERFACE_COUNT, /* The interface count of the HID mouse */
        g_UsbDeviceHidMouseInterfaces, /* The interfaces handle */
    },
};

usb_device_class_struct_t g_UsbDeviceHidMouseConfig = {
    g_UsbDeviceHidMouseInterfaceList, /* The interface list of the HID mouse */
    kUSB_DeviceClassTypeHid,          /* The HID class type */
    USB_DEVICE_CONFIGURATION_COUNT,   /* The configuration count */
};

#define MTOUCH_FIRST_SUB_DESC   \
    0x05, 0x0D,                         /*  2   USAGE_PAGE (Digitizers)*/    \
    0x09, 0x04,                         /*  4   USAGE (Touch Screen)*/   \
    0xA1, 0x01,                         /*  6   COLLECTION (Application)*/   \
    0x85, MULTI_TOUCH_DATA_REPORT_ID,	/*  8   REPORT_ID (Touch) //When the firmware wants to send a HID report (containing multi-touch contact information), the Report ID byte should match this number*/  \
    0x09, 0x22,                         /*  10  USAGE (Finger)*/   \
    0xA1, 0x02,                         /*  12  COLLECTION (Logical) */ \
    0x09, 0x42,                         /*  14  USAGE (Tip Switch) */   \
    0x15, 0x00,                         /*  16  LOGICAL_MINIMUM (0) */  \
    0x25, 0x01,                         /*  18  LOGICAL_MAXIMUM (1) */  \
    0x75, 0x01,                         /*  20  REPORT_SIZE (1) */  \
    0x95, 0x01,                         /*  22  REPORT_COUNT (1) */ \
    0x81, 0x02,                         /*  24  INPUT (Data,Var,Abs) //Makes one, 1-bit field for tip switch */ \
    0x09, 0x32,                         /*  26  USAGE (In Range) */ \
    0x81, 0x02,                         /*  28  INPUT (Data,Var,Abs) //Makes one, 1-bit field for In Range indicator */ \
    0x95, 0x06,                         /*  30  REPORT_COUNT (6) */ \
    0x81, 0x03,                         /*  32  INPUT (Cnst,Ary,Abs) //Makes six, 1-bit fields, which are pad bits (no valid data) */  \
    0x75, 0x08,                         /*  34  REPORT_SIZE (8) */ \
    0x09, 0x51,                         /*  36  USAGE (Contact Identifier) */    \
    0x95, 0x01,                         /*  38  REPORT_COUNT (1) */  \
    0x81, 0x02,                         /*  40  INPUT (Data,Var,Abs) //Makes one, 8-bit field for the contact identifier number.  Firmware arbitrarily assigns a contact ID, which stays the same until the contact becomes invalid (finger removed from screen). */ \
    0x05, 0x01,                         /*  42  USAGE_PAGE (Generic Desk.. */    \
    0x55, 0x0E,                         /*  44  UNIT_EXPONENT (-2) //10^(-2) */    \
    0x65, 0x11,                         /*  46  UNIT (System: SI Linear, Length: Centimeter) //But exponent -2, so Physical Maximum is in 10???s of mils. */   \
    0x75, 0x10,                         /*  48  REPORT_SIZE (16) */ \
    0x35, 0x00,                         /*  50  PHYSICAL_MINIMUM (0) */  \
	0x15, X_LOGICAL_MIN,                /*	52  LOGICAL_MINIMUM	*/	\
    0x26, USB_DEVICE_16bitTo8bitArrange(X_LOGICAL_MAX),  /*  55  LOGICAL_MAXIMUM (4800) //This is the maximum value the firmware should send for the X coordinate contact point.  It can potentially make the firmware design mathematically simpler if this is set to some meaningful value, such as equal to the PHYSICAL_MAXIMUM (which specifies the dimensions of the actual touch screen, in terms of the UNITS parameter). */    \
    0x46, USB_DEVICE_16bitTo8bitArrange(X_PHYSICAL_MAX), /*  58  PHYSICAL_MAXIMUM (0x640 = 1600) //1600 * 10^(-2) = 16 inches X-dimension */    \
    0x09, 0x30,                         /*  60  USAGE (X) */ \
    0x81, 0x02,                         /*  62  INPUT (Data,Var,Abs) //Makes one, 16-bit field for X coordinate info.  Valid values from: 0-4800 (4800 is the LOGICAL_MAXIMUM, which would correspond to far edge of the screen, which is 1600x10mil distance from X origin [left of screen]) */   \
	0x15, Y_LOGICAL_MIN,                /*  64	LOGICAL_MINIMUM (0) */	\
    0x26, USB_DEVICE_16bitTo8bitArrange(Y_LOGICAL_MAX),  /*  67  LOGICAL_MAXIMUM (3000) //16:10 aspect ratio (X:Y) (ex: 4800/3000 = 16/10).  //This is the maximum value the firmware should send for the Y coordinate contact point.  It can potentially make the firmware design mathematically simpler if this is set to some meaningful value, such as equal to the PHYSICAL_MAXIMUM (which specifies the dimensions of the actual touch screen, in terms of the UNIT parameter). */  \
    0x46, USB_DEVICE_16bitTo8bitArrange(Y_PHYSICAL_MAX), /*  70  PHYSICAL_MAXIMUM (0x3E8 = 1000 //1000 * 10^(-2) = 10 inches Y-dimension */ \
    0x09, 0x31,                         /*  72  USAGE (Y) */ \
    0x81, 0x02,                         /*  74  INPUT (Data,Var,Abs) //Makes one, 16-bit field for Y coordinate info.  Valid values from: 0-3000 (3000 is the LOGICAL_MAIXMUM, which would correspond to the far bottom of the screen, which is 1000x10mil distance from the Y origin [top of screen]) */    \
	0x35, 0x00,                         /*  76  Physical Minimum (0)	*/ \
	0x05, 0x0D,                         /*  78  Usage Page (Digitizer)	*/ \
	0x26, 0xFF, 0x00,                   /*  81  Logical Maximum (255)	*/ \
	0x46, 0xFF, 0x00,                   /*  84  Physical Maximum (255)	*/ \
	0x75, 0x08,                         /*  86   Report Size (8)	*/ \
	0x95, 0x02,                         /*  88   Report Count (2)	*/ \
	0x09, 0x48,                         /*  90   Usage (0x48)	*/ \
	0x09, 0x49,                         /*  92   Usage (0x49)	*/ \
	0x81, 0x02,                         /*  94   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)	*/ \
	0x05, 0x0D,                         /*  96   Usage Page (Digitizer)	*/ \
	0x09, 0x30,                         /*  98   Usage (Tip Pressure)	*/ \
	0x26, USB_DEVICE_16bitTo8bitArrange(PRESSURE_RESOLUTION),  /*  101   Logical Maximum (1024)	*/ \
	0x95, 0x01,                         /*  103  Report Count (1)	*/ \
	0x75, 0x10,                         /*  105  Report Size (16)	*/ \
	0x81, 0x02,                         /*  107   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)	*/ \
    0xC0                                /*  108  END_COLLECTION */

#define MTOUCH_SECOND_SUB_DESC   \
    0xA1, 0x02,                         /*  2   COLLECTION (Logical) */ \
    0x05, 0x0D,                         /*  4   USAGE_PAGE (Digitizers) */   \
    0x09, 0x42,                         /*  6   USAGE (Tip Switch) */    \
    0x15, 0x00,                         /*  8   LOGICAL_MINIMUM (0) */   \
    0x25, 0x01,                         /*  10  LOGICAL_MAXIMUM (1) */  \
    0x75, 0x01,                         /*  12  REPORT_SIZE (1) */  \
    0x95, 0x01,                         /*  14  REPORT_COUNT (1) */ \
    0x81, 0x02,                         /*  16  INPUT (Data,Var,Abs) //Makes one, 1-bit field for Tip Switch */  \
    0x09, 0x32,                         /*  18  USAGE (In Range) */ \
    0x81, 0x02,                         /*  20  INPUT (Data,Var,Abs) //Makes one, 1-bit field for In Range Indicator */  \
	0x95, 0x06,                         /*  22  REPORT_COUNT (6) */	\
	0x81, 0x03,                         /*  24  INPUT (Cnst,Ary,Abs) //Makes six, 1-bit fields, which are pad bits (no valid data) */	\
	0x75, 0x08,                         /*  26  REPORT_SIZE (8) */  \
    0x09, 0x51,                         /*  28  USAGE (Contact Identifier) */   \
    0x95, 0x01,                         /*  30  REPORT_COUNT (1) */ \
    0x81, 0x02,                         /*  32  INPUT (Data,Var,Abs) //Makes one, 8-bit field for the contact identifier number. */ \
    0x05, 0x01,                         /*  34  USAGE_PAGE (Generic Desk.. */   \
    0x55, 0x0E,                         /*  36  UNIT_EXPONENT (-2) //10^(-2) */ \
    0x65, 0x11,                         /*  38  UNIT (System: SI Linear, Length: Centimeter)  //But exponent -2, so Physical Maximum is in 10???s of mils. */   \
    0x75, 0x10,                         /*  40  REPORT_SIZE (16) */ \
    0x35, 0x00,                         /*  42  PHYSICAL_MINIMUM (0) */  \
	0x15, X_LOGICAL_MIN,				/*  44  LOGICAL_MINIMUM (0) */	\
    0x26, USB_DEVICE_16bitTo8bitArrange(X_LOGICAL_MAX),  /*  47  LOGICAL_MAXIMUM (4800) //This is the maximum value the firmware should send for the X coordinate contact point.  It can potentially make the firmware design mathematically simpler if this is set to some meaningful value, such as equal to the PHYSICAL_MAXIMUM (which specifies the dimensions of the actual touch screen, in terms of the UNITS parameter). */ \
    0x46, USB_DEVICE_16bitTo8bitArrange(X_PHYSICAL_MAX), /*  50  PHYSICAL_MAXIMUM (0x640 = 1600)     //1600 * 10^(-2) = 16 inches X-dimension */ \
    0x09, 0x30,                         /*  52  USAGE (X) */    \
    0x81, 0x02,                         /*  54  INPUT (Data,Var,Abs) //Makes one, 16-bit field for X coordinate info.  Valid values from: 0-4800 (4800 is the LOGICAL_MAXIMUM, which would correspond to far edge of the screen, which is 1600x10mil distance from X origin [left of screen]) */    \
	0x15, Y_LOGICAL_MIN,				/*  56  LOGICAL_MINIMUM (0) */	\
    0x26, USB_DEVICE_16bitTo8bitArrange(Y_LOGICAL_MAX),  /*  59  LOGICAL_MAXIMUM (3000) //16:10 aspect ratio (X:Y) (ex: 4800/3000 = 16/10).  //This is the maximum value the firmware should send for the Y coordinate contact point.  It can potentially make the firmware design mathematically simpler if this is set to some meaningful value, such as equal to the PHYSICAL_MAXIMUM (which specifies the dimensions of the actual touch screen, in terms of the UNIT parameter).*/  \
    0x46, USB_DEVICE_16bitTo8bitArrange(Y_PHYSICAL_MAX), /*  62  PHYSICAL_MAXIMUM (0x3E8 = 1000)    //1000 * 10^(-2) = 10 inches Y-dimension */  \
    0x09, 0x31,                         /*  64  USAGE (Y) */    \
    0x81, 0x02,                         /*  66  INPUT (Data,Var,Abs) //Makes one, 16-bit field for Y coordinate info.  Valid values from: 0-3000 (3000 is the LOGICAL_MAIXMUM, which would correspond to the far bottom of the screen, which is 1000x10mil distance from the Y origin [top of screen]) */    \
	0x35, 0x00,                         /*  68  Physical Minimum (0)	*/ \
	0x05, 0x0D,                         /*  70  Usage Page (Digitizer)	*/ \
	0x26, 0xFF, 0x00,                   /*  73  Logical Maximum (255)	*/ \
	0x46, 0xFF, 0x00,                   /*  76  Physical Maximum (255)	*/ \
	0x75, 0x08,                         /*  78  Report Size (8)	*/ \
	0x95, 0x02,                         /*  80  Report Count (2)	*/ \
	0x09, 0x48,                         /*  82  Usage (0x48)	*/ \
	0x09, 0x49,                         /*  84  Usage (0x49)	*/ \
	0x81, 0x02,                         /*  86  Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)	*/ \
	0x05, 0x0D,                         /*  88  Usage Page (Digitizer)	*/ \
	0x09, 0x30,                         /*  90  Usage (Tip Pressure)	*/ \
	0x26, USB_DEVICE_16bitTo8bitArrange(PRESSURE_RESOLUTION),  /*  93   Logical Maximum (1024)	*/ \
	0x95, 0x01,                         /*  95  Report Count (1)	*/ \
	0x75, 0x10,                         /*  97  Report Size (16)	*/ \
	0x81, 0x02,                         /*  99  Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)	*/ \
    0xC0                                /*  100 END_COLLECTION */

#define MTOUCH_VALIDCONTACT_SUB_DESC    \
    0x05, 0x0D,                         /*  2   USAGE_PAGE (Digitizers) */  \
    0x09, 0x54,                         /*  4   USAGE (Actual (contact) count) */   \
    0x95, 0x01,                         /*  6   REPORT_COUNT (1) */ \
    0x75, 0x08,                         /*  8   REPORT_SIZE (8) */  \
	0x15, 0x00,							/* 	10	LOGICAL_MINIMUM (0) */	\
    0x25, MAX_VALID_CONTACT_POINTS,     /*  12  LOGICAL_MAXIMUM (3)	//Maximum number of valid contact points simutaneously supported.  Increase this number if supporting more than 3 simultaneous contacts. */ \
    0x81, 0x02,                         /*  14  INPUT (Data,Var,Abs) //Makes one, 8-bit field for the actual number of valid contacts reported (valid values: 1 and 2) */   \
    0x85, VALID_CONTACTS_FEATURE_REPORT_ID,/*  16  REPORT_ID (Feature)   //When the firmware wants to send a feature report (containing the number of valid contacts that can be reported), the Report ID byte should match this number. */ \
    0x09, 0x55,                         /*  18  USAGE(Maximum Count) */ \
    0xB1, 0x02,                         /*  20  FEATURE (Data,Var,Abs) //Makes one, 8-bit field for reporting the maximum number of simultaneous contacts supported by the device.  Since this report descriptor contains two logical collections (one for each contact point), the firmware should always send 0x02 in response to a get feature report request. */    \
    0xC0                                /*  22  END_COLLECTION */

#define MTOUCH_MODEFEATURE_SUB_DESC     \
    0x09, 0x0E,                         /*  2   USAGE (Device Configuration) */ \
    0xa1, 0x01,                         /*  4   COLLECTION (Application) */ \
    0x85, DEVICE_MODE_FEATURE_REPORT_ID,/*  6   REPORT_ID (Configuration) */    \
    0x09, 0x23,                         /*  8   USAGE (Device Settings) */  \
    0xA1, 0x02,                         /*  10  COLLECTION (logical) */ \
    0x09, 0x52,                         /*  12  USAGE (Device Mode) */  \
    0x09, 0x53,                         /*  14  USAGE (Device Identifier) */    \
    0x15, 0x00,                         /*  16  LOGICAL_MINIMUM (0) */  \
    0x25, 0xFF,                         /*  18  LOGICAL_MAXIMUM (10) */ \
    0x75, 0x08,                         /*  20  REPORT_SIZE (8) */  \
    0x95, 0x02,                         /*  22  REPORT_COUNT (2) nsmoon@210202 A=>2*/\
    0xB1, 0x02,                         /*  24  FEATURE (Data,Var,Abs) */   \
    0xC0,                               /*  25  END_COLLECTION (logical) */ \
    0xC0                                /*  26  END_COLLECTION (application) */

#if 0 //for test
#define MOUSE_SUB_DESC  \
    0x05, 0x0D,                         /*  2   USAGE_PAGE (Digitizers) */  \
    0x09, 0x02,                         /*  4   Usage (Mouse) */    \
    0xA1, 0x01,                         /*  6   Collection (Application) */ \
    0x85, MOUSE_DATA_REPORT_ID,         /*  8   REPORT_ID (mouse) */    \
    0x09, 0x01,                         /*  10  Usage (Pointer) */  \
    0xA1, 0x00,                         /*  12  Collection (Physical) */    \
    0x05, 0x09,                         /*  14  Usage Page (Buttons) */ \
    0x19, 0x01,                         /*  16  Usage Minimum (01) */   \
    0x29, 0x03,                         /*  18  Usage Maximum (03) */   \
    0x15, 0x00,                         /*  20  Logical Minimum (0) */  \
    0x25, 0x01,                         /*  22  Logical Maximum (1) */  \
    0x45, 0x00,                         /*  24  Physical Maximum (0) */ \
    0x65, 0x00,                         /*  26  Unit (none) */  \
    0x95, 0x03,                         /*  28  Report Count (3) */ \
    0x75, 0x01,                         /*  30  Report Size (1) */  \
    0x81, 0x02,                         /*  32  Input (Data, Variable, Absolute) */ \
    0x95, 0x01,                         /*  34  Report Count (1) */ \
    0x75, 0x05,                         /*  36  Report Size (5) */  \
    0x81, 0x01,                         /*  38  Input (Constant)    ;5 bit padding */   \
    0x05, 0x01,                         /*  40  Usage Page (Generic Desktop) */ \
    0x09, 0x30,                         /*  42  Usage (X) */    \
    0x09, 0x31,                         /*  44  Usage (Y) */    \
	0x09, 0x36,                         /*         USAGE (Wheel)*/ \
	0x09, 0x38,                         /*         USAGE (Wheel)*/ \
    0x15, 0x81,                         /*  46  Logical Minimum (-127) */ \
    0x25, 0x7F,                         /*  47  Logical Maximum (127) */  \
    0x75, 0x08,                         /*  50  Report Size (8) */  \
    0x95, 0x04,                         /*  52  Report Count (4) */ \
    0x81, 0x06,                         /*  54  Input (Data, Variable, Relative) */ \
    0xC0,                               /*  55  End Collection */   \
    0xC0                                /*  56  End Collection */
#endif

#define STOUCH_SUB_DESC \
    0x05, 0x0D,                         /*  2   USAGE_PAGE (Digitizers) */  \
    0x09, 0x02,                         /*  4   USAGE (Pen) */  \
    0xA1, 0x01,                         /*  6   COLLECTION (Application) */  \
    0x85, SINGLE_TOUCH_DATA_REPORT_ID, 	/*  8   REPORT_ID (Pen digitizer/single touch)  //To send a HID input report containing single-touch digitizer data, use the SINGLE_TOUCH_DATA_REPORT_ID report ID */  \
    0x09, 0x20,                         /*  10  USAGE (Stylus) */  \
    0xA1, 0x00,                         /*  12  COLLECTION (Physical) */  \
    0x09, 0x42,                         /*  14  USAGE (Tip Switch)           //(byte[1] bit 0) */  \
    0x09, 0x32,                         /*  16  USAGE (In Range)             //(byte[1] bit 1) */  \
    0x15, 0x00,                         /*  18  LOGICAL_MINIMUM (0) */  \
    0x25, 0x01,                         /*  20  LOGICAL_MAXIMUM (1) */  \
    0x75, 0x01,                         /*  22  REPORT_SIZE (1)              //1-bit */  \
    0x95, 0x02,                         /*  24  REPORT_COUNT (2)             //two */  \
    0x81, 0x02,                         /*  26  INPUT (Data,Var,Abs)         //Makes two, 1-bit IN packet fields (byte[1] bits 0-4)) for (USAGE) tip sw, barrel sw, invert sw, in range sw.  Send '1' here when switch is active.  Send '0' when switch not active. */  \
    0x95, 0x0E,                         /*  28  REPORT_COUNT (14)            //fourteen */  \
    0x81, 0x03,                         /*  30  INPUT (Cnst,Var,Abs)         //Makes fourteen, 1-bit IN packet fields (byte[1] bits 5-7, and byte[2] all bits) with no usage.  These are pad bits that don't contain useful data. */  \
    0x05, 0x01,                         /*  32  USAGE_PAGE (Generic Desktop) */  \
    0x26, USB_DEVICE_16bitTo8bitArrange(X_LOGICAL_MAX),/*  35  LOGICAL_MAXIMUM (4800) */  \
    0x75, 0x10,                         /*  37  REPORT_SIZE (16) */  \
    0x95, 0x01,                         /*  39  REPORT_COUNT (1)   			//one */  \
    0x55, 0x0E,                         /*  41  UNIT_EXPONENT (-2)       //10^(-2) */  \
    0x65, 0x33,                         /*  43  UNIT (Inches, English Linear)  //But exponent -2, so Physical Maximum is in 10???s of mils. */  \
    0x09, 0x30,                         /*  45  USAGE (X) */  \
    0x35, 0x00,                         /*  47  PHYSICAL_MINIMUM (0) */  \
    0x46, USB_DEVICE_16bitTo8bitArrange(X_PHYSICAL_MAX), /*  50  PHYSICAL_MAXIMUM (0x640 = 1600)     //1600 * 10^(-2) = 16 inches X-dimension */  \
    0x81, 0x02,                         /*  52  INPUT (Data,Var,Abs)           //Makes one, 16-bit field for X coordinate info.  Valid values from: 0-4800 */  \
    0x26, USB_DEVICE_16bitTo8bitArrange(Y_LOGICAL_MAX), /*  55  LOGICAL_MAXIMUM (3000)             //16:10 aspect ratio (X:Y) */  \
    0x46, USB_DEVICE_16bitTo8bitArrange(Y_PHYSICAL_MAX), /*  58  PHYSICAL_MAXIMUM (0x3E8 = 1000)    //1000 * 10^(-2) = 10 inches Y-dimension */  \
    0x09, 0x31,                         /*  60  USAGE (Y) */  \
    0x81, 0x02,                         /*  62  INPUT (Data,Var,Abs)		    //Makes one, 16-bit field for Y coordinate info.  Valid values from: 0-3000 */  \
    0xC0,                               /*  63  END_COLLECTION (Physical) */  \
    0xC0                                /*  64  END_COLLECTION (Application) */

#define DIAGNOSTIC_SUB_DESC \
	0x06, 0x00, 0xFF,                   /*  3	Usage Page = 0xFFFF (Vendor Defined) */  \
	0x09, 0x01, 						/*  5	Usage */  \
	0xA1, 0x01, 						/*  7	Collection (Application, probably not important because vendor defined usage) */  \
	0x85, DIAGNOSTIC_REPORT_ID,         /*  9	REPORT_ID */  \
	0x19, 0x01, 						/*  11	Usage Minimum (Vendor Usage = 0) (minimum bytes the device should send is 0) */  \
	0x29, 0x40, 						/*  13	Usage Maximum (Vendor Usage = 64) (maximum bytes the device should send is 64) */  \
	0x15, 0x00, 						/*  15	Logical Minimum (Vendor Usage = 0) */  \
	0x26, 0xFF, 0x00,                   /*  18	Logical Maximum (Vendor Usage = 255) */  \
	0x75, 0x08, 						/*  20	Report Size 8 bits (one full byte) for each report. */  \
	0x95, 0x3f, 						/*  22	Report Count 64 bytes in a full report.  */  \
	0x81, 0x02, 						/*  24	Input (Data, Var, Abs) */  \
	0x19, 0x01, 						/*  26	Usage Minimum (Vendor Usage = 0) */  \
	0x29, 0x40,                         /*  28	Usage Maximum (Vendor Usage = 64) */  \
	0x91, 0x02, 						/*  30	Output (Data, Var, Ads) */  \
	0xC0                                /*  31	End Collection */

#define DIAGNOSTIC_FIRST_SUB_DESC \
    0x06, 0x00, 0xFF,  /* Usage Page = 0xFF00 (Vendor Defined Page 1) */ \
    0x09, 0x01,        /* Usage (Vendor Usage 1)  */ \
    0xA1, 0x01,        /* Collection (Application)  */ \
    0x19, 0x01,        /* Usage Minimum   */ \
    0x29, 0x40,        /* Usage Maximum   //64 input usages total (0x01 to 0x40)  */ \
    0x15, 0x00,        /* Logical Minimum (data bytes in the report may have minimum value = 0x00)  */ \
    0x25, 0x40,        /* Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)  */ \
    0x75, 0x08,        /* Report Size: 8-bit field size  */ \
    0x95, USB_DEVICE_EP0_BUFFER_SIZE, /* Report Count: Make sixty-four 8-bit fields (the next time the parser hits an "Input", "Output", or "Feature" item)  */ \
    0x81, 0x00,        /* Input (Data, Array, Abs): Instantiates input packet fields based on the above report size, count, logical min/max, and usage.  */ \
    0x19, 0x01,        /* Usage Minimum   */ \
    0x29, 0x40,        /* Usage Maximum 	//64 output usages total (0x01 to 0x40)  */ \
    0x91, 0x00,        /* Output (Data, Array, Abs): Instantiates output packet fields.    */ \
    0xC0               /* End Collection  */

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceHidMouseReportDescriptor[] = {
#if 1 //nsmoon@210609
	0x05U, 0x01U, /* Usage Page (Generic Desktop)*/
	0x09U, 0x02U, /* Usage (Mouse) */
	0xA1U, 0x01U, /* Collection (Application) */
	0x09U, 0x01U, /* Usage (Pointer) */

	0xA1U, 0x00U, /* Collection (Physical) */
	0x05U, 0x09U, /* Usage Page (Buttons) */
	0x19U, 0x01U, /* Usage Minimum (01U) */
	0x29U, 0x03U, /* Usage Maximum (03U) */

	0x15U, 0x00U, /* Logical Minimum (0U) */
	0x25U, 0x01U, /* Logical Maximum (1U) */
	0x95U, 0x03U, /* Report Count (3U) */
	0x75U, 0x01U, /* Report Size (1U) */

	0x81U, 0x02U, /* Input(Data, Variable, Absolute) 3U button bit fields */
	0x95U, 0x01U, /* Report Count (1U) */
	0x75U, 0x05U, /* Report Size (5U) */
	0x81U, 0x01U, /* Input (Constant), 5U constant field */

	0x05U, 0x01U, /* Usage Page (Generic Desktop) */
	0x09U, 0x30U, /* Usage (X) */
	0x09U, 0x31U, /* Usage (Y) */
	0x09U, 0x36U, /* Usage (Wheel)*/
	0x09U, 0x38U, /* Usage (Wheel)*/

	0x15U, 0x81U, /* Logical Minimum (-127) */
	0x25U, 0x7FU, /* Logical Maximum (127) */
	0x75U, 0x08U, /* Report Size (8U) */
	0x95U, 0x04U, /* Report Count (4U) */

	0x81U, 0x06U, /* Input(Data, Variable, Relative), Three position bytes (X & Y & Z)*/
	0xC0U,        /* End collection, Close Pointer collection*/
	0xC0U         /* End collection, Close Mouse collection */
#endif
#if 0
    0x05U, 0x01U, /* Usage Page (Generic Desktop)*/
    0x09U, 0x02U, /* Usage (Mouse) */
    0xA1U, 0x01U, /* Collection (Application) */
    0x09U, 0x01U, /* Usage (Pointer) */

    0xA1U, 0x00U, /* Collection (Physical) */
    0x05U, 0x09U, /* Usage Page (Buttons) */
    0x19U, 0x01U, /* Usage Minimum (01U) */
    0x29U, 0x03U, /* Usage Maximum (03U) */

    0x15U, 0x00U, /* Logical Minimum (0U) */
    0x25U, 0x01U, /* Logical Maximum (1U) */
    0x95U, 0x03U, /* Report Count (3U) */
    0x75U, 0x01U, /* Report Size (1U) */

    0x81U, 0x02U, /* Input(Data, Variable, Absolute) 3U button bit fields */
    0x95U, 0x01U, /* Report Count (1U) */
    0x75U, 0x05U, /* Report Size (5U) */
    0x81U, 0x01U, /* Input (Constant), 5U constant field */

    0x05U, 0x01U, /* Usage Page (Generic Desktop) */
    0x09U, 0x30U, /* Usage (X) */
    0x09U, 0x31U, /* Usage (Y) */
    0x09U, 0x38U, /* Usage (Z) */

    0x15U, 0x81U, /* Logical Minimum (-127) */
    0x25U, 0x7FU, /* Logical Maximum (127) */
    0x75U, 0x08U, /* Report Size (8U) */
    0x95U, 0x03U, /* Report Count (3U) */

    0x81U, 0x06U, /* Input(Data, Variable, Relative), Three position bytes (X & Y & Z)*/
    0xC0U,        /* End collection, Close Pointer collection*/
    0xC0U         /* End collection, Close Mouse collection */
#endif
};

/* HID generic report descriptor */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceHidGenericReportDescriptor[] = {
	MTOUCH_FIRST_SUB_DESC,
	MTOUCH_SECOND_SUB_DESC,
	MTOUCH_SECOND_SUB_DESC,
	MTOUCH_SECOND_SUB_DESC,
	MTOUCH_SECOND_SUB_DESC,
	MTOUCH_VALIDCONTACT_SUB_DESC,
	MTOUCH_MODEFEATURE_SUB_DESC,
	//MOUSE_SUB_DESC, //nsmoon@210609 not-used
	STOUCH_SUB_DESC,
	DIAGNOSTIC_SUB_DESC,
};
#endif

#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
/* cdc virtual com information */
/* Define endpoint for communication class */
usb_device_endpoint_struct_t g_cdcVcomCicEndpoints[USB_CDC_VCOM_CIC_ENDPOINT_COUNT] = {
    {
        USB_CDC_VCOM_CIC_INTERRUPT_IN_ENDPOINT | (USB_IN << 7U),
        USB_ENDPOINT_INTERRUPT,
        FS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE,
        FS_CDC_VCOM_INTERRUPT_IN_INTERVAL,
    },
};

/* Define endpoint for data class */
usb_device_endpoint_struct_t g_cdcVcomDicEndpoints[USB_CDC_VCOM_DIC_ENDPOINT_COUNT] = {
    {
        USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT | (USB_IN << 7U),
        USB_ENDPOINT_BULK,
        FS_CDC_VCOM_BULK_IN_PACKET_SIZE,
        0U,
    },
    {
        USB_CDC_VCOM_DIC_BULK_OUT_ENDPOINT | (USB_OUT << 7U),
        USB_ENDPOINT_BULK,
        FS_CDC_VCOM_BULK_OUT_PACKET_SIZE,
        0U,
    },
};

/* Define interface for communication class */
usb_device_interface_struct_t g_cdcVcomCicInterface[] = {{USB_CDC_VCOM_CIC_INTERFACE_ALTERNATE_0,
                                                          {
                                                              USB_CDC_VCOM_CIC_ENDPOINT_COUNT,
                                                              g_cdcVcomCicEndpoints,
                                                          },
                                                          NULL}};

/* Define interface for data class */
usb_device_interface_struct_t g_cdcVcomDicInterface[] = {{USB_CDC_VCOM_DIC_INTERFACE_ALTERNATE_0,
                                                          {
                                                              USB_CDC_VCOM_DIC_ENDPOINT_COUNT,
                                                              g_cdcVcomDicEndpoints,
                                                          },
                                                          NULL}};

/* Define interfaces for virtual com */
usb_device_interfaces_struct_t g_cdcVcomInterfaces[USB_CDC_VCOM_INTERFACE_COUNT] = {
    {USB_CDC_VCOM_CIC_CLASS, USB_CDC_VCOM_CIC_SUBCLASS, USB_CDC_VCOM_CIC_PROTOCOL, USB_CDC_VCOM_CIC_INTERFACE_INDEX,
     g_cdcVcomCicInterface, sizeof(g_cdcVcomCicInterface) / sizeof(usb_device_interface_struct_t)},
    {USB_CDC_VCOM_DIC_CLASS, USB_CDC_VCOM_DIC_SUBCLASS, USB_CDC_VCOM_DIC_PROTOCOL, USB_CDC_VCOM_DIC_INTERFACE_INDEX,
     g_cdcVcomDicInterface, sizeof(g_cdcVcomDicInterface) / sizeof(usb_device_interface_struct_t)},
};

/* Define configurations for virtual com */
usb_device_interface_list_t g_UsbDeviceCdcVcomInterfaceList[USB_DEVICE_CONFIGURATION_COUNT] = {
    {
        USB_CDC_VCOM_INTERFACE_COUNT,
        g_cdcVcomInterfaces,
    },
};

/* Define class information for virtual com */
usb_device_class_struct_t g_UsbDeviceCdcVcomConfig = {
    g_UsbDeviceCdcVcomInterfaceList,
    kUSB_DeviceClassTypeCdc,
    USB_DEVICE_CONFIGURATION_COUNT,
};
#endif

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceDescriptor[] = {
    USB_DESCRIPTOR_LENGTH_DEVICE, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_DEVICE,   /* DEVICE Descriptor Type */
    USB_SHORT_GET_LOW(USB_DEVICE_SPECIFIC_BCD_VERSION),
    USB_SHORT_GET_HIGH(USB_DEVICE_SPECIFIC_BCD_VERSION), /* USB Specification Release Number in
                                                            Binary-Coded Decimal (i.e., 2.10 is 210H). */
    USB_DEVICE_CLASS,                                    /* Class code (assigned by the USB-IF). */
    USB_DEVICE_SUBCLASS,                                 /* Subclass code (assigned by the USB-IF). */
    USB_DEVICE_PROTOCOL,                                 /* Protocol code (assigned by the USB-IF). */
    USB_CONTROL_MAX_PACKET_SIZE,                         /* Maximum packet size for endpoint zero
                                                            (only 8, 16, 32, or 64 are valid) */
    USB_SHORT_GET_LOW(USB_DEVICE_VID),
    USB_SHORT_GET_HIGH(USB_DEVICE_VID), /* Vendor ID (assigned by the USB-IF) */
    USB_SHORT_GET_LOW(USB_DEVICE_PID),
    USB_SHORT_GET_HIGH(USB_DEVICE_PID), /* Product ID (assigned by the manufacturer) */
    USB_SHORT_GET_LOW(USB_DEVICE_DEMO_BCD_VERSION),
    USB_SHORT_GET_HIGH(USB_DEVICE_DEMO_BCD_VERSION), /* Device release number in binary-coded decimal */
    0x01U,                                           /* Index of string descriptor describing manufacturer */
    0x02U,                                           /* Index of string descriptor describing product */
    0x06U,                                           /* Index of string descriptor describing the
                                                        device's serial number */ /*nsmoon@230321 0x00=>0x06*/
    USB_DEVICE_CONFIGURATION_COUNT,                  /* Number of possible configurations */
};

#if (USB_DEVICE_CONFIG_CDC_ACM > 0)
#define CDC_USB_DESCRIPTOR_LENGTH ( \
	 USB_IAD_DESC_SIZE + \
	 USB_DESCRIPTOR_LENGTH_INTERFACE + \
	 USB_DESCRIPTOR_LENGTH_CDC_HEADER_FUNC + \
	 USB_DESCRIPTOR_LENGTH_CDC_CALL_MANAG + \
	 USB_DESCRIPTOR_LENGTH_CDC_ABSTRACT + \
	 USB_DESCRIPTOR_LENGTH_CDC_UNION_FUNC + \
	 USB_DESCRIPTOR_LENGTH_ENDPOINT + \
	 USB_DESCRIPTOR_LENGTH_INTERFACE + \
	 USB_DESCRIPTOR_LENGTH_ENDPOINT + \
	 USB_DESCRIPTOR_LENGTH_ENDPOINT)
#else
#define CDC_USB_DESCRIPTOR_LENGTH (0)
#endif

#if (USB_DEVICE_CONFIG_HID > 0)
#define HID_USB_DESCRIPTOR_LENGTH ( \
		USB_IAD_DESC_SIZE + \
	    USB_DESCRIPTOR_LENGTH_INTERFACE + \
	    USB_DESCRIPTOR_LENGTH_HID + \
	    USB_DESCRIPTOR_LENGTH_ENDPOINT + \
	    USB_DESCRIPTOR_LENGTH_ENDPOINT + \
		USB_IAD_DESC_SIZE + \
		USB_DESCRIPTOR_LENGTH_INTERFACE + \
		USB_DESCRIPTOR_LENGTH_HID + \
	    USB_DESCRIPTOR_LENGTH_ENDPOINT)
#else
#define HID_USB_DESCRIPTOR_LENGTH (0)
#endif

#define USB_DESCRIPTOR_LENGTH_TOTAL (USB_DESCRIPTOR_LENGTH_CONFIGURE + \
		CDC_USB_DESCRIPTOR_LENGTH + HID_USB_DESCRIPTOR_LENGTH)

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceConfigurationDescriptor[] = {
    USB_DESCRIPTOR_LENGTH_CONFIGURE, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_CONFIGURE,   /* CONFIGURATION Descriptor Type */
    /* Total length of data returned for this configuration. */
    USB_SHORT_GET_LOW(USB_DESCRIPTOR_LENGTH_TOTAL),
    USB_SHORT_GET_HIGH(USB_DESCRIPTOR_LENGTH_TOTAL),
    USB_COMPOSITE_INTERFACE_COUNT, /* Number of interfaces supported by this configuration */
    USB_COMPOSITE_CONFIGURE_INDEX, /* Value to use as an argument to the SetConfiguration() request to select this configuration */
    0x00U, /* Index of string descriptor describing this configuration */
    (USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_D7_MASK) |
        (USB_DEVICE_CONFIG_SELF_POWER << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_SELF_POWERED_SHIFT) |
        (USB_DEVICE_CONFIG_REMOTE_WAKEUP << USB_DESCRIPTOR_CONFIGURE_ATTRIBUTE_REMOTE_WAKEUP_SHIFT),
    /* Configuration characteristics
         D7: Reserved (set to one)
         D6: Self-powered
         D5: Remote Wakeup
         D4...0: Reserved (reset to zero)
    */
    USB_DEVICE_MAX_POWER,            /* Maximum power consumption of the USB
                                      * device from the bus in this specific
                                      * configuration when the device is fully
                                      * operational. Expressed in 2 mA units
                                      *  (i.e., 50 = 100 mA).
                                      */
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
    /* Interface Association Descriptor */
    /* Size of this descriptor in bytes */
    USB_IAD_DESC_SIZE,
    /* INTERFACE_ASSOCIATION Descriptor Type  */
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,
    /* The first interface number associated with this function */
	USB_HID_GENERIC_INTERFACE_INDEX,
    /* The number of contiguous interfaces associated with this function */
    0x01,
    /* The function belongs to the Communication Device/Interface Class  */
	USB_HID_GENERIC_CLASS, USB_HID_GENERIC_SUBCLASS, /*0x03,*/ /*bFunctionClass: 2:CDC*/
    /* The function uses the No class specific protocol required Protocol  */
	USB_HID_GENERIC_PROTOCOL, /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    /* The Function string descriptor index */
    0x03,
#endif
	/* digitizer */
    USB_DESCRIPTOR_LENGTH_INTERFACE, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_INTERFACE,   /* INTERFACE Descriptor Type */
    USB_HID_GENERIC_INTERFACE_INDEX, /* Number of this interface. */
    0x00U,                           /* Value used to select this alternate setting
                                        for the interface identified in the prior field */
    USB_HID_GENERIC_ENDPOINT_COUNT,  /* Number of endpoints used by this
                                        interface (excluding endpoint zero). */
    USB_HID_GENERIC_CLASS,           /* Class code (assigned by the USB-IF). */
    USB_HID_GENERIC_SUBCLASS,        /* Subclass code (assigned by the USB-IF). */
    USB_HID_GENERIC_PROTOCOL,        /* Protocol code (assigned by the USB). */
    0x04U,                            /* Index of string descriptor describing this interface */

    USB_DESCRIPTOR_LENGTH_HID,      /* Numeric expression that is the total size of the
                                       HID descriptor. */
    USB_DESCRIPTOR_TYPE_HID,        /* Constant name specifying type of HID
                                       descriptor. */
    0x00U, 0x01U,                   /* Numeric expression identifying the HID Class
                                       Specification release. */
    0x00U,                          /* Numeric expression identifying country code of
                                       the localized hardware */
    0x01U,                          /* Numeric expression specifying the number of
                                       class descriptors(at least one report descriptor) */
    USB_DESCRIPTOR_TYPE_HID_REPORT, /* Constant name identifying type of class descriptor. */
    USB_SHORT_GET_LOW(USB_DESCRIPTOR_LENGTH_HID_GENERIC_REPORT),
    USB_SHORT_GET_HIGH(USB_DESCRIPTOR_LENGTH_HID_GENERIC_REPORT),
    /* Numeric expression that is the total size of the
       Report descriptor. */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_ENDPOINT,   /* ENDPOINT Descriptor Type */
    USB_HID_GENERIC_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
    /* The address of the endpoint on the USB device
       described by this descriptor. */
    USB_ENDPOINT_INTERRUPT, /* This field describes the endpoint's attributes */
    USB_SHORT_GET_LOW(USB_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE),
    USB_SHORT_GET_HIGH(USB_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE),
    /* Maximum packet size this endpoint is capable of
       sending or receiving when this configuration is
       selected. */
    FS_HID_GENERIC_INTERRUPT_IN_INTERVAL, /* Interval for polling endpoint for data transfers. */
    /* Numeric expression that is the total size of the
       Report descriptor. */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_ENDPOINT,   /* ENDPOINT Descriptor Type */
    USB_HID_GENERIC_ENDPOINT_OUT | (USB_OUT << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
    /* The address of the endpoint on the USB device
       described by this descriptor. */
    USB_ENDPOINT_INTERRUPT, /* This field describes the endpoint's attributes */
    USB_SHORT_GET_LOW(USB_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE),
    USB_SHORT_GET_HIGH(USB_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE),
    /* Maximum packet size this endpoint is capable of
       sending or receiving when this configuration is
       selected. */
    FS_HID_GENERIC_INTERRUPT_OUT_INTERVAL, /* Interval for polling endpoint for data transfers. */

#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
    /* Interface Association Descriptor */
    /* Size of this descriptor in bytes */
    USB_IAD_DESC_SIZE,
    /* INTERFACE_ASSOCIATION Descriptor Type  */
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,
    /* The first interface number associated with this function */
	USB_HID_MOUSE_INTERFACE_INDEX,
    /* The number of contiguous interfaces associated with this function */
    0x01,
    /* The function belongs to the Communication Device/Interface Class  */
	USB_HID_MOUSE_CLASS, USB_HID_MOUSE_SUBCLASS, /*0x03,*/ /*bFunctionClass: 2:CDC*/
    /* The function uses the No class specific protocol required Protocol  */
	USB_HID_MOUSE_PROTOCOL, /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    /* The Function string descriptor index */
    0x04,
#endif
	/* mouse */
    USB_DESCRIPTOR_LENGTH_INTERFACE, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_INTERFACE,   /* INTERFACE Descriptor Type */
    USB_HID_MOUSE_INTERFACE_INDEX,   /* Number of this interface. */
    0x00U,                           /* Value used to select this alternate setting
                                        for the interface identified in the prior field */
    USB_HID_MOUSE_ENDPOINT_COUNT,    /* Number of endpoints used by this
                                          interface (excluding endpoint zero). */
    USB_HID_MOUSE_CLASS,             /* Class code (assigned by the USB-IF). */
    USB_HID_MOUSE_SUBCLASS,          /* Subclass code (assigned by the USB-IF). */
    USB_HID_MOUSE_PROTOCOL,          /* Protocol code (assigned by the USB). */
    0x03U,                           /* Index of string descriptor describing this interface */

    USB_DESCRIPTOR_LENGTH_HID,      /* Numeric expression that is the total size of the
                                       HID descriptor. */
    USB_DESCRIPTOR_TYPE_HID,        /* Constant name specifying type of HID
                                       descriptor. */
    0x00U, 0x01U,                   /* Numeric expression identifying the HID Class
                                       Specification release. */
    0x00U,                          /* Numeric expression identifying country code of
                                       the localized hardware */
    0x01U,                          /* Numeric expression specifying the number of
                                       class descriptors(at least one report descriptor) */
    USB_DESCRIPTOR_TYPE_HID_REPORT, /* Constant name identifying type of class descriptor. */
    USB_SHORT_GET_LOW(USB_DESCRIPTOR_LENGTH_HID_MOUSE_REPORT),
    USB_SHORT_GET_HIGH(USB_DESCRIPTOR_LENGTH_HID_MOUSE_REPORT),

    /* Numeric expression that is the total size of the
       Report descriptor. */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_ENDPOINT,   /* ENDPOINT Descriptor Type */
    USB_HID_MOUSE_ENDPOINT_IN | (USB_IN << USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT),
    /* The address of the endpoint on the USB device
       described by this descriptor. */
    USB_ENDPOINT_INTERRUPT, /* This field describes the endpoint's attributes */
    USB_SHORT_GET_LOW(USB_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE),
	USB_SHORT_GET_HIGH(USB_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE),
    /* Maximum packet size this endpoint is capable of
       sending or receiving when this configuration is
       selected. */
    FS_HID_MOUSE_INTERRUPT_IN_INTERVAL, /* Interval for polling endpoint for data transfers. */
#endif //1
#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
    /* Interface Association Descriptor */
    /* Size of this descriptor in bytes */
    USB_IAD_DESC_SIZE,
    /* INTERFACE_ASSOCIATION Descriptor Type  */
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,
    /* The first interface number associated with this function */
	USB_CDC_VCOM_CIC_INTERFACE_INDEX, /*starts from zero .. hence 0-1-*/
    /* The number of contiguous interfaces associated with this function */
    0x02,
    /* The function belongs to the Communication Device/Interface Class  */
    USB_CDC_VCOM_CIC_CLASS, USB_CDC_VCOM_CIC_SUBCLASS, /*0x03,*/ /*bFunctionClass: 2:CDC*/
    /* The function uses the No class specific protocol required Protocol  */
	USB_CDC_VCOM_CIC_PROTOCOL, /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    /* The Function string descriptor index */
    0x05,

	/* Interface Descriptor */
    USB_DESCRIPTOR_LENGTH_INTERFACE, USB_DESCRIPTOR_TYPE_INTERFACE, USB_CDC_VCOM_CIC_INTERFACE_INDEX, USB_CDC_VCOM_CIC_INTERFACE_ALTERNATE_0,
    USB_CDC_VCOM_CIC_ENDPOINT_COUNT, USB_CDC_VCOM_CIC_CLASS, USB_CDC_VCOM_CIC_SUBCLASS, USB_CDC_VCOM_CIC_PROTOCOL, 0x00,

    /* CDC Class-Specific descriptor */
    USB_DESCRIPTOR_LENGTH_CDC_HEADER_FUNC, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_CDC_CS_INTERFACE,  /* CS_INTERFACE Descriptor Type */
    USB_CDC_HEADER_FUNC_DESC, 0x10,
    0x01, /* USB Class Definitions for Communications the Communication specification version 1.10 */

    USB_DESCRIPTOR_LENGTH_CDC_CALL_MANAG, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_CDC_CS_INTERFACE, /* CS_INTERFACE Descriptor Type */
    USB_CDC_CALL_MANAGEMENT_FUNC_DESC,
    0x01, /*Bit 0: Whether device handle call management itself 1, Bit 1: Whether device can send/receive call
             management information over a Data Class Interface 0 */
    0x01, /* Indicates multiplexed commands are handled via data interface */

    USB_DESCRIPTOR_LENGTH_CDC_ABSTRACT,   /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_CDC_CS_INTERFACE, /* CS_INTERFACE Descriptor Type */
    USB_CDC_ABSTRACT_CONTROL_FUNC_DESC,
    0x06, /* Bit 0: Whether device supports the request combination of Set_Comm_Feature, Clear_Comm_Feature, and
             Get_Comm_Feature 0, Bit 1: Whether device supports the request combination of Set_Line_Coding,
             Set_Control_Line_State, Get_Line_Coding, and the notification Serial_State 1, Bit ...  */

    USB_DESCRIPTOR_LENGTH_CDC_UNION_FUNC, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_CDC_CS_INTERFACE, /* CS_INTERFACE Descriptor Type */
    USB_CDC_UNION_FUNC_DESC, 0x00,        /* The interface number of the Communications or Data Class interface  */
    0x01,                                 /* Interface number of subordinate interface in the Union  */

    /*Notification Endpoint descriptor */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, USB_DESCRIPTOR_TYPE_ENDPOINT,
    USB_CDC_VCOM_CIC_INTERRUPT_IN_ENDPOINT | (USB_IN << 7U), USB_ENDPOINT_INTERRUPT,
    USB_SHORT_GET_LOW(FS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE), USB_SHORT_GET_HIGH(FS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE),
    FS_CDC_VCOM_INTERRUPT_IN_INTERVAL,

    /* Data Interface Descriptor */
    USB_DESCRIPTOR_LENGTH_INTERFACE, USB_DESCRIPTOR_TYPE_INTERFACE, USB_CDC_VCOM_DIC_INTERFACE_INDEX, USB_CDC_VCOM_DIC_INTERFACE_ALTERNATE_0,
    USB_CDC_VCOM_DIC_ENDPOINT_COUNT, USB_CDC_VCOM_DIC_CLASS, USB_CDC_VCOM_DIC_SUBCLASS, USB_CDC_VCOM_DIC_PROTOCOL,
    0x00, /* Interface Description String Index*/

    /*Bulk IN Endpoint descriptor */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, USB_DESCRIPTOR_TYPE_ENDPOINT, USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT | (USB_IN << 7U),
    USB_ENDPOINT_BULK, USB_SHORT_GET_LOW(FS_CDC_VCOM_BULK_IN_PACKET_SIZE),
    USB_SHORT_GET_HIGH(FS_CDC_VCOM_BULK_IN_PACKET_SIZE), 0x00, /* The polling interval value is every 0 Frames */

    /*Bulk OUT Endpoint descriptor */
    USB_DESCRIPTOR_LENGTH_ENDPOINT, USB_DESCRIPTOR_TYPE_ENDPOINT, USB_CDC_VCOM_DIC_BULK_OUT_ENDPOINT | (USB_OUT << 7U),
    USB_ENDPOINT_BULK, USB_SHORT_GET_LOW(FS_CDC_VCOM_BULK_OUT_PACKET_SIZE),
    USB_SHORT_GET_HIGH(FS_CDC_VCOM_BULK_OUT_PACKET_SIZE), 0x00, /* The polling interval value is every 0 Frames */
#endif
};

#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceQualifierDescriptor[] = {
    USB_DESCRIPTOR_LENGTH_DEVICE_QUALITIER, /* Size of this descriptor in bytes */
    USB_DESCRIPTOR_TYPE_DEVICE_QUALITIER,   /* DEVICE Descriptor Type */
    USB_SHORT_GET_LOW(USB_DEVICE_SPECIFIC_BCD_VERSION),
    USB_SHORT_GET_HIGH(USB_DEVICE_SPECIFIC_BCD_VERSION), /* USB Specification Release Number in
                                                            Binary-Coded Decimal (i.e., 2.10 is 210H). */
    USB_DEVICE_CLASS,                                    /* Class code (assigned by the USB-IF). */
    USB_DEVICE_SUBCLASS,                                 /* Subclass code (assigned by the USB-IF). */
    USB_DEVICE_PROTOCOL,                                 /* Protocol code (assigned by the USB-IF). */
    USB_CONTROL_MAX_PACKET_SIZE,                         /* Maximum packet size for endpoint zero
                                                            (only 8, 16, 32, or 64 are valid) */
    0x00U,                                               /* Number of Other-speed Configurations */
    0x00U,                                               /* Reserved for future use, must be zero */
};
#endif

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString0[] = {
    2U + 2U,
    USB_DESCRIPTOR_TYPE_STRING,
    0x09U,
    0x04U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString1[] = {
    2U + 2U * 18U, USB_DESCRIPTOR_TYPE_STRING,
    'N',           0x00U,
    'X',           0x00U,
    'P',           0x00U,
    ' ',           0x00U,
    'S',           0x00U,
    'E',           0x00U,
    'M',           0x00U,
    'I',           0x00U,
    'C',           0x00U,
    'O',           0x00U,
    'N',           0x00U,
    'D',           0x00U,
    'U',           0x00U,
    'C',           0x00U,
    'T',           0x00U,
    'O',           0x00U,
    'R',           0x00U,
    'S',           0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString2[] = {
    2U + 2U * 16U, USB_DESCRIPTOR_TYPE_STRING,
    'C',           0x00U,
    'O',           0x00U,
    'M',           0x00U,
    'P',           0x00U,
    'O',           0x00U,
    'S',           0x00U,
    'I',           0x00U,
    'T',           0x00U,
    'E',           0x00U,
    ' ',           0x00U,
    'D',           0x00U,
    'E',           0x00U,
    'V',           0x00U,
    'I',           0x00U,
    'C',           0x00U,
    'E',           0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString3[] = {
    2U + 2U * 20U, USB_DESCRIPTOR_TYPE_STRING,
    'H',           0x00U,
    'I',           0x00U,
    'D',           0x00U,
    ' ',           0x00U,
    'D',           0x00U,
    'I',           0x00U,
    'G',           0x00U,
    'I',           0x00U,
    'T',           0x00U,
    'I',           0x00U,
    'Z',           0x00U,
    'E',           0x00U,
    'R',           0x00U,
    ' ',           0x00U,
    'D',           0x00U,
    'E',           0x00U,
    'V',           0x00U,
    'I',           0x00U,
    'C',           0x00U,
    'E',           0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString4[] = {
    2U + 2U * 16U, USB_DESCRIPTOR_TYPE_STRING,
    'H',           0x00U,
    'I',           0x00U,
    'D',           0x00U,
    ' ',           0x00U,
    'M',           0x00U,
    'O',           0x00U,
    'U',           0x00U,
    'S',           0x00U,
    'E',           0x00U,
    ' ',           0x00U,
    'D',           0x00U,
    'E',           0x00U,
    'V',           0x00U,
    'I',           0x00U,
    'C',           0x00U,
    'E',           0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString5[] = {
    2U + 2U * 15U, USB_DESCRIPTOR_TYPE_STRING,
    'C',           0x00U,
    'D',           0x00U,
    'C',           0x00U,
    ' ',           0x00U,
    'V',           0x00U,
    'I',           0x00U,
    'R',           0x00U,
    'T',           0x00U,
    'U',           0x00U,
    'A',           0x00U,
    'L',           0x00U,
    ' ',           0x00U,
    'C',           0x00U,
    'O',           0x00U,
    'M',           0x00U,
};

USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t g_UsbDeviceString6[] = {2U + 2U * 16U, USB_DESCRIPTOR_TYPE_STRING,
                                '0',           0x00U,
                                '1',           0x00U,
                                '2',           0x00U,
                                '3',           0x00U,
                                '4',           0x00U,
                                '5',           0x00U,
                                '6',           0x00U,
                                '7',           0x00U,
                                '8',           0x00U,
                                '9',           0x00U,
                                'A',           0x00U,
                                'B',           0x00U,
                                'C',           0x00U,
                                'D',           0x00U,
                                'E',           0x00U,
                                'F',           0x00U};

uint32_t g_UsbDeviceStringDescriptorLength[USB_DEVICE_STRING_COUNT] = {
    sizeof(g_UsbDeviceString0), sizeof(g_UsbDeviceString1), sizeof(g_UsbDeviceString2),
    sizeof(g_UsbDeviceString3), sizeof(g_UsbDeviceString4),
	sizeof(g_UsbDeviceString5), sizeof(g_UsbDeviceString6), /*nsmoon230321 add 5, 6*/
};

uint8_t *g_UsbDeviceStringDescriptorArray[USB_DEVICE_STRING_COUNT] = {
    g_UsbDeviceString0, g_UsbDeviceString1, g_UsbDeviceString2, g_UsbDeviceString3, g_UsbDeviceString4,
	g_UsbDeviceString5, g_UsbDeviceString6, /*nsmoon230321 add 5, 6*/
};

usb_language_t g_UsbDeviceLanguage[USB_DEVICE_LANGUAGE_COUNT] = {{
    g_UsbDeviceStringDescriptorArray,
    g_UsbDeviceStringDescriptorLength,
    (uint16_t)0x0409U,
}};

usb_language_list_t g_UsbDeviceLanguageList = {
    g_UsbDeviceString0,
    sizeof(g_UsbDeviceString0),
    g_UsbDeviceLanguage,
    USB_DEVICE_LANGUAGE_COUNT,
};

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Get device descriptor request */
usb_status_t USB_DeviceGetDeviceDescriptor(usb_device_handle handle,
                                           usb_device_get_device_descriptor_struct_t *deviceDescriptor)
{
    deviceDescriptor->buffer = g_UsbDeviceDescriptor;
    deviceDescriptor->length = USB_DESCRIPTOR_LENGTH_DEVICE;
    return kStatus_USB_Success;
}
#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
/* Get device qualifier descriptor request */
usb_status_t USB_DeviceGetDeviceQualifierDescriptor(
    usb_device_handle handle, usb_device_get_device_qualifier_descriptor_struct_t *deviceQualifierDescriptor)
{
    deviceQualifierDescriptor->buffer = g_UsbDeviceQualifierDescriptor;
    deviceQualifierDescriptor->length = USB_DESCRIPTOR_LENGTH_DEVICE_QUALITIER;
    return kStatus_USB_Success;
}
#endif
/* Get device configuration descriptor request */
usb_status_t USB_DeviceGetConfigurationDescriptor(
    usb_device_handle handle, usb_device_get_configuration_descriptor_struct_t *configurationDescriptor)
{
    if (USB_COMPOSITE_CONFIGURE_INDEX > configurationDescriptor->configuration)
    {
        configurationDescriptor->buffer = g_UsbDeviceConfigurationDescriptor;
        configurationDescriptor->length = USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL;
        return kStatus_USB_Success;
    }
    return kStatus_USB_InvalidRequest;
}

/* Get device string descriptor request */
usb_status_t USB_DeviceGetStringDescriptor(usb_device_handle handle,
                                           usb_device_get_string_descriptor_struct_t *stringDescriptor)
{
    if (stringDescriptor->stringIndex == 0U)
    {
        stringDescriptor->buffer = (uint8_t *)g_UsbDeviceLanguageList.languageString;
        stringDescriptor->length = g_UsbDeviceLanguageList.stringLength;
    }
    else
    {
        uint8_t languageId = 0U;
        uint8_t languageIndex = USB_DEVICE_STRING_COUNT;

        for (; languageId < USB_DEVICE_LANGUAGE_COUNT; languageId++)
        {
            if (stringDescriptor->languageId == g_UsbDeviceLanguageList.languageList[languageId].languageId)
            {
                if (stringDescriptor->stringIndex < USB_DEVICE_STRING_COUNT)
                {
                    languageIndex = stringDescriptor->stringIndex;
                }
                break;
            }
        }

        if (USB_DEVICE_STRING_COUNT == languageIndex)
        {
            return kStatus_USB_InvalidRequest;
        }
        stringDescriptor->buffer = (uint8_t *)g_UsbDeviceLanguageList.languageList[languageId].string[languageIndex];
        stringDescriptor->length = g_UsbDeviceLanguageList.languageList[languageId].length[languageIndex];
    }
    return kStatus_USB_Success;
}

#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
/* Get hid descriptor request */
usb_status_t USB_DeviceGetHidDescriptor(usb_device_handle handle, usb_device_get_hid_descriptor_struct_t *hidDescriptor)
{
        return kStatus_USB_InvalidRequest;
    }

/* Get hid report descriptor request */
usb_status_t USB_DeviceGetHidReportDescriptor(usb_device_handle handle,
                                              usb_device_get_hid_report_descriptor_struct_t *hidReportDescriptor)
{
    if (USB_HID_MOUSE_INTERFACE_INDEX == hidReportDescriptor->interfaceNumber)
    {
        hidReportDescriptor->buffer = g_UsbDeviceHidMouseReportDescriptor;
        hidReportDescriptor->length = USB_DESCRIPTOR_LENGTH_HID_MOUSE_REPORT;
    }
    else if (USB_HID_GENERIC_INTERFACE_INDEX == hidReportDescriptor->interfaceNumber)
    {
        hidReportDescriptor->buffer = g_UsbDeviceHidGenericReportDescriptor;
        hidReportDescriptor->length = USB_DESCRIPTOR_LENGTH_HID_GENERIC_REPORT;
    }
    else
    {
        return kStatus_USB_InvalidRequest;
    }
    return kStatus_USB_Success;
}

/* Get hid physical descriptor request */
usb_status_t USB_DeviceGetHidPhysicalDescriptor(usb_device_handle handle,
                                                usb_device_get_hid_physical_descriptor_struct_t *hidPhysicalDescriptor)
{
    return kStatus_USB_InvalidRequest;
}
#endif

/* Due to the difference of HS and FS descriptors, the device descriptors and configurations need to be updated to match
 * current speed.
 * As the default, the device descriptors and configurations are configured by using FS parameters for both EHCI and
 * KHCI.
 * When the EHCI is enabled, the application needs to call this function to update device by using current speed.
 * The updated information includes endpoint max packet size, endpoint interval, etc. */
usb_status_t USB_DeviceSetSpeed(usb_device_handle handle, uint8_t speed)
{
    usb_descriptor_union_t *descriptorHead;
    usb_descriptor_union_t *descriptorTail;

    descriptorHead = (usb_descriptor_union_t *)&g_UsbDeviceConfigurationDescriptor[0];
    descriptorTail =
        (usb_descriptor_union_t *)(&g_UsbDeviceConfigurationDescriptor[USB_DESCRIPTOR_LENGTH_CONFIGURATION_ALL - 1U]);

    while (descriptorHead < descriptorTail)
    {
        if (descriptorHead->common.bDescriptorType == USB_DESCRIPTOR_TYPE_ENDPOINT)
        {
            if (USB_SPEED_HIGH == speed)
            {
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
                if (((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                    (USB_HID_MOUSE_ENDPOINT_IN ==
                     (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = HS_HID_MOUSE_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(USB_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else if (((descriptorHead->endpoint.bEndpointAddress &
                           USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                         (USB_HID_GENERIC_ENDPOINT_IN ==
                          (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = FS_HID_GENERIC_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(USB_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
#endif
#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
                if ((USB_CDC_VCOM_CIC_INTERRUPT_IN_ENDPOINT ==
                     (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)) &&
                    ((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN))
                {
                	descriptorHead->endpoint.bInterval = HS_CDC_VCOM_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE,
                    		descriptorHead->endpoint.wMaxPacketSize);
                }
                else if ((USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT ==
                          (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)) &&
                         ((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_CDC_VCOM_BULK_IN_PACKET_SIZE, descriptorHead->endpoint.wMaxPacketSize);
                }
                else if ((USB_CDC_VCOM_DIC_BULK_OUT_ENDPOINT ==
                          (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)) &&
                         ((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(HS_CDC_VCOM_BULK_OUT_PACKET_SIZE, descriptorHead->endpoint.wMaxPacketSize);
                }
#endif
                //else
                //{
                //}
            }
            else
            {
#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
                if (((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                    (USB_HID_MOUSE_ENDPOINT_IN ==
                     (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = FS_HID_MOUSE_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(USB_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
                else if (((descriptorHead->endpoint.bEndpointAddress &
                           USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                         (USB_HID_GENERIC_ENDPOINT_IN ==
                          (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)))
                {
                    descriptorHead->endpoint.bInterval = FS_HID_GENERIC_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(USB_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE,
                                                       descriptorHead->endpoint.wMaxPacketSize);
                }
#endif
#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
                if ((USB_CDC_VCOM_CIC_INTERRUPT_IN_ENDPOINT ==
                     (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)) &&
                    ((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                     USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN))
                {
                	descriptorHead->endpoint.bInterval = FS_CDC_VCOM_INTERRUPT_IN_INTERVAL;
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_CDC_VCOM_INTERRUPT_IN_PACKET_SIZE,
                    		descriptorHead->endpoint.wMaxPacketSize);
                }
                else if ((USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT ==
                          (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)) &&
                         ((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_CDC_VCOM_BULK_IN_PACKET_SIZE, descriptorHead->endpoint.wMaxPacketSize);
                }
                else if ((USB_CDC_VCOM_DIC_BULK_OUT_ENDPOINT ==
                          (descriptorHead->endpoint.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK)) &&
                         ((descriptorHead->endpoint.bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                          USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT))
                {
                    USB_SHORT_TO_LITTLE_ENDIAN_ADDRESS(FS_CDC_VCOM_BULK_OUT_PACKET_SIZE, descriptorHead->endpoint.wMaxPacketSize);
                }
#endif
                //else
                //{
                //}
            }
        }
        descriptorHead = (usb_descriptor_union_t *)((uint8_t *)descriptorHead + descriptorHead->common.bLength);
    }

#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
    if (USB_SPEED_HIGH == speed)
    {
        g_UsbDeviceHidMouseEndpoints[0].maxPacketSize = USB_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE;
    }
    else
    {
        g_UsbDeviceHidMouseEndpoints[0].maxPacketSize = USB_HID_MOUSE_INTERRUPT_IN_PACKET_SIZE;
    }

    if (USB_SPEED_HIGH == speed)
    {
        g_UsbDeviceHidGenericEndpoints[0].maxPacketSize = USB_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE;
    }
    else
    {
        g_UsbDeviceHidGenericEndpoints[0].maxPacketSize = USB_HID_GENERIC_INTERRUPT_IN_PACKET_SIZE;
    }
#endif

#if (USB_DEVICE_CONFIG_CDC_ACM > 0) //nsmoon@230321
    for (int i = 0; i < USB_CDC_VCOM_DIC_ENDPOINT_COUNT; i++)
    {
        if (USB_SPEED_HIGH == speed)
        {
            if (g_cdcVcomDicEndpoints[i].endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK)
            {
                g_cdcVcomDicEndpoints[i].maxPacketSize = HS_CDC_VCOM_BULK_IN_PACKET_SIZE;
            }
            else
            {
                g_cdcVcomDicEndpoints[i].maxPacketSize = HS_CDC_VCOM_BULK_OUT_PACKET_SIZE;
            }
        }
        else
        {
            if (g_cdcVcomDicEndpoints[i].endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK)
            {
                g_cdcVcomDicEndpoints[i].maxPacketSize = FS_CDC_VCOM_BULK_IN_PACKET_SIZE;
            }
            else
            {
                g_cdcVcomDicEndpoints[i].maxPacketSize = FS_CDC_VCOM_BULK_OUT_PACKET_SIZE;
            }
        }
    }
#endif

    return kStatus_USB_Success;
}
/* end of file */
