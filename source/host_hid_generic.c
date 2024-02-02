/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_host_config.h"
#include "usb_host.h"
#include "usb_host_hid.h"
#include "host_hid_generic.h"
#include "bootloader.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if (_DEBUG == 1) //nsmoon@190416
#define TRACE(...) usb_echo(__VA_ARGS__);usb_echo("\r\n")
#define TRACENR(...) usb_echo(__VA_ARGS__);
#define TRACE_HOST(...) //TRACE(__VA_ARGS__)
#else
#define TRACE(...) //usb_echo(__VA_ARGS__);usb_echo("\r\n")
#define TRACENR(...)
#define TRACE_HOST(...)
#endif
#define TRACE_ERROR(...) //TRACE(__VA_ARGS__)

#ifndef ATTRIBUTE_RAMFUNC
#define ATTRIBUTE_RAMFUNC		__attribute__((section(".ramfunc*")))  //FIXME
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief process hid data and print generic data.
 *
 * @param genericInstance   hid generic instance pointer.
 */
//static void USB_HostHidGenericProcessBuffer(usb_host_hid_generic_instance_t *genericInstance);

/*!
 * @brief host hid generic control transfer callback.
 *
 * This function is used as callback function for control transfer .
 *
 * @param param      the host hid generic instance pointer.
 * @param data       data buffer pointer.
 * @param dataLength data length.
 * @status           transfer result status.
 */
static void USB_HostHidControlCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status);

/*!
 * @brief host hid generic interrupt in transfer callback.
 *
 * This function is used as callback function when call USB_HostHidRecv .
 *
 * @param param      the host hid generic instance pointer.
 * @param data       data buffer pointer.
 * @param dataLength data length.
 * @status           transfer result status.
 */
static void USB_HostHidInCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status);

/*!
 * @brief host hid generic interrupt out transfer callback.
 *
 * This function is used as callback function when call USB_HostHidSend .
 *
 * @param param    the host hid generic instance pointer.
 * @param data     data buffer pointer.
 * @param dataLength data length.
 * @status         transfer result status.
 */
static void USB_HostHidOutCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status);

/*!
 * @brief host hid generic prepare data for sending.
 *
 * @param genericInstance  the host hid generic instance pointer.
 *
 * @retval kStatus_USB_Sucess  there is data.
 * @retval kStatus_USB_Error   data is sent done.
 */
//static usb_status_t USB_HostHidGenericPrepareOutData(usb_host_hid_generic_instance_t *genericInstance);
static int feBtDataIn(uint8_t *buffIn, uint8_t *buffOut, int len);
static int feBtDataOut(uint8_t *buffIn, uint8_t *buffOut, int bufferSize);

/*******************************************************************************
 * Variables
 ******************************************************************************/
usb_host_handle g_HostHandle;
usb_host_hid_generic_instance_t g_HostHidGeneric; /* hid generic instance */
uint8_t testData[] =
    "Test string: This is usb host hid generic demo, it only support pid=0x00a2 and vid=0x1fc9 hid device. Host send "
    "this test string to device, device reply the data to host then host print the data";

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t s_GenericInBuffer[HID_GENERIC_IN_BUFFER_SIZE]; /*!< use to receive report descriptor and data */
uint8_t s_GenericInBufferSav[HID_GENERIC_IN_BUFFER_SIZE];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
uint8_t s_GenericOutBuffer[HID_GENERIC_IN_BUFFER_SIZE]; /*!< use to send data */
uint8_t s_FeBtOutBuffer[HID_GENERIC_IN_BUFFER_SIZE];
uint8_t s_FeBtInBuffer[HID_GENERIC_IN_BUFFER_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/
#if 0  //for testing
static void debug_dump_buffer(uint8_t *buff, int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		TRACENR("%02x ", buff[i]);
		if (((i+1)%16) == 0) TRACENR("\r\n");
	}
	TRACENR("\r\n");
}
#endif
#if 1  //for testing
void USB_host_delay(uint32_t maxCnt)
{
	volatile uint32_t debugCnt = 0; \
	for (debugCnt = 0; debugCnt < (maxCnt*1000); debugCnt++) { \
	    __asm("NOP"); /* delay */ \
	} /*sys clock check, -O0, 8-instuctions */
}
#endif

#if 0 //for test
static void USB_HostHidGenericProcessBuffer(usb_host_hid_generic_instance_t *genericInstance)
{
}
#endif

