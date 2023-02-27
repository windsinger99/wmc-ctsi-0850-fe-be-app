/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "composite.h"
#include "hid_generic.h"
#include "hid_mouse.h"
#include "backend.h"
#include "pin_mux.h"

#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include "fsl_sysmpu.h"
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

#if (USB_DEVICE_CONFIG_HID < 2U)
#error USB_DEVICE_CONFIG_HID need to > 1U, Please change the MARCO USB_DEVICE_CONFIG_HID in file "usb_device_config.h".
#endif

#include "usb_phy.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
//void BOARD_InitHardware(void);
static void USB_DeviceClockInit(void);
void USB_DeviceIsrEnable(void);
#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle);
#endif

static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param);

#if USB_DEVICE_SOF_ENABLE //nsmoon@211222
extern void USB_SOF_IntEnable(void);
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern usb_device_class_struct_t g_UsbDeviceHidMouseConfig;
extern usb_device_class_struct_t g_UsbDeviceHidGenericConfig;

static usb_device_composite_struct_t s_UsbDeviceComposite;

/* Set class configurations */
usb_device_class_config_struct_t g_CompositeClassConfig[USB_COMPOSITE_INTERFACE_COUNT] = {
    {
        USB_DeviceHidGenericCallback, /* HID generic class callback pointer */
        (class_handle_t)NULL,          /* The HID class handle, This field is set by USB_DeviceClassInit */
        &g_UsbDeviceHidGenericConfig, /* The HID generic configuration, including class code, subcode, and protocol,
                                  class
                                  type, transfer type, endpoint address, max packet size, etc.*/
    },
    {
        USB_DeviceHidMouseCallback, /* HID mouse class callback pointer */
        (class_handle_t)NULL,       /* The HID class handle, This field is set by USB_DeviceClassInit */
        &g_UsbDeviceHidMouseConfig, /* The HID mouse configuration, including class code, subcode, and protocol, class
                               type,
                               transfer type, endpoint address, max packet size, etc.*/
    }};

/* Set class configuration list */
usb_device_class_config_list_struct_t g_UsbDeviceCompositeConfigList = {
    g_CompositeClassConfig,        /* Class configurations */
    USB_DeviceCallback,            /* Device callback pointer */
    USB_COMPOSITE_INTERFACE_COUNT, /* Class count */
};

/*******************************************************************************
 * Code
 ******************************************************************************/
#if 0
void USB_OTG1_IRQHandler(void)
{
    USB_DeviceEhciIsrFunction(s_UsbDeviceComposite.deviceHandle);
}

void USB_OTG2_IRQHandler(void)
{
    USB_DeviceEhciIsrFunction(s_UsbDeviceComposite.deviceHandle);
}
#endif

void USB_DeviceClockInit(void)
{
    usb_phy_config_struct_t phyConfig = {
        BOARD_USB_PHY_D_CAL,
        BOARD_USB_PHY_TXCAL45DP,
        BOARD_USB_PHY_TXCAL45DM,
    };

    if (CONTROLLER_ID == kUSB_ControllerEhci0)
    {
        CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);
    }
    else
    {
        CLOCK_EnableUsbhs1PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
        CLOCK_EnableUsbhs1Clock(kCLOCK_Usb480M, 480000000U);
    }
    USB_EhciPhyInit(CONTROLLER_ID, BOARD_XTAL0_CLK_HZ, &phyConfig);
}

void USB_DeviceIsrEnable(void)
{
    uint8_t irqNumber;

    uint8_t usbDeviceEhciIrq[] = USBHS_IRQS;
    irqNumber                  = usbDeviceEhciIrq[CONTROLLER_ID - kUSB_ControllerEhci0];

    /* Install isr, set priority, and enable IRQ. */
    NVIC_SetPriority((IRQn_Type)irqNumber, USB_DEVICE_INTERRUPT_PRIORITY);
    EnableIRQ((IRQn_Type)irqNumber);
}

void USB_DeviceIsrDisable(void)
{
    uint8_t irqNumber;

    uint8_t usbDeviceEhciIrq[] = USBHS_IRQS;
    irqNumber                  = usbDeviceEhciIrq[CONTROLLER_ID - kUSB_ControllerEhci0];

    /* Install isr, set priority, and enable IRQ. */
    //NVIC_SetPriority((IRQn_Type)irqNumber, USB_DEVICE_INTERRUPT_PRIORITY);
    DisableIRQ((IRQn_Type)irqNumber);
}

