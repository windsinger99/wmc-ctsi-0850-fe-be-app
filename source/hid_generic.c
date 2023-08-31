/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "usb_device_config.h"

#if (USB_DEVICE_CONFIG_HID > 0) //nsmoon@230321
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_hid.h"

#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"

#include "composite.h"
#include "hid_generic.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TRACE_HID(...)		//TRACE(__VA_ARGS__)
#define TRACE_DBG(...)		//DBG_PRINTF(__VA_ARGS__);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

//USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_GenericBuffer[USB_HID_GENERIC_REPORT_LENGTH];
//USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_GenericBuff0[USB_HID_GENERIC_OUT_BUFFER_LENGTH];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_GenericFeatureBuff[USB_HID_GENERIC_OUT_BUFFER_LENGTH];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_GenericBuffRx[USB_HID_GENERIC_OUT_BUFFER_LENGTH];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_GenericBuffTx[USB_HID_GENERIC_IN_BUFFER_LENGTH];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_GenericBuffTxDiag[USB_HID_GENERIC_IN_BUFFER_LENGTH];
static usb_device_composite_struct_t *s_UsbDeviceComposite;
usb_device_hid_generic_struct_t g_UsbDeviceHidGeneric;

/*******************************************************************************
 * Code
 ******************************************************************************/