static void USB_HostHidControlCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    usb_host_hid_generic_instance_t *genericInstance = (usb_host_hid_generic_instance_t *)param;

    if (kStatus_USB_TransferStall == status)
    {
    	TRACE_ERROR("BT_ERROR! device don't support this ruquest");
    }
    else if (kStatus_USB_Success != status)
    {
    	TRACE_ERROR("BT_ERROR! control transfer failed");
    }
    else
    {
    }

    if (genericInstance->runWaitState == kUSB_HostHidRunWaitSetInterface) /* set interface finish */
    {
        genericInstance->runState = kUSB_HostHidRunSetInterfaceDone;
    }
    else if (genericInstance->runWaitState == kUSB_HostHidRunWaitSetIdle) /* hid set idle finish */
    {
        genericInstance->runState = kUSB_HostHidRunSetIdleDone;
    }
    else if (genericInstance->runWaitState == kUSB_HostHidRunWaitGetReportDescriptor) /* hid get report descriptor finish */
    {
        genericInstance->runState = kUSB_HostHidRunGetReportDescriptorDone;
    }
    else if (genericInstance->runWaitState == kUSB_HostHidRunWaitSetProtocol) /* hid set protocol finish */
    {
        genericInstance->runState = kUSB_HostHidRunSetProtocolDone;
    }
    else if (genericInstance->runWaitState == kUSB_HostHidRunWaitGetProtocol) /* hid set protocol finish */
    {
        genericInstance->runState = kUSB_HostHidRunGetProtocolDone;
    }
    else if (genericInstance->runWaitState == kUSB_HostHidRunDataTransmitting) /* transmitting */
    {
        genericInstance->runState = kUSB_HostHidRunGetProtocolDone;
    }
    else if (genericInstance->runWaitState == kUSB_HostHidRunWaitDataReceiving) /* transmitting */
    {
        genericInstance->runState = kUSB_HostHidRunDataTransmitted;
    }
    else
    {
    }
}

static void USB_HostHidInCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    usb_host_hid_generic_instance_t *genericInstance = (usb_host_hid_generic_instance_t *)param;

    if (genericInstance->runWaitState == kUSB_HostHidRunWaitDataReceived)
    {
        if (status == kStatus_USB_Success)
        {
            genericInstance->runState = kUSB_HostHidRunDataReceived; /* go to process data */
            //genericInstance->runState = kUSB_HostHidRunDataTransmitted;
        }
#if 0
        else
        {
            if (genericInstance->deviceState == kStatus_DEV_Attached)
            {
                genericInstance->runState = kUSB_HostHidRunPrimeDataReceive; /* go to prime next receiving */
            }
        }
#endif
    }
}

extern void MY_WDOG_soft_reset();
static void USB_HostHidOutCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    usb_host_hid_generic_instance_t *genericInstance = (usb_host_hid_generic_instance_t *)param;
    if (genericInstance->runWaitState == kUSB_HostHidRunWaitDataTransmitted)
    {
        if (status == kStatus_USB_Success)
        {
            genericInstance->runState = kUSB_HostHidRunDataTransmitted; /* go to process data */
        }
    }
#ifdef USB_HOST_FOR_FRONTEND
#ifndef FRONTEND_RESET_ENABLE
    if (bootloaderData.modeChange == FE_BT_MODE && g_HostHidGeneric.softReset == 1) {
    	MY_WDOG_soft_reset();
    	while(1);
    }
#endif
#endif
}

#if 0 //for test
static usb_status_t USB_HostHidGenericPrepareOutData(usb_host_hid_generic_instance_t *genericInstance)
{
    uint16_t index = 0;

    if (genericInstance->sendIndex < (sizeof(testData) - 1)) /* sendIndex indicate the current position of testData */
    {
        /* get the max packet data, note: the data should be 0 when there is no actual data to send */
        for (index = 0; ((index + genericInstance->sendIndex) < (sizeof(testData) - 1)) &&
                        (index < genericInstance->outMaxPacketSize);
             ++index)
        {
            genericInstance->genericOutBuffer[index] = testData[index + genericInstance->sendIndex];
        }
        for (; index < genericInstance->outMaxPacketSize; ++index)
        {
            genericInstance->genericOutBuffer[index] = 0x00;
        }
        genericInstance->sendIndex += genericInstance->outMaxPacketSize;

        return kStatus_USB_Success;
    }
    else
    {
        return kStatus_USB_Error; /* there is no data to send */
    }
}
#endif