#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle)
{
    USB_DeviceEhciTaskFunction(deviceHandle);
}
#endif

/* The Device callback */
#if (_DEBUG == 1)
//#define DEBUG_USB_DeviceCallback
#endif
#ifdef DEBUG_USB_DeviceCallback
#define TRACE_UDC(...)    usb_echo(__VA_ARGS__)
#else
#define TRACE_UDC(...)
#endif
static usb_status_t USB_DeviceCallback(usb_device_handle handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;
    uint16_t *temp16   = (uint16_t *)param;
    uint8_t *temp8     = (uint8_t *)param;

    switch (event)
    {
        case kUSB_DeviceEventBusReset:
        {
        	TRACE_UDC("kUSB_DeviceEventBusReset...\r\n");
            /* USB bus reset signal detected */
        	s_UsbDeviceComposite.attach               = 0U;
            s_UsbDeviceComposite.currentConfiguration = 0U;
            error                                     = kStatus_USB_Success;
#if (defined(USB_DEVICE_CONFIG_EHCI) && (USB_DEVICE_CONFIG_EHCI > 0U)) || \
    (defined(USB_DEVICE_CONFIG_LPCIP3511HS) && (USB_DEVICE_CONFIG_LPCIP3511HS > 0U))
            /* Get USB speed to configure the device, including max packet size and interval of the endpoints. */
            if (kStatus_USB_Success == USB_DeviceClassGetSpeed(CONTROLLER_ID, &s_UsbDeviceComposite.speed))
            {
                USB_DeviceSetSpeed(handle, s_UsbDeviceComposite.speed);
            }
#endif
        }
        break;
        case kUSB_DeviceEventSetConfiguration:
        {
        	TRACE_UDC("kUSB_DeviceEventSetConfiguration...%x\r\n", (*temp8));
            if (0U == (*temp8))
            {
            	s_UsbDeviceComposite.attach               = 0U;
            	s_UsbDeviceComposite.currentConfiguration = 0U;
            }
            else if (USB_COMPOSITE_CONFIGURE_INDEX == (*temp8))
            {
                /* Set device configuration request */
            	s_UsbDeviceComposite.attach = 1U;
                s_UsbDeviceComposite.currentConfiguration = *temp8;
                USB_DeviceHidMouseSetConfigure(s_UsbDeviceComposite.hidMouseHandle, *temp8);
                USB_DeviceHidGenericSetConfigure(s_UsbDeviceComposite.hidGenericHandle, *temp8);
#if USB_DEVICE_SOF_ENABLE //nsmoon@211222
				USB_SOF_IntEnable();
#endif
                error = kStatus_USB_Success;
            }
            else
            {
                error = kStatus_USB_InvalidRequest;
            }
        }
            break;
        case kUSB_DeviceEventSetInterface:
        {
        	TRACE_UDC("kUSB_DeviceEventSetInterface...%x\r\n", s_UsbDeviceComposite.attach);
            if (s_UsbDeviceComposite.attach)
            {
                /* Set device interface request */
                uint8_t interface        = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                uint8_t alternateSetting = (uint8_t)(*temp16 & 0x00FFU);
            	usb_echo(" ..interface=%d\r\n", interface);
                if (interface < USB_COMPOSITE_INTERFACE_COUNT)
                {
                	s_UsbDeviceComposite.currentInterfaceAlternateSetting[interface] = alternateSetting;
                    USB_DeviceHidMouseSetInterface(s_UsbDeviceComposite.hidMouseHandle, interface, alternateSetting);
                    USB_DeviceHidGenericSetInterface(s_UsbDeviceComposite.hidGenericHandle, interface,
                                                      alternateSetting);
                    error = kStatus_USB_Success;
                }
            }
        }
            break;
        case kUSB_DeviceEventGetConfiguration:
        {
        	TRACE_UDC("kUSB_DeviceEventGetConfiguration...%x %x\r\n", param, s_UsbDeviceComposite.currentConfiguration);
            if (param)
            {
                /* Get current configuration request */
                *temp8 = s_UsbDeviceComposite.currentConfiguration;
                error  = kStatus_USB_Success;
            }
        }
            break;
        case kUSB_DeviceEventGetInterface:
        {
        	TRACE_UDC("kUSB_DeviceEventGetInterface...%x\r\n", param);
            if (param)
            {
                /* Get current alternate setting of the interface request */
                uint8_t interface = (uint8_t)((*temp16 & 0xFF00U) >> 0x08U);
                TRACE_UDC(" ..interface=%x\r\n", interface);
                if (interface < USB_COMPOSITE_INTERFACE_COUNT)
                {
                    *temp16 = (*temp16 & 0xFF00U) | s_UsbDeviceComposite.currentInterfaceAlternateSetting[interface];
                    error   = kStatus_USB_Success;
                }
                else
                {
                    error = kStatus_USB_InvalidRequest;
                }
            }
        }
            break;
        case kUSB_DeviceEventGetDeviceDescriptor:
        {
        	TRACE_UDC("kUSB_DeviceEventGetDeviceDescriptor...%x\r\n", param);
            if (param)
            {
#ifdef DEBUG_USB_DeviceCallback
            	usb_device_get_device_descriptor_struct_t *deviceDescriptor = (usb_device_get_device_descriptor_struct_t *)param;
            	TRACE_UDC(" ..length=%x\r\n", deviceDescriptor->length);
#endif
                /* Get device descriptor request */
                error = USB_DeviceGetDeviceDescriptor(handle, (usb_device_get_device_descriptor_struct_t *)param);
            }
        }
            break;
        case kUSB_DeviceEventGetConfigurationDescriptor:
        {
        	TRACE_UDC("kUSB_DeviceEventGetConfigurationDescriptor...%x\r\n", param);
            if (param)
            {
#ifdef DEBUG_USB_DeviceCallback
            	usb_device_get_configuration_descriptor_struct_t *configurationDescriptor = (usb_device_get_configuration_descriptor_struct_t *)param;
            	TRACE_UDC(" ..configuration=%x %x\r\n", configurationDescriptor->configuration, configurationDescriptor->length);
#endif
            	/* Get device configuration descriptor request */
                error = USB_DeviceGetConfigurationDescriptor(handle, (usb_device_get_configuration_descriptor_struct_t *)param);
            }
        }
            break;
        case kUSB_DeviceEventGetStringDescriptor:
        {
        	TRACE_UDC("kUSB_DeviceEventGetStringDescriptor...%x\r\n", param);
            if (param)
            {
#ifdef DEBUG_USB_DeviceCallback
            	usb_device_get_string_descriptor_struct_t *stringDescriptor = (usb_device_get_string_descriptor_struct_t *)param;
            	TRACE_UDC(" ..languageId=%x %x %x\r\n", stringDescriptor->languageId, stringDescriptor->stringIndex, stringDescriptor->length);
#endif
                /* Get device string descriptor request */
                error = USB_DeviceGetStringDescriptor(handle, (usb_device_get_string_descriptor_struct_t *)param);
            }
        }
            break;
        case kUSB_DeviceEventGetHidDescriptor:
        {
        	TRACE_UDC("kUSB_DeviceEventGetHidDescriptor...%x\r\n", param);
            if (param)
            {
#ifdef DEBUG_USB_DeviceCallback
            	usb_device_get_hid_report_descriptor_struct_t *hidReportDescriptor = (usb_device_get_hid_report_descriptor_struct_t *)param;
            	TRACE_UDC(" ..interfaceNumber=%x %x\r\n", hidReportDescriptor->interfaceNumber, hidReportDescriptor->length);
#endif
            	/* Get hid descriptor request */
                error = USB_DeviceGetHidDescriptor(handle, (usb_device_get_hid_descriptor_struct_t *)param);
            }
        }
            break;
        case kUSB_DeviceEventGetHidReportDescriptor:
        {
        	TRACE_UDC("kUSB_DeviceEventGetHidReportDescriptor...%x\r\n", param);
            if (param)
            {
#ifdef DEBUG_USB_DeviceCallback
            	usb_device_get_hid_report_descriptor_struct_t *hidReportDescriptor = (usb_device_get_hid_report_descriptor_struct_t *)param;
            	TRACE_UDC(" ..interfaceNumber=%x %x\r\n", hidReportDescriptor->interfaceNumber, hidReportDescriptor->length);
#endif
                /* Get hid report descriptor request */
                error = USB_DeviceGetHidReportDescriptor(handle, (usb_device_get_hid_report_descriptor_struct_t *)param);
            }
        }
            break;
#if (defined(USB_DEVICE_CONFIG_CV_TEST) && (USB_DEVICE_CONFIG_CV_TEST > 0U))
        case kUSB_DeviceEventGetDeviceQualifierDescriptor:
        {
        	//usb_device_get_device_qualifier_descriptor_struct_t *deviceQualifierDescriptor = (usb_device_get_device_qualifier_descriptor_struct_t *)param;
        	TRACE_UDC("kUSB_DeviceEventGetDeviceQualifierDescriptor...%x\r\n", param);
            if (param)
            {
                /* Get Qualifier descriptor request */
                error = USB_DeviceGetDeviceQualifierDescriptor(
                    handle, (usb_device_get_device_qualifier_descriptor_struct_t *)param);
            }
        }
            break;
#endif
        case kUSB_DeviceEventGetHidPhysicalDescriptor:
        {
        	TRACE_UDC("kUSB_DeviceEventGetHidPhysicalDescriptor...%x\r\n", param);
            if (param)
            {
                /* Get hid physical descriptor request */
                error = USB_DeviceGetHidPhysicalDescriptor(handle, (usb_device_get_hid_physical_descriptor_struct_t *)param);
            }
        }
            break;
        default:
            break;
    }

    return error;
}

