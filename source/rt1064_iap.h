/*
 * rt1064_iap.h
 *
 *  Created on: May 14, 2019
 *      Author: user
 */

#ifndef RT1064_IAP_H_
#define RT1064_IAP_H_

#include "evkmimxrt1064_flexspi_nor_config.h" // Contains definition of flexspi_nor_config_t etc
#include "fsl_rtwdog.h"
#include "fsl_wdog.h"

#define g_bootloaderTree	(*(bootloader_api_entry_t**)(0x0020001c))

//RTWDOG driver API structure
typedef struct {
     void (*RTWDOG_GetDefaultConfig)(rtwdog_config_t *config);
     void (*RTWDOG_Init)(RTWDOG_Type *base, const rtwdog_config_t *config);
     void (*RTWDOG_Deinit)(RTWDOG_Type *base);
     void (*RTWDOG_Enable)(RTWDOG_Type *base);
     void (*RTWDOG_Disable)(RTWDOG_Type *base);
     void (*RTWDOG_EnableInterrupts)(RTWDOG_Type *base, uint32_t mask);
     void (*RTWDOG_DisableInterrupts)(RTWDOG_Type *base, uint32_t mask);
     uint32_t (*RTWDOG_GetStatusFlags)(RTWDOG_Type *base);
     void (*RTWDOG_ClearStatusFlags)(RTWDOG_Type *base, uint32_t mask);
     void (*RTWDOG_SetTimeoutValue)(RTWDOG_Type *base, uint16_t timeoutCount);
     void (*RTWDOG_SetWindowValue)(RTWDOG_Type *base, uint16_t windowValue);
     void (*RTWDOG_Unlock)(RTWDOG_Type *base);
     void (*RTWDOG_Refresh)(RTWDOG_Type *base);
     uint16_t (*RTWDOG_GetCounterValue)(RTWDOG_Type *base);
} rtwdog_driver_interface_t;

//WDOG driver API structure
typedef struct {
     void (*WDOG_GetDefaultConfig)(wdog_config_t *config);
     void (*WDOG_Init)(WDOG_Type *base, const wdog_config_t *config);
     void (*WDOG_Deinit)(WDOG_Type *base);
     void (*WDOG_Enable)(WDOG_Type *base);
     void (*WDOG_Disable)(WDOG_Type *base);
     void (*WDOG_EnableInterrupts)(WDOG_Type *base, uint16_t mask);
     uint16_t (*WDOG_GetStatusFlags)(WDOG_Type *base);
     void (*WDOG_ClearInterruptStatus)(WDOG_Type *base, uint16_t mask);
     void (*WDOG_SetTimeoutValue)(WDOG_Type *base, uint16_t timeoutCount);
     void (*WDOG_SetInterrputTimeoutValue)(WDOG_Type *base, uint16_t timeoutCount);
     void (*WDOG_DisablePowerDownEnable)(WDOG_Type *base);
     void (*WDOG_Refresh)(WDOG_Type *base);
} wdog_driver_interface_t;

//serial_nor_config_t definition
typedef struct _serial_nor_config_option {
     union {
          struct {
               uint32_t max_freq : 4; //!< Maximum supported Frequency
               uint32_t misc_mode : 4; //!< miscellaneous mode
               uint32_t quad_mode_setting : 4; //!< Quad mode setting
               uint32_t cmd_pads : 4; //!< Command pads
               uint32_t query_pads : 4; //!< SFDP read pads
               uint32_t device_type : 4; //!< Device type
               uint32_t option_size : 4; //!< Option size, in terms of uint32_t, size = (option_size + 1) * 4
               uint32_t tag : 4; //!< Tag, must be 0x0E
          } B;
          uint32_t U;
     } option0;
     union {
          struct {
               uint32_t dummy_cycles : 8; //!< Dummy cycles before read
               uint32_t reserved0 : 8; //!< Reserved for future use
               uint32_t pinmux_group : 4; //!< The pinmux group selection
               uint32_t reserved1 : 8; //!< Reserved for future use
               uint32_t flash_connection : 4; //!< Flash connection option: 0 - Single Flash connected to port A
          } B;
          uint32_t U;
     } option1;
} serial_nor_config_option_t;