#if 0 //for test
#define SOH		01
#define EOT		04
#define DLE		16
typedef enum febt_commands
{
    READ_BOOT_INFO = 1,
    ERASE_FLASH,
    PROGRAM_FLASH,
    READ_CRC,
    JMP_TO_APP
} FEBT_COMMANDS_T;

#endif

#if 1 //nsmoon191022
static const uint16_t crc_table[16] =
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

static uint16_t USB_CalculateCrc(uint8_t *data, uint32_t len)
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

static int feBtDataOut(uint8_t *buffIn, uint8_t *buffOut, int bufferSize)
{
	int i;
	size_t BuffLen = 0;
	uint16_t crc;

	if (bufferSize) {
		/* Calculate the CRC of the response*/
		crc = USB_CalculateCrc(buffIn, bufferSize);
		buffIn[bufferSize++] = (uint8_t)crc;
		buffIn[bufferSize++] = (crc >> 8);

		buffOut[BuffLen++] = 0x00; // (*) hid id
		buffOut[BuffLen++] = (uint8_t)SOH;

		for (i = 0; i < bufferSize; i++) {
			if ((buffIn[i] == (uint8_t)EOT) ||
				(buffIn[i] == (uint8_t)SOH) ||
				(buffIn[i] == (uint8_t)DLE)) {
				buffOut[BuffLen++] = (uint8_t)DLE;
			}
			buffOut[BuffLen++] = buffIn[i];
		}
		buffOut[BuffLen++] = (uint8_t)EOT;

#if 0  //for testing
		for (i = 0; i < BuffLen; i++)
		{
			TRACENR("%02x ", buffOut[i]);
			if (((i+1)%16) == 0) TRACENR("\r\n");
		}
		TRACENR("\r\n");
#endif
	}
	return BuffLen;
}

static int feBtDataIn(uint8_t *buffIn, uint8_t *buffOut, int len)
{
	static bool Escape = false;
	uint16_t crc, crc2;
	int i, BuffLen = 0;

	for (i = 0; i < len; i++) {
		// If we were in an Escape sequence, copy the data on and reset the flag.
		if (Escape)
		{
			buffOut[BuffLen++] = buffIn[i];
			Escape = false;
		}
		else
		{
			switch (buffIn[i])
			{
			case (uint8_t)SOH:   // Start of header
				BuffLen = 0;
				break;

			case (uint8_t)EOT:   // End of transmission, Calculate CRC and see if this is valid
				if (BuffLen > 2)
				{
#if 0  //for testing
					for (i = 0; i < BuffLen; i++)
					{
						TRACENR("%02x ", btData.buff2[i]);
						if (((i+1) % 16) == 0) TRACENR("\r\n%02x: ", i);
					}
					TRACENR("\r\n");
#endif
					crc = buffOut[BuffLen - 2];
					crc += ((buffOut[BuffLen - 1]) << 8);
					crc2 = USB_CalculateCrc(buffOut, BuffLen - 2);
					//TRACE("crc %04x crc2 %04x \r\n", crc, crc2);
					if (crc != crc2) {
						TRACE_ERROR("BT_ERROR! invalid crc: %x %x", crc, crc2);
						return 0;
					}
					// CRC matches so frame is valid.
					//usrBufferEventComplete = true;
					return (BuffLen - 2);
				}
				break;

			case (uint8_t)DLE:   // Escape sequence
				Escape = true;
				break;

			default:
				buffOut[BuffLen++] = buffIn[i];
				break;
			} //switch (buffIn[i])
		} //if (Escape)
	} //for (i = 0; i < len; i++)
	return 0; //error
}

