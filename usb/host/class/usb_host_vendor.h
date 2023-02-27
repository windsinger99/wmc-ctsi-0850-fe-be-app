/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USB_HOST_VENDOR_H_
#define _USB_HOST_VENDOR_H_

/*******************************************************************************
 * VENDOR class public structure, enumerations, macros, functions
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @addtogroup usb_host_vendor_drv
 * @{
 */

/*! @brief VENDOR class-specific request (get report) */
#define USB_HOST_VENDOR_GET_REPORT (0x01U)
/*! @brief VENDOR class-specific request (get idle) */
#define USB_HOST_VENDOR_GET_IDLE (0x02U)
/*! @brief VENDOR class-specific request (get protocol) */
#define USB_HOST_VENDOR_GET_PROTOCOL (0x03U)
/*! @brief VENDOR class-specific request (set report) */
#define USB_HOST_VENDOR_SET_REPORT (0x09U)
/*! @brief VENDOR class-specific request (set idle) */
#define USB_HOST_VENDOR_SET_IDLE (0x0AU)
/*! @brief VENDOR class-specific request (set protocol) */
#define USB_HOST_VENDOR_SET_PROTOCOL (0x0BU)

/*! @brief VENDOR class code */
#define USB_HOST_VENDOR_CLASS_CODE (0xFFU) //Vendor Specific
/*! @brief VENDOR sub-class code */
#define USB_HOST_VENDOR_SUBCLASS_CODE (0xFFU)

/*! @brief VENDOR get/set protocol request data code */
#define USB_HOST_VENDOR_REQUEST_PROTOCOL_BOOT (0U)
/*! @brief VENDOR get/set protocol request data code */
#define USB_HOST_VENDOR_REQUEST_PROTOCOL_REPORT (1U)

/*! @brief VENDOR instance structure and VENDOR usb_host_class_handle pointer to this structure */
typedef struct _usb_host_vendor_instance
{
    usb_host_handle hostHandle;                /*!< This instance's related host handle*/
    usb_device_handle deviceHandle;            /*!< This instance's related device handle*/
    usb_host_interface_handle interfaceHandle; /*!< This instance's related interface handle*/
    usb_host_pipe_handle controlPipe;          /*!< This instance's related device control pipe*/
    usb_host_pipe_handle inPipe;               /*!< VENDOR interrupt in pipe*/
    usb_host_pipe_handle outPipe;              /*!< VENDOR interrupt out pipe*/
    transfer_callback_t inCallbackFn;          /*!< VENDOR interrupt in transfer callback function pointer*/
    void *inCallbackParam;                     /*!< VENDOR interrupt in transfer callback parameter*/
    transfer_callback_t outCallbackFn;         /*!< VENDOR interrupt out transfer callback function pointer*/
    void *outCallbackParam;                    /*!< VENDOR interrupt out transfer callback parameter*/
    transfer_callback_t controlCallbackFn;     /*!< VENDOR control transfer callback function pointer*/
    void *controlCallbackParam;                /*!< VENDOR control transfer callback parameter*/
    usb_host_transfer_t *controlTransfer;      /*!< Ongoing control transfer*/
#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    uint8_t *stallDataBuffer; /*!< keep the data buffer for stall transfer's data*/
    uint32_t stallDataLength; /*!< keep the data length for stall transfer's data*/
#endif

    uint16_t inPacketSize;  /*!< VENDOR interrupt in maximum packet size*/
    uint16_t outPacketSize; /*!< VENDOR interrupt out maximum packet size*/
} usb_host_vendor_instance_t;

/*! @brief VENDOR descriptor structure according to the 6.2.1 in VENDOR specification */
typedef struct _usb_host_vendor_descriptor
{
    uint8_t bLength;              /*!< Total size of the VENDOR descriptor*/
    uint8_t bDescriptorType;      /*!< Constant name specifying type of VENDOR descriptor*/
    uint8_t bcdVENDOR[2];            /*!< Numeric expression identifying the VENDOR Class Specification release*/
    uint8_t bCountryCode;         /*!< Numeric expression identifying country code of the localized hardware*/
    uint8_t bNumDescriptors;      /*!< Numeric expression specifying the number of class descriptors*/
    uint8_t bVendorDescriptorType;   /*!< Constant name identifying type of class descriptor*/
    uint8_t wDescriptorLength[2]; /*!< Numeric expression that is the total size of the Report descriptor*/
} usb_host_vendor_descriptor_t;

/*! @brief VENDOR descriptor structure according to the 6.2.1 in VENDOR specification */
typedef struct _usb_host_vendor_class_descriptor
{
    uint8_t bVendorDescriptorType;   /*!< Constant name specifying type of optional descriptor*/
    uint8_t wDescriptorLength[2]; /*!< Numeric expression that is the total size of the optional descriptor*/
} usb_host_vendor_class_descriptor_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @name USB host VENDOR class APIs
 * @{
 */

/*!
 * @brief Initializes the VENDOR instance.
 *
 * This function allocate the resource for the VENDOR instance.
 *
 * @param[in] deviceHandle   The device handle.
 * @param[out] classHandle   Return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
extern usb_status_t USB_HostVendorInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle);

/*!
 * @brief Sets the interface.
 *
 * This function binds the interface with the VENDOR instance.
 *
 * @param[in] classHandle      The class handle.
 * @param[in] interfaceHandle  The interface handle.
 * @param[in] alternateSetting The alternate setting value.
 * @param[in] callbackFn       This callback is called after this function completes.
 * @param[in] callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 * @retval kStatus_USB_Busy           Callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  Callback return status, the transfer is stalled by the device.
 * @retval kStatus_USB_Error          Callback return status, open pipe fail. See the USB_HostOpenPipe.
 */
extern usb_status_t USB_HostVendorSetInterface(usb_host_class_handle classHandle,
                                            usb_host_interface_handle interfaceHandle,
                                            uint8_t alternateSetting,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam);

/*!
 * @brief Deinitializes the VENDOR instance.
 *
 * This function frees the resources for the VENDOR instance.
 *
 * @param[in] deviceHandle   The device handle.
 * @param[in] classHandle    The class handle.
 *
 * @retval kStatus_USB_Success        The device is de-initialized successfully.
 */
extern usb_status_t USB_HostVendorDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle);

