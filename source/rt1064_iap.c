//------------------------------------------------------------------------------
// IAP.c                                                                           20190329 CHG
//------------------------------------------------------------------------------
// See section 8.13.2 in RTR1064 Users Manual regarding FlexSPI NOR API
//------------------------------------------------------------------------------

#include "fsl_debug_console.h"
#include "rt1064_iap.h"

#ifndef ATTRIBUTE_RAMFUNC
#define ATTRIBUTE_RAMFUNC		__attribute__((section(".ramfunc*")))  //FIXME
#endif

//------------------------------------------------------------------------------
// 1. lexspi_nor_flash_init
//    Initialize the Serial NOR device via FLEXSPI
//------------------------------------------------------------------------------
ATTRIBUTE_RAMFUNC
status_t flexspi_nor_flash_init(uint32_t instance, flexspi_nor_config_t *config) {
     return g_bootloaderTree->flexSpiNorDriver->init(instance, config);
}

//------------------------------------------------------------------------------
// 2. flexspi_nor_flash_page_program
//    Program data to specified Flash address
//------------------------------------------------------------------------------
ATTRIBUTE_RAMFUNC
status_t flexspi_nor_flash_page_program(uint32_t instance, flexspi_nor_config_t *config, uint32_t dstAddr, const uint32_t *src) {
     return g_bootloaderTree->flexSpiNorDriver->program(instance, config, dstAddr, src);
}

//------------------------------------------------------------------------------
// 3. flexspi_nor_flash_erase_all
//    Erase the whole Flash array via FLEXSPI
//------------------------------------------------------------------------------
ATTRIBUTE_RAMFUNC
status_t flexspi_nor_flash_erase_all(uint32_t instance, flexspi_nor_config_t *config) {
     return g_bootloaderTree->flexSpiNorDriver->erase_all(instance, config);
}

//------------------------------------------------------------------------------
// 4. flexspi_nor_get_config
//    Get the Flash configuration block via the serial_nor_config_option_t block,
//    see section "serial_nor_config_t definition"
//    See example code in section "FLASH API example on RT1064-EVK board"
//------------------------------------------------------------------------------
ATTRIBUTE_RAMFUNC
status_t flexspi_nor_get_config(uint32_t instance, flexspi_nor_config_t *config, serial_nor_config_option_t *option) {
     return g_bootloaderTree->flexSpiNorDriver->get_config(instance, config, option);
}

//------------------------------------------------------------------------------
// 5. flexspi_nor_flash_erase
//    Erase specified Flash region, the minimum erase unit is one sector
//------------------------------------------------------------------------------
ATTRIBUTE_RAMFUNC
status_t flexspi_nor_flash_erase(uint32_t instance, flexspi_nor_config_t *config, uint32_t start, uint32_t length) {
     return g_bootloaderTree->flexSpiNorDriver->erase(instance, config, start, length);
}

//------------------------------------------------------------------------------
// 6. flexspi_nor_flash_read
//    Read the FLASH via FLEXSPI using IP read command
//------------------------------------------------------------------------------
ATTRIBUTE_RAMFUNC
status_t flexspi_nor_flash_read(uint32_t instance, flexspi_nor_config_t *config, uint32_t *dst, uint32_t start, uint32_t bytes) {
     return g_bootloaderTree->flexSpiNorDriver->read(instance, config, dst, start, bytes);
}

//------------------------------------------------------------------------------
// 7. flexspi_update_lut
//    Update the specified LUT entries
//------------------------------------------------------------------------------
ATTRIBUTE_RAMFUNC
status_t flexspi_update_lut(uint32_t instance, uint32_t seqIndex, const uint32_t *lutBase, uint32_t numberOfSeq) {
    return g_bootloaderTree->flexSpiNorDriver->update_lut(instance, seqIndex, lutBase, numberOfSeq);
}

//------------------------------------------------------------------------------
// 8. flexspi_command_xfer
//    Execute LUT sequence specified by xfer
//------------------------------------------------------------------------------
ATTRIBUTE_RAMFUNC
status_t flexspi_command_xfer(uint32_t instance, flexspi_xfer_t *xfer) {
    return g_bootloaderTree->flexSpiNorDriver->xfer(instance, xfer);
}

//------------------------------------------------------------------------------
// 9. flexspi_clear_cache
//    Clear the AHB buffer in FLEXSPI module.
//------------------------------------------------------------------------------
ATTRIBUTE_RAMFUNC
void flexspi_clear_cache(uint32_t instance) {
    g_bootloaderTree->flexSpiNorDriver->clear_cache(instance);
}

#if 0 //(APP_TEST_FLASH_ENABLE == 1)
// Test data
static flexspi_nor_config_t config;
static serial_nor_config_option_t option;
static status_t status;
static uint32_t address = 0x40000; // The 256 bytes page we are programming starts at 0x70040000
static uint32_t sector_size = 0x1000; // a sector is 4KB
static uint32_t page_buffer[256 / sizeof(uint32_t)];
static uint32_t instance = 1; // Should identify NOR memory instance

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
ATTRIBUTE_RAMFUNC
int testIAP(void) {
	PRINTF("Start testIAP()..\r\n");
     option.option0.U = 0xC0000008; // QuadSPI NOR, Frequency: 133MHz
     
     // Need to run with interrupts disabled as all our code is running out of FlexSPI2 (internal NOT flash in RT1064)
     __disable_irq();
     
     
     status = flexspi_nor_get_config(instance, &config, &option);
     if (status != kStatus_Success) {
          __enable_irq();
          PRINTF("ERROR! flexspi_nor_get_config().. %d\r\n", status);
          return status;
     }
     
     status = flexspi_nor_flash_init(instance, &config);
     if (status != kStatus_Success) {
          __enable_irq();
          PRINTF("ERROR! flexspi_nor_flash_init().. %d\r\n", status);
          return status;
     }
     status = flexspi_nor_flash_erase(instance, &config, address , sector_size); // Erase 1 sector
     if (status != kStatus_Success) {
          __enable_irq();
          PRINTF("ERROR! flexspi_nor_flash_erase().. %d\r\n", status);
          return status;
     }
     // Fill data into the page_buffer;
     for (uint32_t i=0; i<sizeof(page_buffer)/sizeof(page_buffer[0]); i++) {
          page_buffer[i] = (i << 24) | (i << 16) | (i << 8) | i;
     }
     
     // Program data to destination (256 bytes)
     status = flexspi_nor_flash_page_program(instance, &config, address, page_buffer); // program 1 page
     if (status != kStatus_Success) {
          __enable_irq();
          PRINTF("ERROR! flexspi_nor_flash_page_program().. %d\r\n", status);
          return status;
     }
     
     SCB_CleanDCache();
     // Do cache maintenance here if the D-Cache is enabled
     // Use memory mapped access to verify whether data are programmed into Flash correctly
     uint32_t mem_address = 0x70000000 + address;
     if (0 == memcmp((void*)mem_address, page_buffer, sizeof(page_buffer))) {
         // Success
          __enable_irq();
          PRINTF("Success!!\r\n");
          return kStatus_Success;
     } else { // Report error
          __enable_irq();
          PRINTF("Failed!!\r\n");
          return kStatus_Fail;
     }
}
#endif
/*end of file*/