#define TRACE_USFBC(...)	TRACE(__VA_ARGS__)
#define USB_OUT_FE_BT_SIZE	(1+1+2) //id+cmd+crc
static usb_status_t USB_SendFeBtCmd(usb_host_hid_generic_instance_t *genericInstance, uint8_t cmd)
{
	usb_status_t status = kStatus_USB_Success;
	int outBufferLen, bufferSize = 0;

	//memset(s_FeBtOutBuffer, 0, genericInstance->outMaxPacketSize);
	s_FeBtOutBuffer[bufferSize++] = cmd;
	outBufferLen = feBtDataOut(s_FeBtOutBuffer, &genericInstance->genericOutBuffer[0], bufferSize);
	if (outBufferLen > 0) {
		TRACE("BT_USB_SendFeBtCmd..0x%x %d", cmd, outBufferLen);
		status = USB_HostHidSend(genericInstance->classHandle, genericInstance->genericOutBuffer,
				                 genericInstance->outMaxPacketSize, USB_HostHidOutCallback, genericInstance);
	    if (status != kStatus_USB_Success)
	    {
	        TRACE_ERROR("BT_ERROR! USB_HostHidSend.1. %d", status);
	        return status;
	    }
#if 0 //for test
        if (USB_HostHidSend(genericInstance->classHandle, genericInstance->genericOutBuffer,
                            genericInstance->outMaxPacketSize, USB_HostHidOutCallback,
                            genericInstance) != kStatus_USB_Success)
        {
        	TRACE_ERROR("BT_ERROR! in USB_HostHidSend");
        }
#endif
	}
	else {
		TRACE("%");
		status = kStatus_USB_Error;
	}
    return status;
}

int USB_HostCmd(int cmd)
{
	if (g_HostHidGeneric.deviceState == kStatus_DEV_Attached)
	{
		if (USB_SendFeBtCmd(&g_HostHidGeneric, (uint8_t)cmd) == kStatus_USB_Success)
		{
			return USB_HOST_OK;
		}
	}
	return USB_HOST_ERR;
}

int USB_HostSendPacket(uint8_t *buff, int len)
{
    //debug_dump_buffer(buff, len);
	if (g_HostHidGeneric.deviceState != kStatus_DEV_Attached)
	{
        TRACE_ERROR("BT_ERROR! not kStatus_DEV_Attached..");
        return USB_HOST_ERR;
	}
    if (len != g_HostHidGeneric.outMaxPacketSize) {
        TRACE_ERROR("BT_ERROR! mismatch outMaxPacketSize..%d %d", len, g_HostHidGeneric.outMaxPacketSize);
        return USB_HOST_ERR;
    }

	memcpy(g_HostHidGeneric.genericOutBuffer, buff, len);
	g_HostHidGeneric.rxDone = 0; //reset

	usb_status_t status = USB_HostHidSend(g_HostHidGeneric.classHandle, g_HostHidGeneric.genericOutBuffer,
			                 g_HostHidGeneric.outMaxPacketSize, USB_HostHidOutCallback, &g_HostHidGeneric);
    if (status != kStatus_USB_Success)
    {
        TRACE_ERROR("BT_ERROR! USB_HostHidSend.2. %d", status);
        return USB_HOST_ERR;
    }
	return USB_HOST_OK;
}

int USB_HostRecvPacket(uint8_t *buff)
{
	if (g_HostHidGeneric.deviceState != kStatus_DEV_Attached)
	{
        TRACE_ERROR("BT_ERROR! not kStatus_DEV_Attached..");
        return USB_HOST_ERR;
	}
	if (g_HostHidGeneric.rxDone == 1) {
		g_HostHidGeneric.rxDone = 0; //reset
		memcpy(buff, g_HostHidGeneric.genericInBuffer, g_HostHidGeneric.inMaxPacketSize);
        //debug_dump_buffer(buff, g_HostHidGeneric.inMaxPacketSize);
		return g_HostHidGeneric.inMaxPacketSize;
	}
    return 0; //error
}
#endif //1