/* The hid class callback */
#if (_DEBUG == 1)
//#define DEBUG_USB_DeviceHidGenericCallback
#endif
#ifdef DEBUG_USB_DeviceHidGenericCallback
#define TRACE_UDHGCB(...)    usb_echo(__VA_ARGS__);usb_echo("\r\n")
#else
#define TRACE_UDHGCB(...)
#endif
usb_status_t USB_DeviceHidGenericCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Success; //kStatus_USB_Error;
    if (s_UsbDeviceComposite->attach == 0) {
        return kStatus_USB_Error;
    }

    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
        {
            TRACE_UDHGCB("kUSB_DeviceHidEventSendResponse..");
#if 0 //nsmoon@190719, not-used
            if (s_UsbDeviceComposite->attach) {
            	g_UsbDeviceHidGeneric.DataSent = 1U;
            }
#endif
        }
            break;

        case kUSB_DeviceHidEventRecvResponse:
        {
            TRACE_UDHGCB("kUSB_DeviceHidEventRecvResponse..");
#if 1 //nsmoon@190719
        	if (g_UsbDeviceHidGeneric.DataRecv) {
        		//TRACE_ERROR("ERROR! invalid g_UsbDeviceHidGeneric.DataRecv");
				g_UsbDeviceHidGeneric.DataRecvOv++;
        		//return kStatus_USB_Busy; //FIXME
        	}
    	    //g_UsbDeviceHidGeneric.DataRecv = 1U; //nsmoon@190902, move to below
            error = USB_DeviceHidRecv(handle, USB_HID_GENERIC_ENDPOINT_OUT,
                              (uint8_t *)&g_UsbDeviceHidGeneric.buffRx[0], USB_HID_GENERIC_OUT_BUFFER_LENGTH);
            if (error == kStatus_USB_Success) {
        	    g_UsbDeviceHidGeneric.RxCmd = g_UsbDeviceHidGeneric.buffRx[0]; //save
            }
    	    g_UsbDeviceHidGeneric.DataRecv = 1U; //nsmoon@190902
            //return error;
#endif
        }
            break;

        case kUSB_DeviceHidEventGetReport:
        {
            usb_device_hid_report_struct_t *getReport = (usb_device_hid_report_struct_t *)param;
       	    TRACE_UDHGCB("kUSB_DeviceHidEventGetReport..%x", getReport);
#if 1 //nsmoon@190724
            if (getReport) {
           	    TRACE_UDHGCB(" ..reportType=%x/%x/%x", getReport->reportType, getReport->reportId, getReport->reportLength);
				if(getReport->reportType == 0x03 && getReport->reportId == VALID_CONTACTS_FEATURE_REPORT_ID)
				{
					//Prepare a response packet for the host
					g_UsbDeviceHidGeneric.featurebuff[0] = VALID_CONTACTS_FEATURE_REPORT_ID;
					g_UsbDeviceHidGeneric.featurebuff[1] = MAX_VALID_CONTACT_POINTS;
					getReport->reportBuffer = g_UsbDeviceHidGeneric.featurebuff;
					getReport->reportLength = 2;
				}
    #if 0 //nsmoon@210610 not-used
				else if(getReport->reportType == 0x03 && getReport->reportId == DEVICE_MODE_FEATURE_REPORT_ID)
				{
					/*Report ID = 0x03 = Configuration report, which is used to select between mouse, single/multi digitizer modes.
					Prepare a response packet for the host*/
					g_UsbDeviceHidGeneric.featurebuff[0] = DEVICE_MODE_FEATURE_REPORT_ID;
					g_UsbDeviceHidGeneric.featurebuff[1] = 0x02; //Device Mode: Mouse=0x00, Single-input(or stylus)=0x01, Multi-input=0x02
					g_UsbDeviceHidGeneric.featurebuff[2] = 0x01; //Device Identifier
					getReport->reportBuffer = g_UsbDeviceHidGeneric.featurebuff;
					getReport->reportLength = 3;
			    }
    #endif //0
				else {
					error = kStatus_USB_InvalidRequest;
				}
			    //usb_debug("rType(0x%02x) rID(0x%02x)\r\n", getReport->reportType, getReport->reportId);
            }
#else
			error = kStatus_USB_InvalidRequest;
#endif
        }
            break;

        case kUSB_DeviceHidEventSetReport:
        {
#if 0 //for test, nsmoon@210609
            usb_device_hid_report_struct_t *getReport = (usb_device_hid_report_struct_t *)param;
       	    TRACE_UDHGCB("kUSB_DeviceHidEventSetReport..%x", getReport);
            if (getReport) {
           	    TRACE_UDHGCB(" ..reportType=%x/%x/%x", getReport->reportType, getReport->reportId, getReport->reportLength);
    #if 0 //def DEBUG_USB_DeviceHidGenericCallback
                int i;
                for (i = 0; i < getReport->reportLength; i++) {
                	usb_echo("%02x ", getReport->reportBuffer[i]);
                }
                usb_echo("\r\n");
                for (i = 0; i < getReport->reportLength; i++) {
                	usb_echo("%02x ", g_UsbDeviceHidGeneric.featurebuff[i]);
                }
                usb_echo("\r\n");
    #endif
            }
            else {
                error = kStatus_USB_InvalidRequest;
            }
#else
            error = kStatus_USB_InvalidRequest;
#endif
        }
            break;

        case kUSB_DeviceHidEventRequestReportBuffer:
        {
#if 0 //for test //nsmoon@210609
            usb_device_hid_report_struct_t *getReport = (usb_device_hid_report_struct_t *)param;
       	    TRACE_UDHGCB("kUSB_DeviceHidEventRequestReportBuffer..%x %x %x", getReport->reportType, getReport->reportId, getReport->reportLength);
            if (getReport) {
				 if(getReport->reportType == 0x03 && getReport->reportId == DEVICE_MODE_FEATURE_REPORT_ID)
				 {
					 getReport->reportBuffer = g_UsbDeviceHidGeneric.featurebuff;
					 getReport->reportLength = USB_HID_GENERIC_OUT_BUFFER_LENGTH;
				 }
				 else {
					 error = kStatus_USB_InvalidRequest;
				 }
            }
            else {
				error = kStatus_USB_InvalidRequest;
            }
#else
            error = kStatus_USB_InvalidRequest;
#endif
        }
            break;
        case kUSB_DeviceHidEventGetIdle:
        {
            TRACE_UDHGCB("kUSB_DeviceHidEventGetIdle..");
        }
            break;

        case kUSB_DeviceHidEventGetProtocol:
        {
            TRACE_UDHGCB("kUSB_DeviceHidEventGetProtocol..");
        }
            break;

        case kUSB_DeviceHidEventSetIdle:
        {
            TRACE_UDHGCB("kUSB_DeviceHidEventSetIdle..");
        }
            break;

        case kUSB_DeviceHidEventSetProtocol:
        {
            TRACE_UDHGCB("kUSB_DeviceHidEventSetProtocol..");
        }
            break;
        default:
        {
            TRACE_ERROR("ERROR! unknown event..%d", event);
        }
            break;
    }

    return error;
}

