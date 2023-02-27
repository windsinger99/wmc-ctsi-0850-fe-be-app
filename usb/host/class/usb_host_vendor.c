/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_host_config.h"
#if ((defined USB_HOST_CONFIG_VENDOR) && (USB_HOST_CONFIG_VENDOR))
#include "usb_host.h"
#include "usb_host_vendor.h"

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief vendor in pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVendorInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief vendor out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVendorOutPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief vendor control pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVendorControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief vendor open interface. It is called when set interface request success or open alternate setting 0 interface.
 *
 * @param vendorInstance     vendor instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostVendorOpenInterface(usb_host_vendor_instance_t *vendorInstance);

/*!
 * @brief vendor set interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostVendorSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief vendor send control transfer common code.
 *
 * @param classHandle    the class handle.
 * @param requestType    setup packet request type.
 * @param request        setup packet request value.
 * @param wvalueL        setup packet wvalue low byte.
 * @param wvalueH        setup packet wvalue high byte.
 * @param wlength        setup packet wlength value.
 * @param data           data buffer pointer
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @return An error code or kStatus_USB_Success.
 */
static usb_status_t USB_HostVendorControl(usb_host_class_handle classHandle,
                                       uint8_t requestType,
                                       uint8_t request,
                                       uint8_t wvalueL,
                                       uint8_t wvalueH,
                                       uint16_t wlength,
                                       uint8_t *data,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam);

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/
/*******************************************************************************
 * Code
 ******************************************************************************/

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)

static void USB_HostVendorClearInHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)param;

    vendorInstance->controlTransfer = NULL;
    if (vendorInstance->inCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostVendorRecv */
        vendorInstance->inCallbackFn(vendorInstance->inCallbackParam, vendorInstance->stallDataBuffer,
                                  vendorInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
}

static void USB_HostVendorClearOutHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)param;

    vendorInstance->controlTransfer = NULL;
    if (vendorInstance->outCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in USB_HostVendorSend */
        vendorInstance->outCallbackFn(vendorInstance->outCallbackParam, vendorInstance->stallDataBuffer,
                                   vendorInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
}

static usb_status_t USB_HostVendorClearHalt(usb_host_vendor_instance_t *vendorInstance,
                                         usb_host_transfer_t *stallTransfer,
                                         host_inner_transfer_callback_t callbackFn,
                                         uint8_t endpoint)
{
    usb_status_t status;
    usb_host_transfer_t *transfer;

    /* malloc one transfer */
    status = USB_HostMallocTransfer(vendorInstance->hostHandle, &transfer);
    if (status != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("allocate transfer error\r\n");
#endif
        return status;
    }
    vendorInstance->stallDataBuffer = stallTransfer->transferBuffer;
    vendorInstance->stallDataLength = stallTransfer->transferSofar;
    /* save the application callback function */
    vendorInstance->controlCallbackFn = NULL;
    vendorInstance->controlCallbackParam = NULL;
    /* initialize transfer */
    transfer->callbackFn = callbackFn;
    transfer->callbackParam = vendorInstance;
    transfer->transferBuffer = NULL;
    transfer->transferLength = 0;
    transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_CLEAR_FEATURE;
    transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_ENDPOINT;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT);
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(endpoint);
    transfer->setupPacket->wLength = 0;
    status = USB_HostSendSetup(vendorInstance->hostHandle, vendorInstance->controlPipe, transfer);

    if (status != kStatus_USB_Success)
    {
        USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
    }
    vendorInstance->controlTransfer = transfer;

    return status;
}
#endif

static void USB_HostVendorInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)param;

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostVendorClearHalt(vendorInstance, transfer, USB_HostVendorClearInHaltCallback,
                                 (USB_REQUEST_TYPE_DIR_IN |
                                  ((usb_host_pipe_t *)vendorInstance->inPipe)->endpointAddress)) == kStatus_USB_Success)
        {
            USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
            return;
        }
    }