ATTRIBUTE_RAMFUNC
void USB_HostHidGenericTask(void *param)
{
    //usb_status_t status;
    usb_host_hid_descriptor_t *hidDescriptor;
    uint32_t hidReportLength = 0;
    uint8_t *descriptor;
    uint32_t endPosition;
    usb_host_hid_generic_instance_t *genericInstance = (usb_host_hid_generic_instance_t *)param;

    /* device state changes, process once for each state */
    if (genericInstance->deviceState != genericInstance->prevState)
    {
        genericInstance->prevState = genericInstance->deviceState;
        switch (genericInstance->deviceState)
        {
            case kStatus_DEV_Idle:
                break;

            case kStatus_DEV_Attached: /* deivce is attached and numeration is done */
                genericInstance->runState = kUSB_HostHidRunSetInterface;
                if (USB_HostHidInit(genericInstance->deviceHandle, &genericInstance->classHandle) !=
                    kStatus_USB_Success)
                {
                	TRACE_ERROR("BT_ERROR! host hid class initialize fail");
                }
                else
                {
                	TRACE_HOST("BT_hid generic attached");
                }
                genericInstance->sendIndex = 0;
                break;

            case kStatus_DEV_Detached: /* device is detached */
                genericInstance->deviceState = kStatus_DEV_Idle;
                genericInstance->runState = kUSB_HostHidRunIdle;
                USB_HostHidDeinit(genericInstance->deviceHandle, genericInstance->classHandle);
                genericInstance->classHandle = NULL;
                TRACE_HOST("BT_hid generic detached");
                break;

            default:
                break;
        }
    }

    /* run state */
    switch (genericInstance->runState)
    {
        case kUSB_HostHidRunIdle:
            break;

        case kUSB_HostHidRunSetInterface: /* 1. set hid interface */
        {
        	TRACE_HOST("BT_kUSB_HostHidRunSetInterface..");
            genericInstance->runWaitState = kUSB_HostHidRunWaitSetInterface;
            genericInstance->runState = kUSB_HostHidRunIdle;
            if (USB_HostHidSetInterface(genericInstance->classHandle, genericInstance->interfaceHandle, 0,
                                        USB_HostHidControlCallback, genericInstance) != kStatus_USB_Success)
            {
            	TRACE_ERROR("BT_ERROR! set interface error");
            }
        }
            break;

        case kUSB_HostHidRunSetInterfaceDone: /* 2. hid set idle */
        {
        	TRACE_HOST("BT_kUSB_HostHidRunSetInterfaceDone..");
            genericInstance->inMaxPacketSize =
                USB_HostHidGetPacketsize(genericInstance->classHandle, USB_ENDPOINT_INTERRUPT, USB_IN);
            genericInstance->outMaxPacketSize =
                USB_HostHidGetPacketsize(genericInstance->classHandle, USB_ENDPOINT_INTERRUPT, USB_OUT);
            TRACE_HOST("BT_inMaxPacketSize,outMaxPacketSize: %d %d", genericInstance->inMaxPacketSize, genericInstance->outMaxPacketSize);

            /* first: set idle */
            genericInstance->runWaitState = kUSB_HostHidRunWaitSetIdle;
            genericInstance->runState = kUSB_HostHidRunIdle;
            if (USB_HostHidSetIdle(genericInstance->classHandle, 0, 0, USB_HostHidControlCallback, genericInstance) !=
                kStatus_USB_Success)
            {
            	TRACE_ERROR("BT_ERROR! in USB_HostHidSetIdle");
            }
        }
            break;

        case kUSB_HostHidRunSetIdleDone: /* 3. hid get report descriptor */
        {
        	TRACE_HOST("BT_kUSB_HostHidRunSetIdleDone..");
            /* get report descriptor's length */
            hidDescriptor = NULL;
            descriptor = (uint8_t *)((usb_host_interface_t *)genericInstance->interfaceHandle)->interfaceExtension;
            endPosition = (uint32_t)descriptor +
                          ((usb_host_interface_t *)genericInstance->interfaceHandle)->interfaceExtensionLength;

            while ((uint32_t)descriptor < endPosition)
            {
                if (*(descriptor + 1) == USB_DESCRIPTOR_TYPE_HID) /* descriptor type */
                {
                    hidDescriptor = (usb_host_hid_descriptor_t *)descriptor;
                    break;
                }
                else
                {
                    descriptor = (uint8_t *)((uint32_t)descriptor + (*descriptor)); /* next descriptor */
                }
            }

            if (hidDescriptor != NULL)
            {
                usb_host_hid_class_descriptor_t *hidClassDescriptor;
                hidClassDescriptor = (usb_host_hid_class_descriptor_t *)&(hidDescriptor->bHidDescriptorType);
                for (uint8_t index = 0; index < hidDescriptor->bNumDescriptors; ++index)
                {
                    hidClassDescriptor += index;
                    if (hidClassDescriptor->bHidDescriptorType == USB_DESCRIPTOR_TYPE_HID_REPORT)
                    {
                        hidReportLength =
                            (uint16_t)USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(hidClassDescriptor->wDescriptorLength);
                        break;
                    }
                }
            }
            if (hidReportLength > HID_GENERIC_IN_BUFFER_SIZE)
            {
            	TRACE_ERROR("BT_ERROR! hid buffer is too small");
                genericInstance->runState = kUSB_HostHidRunIdle;
                return;
            }

            genericInstance->runWaitState = kUSB_HostHidRunWaitGetReportDescriptor;
            genericInstance->runState = kUSB_HostHidRunIdle;

            /* second: get report descriptor */
            USB_HostHidGetReportDescriptor(genericInstance->classHandle, genericInstance->genericInBuffer,
                                           hidReportLength, USB_HostHidControlCallback, genericInstance);
        }
            break;

#if 0 //for test
        case kUSB_HostHidRunGetReportDescriptorDone: /* 4. hid set protocol */
        {
        	TRACE_HOST("BT_kUSB_HostHidRunGetReportDescriptorDone..");
            genericInstance->runWaitState = kUSB_HostHidRunWaitSetProtocol;
            genericInstance->runState = kUSB_HostHidRunIdle;

            /* third: set protocol */
            if (USB_HostHidSetProtocol(genericInstance->classHandle, USB_HOST_HID_REQUEST_PROTOCOL_REPORT,
                                       USB_HostHidControlCallback, genericInstance) != kStatus_USB_Success)
            {
            	TRACE_ERROR("BT_ERROR! Error in USB_HostHidSetProtocol");
            }
        }
            break;

        case kUSB_HostHidRunSetProtocolDone:
        {
        	TRACE_HOST("BT_kUSB_HostHidRunSetProtocolDone..");
            genericInstance->runWaitState = kUSB_HostHidRunWaitGetProtocol;
            genericInstance->runState = kUSB_HostHidRunIdle;
            uint8_t protocol;

            if (USB_HostHidGetProtocol(genericInstance->classHandle, &protocol,
                                       USB_HostHidControlCallback, genericInstance) != kStatus_USB_Success)
            {
            	TRACE_ERROR("BT_ERROR! Error in USB_HostHidGetProtocol");
            }
            TRACE_HOST("BT_protocol=%x");
        }
            break;

        case kUSB_HostHidRunGetProtocolDone: /* 5. start to receive data and send data */
        {
        	//TRACE_HOST("BT_kUSB_HostHidRunSetProtocolDone..");
            genericInstance->runWaitState = kUSB_HostHidRunWaitDataTransmitted;
            genericInstance->runState = kUSB_HostHidRunIdle;
            if (USB_SendFeBtCmd(genericInstance, (uint8_t)READ_BOOT_INFO) != kStatus_USB_Success)
            {
            	//error! retry~
            	genericInstance->runWaitState = kUSB_HostHidRunDataTransmitting;
            	TRACE_ERROR("BT_ERROR! in USB_SendFeBtCmd");
            	//USB_host_delay(100000);
            }
        }
            break;
#endif //0

        case kUSB_HostHidRunGetReportDescriptorDone:
#if 0 //
        {
            genericInstance->runWaitState = kUSB_HostHidRunWaitDataTransmitted;
            genericInstance->runState = kUSB_HostHidRunIdle;
            if (USB_SendFeBtCmd(genericInstance, (uint8_t)READ_BOOT_INFO) != kStatus_USB_Success)
            {
            	//error! retry~
            	genericInstance->runWaitState = kUSB_HostHidRunWaitGetReportDescriptor;
            	TRACE_ERROR("BT_ERROR! in USB_SendFeBtCmd");
            }
        }
            break;
#endif
        case kUSB_HostHidRunDataTransmitted:
        {
            genericInstance->runWaitState = kUSB_HostHidRunWaitDataReceived;
            genericInstance->runState = kUSB_HostHidRunIdle;
            if (USB_HostHidRecv(genericInstance->classHandle, genericInstance->genericInBuffer,
                                genericInstance->inMaxPacketSize, USB_HostHidInCallback,
                                genericInstance) != kStatus_USB_Success)
            {
            	//error! retry~
                genericInstance->runWaitState = kUSB_HostHidRunWaitDataReceiving;
            	TRACE_ERROR("BT_ERROR! in USB_HostHidRecv");
            	//USB_host_delay(100000);
            }
        }
            break;

        case kUSB_HostHidRunDataReceived: /* process received data, receive next data and send next data */
        {
        	//TRACE("kUSB_HostHidRunDataReceived..");
        	genericInstance->rxDone = 1; //ready to read
            //debug_dump_buffer(genericInstance->genericInBuffer, genericInstance->inMaxPacketSize);
            int len = feBtDataIn(genericInstance->genericInBuffer, &s_FeBtInBuffer[0], genericInstance->inMaxPacketSize);
            if (len < 0x10) {
            	if (s_FeBtInBuffer[1] == BT_OK) {
#ifdef USB_HOST_FOR_FRONTEND
#ifndef FRONTEND_RESET_ENABLE
                	if (s_FeBtInBuffer[0] == MCU_RESET) {
                		TRACE("BT_SOFT_RESET");
                		g_HostHidGeneric.softReset = 1;
                	}
                	else
#endif
#endif
                	if (s_FeBtInBuffer[0] == MODE_CHANGE && s_FeBtInBuffer[2] == BE_BT_MODE) {
                		g_HostHidGeneric.modeChange = 1;
                	}
                    //debug_dump_buffer(s_FeBtInBuffer, len);
            	}
            }
            genericInstance->runState = kUSB_HostHidRunDataTransmitted;
        }
            break;

        case kUSB_HostHidRunPrimeDataReceive: /* receive next data and send next data */
        {
        	TRACE_HOST("BT_kUSB_HostHidRunPrimeDataReceive..");
        	genericInstance->runState = kUSB_HostHidRunDataTransmitted;
#if 0 //for test
            genericInstance->runWaitState = kUSB_HostHidRunWaitDataReceived;
            genericInstance->runState = kUSB_HostHidRunIdle;
            if (USB_HostHidRecv(genericInstance->classHandle, genericInstance->genericInBuffer,
                                genericInstance->inMaxPacketSize, USB_HostHidInCallback,
                                genericInstance) != kStatus_USB_Success)
            {
            	TRACE_ERROR("BT_ERROR! in USB_HostHidRecv");
            }
            status = USB_HostHidGenericPrepareOutData(genericInstance);
            if (status == kStatus_USB_Success)
            {
                if (USB_HostHidSend(genericInstance->classHandle, genericInstance->genericOutBuffer,
                                    genericInstance->outMaxPacketSize, USB_HostHidOutCallback,
                                    genericInstance) != kStatus_USB_Success)
                {
                	TRACE_ERROR("BT_ERROR! in USB_HostHidSend");
                }
            }
#endif //0
        }
            break;

        default:
            break;
    }
}

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
    irqNumber                = usbHOSTEhciIrq[HOST_CONTROLLER_ID - kUSB_ControllerEhci0];