//flexspi_operation_t definition
typedef enum _FlexSPIOperationType {
     kFlexSpiOperation_Command, //!< FlexSPI operation: Only command, both TX and RX buffer are ignored.
     kFlexSpiOperation_Config, //!< FlexSPI operation: Configure device mode, the TX FIFO size is fixed in LUT.
     kFlexSpiOperation_Write, //!< FlexSPI operation: Write, only TX buffer is effective
     kFlexSpiOperation_Read, //!< FlexSPI operation: Read, only Rx Buffer is effective.
     kFlexSpiOperation_End = kFlexSpiOperation_Read,
} flexspi_operation_t;

//!@brief FlexSPI Transfer Context
typedef struct _FlexSpiXfer {
     flexspi_operation_t operation; //!< FlexSPI operation
     uint32_t baseAddress; //!< FlexSPI operation base address
     uint32_t seqId; //!< Sequence Id
     uint32_t seqNum; //!< Sequence Number
     bool isParallelModeEnable; //!< Is a parallel transfer
     uint32_t *txBuffer; //!< Tx buffer
     uint32_t txSize; //!< Tx size in bytes
     uint32_t *rxBuffer; //!< Rx buffer
     uint32_t rxSize; //!< Rx size in bytes
} flexspi_xfer_t;

//FlexSPI NOR Driver API structure
typedef struct {
     uint32_t version;
     status_t (*init)(uint32_t instance, flexspi_nor_config_t *config);
     status_t (*program)(uint32_t instance, flexspi_nor_config_t *config, uint32_t dst_addr, const uint32_t *src);
     status_t (*erase_all)(uint32_t instance, flexspi_nor_config_t *config);
     status_t (*erase)(uint32_t instance, flexspi_nor_config_t *config, uint32_t start, uint32_t lengthInBytes);
     status_t (*read)(uint32_t instance, flexspi_nor_config_t *config, uint32_t *dst, uint32_t addr, uint32_t lengthInBytes);
     void (*clear_cache)(uint32_t instance);
     status_t (*xfer)(uint32_t instance, flexspi_xfer_t *xfer);
     status_t (*update_lut)(uint32_t instance, uint32_t seqIndex, const uint32_t *lutBase, uint32_t seqNumber);
     status_t (*get_config)(uint32_t instance, flexspi_nor_config_t *config, serial_nor_config_option_t *option);
} flexspi_nor_driver_interface_t;

//Bootloader API Entry Structure
typedef struct {
     const uint32_t version; //!< Bootloader version number
     const char *copyright; //!< Bootloader Copyright
     void (*runBootloader)(void *arg); //!< Function to start the bootloader executing
     const uint32_t *reserved0; //!< Reserved
     const flexspi_nor_driver_interface_t *flexSpiNorDriver; //!< FlexSPI NOR Flash API
     const uint32_t *reserved1[2]; //!< Reserved
     const rtwdog_driver_interface_t *rtwdogDriver;
     const wdog_driver_interface_t *wdogDriver;
     const uint32_t *reserved2;
} bootloader_api_entry_t;

extern status_t flexspi_nor_flash_init(uint32_t instance, flexspi_nor_config_t *config);
extern status_t flexspi_nor_flash_page_program(uint32_t instance, flexspi_nor_config_t *config, uint32_t dstAddr, const uint32_t *src);
extern status_t flexspi_nor_flash_erase_all(uint32_t instance, flexspi_nor_config_t *config);
extern status_t flexspi_nor_get_config(uint32_t instance, flexspi_nor_config_t *config, serial_nor_config_option_t *option);
extern status_t flexspi_nor_flash_erase(uint32_t instance, flexspi_nor_config_t *config, uint32_t start, uint32_t length);
extern status_t flexspi_nor_flash_read(uint32_t instance, flexspi_nor_config_t *config, uint32_t *dst, uint32_t start, uint32_t bytes);
extern status_t flexspi_update_lut(uint32_t instance, uint32_t seqIndex, const uint32_t *lutBase, uint32_t numberOfSeq);
extern status_t flexspi_command_xfer(uint32_t instance, flexspi_xfer_t *xfer);
extern void flexspi_clear_cache(uint32_t instance);
#if 0 //(APP_TEST_FLASH_ENABLE == 1)
extern int testIAP(void);
#endif

#endif /* RT1064_IAP_H_ */