/*!
 * @brief Gets the pipe maximum packet size.
 *
 * @param[in] classHandle The class handle.
 * @param[in] pipeType    Its value is USB_ENDPOINT_CONTROL, USB_ENDPOINT_ISOCHRONOUS, USB_ENDPOINT_BULK or
 * USB_ENDPOINT_INTERRUPT.
 *                        See the usb_spec.h
 * @param[in] direction   Pipe direction.
 *
 * @retval 0        The classHandle is NULL.
 * @retval          Maximum packet size.
 */
extern uint16_t USB_HostVendorGetPacketsize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction);

/*!
 * @brief VENDOR get report descriptor.
 *
 * This function implements the VENDOR report descriptor request.
 *
 * @param[in] classHandle   The class handle.
 * @param[out] buffer        The buffer pointer.
 * @param[in] buffer_len     The buffer length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostVendorGetReportDescriptor(usb_host_class_handle classHandle,
                                                   uint8_t *buffer,
                                                   uint16_t buffer_len,
                                                   transfer_callback_t callbackFn,
                                                   void *callbackParam);

/*!
 * @brief Receives data.
 *
 * This function implements the VENDOR receiving data.
 *
 * @param[in] classHandle   The class handle.
 * @param[out] buffer       The buffer pointer.
 * @param[in] bufferLength  The buffer length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostRecv.
 */
extern usb_status_t USB_HostVendorRecv(usb_host_class_handle classHandle,
                                    uint8_t *buffer,
                                    uint32_t bufferLength,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam);

/*!
 * @brief Sends data.
 *
 * This function implements the VENDOR sending data.
 *
 * @param[in] classHandle   The class handle.
 * @param[in] buffer        The buffer pointer.
 * @param[in] bufferLength  The buffer length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Send request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSend.
 */
extern usb_status_t USB_HostVendorSend(usb_host_class_handle classHandle,
                                    uint8_t *buffer,
                                    uint32_t bufferLength,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam);

/*!
 * @brief VENDOR get idle.
 *
 * This function implements the VENDOR class-specific request (get idle).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] reportId      Report ID.
 * @param[out] idleRate     Return idle rate value.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostVendorGetIdle(usb_host_class_handle classHandle,
                                       uint8_t reportId,
                                       uint8_t *idleRate,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam);

/*!
 * @brief VENDOR set idle.
 *
 * This function implements the VENDOR class-specific request (set idle).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] reportId      Report ID.
 * @param[in] idleRate      Idle rate value.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostVendorSetIdle(usb_host_class_handle classHandle,
                                       uint8_t reportId,
                                       uint8_t idleRate,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam);

/*!
 * @brief VENDOR get protocol.
 *
 * This function implements the VENDOR class-specific request (get protocol).
 *
 * @param[in] classHandle   The class handle.
 * @param[out] protocol      Return protocol value.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostVendorGetProtocol(usb_host_class_handle classHandle,
                                           uint8_t *protocol,
                                           transfer_callback_t callbackFn,
                                           void *callbackParam);

/*!
 * @brief VENDOR set protocol.
 *
 * This function implements the VENDOR class-specific request (set protocol).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] protocol      Protocol value.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostVendorSetProtocol(usb_host_class_handle classHandle,
                                           uint8_t protocol,
                                           transfer_callback_t callbackFn,
                                           void *callbackParam);

/*!
 * @brief VENDOR get report.
 *
 * This function implements the VENDOR class-specific request (get report).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] reportId      Report ID.
 * @param[in] reportType    Report type.
 * @param[out] buffer       The buffer pointer.
 * @param[in] bufferLength  The buffer length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostVendorGetReport(usb_host_class_handle classHandle,
                                         uint8_t reportId,
                                         uint8_t reportType,
                                         uint8_t *buffer,
                                         uint32_t bufferLength,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam);

/*!
 * @brief VENDOR set report.
 *
 * This function implements the VENDOR class-specific request (set report).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] reportId      Report ID.
 * @param[in] reportType    Report type.
 * @param[in] buffer        The buffer pointer.
 * @param[in] bufferLength  The buffer length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostVendorSetReport(usb_host_class_handle classHandle,
                                         uint8_t reportId,
                                         uint8_t reportType,
                                         uint8_t *buffer,
                                         uint32_t bufferLength,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam);

/*! @}*/

#ifdef __cplusplus
}
#endif

/*! @}*/

#endif /* _USB_HOST_VENDOR_H_ */