#if defined(USB_DEVICE_FULL_SPEED_ENABLE) && (USB_DEVICE_FULL_SPEED_ENABLE > 0U)
extern void USB_DeviceEhciFullSpeed(uint8_t usb_speed);
#endif

#define DELAY_MULTIPLY	10
void USB_delay(uint32_t cnt)
{
    volatile uint32_t i = 0;
    uint32_t cntTotal = cnt * DELAY_MULTIPLY;
    for (i = 0; i < cntTotal; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

/* Application initialization */
void USB_DeviceApplicationInit(void)
{
    USB_DeviceClockInit();
#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
    SYSMPU_Enable(SYSMPU, 0);
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

    /* Set composite device to default state */
    s_UsbDeviceComposite.speed             = USB_SPEED_FULL;
    s_UsbDeviceComposite.attach            = 0U;
    s_UsbDeviceComposite.hidMouseHandle    = (class_handle_t)NULL;
    s_UsbDeviceComposite.hidGenericHandle = (class_handle_t)NULL;
    s_UsbDeviceComposite.deviceHandle      = NULL;

#if defined(USB_DEVICE_FULL_SPEED_ENABLE) && (USB_DEVICE_FULL_SPEED_ENABLE > 0U)
    USB_DeviceEhciFullSpeed(USB_SPEED_FULL); //nsmoon@210610
#endif

    if (kStatus_USB_Success !=
        USB_DeviceClassInit(CONTROLLER_ID, &g_UsbDeviceCompositeConfigList, &s_UsbDeviceComposite.deviceHandle))
    {
        usb_echo("USB device composite demo init failed\r\n");
        return;
    }
    else
    {
        usb_echo("USB device composite demo\r\n");
        /* Get the HID generic class handle */
        s_UsbDeviceComposite.hidGenericHandle = g_UsbDeviceCompositeConfigList.config[0].classHandle;
        /* Get the HID mouse class handle */
        s_UsbDeviceComposite.hidMouseHandle = g_UsbDeviceCompositeConfigList.config[1].classHandle;

        USB_DeviceHidGenericInit(&s_UsbDeviceComposite);
        USB_DeviceHidMouseInit(&s_UsbDeviceComposite);
    }

    USB_DeviceIsrEnable();

    /* Start the device function */
    /*Add one delay here to make the DP pull down long enough to allow host to detect the previous disconnection.*/
    //SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    USB_delay(2000); //nsmoon@210609
    USB_DeviceRun(s_UsbDeviceComposite.deviceHandle);
}

uint8_t USB_Device_ready(void)
{
	return s_UsbDeviceComposite.attach;
}

usb_device_handle USB_Get_Device_Handle(void)
{
	return s_UsbDeviceComposite.deviceHandle;
}

#if 0 //for test
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION)) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif
{
    BOARD_ConfigMPU();

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    USB_DeviceApplicationInit();

    while (1U)
    {
#if USB_DEVICE_CONFIG_USE_TASK
        USB_DeviceTaskFn(s_UsbDeviceComposite.deviceHandle);
#endif
    }
}
#endif

/*end of file*/