/* USB_HOST_CONFIG_EHCI */

/* Install isr, set priority, and enable IRQ. */
#if defined(__GIC_PRIO_BITS)
    GIC_SetPriority((IRQn_Type)irqNumber, USB_HOST_INTERRUPT_PRIORITY);
#else
    NVIC_SetPriority((IRQn_Type)irqNumber, USB_HOST_INTERRUPT_PRIORITY);
#endif
    EnableIRQ((IRQn_Type)irqNumber);
}

void USB_HostApplicationInit(void)
{
    usb_status_t status = kStatus_USB_Success;

    USB_HostClockInit();

#if ((defined FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT))
    SYSMPU_Enable(SYSMPU, 0);
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

    status = USB_HostInit(HOST_CONTROLLER_ID, &g_HostHandle, USB_HostEvent);
    if (status != kStatus_USB_Success)
    {
    	TRACE_ERROR("BT_ERROR! host init error");
        return;
    }
    USB_HostIsrEnable();

#if 1 //init variables
    g_HostHidGeneric.rxDone = 0; //rx empty
#endif
#ifdef USB_HOST_FOR_FRONTEND
#ifndef FRONTEND_RESET_ENABLE
    g_HostHidGeneric.softReset = 0;
#endif
#endif

    TRACE_HOST("BT_host init done");
}