#endif
    if (vendorInstance->inCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostVendorRecv */
        vendorInstance->inCallbackFn(vendorInstance->inCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                  status);
    }
    USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
}

static void USB_HostVendorOutPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)param;

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostVendorClearHalt(vendorInstance, transfer, USB_HostVendorClearOutHaltCallback,
                                 (USB_REQUEST_TYPE_DIR_OUT |
                                  ((usb_host_pipe_t *)vendorInstance->outPipe)->endpointAddress)) == kStatus_USB_Success)
        {
            USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
            return;
        }
    }
#endif
    if (vendorInstance->outCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in USB_HostVendorSend */
        vendorInstance->outCallbackFn(vendorInstance->outCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                   status); /* callback to application */
    }
    USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
}

static void USB_HostVendorControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)param;

    vendorInstance->controlTransfer = NULL;
    if (vendorInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostVendorSetInterface
        or USB_HostVendorControl, but is the same function */
        vendorInstance->controlCallbackFn(vendorInstance->controlCallbackParam, transfer->transferBuffer,
                                       transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
}

static usb_status_t USB_HostVendorOpenInterface(usb_host_vendor_instance_t *vendorInstance)
{
    usb_status_t status;
    uint8_t epIndex = 0;
    usb_host_pipe_init_t pipeInit;
    usb_descriptor_endpoint_t *epDesc = NULL;
    usb_host_interface_t *interfacePointer;

    if (vendorInstance->inPipe != NULL) /* close interrupt in pipe if it is open */
    {
        status = USB_HostClosePipe(vendorInstance->hostHandle, vendorInstance->inPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        vendorInstance->inPipe = NULL;
    }
    if (vendorInstance->outPipe != NULL) /* close interrupt out pipe if it is open */
    {
        status = USB_HostClosePipe(vendorInstance->hostHandle, vendorInstance->outPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        vendorInstance->outPipe = NULL;
    }

    /* open interface pipes */
    interfacePointer = (usb_host_interface_t *)vendorInstance->interfaceHandle;
#ifdef HOST_ECHO
    usb_echo("interfacePointer->epCount: %d\r\n", interfacePointer->epCount);
#endif

    for (epIndex = 0; epIndex < interfacePointer->epCount; ++epIndex)
    {
        epDesc = interfacePointer->epList[epIndex].epDesc;
#ifdef HOST_ECHO
        usb_echo("epDesc->bEndpointAddress,bmAttributes: %x %x\r\n", epDesc->bEndpointAddress, epDesc->bmAttributes);
#endif
        if (((epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((epDesc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_BULK))
        {
            pipeInit.devInstance = vendorInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_BULK;
            pipeInit.direction = USB_IN;
            pipeInit.endpointAddress = (epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = epDesc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;

            vendorInstance->inPacketSize = pipeInit.maxPacketSize;
#ifdef HOST_ECHO
            usb_echo("vendorInstance->inPacketSize: %d\r\n", vendorInstance->inPacketSize);
#endif

            status = USB_HostOpenPipe(vendorInstance->hostHandle, &vendorInstance->inPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostVendorSetInterface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
        else if (((epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                 ((epDesc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_BULK))
        {
            pipeInit.devInstance = vendorInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_BULK;
            pipeInit.direction = USB_OUT;
            pipeInit.endpointAddress = (epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = epDesc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;

            vendorInstance->outPacketSize = pipeInit.maxPacketSize;
#ifdef HOST_ECHO
            usb_echo("vendorInstance->outPacketSize: %d\r\n", vendorInstance->outPacketSize);
#endif

            status = USB_HostOpenPipe(vendorInstance->hostHandle, &vendorInstance->outPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostVendorSetInterface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
        else
        {
#ifdef HOST_ECHO
            usb_echo("ERROR!: invalid bEndpointAddress\r\n");
#endif
        }
    }

    return kStatus_USB_Success;
}

static void USB_HostVendorSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)param;

    vendorInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = USB_HostVendorOpenInterface(vendorInstance); /* vendor open interface */
    }

    if (vendorInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostVendorSetInterface
        or USB_HostVendorControl, but is the same function */
        vendorInstance->controlCallbackFn(vendorInstance->controlCallbackParam, NULL, 0, status);
    }
    USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
}

usb_status_t USB_HostVendorInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle)
{
    uint32_t infoValue;
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)USB_OsaMemoryAllocate(
        sizeof(usb_host_vendor_instance_t)); /* malloc vendor class instance */

    if (vendorInstance == NULL)
    {
        return kStatus_USB_AllocFail;
    }

    /* initialize vendor instance */
    vendorInstance->deviceHandle = deviceHandle;
    vendorInstance->interfaceHandle = NULL;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetHostHandle, &infoValue);
    vendorInstance->hostHandle = (usb_host_handle)infoValue;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceControlPipe, &infoValue);
    vendorInstance->controlPipe = (usb_host_pipe_handle)infoValue;

    *classHandle = vendorInstance;
    return kStatus_USB_Success;
}

usb_status_t USB_HostVendorSetInterface(usb_host_class_handle classHandle,
                                     usb_host_interface_handle interfaceHandle,
                                     uint8_t alternateSetting,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam)
{
    usb_status_t status;
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
#ifdef HOST_ECHO
        usb_echo("ERROR! classHandle is NULL\r\n");
#endif
        return kStatus_USB_InvalidParameter;
    }

    vendorInstance->interfaceHandle = interfaceHandle;
    status = USB_HostOpenDeviceInterface(vendorInstance->deviceHandle,
                                         interfaceHandle); /* notify host driver the interface is open */
    if (status != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("ERROR! USB_HostOpenDeviceInterface %d\r\n", status);
#endif
        return status;
    }

    /* cancel transfers */
    if (vendorInstance->inPipe != NULL)
    {
        status = USB_HostCancelTransfer(vendorInstance->hostHandle, vendorInstance->inPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe %d\r\n", status);
#endif
        }
    }
    if (vendorInstance->outPipe != NULL)
    {
        status = USB_HostCancelTransfer(vendorInstance->hostHandle, vendorInstance->outPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe %d\r\n", status);
#endif
        }
    }

    if (alternateSetting == 0) /* open interface directly */
    {
        if (callbackFn != NULL)
        {
            status = USB_HostVendorOpenInterface(vendorInstance);
            callbackFn(callbackParam, NULL, 0, status);
        }
    }
    else /* send setup transfer */
    {
        /* malloc one transfer */
        if ((status = USB_HostMallocTransfer(vendorInstance->hostHandle, &transfer)) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer %d\r\n", status);
#endif
            return kStatus_USB_Error;
        }

        /* save the application callback function */
        vendorInstance->controlCallbackFn = callbackFn;
        vendorInstance->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn = USB_HostVendorSetInterfaceCallback;
        transfer->callbackParam = vendorInstance;
        transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)vendorInstance->interfaceHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer = NULL;
        transfer->transferLength = 0;
        status = USB_HostSendSetup(vendorInstance->hostHandle, vendorInstance->controlPipe, transfer);
#ifdef HOST_ECHO
        usb_echo("status2: %d \r\n", status);
#endif
        if (status == kStatus_USB_Success)
        {
            vendorInstance->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
        }
    }

    return status;
}

usb_status_t USB_HostVendorDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle)
{
    usb_status_t status;
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)classHandle;

    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (classHandle != NULL) /* class instance has initialized */
    {
        if (vendorInstance->inPipe != NULL)
        {
            status = USB_HostCancelTransfer(vendorInstance->hostHandle, vendorInstance->inPipe, NULL); /* cancel pipe */
            status = USB_HostClosePipe(vendorInstance->hostHandle, vendorInstance->inPipe);            /* close pipe */

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            vendorInstance->inPipe = NULL;
        }
        if (vendorInstance->outPipe != NULL)
        {
            status = USB_HostCancelTransfer(vendorInstance->hostHandle, vendorInstance->outPipe, NULL); /* cancel pipe */
            status = USB_HostClosePipe(vendorInstance->hostHandle, vendorInstance->outPipe);            /* close pipe */

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            vendorInstance->outPipe = NULL;
        }
        if ((vendorInstance->controlPipe != NULL) &&
            (vendorInstance->controlTransfer != NULL)) /* cancel control transfer if there is on-going control transfer */
        {
            status =
                USB_HostCancelTransfer(vendorInstance->hostHandle, vendorInstance->controlPipe, vendorInstance->controlTransfer);
        }
        USB_HostCloseDeviceInterface(deviceHandle,
                                     vendorInstance->interfaceHandle); /* notify host driver the interface is closed */
        USB_OsaMemoryFree(vendorInstance);
    }
    else
    {
        USB_HostCloseDeviceInterface(deviceHandle, NULL);
    }

    return kStatus_USB_Success;
}

uint16_t USB_HostVendorGetPacketsize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction)
{
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)classHandle;
    if (classHandle == NULL)
    {
        return 0;
    }

    if (pipeType == USB_ENDPOINT_BULK)
    {
        if (direction == USB_IN)
        {
            return vendorInstance->inPacketSize;
        }
        else
        {
            return vendorInstance->outPacketSize;
        }
    }

    return 0;
}

usb_status_t USB_HostVendorRecv(usb_host_class_handle classHandle,
                             uint8_t *buffer,
                             uint32_t bufferLength,
                             transfer_callback_t callbackFn,
                             void *callbackParam)
{
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (vendorInstance->inPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    /* malloc one transfer */
    if (USB_HostMallocTransfer(vendorInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Busy;
    }
    /* save the application callback function */
    vendorInstance->inCallbackFn = callbackFn;
    vendorInstance->inCallbackParam = callbackParam;
    /* initialize transfer */
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = USB_HostVendorInPipeCallback;
    transfer->callbackParam = vendorInstance;

    if (USB_HostRecv(vendorInstance->hostHandle, vendorInstance->inPipe, transfer) !=
        kStatus_USB_Success) /* call host driver api */
    {
#ifdef HOST_ECHO
        usb_echo("failed to USB_HostRecv\r\n");
#endif
        USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

usb_status_t USB_HostVendorSend(usb_host_class_handle classHandle,
                             uint8_t *buffer,
                             uint32_t bufferLength,
                             transfer_callback_t callbackFn,
                             void *callbackParam)
{
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (vendorInstance->outPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    /* malloc one transfer */
    if (USB_HostMallocTransfer(vendorInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    /* save the application callback function */
    vendorInstance->outCallbackFn = callbackFn;
    vendorInstance->outCallbackParam = callbackParam;
    /* initialize transfer */
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = USB_HostVendorOutPipeCallback;
    transfer->callbackParam = vendorInstance;

    if (USB_HostSend(vendorInstance->hostHandle, vendorInstance->outPipe, transfer) !=
        kStatus_USB_Success) /* call host driver api */
    {
#ifdef HOST_ECHO
        usb_echo("failed to USB_HostSend\r\n");
#endif
        USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

static usb_status_t USB_HostVendorControl(usb_host_class_handle classHandle,
                                       uint8_t requestType,
                                       uint8_t request,
                                       uint8_t wvalueL,
                                       uint8_t wvalueH,
                                       uint16_t wlength,
                                       uint8_t *data,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam)
{
    usb_host_vendor_instance_t *vendorInstance = (usb_host_vendor_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    /* malloc one transfer */
    if (USB_HostMallocTransfer(vendorInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Busy;
    }
    /* save the application callback function */
    vendorInstance->controlCallbackFn = callbackFn;
    vendorInstance->controlCallbackParam = callbackParam;
    /* initialize transfer */
    transfer->transferBuffer = data;
    transfer->transferLength = wlength;
    transfer->callbackFn = USB_HostVendorControlCallback;
    transfer->callbackParam = vendorInstance;
    transfer->setupPacket->bmRequestType = requestType;
    transfer->setupPacket->bRequest = request;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(wvalueL | (uint16_t)((uint16_t)wvalueH << 8));
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
        ((usb_host_interface_t *)vendorInstance->interfaceHandle)->interfaceDesc->bInterfaceNumber);
    transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(wlength);

    if (USB_HostSendSetup(vendorInstance->hostHandle, vendorInstance->controlPipe, transfer) !=
        kStatus_USB_Success) /* call host driver api */
    {
#ifdef HOST_ECHO
        usb_echo("failed for USB_HostSendSetup\r\n");
#endif
        USB_HostFreeTransfer(vendorInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    vendorInstance->controlTransfer = transfer;

    return kStatus_USB_Success;
}

usb_status_t USB_HostVendorGetReportDescriptor(usb_host_class_handle classHandle,
                                            uint8_t *buffer,
                                            uint16_t buffer_len,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam)
{
    return USB_HostVendorControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_STANDARD | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_REQUEST_STANDARD_GET_DESCRIPTOR, 0, USB_DESCRIPTOR_TYPE_VENDOR_REPORT, buffer_len, buffer, callbackFn,
        callbackParam);
}

usb_status_t USB_HostVendorGetIdle(usb_host_class_handle classHandle,
                                uint8_t reportId,
                                uint8_t *idleRate,
                                transfer_callback_t callbackFn,
                                void *callbackParam)
{
    return USB_HostVendorControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_VENDOR_GET_IDLE, reportId, 0, 1, idleRate, callbackFn, callbackParam);
}

usb_status_t USB_HostVendorSetIdle(usb_host_class_handle classHandle,
                                uint8_t reportId,
                                uint8_t idleRate,
                                transfer_callback_t callbackFn,
                                void *callbackParam)
{
    return USB_HostVendorControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_VENDOR_SET_IDLE, reportId, idleRate, 0, NULL, callbackFn, callbackParam);
}

usb_status_t USB_HostVendorGetProtocol(usb_host_class_handle classHandle,
                                    uint8_t *protocol,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam)
{
    return USB_HostVendorControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_VENDOR_GET_PROTOCOL, 0, 0, 1, protocol, callbackFn, callbackParam);
}

usb_status_t USB_HostVendorSetProtocol(usb_host_class_handle classHandle,
                                    uint8_t protocol,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam)
{
    return USB_HostVendorControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_VENDOR_SET_PROTOCOL, protocol, 0, 0, NULL, callbackFn, callbackParam);
}

usb_status_t USB_HostVendorGetReport(usb_host_class_handle classHandle,
                                  uint8_t reportId,
                                  uint8_t reportType,
                                  uint8_t *buffer,
                                  uint32_t bufferLength,
                                  transfer_callback_t callbackFn,
                                  void *callbackParam)
{
    return USB_HostVendorControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_VENDOR_GET_REPORT, reportId, reportType, bufferLength, buffer, callbackFn, callbackParam);
}

usb_status_t USB_HostVendorSetReport(usb_host_class_handle classHandle,
                                  uint8_t reportId,
                                  uint8_t reportType,
                                  uint8_t *buffer,
                                  uint32_t bufferLength,
                                  transfer_callback_t callbackFn,
                                  void *callbackParam)
{
    return USB_HostVendorControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_VENDOR_SET_REPORT, reportId, reportType, bufferLength, buffer, callbackFn, callbackParam);
}

#endif /* USB_HOST_CONFIG_VENDOR */