usb_status_t USB_DeviceHidGenericSetConfigure(class_handle_t handle, uint8_t configure)
{
    usb_status_t error = kStatus_USB_Error;
    if (USB_COMPOSITE_CONFIGURE_INDEX == configure)
    {
    	g_UsbDeviceHidGeneric.DataRecv = 0; //reset
    	/* Set device configuration request */
        error = USB_DeviceHidRecv(handle, USB_HID_GENERIC_ENDPOINT_OUT,
                              (uint8_t *)&g_UsbDeviceHidGeneric.buffRx[0],
							  USB_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE);
#if 0 //def DEBUG_USB_DeviceHidGenericCallback
            TRACE("USB_DeviceHidGenericSetConfigure..USB_DeviceHidRecv %d", error);
            if (error == kStatus_USB_Success) {
            	int i;
            	for (i = 0; i < USB_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE; i++)
            	{
            		usb_echo("%02x ", g_UsbDeviceHidGeneric.featurebuff[i]);
            		if (((i+1)%16) == 0) usb_echo("\r\n");
            	}
            	usb_echo("\r\n");
            }
#endif
    }
    return error;
}

usb_status_t USB_DeviceHidGenericSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting)
{
    usb_status_t error = kStatus_USB_Error;
    if (USB_HID_GENERIC_INTERFACE_INDEX == interface)
    {
    	g_UsbDeviceHidGeneric.DataRecv = 0; //reset
        /* Set device interface request */
    	s_UsbDeviceComposite->currentInterfaceAlternateSetting[interface] = alternateSetting;
        if (alternateSetting == 0U)
        {
            error = USB_DeviceHidRecv(
            		handle, USB_HID_GENERIC_ENDPOINT_OUT,
                (uint8_t *)&g_UsbDeviceHidGeneric.buffRx[0],
				USB_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE);
#if 0 //def DEBUG_USB_DeviceHidGenericCallback
            TRACE("USB_DeviceHidGenericSetInterface..USB_DeviceHidRecv %d", error);
            if (error == kStatus_USB_Success) {
            	int i;
            	for (i = 0; i < USB_HID_GENERIC_INTERRUPT_OUT_PACKET_SIZE; i++)
            	{
            		if (((i+1)%16) == 0) usb_echo("\r\n");
            		usb_echo("%02x ", g_UsbDeviceHidGeneric.featurebuff[i]);
            	}
            	usb_echo("\r\n");
            }
#endif
        }
    }
    return error;
}

usb_status_t USB_DeviceHidGenericInit(usb_device_composite_struct_t *deviceComposite)
{
    s_UsbDeviceComposite = deviceComposite;
    //g_UsbDeviceHidGeneric.buffer = s_GenericBuffer;
#if 1 //nsmoon@190719
    //g_UsbDeviceHidGeneric.buff0 = (uint8_t *)&s_GenericBuff0[0];
    g_UsbDeviceHidGeneric.featurebuff = (uint8_t *)&s_GenericFeatureBuff[0];
    g_UsbDeviceHidGeneric.buffRx = (uint8_t *)&s_GenericBuffRx[0];
    g_UsbDeviceHidGeneric.buffTx = (uint8_t *)&s_GenericBuffTx[0];
    g_UsbDeviceHidGeneric.buffTxDiag = (uint8_t *)&s_GenericBuffTxDiag[0];
    //g_UsbDeviceHidGeneric.DataSent = 1U; //ready to send, not-used
    g_UsbDeviceHidGeneric.DataRecv = 0U; //empty
	g_UsbDeviceHidGeneric.DataRecvOv = 0;
#endif
    return kStatus_USB_Success;
}

int USB_SendDigitizerReport(uint8_t *transmitDataBuffer, int len)
{
	usb_status_t error = kStatus_USB_Success;
	//int retry = 0x5000;
    g_UsbDeviceHidGeneric.idleRate = 0;
    //g_UsbDeviceHidGeneric.DataSent = 0; //not-used

    //while (retry-- > 0)
    while (1) //best effort
    {
	    error = USB_DeviceHidSend(s_UsbDeviceComposite->hidGenericHandle, USB_HID_GENERIC_ENDPOINT_IN, transmitDataBuffer, len);
        if (error != kStatus_USB_Busy) {
            if (error != kStatus_USB_Success) {
    	        TRACE_ERROR("ERROR! USB_SendDigitizerReport..USB_DeviceHidSend: %d", error);
    	        return USB_ERR;
            }
            return USB_OK;
        }
        //TRACE_DBG("^");
    }
    //g_UsbDeviceHidGeneric.DataSent = 1; //reset, not-used
    TRACE_ERROR("ERROR! USB_SendDigitizerReport..timeout!!");
    return USB_ERR;
}
#endif
/*end of file*/