void USB_HostApplicationDeinit(void)
{
	USB_HostDeinit(g_HostHandle);
	USB_EhciPhyDeinit(HOST_CONTROLLER_ID);
}

ATTRIBUTE_RAMFUNC
void USB_HostTaskFn(void *param)
{
    USB_HostEhciTaskFunction(param);
}

/*!
 * @brief USB isr function.
 */

usb_status_t USB_HostEvent(usb_device_handle deviceHandle,
                                  usb_host_configuration_handle configurationHandle,
                                  uint32_t eventCode)
{
    usb_status_t status = kStatus_USB_Success;

    switch (eventCode)
    {
        case kUSB_HostEventAttach:
            status = USB_HostHidGenericEvent(deviceHandle, configurationHandle, eventCode);
            break;

        case kUSB_HostEventNotSupported:
        	//TRACE_ERROR("BT_ERROR! device not supported.");
        	bootloaderData.device_not_supported = 1;
            break;

        case kUSB_HostEventEnumerationDone:
            status = USB_HostHidGenericEvent(deviceHandle, configurationHandle, eventCode);
            break;

        case kUSB_HostEventDetach:
            status = USB_HostHidGenericEvent(deviceHandle, configurationHandle, eventCode);
            break;

        default:
            break;
    }
    return status;
}

usb_status_t USB_HostHidGenericEvent(usb_device_handle deviceHandle,
                                     usb_host_configuration_handle configurationHandle,
                                     uint32_t eventCode)
{
    uint32_t pid;
    uint32_t vid;
    usb_host_configuration_t *configuration;
    usb_host_interface_t *interface;
    uint32_t infoValue;
    usb_status_t status = kStatus_USB_Success;
    uint8_t interfaceIndex;
    uint8_t id;

    switch (eventCode)
    {
        case kUSB_HostEventAttach:
        	TRACE_HOST("BT_kUSB_HostEventAttach..");
            /* judge whether is configurationHandle supported */
            configuration = (usb_host_configuration_t *)configurationHandle;
            for (interfaceIndex = 0; interfaceIndex < configuration->interfaceCount; ++interfaceIndex)
            {
                interface = &configuration->interfaceList[0];
                id = interface->interfaceDesc->bInterfaceClass;
                if (id != USB_HOST_HID_CLASS_CODE)
                {
                    continue;
                }
                id = interface->interfaceDesc->bInterfaceSubClass;
                if ((id != USB_HOST_HID_SUBCLASS_CODE_NONE) && (id != USB_HOST_HID_SUBCLASS_CODE_BOOT))
                {
                    continue;
                }
                USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDevicePID, &pid);
                USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceVID, &vid);
                if ((pid == FRONTEND_BT_PID_HEX) && (vid == FRONTEND_VID_HEX))
                {
                    if (g_HostHidGeneric.deviceState == kStatus_DEV_Idle)
                    {
                        /* the interface is supported by the application */
                        g_HostHidGeneric.genericInBuffer = s_GenericInBuffer;
                        //g_HostHidGeneric.genericInBufferSav = s_GenericInBufferSav;
                        g_HostHidGeneric.genericOutBuffer = s_GenericOutBuffer;
                        g_HostHidGeneric.deviceHandle = deviceHandle;
                        g_HostHidGeneric.interfaceHandle = interface;
                        g_HostHidGeneric.configHandle = configurationHandle;
                        return kStatus_USB_Success;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            status = kStatus_USB_NotSupported;
            break;

        case kUSB_HostEventNotSupported:
        	TRACE_HOST("BT_kUSB_HostEventNotSupported..");
            break;

        case kUSB_HostEventEnumerationDone:
        	TRACE_HOST("BT_kUSB_HostEventEnumerationDone..");
            if (g_HostHidGeneric.configHandle == configurationHandle)
            {
                if ((g_HostHidGeneric.deviceHandle != NULL) && (g_HostHidGeneric.interfaceHandle != NULL))
                {
                    /* the device enumeration is done */
                    if (g_HostHidGeneric.deviceState == kStatus_DEV_Idle)
                    {
                        g_HostHidGeneric.deviceState = kStatus_DEV_Attached;

                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDevicePID, &infoValue);
                        TRACE_HOST(" .BT_hid generic attached:pid=0x%x", infoValue);
                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceVID, &infoValue);
                        TRACE_HOST(" .BT_vid=0x%x ", infoValue);
                        USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceAddress, &infoValue);
                        TRACE_HOST(" .BT_address=%d", infoValue);
                    }
                    else
                    {
                    	TRACE_ERROR("BT_ERROR! not idle generic instance");
                        status = kStatus_USB_Error;
                    }
                }
            }
            break;

        case kUSB_HostEventDetach:
        	//TRACE_HOST("BT_kUSB_HostEventDetach..");
            if (g_HostHidGeneric.configHandle == configurationHandle)
            {
                /* the device is detached */
                g_HostHidGeneric.configHandle = NULL;
                if (g_HostHidGeneric.deviceState != kStatus_DEV_Idle)
                {
                    g_HostHidGeneric.deviceState = kStatus_DEV_Detached;
                }
            }
            break;

        default:
            break;
    }
    return status;
}

int USB_HostIsReady()
{
    if (g_HostHidGeneric.deviceState == kStatus_DEV_Attached) {
    	return USB_HOST_OK;
    }
    else {
    	return USB_HOST_DETACH;
    }
}
/* end of file */
