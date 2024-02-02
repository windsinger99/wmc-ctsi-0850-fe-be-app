/*
 * bootloader.c
 *
 *  Created on: Feb 28, 2019
 *      Author: user
 */
#include "bootloader.h"
#include "fsl_debug_console.h"
#include "aes.h" //nsmoon@191010

#if 1 //nsmoon@190817
#define MAX_BOOT_INFO		30
static char BootInfo[MAX_BOOT_INFO];

#ifdef FE_FIXED_INFO //YJ@230223
#define MAX_FE_BOOT_INFO	64
//static char BootInfoFE[MAX_FE_BOOT_INFO];
#endif

#endif

#if (_DEBUG == 1)
#define DEBUG_EEPROM		1 //nsmoon@191010
#define PRINT_CMD(...)		PRINTF(__VA_ARGS__);PRINTF("\r\n")
#define PRINT_BLT(...)		//PRINTF(__VA_ARGS__);PRINTF("\r\n")
#define PRINT_HEX(...)		//PRINTF(__VA_ARGS__);PRINTF("\r\n")
#define PRINT_ESC(...)		//PRINTF(__VA_ARGS__);PRINTF("\r\n")
#define PRINTNR_ESC(...)	//PRINTF(__VA_ARGS__)
#define PRINT_DFD(...)		//PRINTF(__VA_ARGS__);PRINTF("\r\n")
#define PRINT_EEPROM(...)	PRINTF(__VA_ARGS__)
#else
#define PRINT_CMD(...)		//PRINTF(__VA_ARGS__);PRINTF("\r\n")
#define PRINT_BLT(...)      //PRINTF(__VA_ARGS__);PRINTF("\r\n")
#define PRINT_HEX(...)      //PRINTF(__VA_ARGS__);PRINTF("\r\n")
#define PRINT_ESC(...)
#define PRINT_DFD(...)      //PRINTF(__VA_ARGS__);PRINTF("\r\n")
#define PRINTNR_ESC(...)
#define PRINT_EEPROM(...)
#endif
#define PRINT_INFO(...)		//PRINTF(__VA_ARGS__);PRINTF("\r\n")
#define PRINT_ERR(...)		//PRINTF(__VA_ARGS__);PRINTF("\r\n")



#ifdef CYCCNT_ENABLE //for test
#define APP_TIME_LEN  	1
#define APP_TIME_STR_LEN 80
uint32_t curTime, prevTime;
uint32_t app_time[APP_TIME_LEN];
char app_time_str[APP_TIME_STR_LEN];
#define APP_TIME_INIT()	{prevTime=DWT->CYCCNT;}
#define APP_TIME_ADD(a)	{ \
	curTime=DWT->CYCCNT; \
	if ((a) < APP_TIME_LEN)	app_time[(a)]=(curTime-prevTime); \
	prevTime=DWT->CYCCNT; \
}
#define APP_TIME_PRINT() { \
	int i, idx; \
	app_time_str[0] = '\0'; \
	for (i =0; i < APP_TIME_LEN; i++) { \
		idx = strlen(app_time_str); \
		snprintf(&app_time_str[idx], (APP_TIME_STR_LEN-idx), "%ld,", app_time[i]/6); \
    } \
	PRINTF("@@@,%s,\r\n", app_time_str); \
}
#else
#define APP_TIME_INIT()
#define APP_TIME_ADD(a)
#define APP_TIME_PRINT()
#endif

#if 1 //nsmoon@200807
#define RESET_DELAY_DOT_CNT			10000
#define MAX_RESET_DELAY_CNT			3
static int s_reset_delay;
static unsigned int s_reset_delay_dot;
#endif

BOOTLOADER_DATA bootloaderData __attribute__((aligned(16)));
#if PROGRAM_BIN_FILE //nsmoon@191118
static T_BIN_RECORD BinRecordSt;
uint8_t header[MAX_HEADER_SIZE * 2]; //for bin file
header0_t header0;
header1_t header1;
#endif
#ifdef FLASH_DUAL_BANK //nsmoon@200717
uint8_t header2[MAX_HEADER_SIZE]; //for dev file
header0_t header20;
header1_t header21;
#if 1 //nsmoon@230503
uint8_t header22[HD_2_SIZE];
uint8_t header23[HD_3_SIZE];
#endif //1
#endif

#if 1 //nsmoon@230503
char frontend_fwfileText[FE_FWFILE_SIZE];
char frontend_timestampText[FE_TIMESTAMP_SIZE];
char frontend_modelText[FE_MODEL_SIZE];
char frontend_prgLenText[FE_PRGLEN_SIZE];
char frontend_prgCrcText[FE_PRGCRC_SIZE];
char frontend_fwVersion[MAX_VERSION_LEN];
uint16_t frontend_hexCrc;
uint32_t frontend_hexProgLen;
#endif

#if 0 //PROGRAM_BIN_FILE //nsmoon@191118
uint8_t mcuKey[EFUSE_KEY_LEN];
static uint8_t tmpKey0[HD_KEY0_SIZE];
static uint8_t tmpKey1[HD_KEY1_SIZE];
#endif

#ifdef FLASH_DUAL_BANK //nsmoon@200717
int flash_bank;
static int program_is_existed_A, program_is_existed_B;
static uint8_t program_ver_A, program_ver_B;
#else
static int program_is_existed;
#endif

#define BOOTLOADER_WAIT_DOT_CNT	10000
#define FE_WAIT_DOT_CNT			10000
#define MAX_FE_WAIT_CNT			50
static unsigned int fe_wait_cnt, fe_wait_dot;

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t Bootloader_Tx_Buffer[USB_HID_GENERIC_IN_BUFFER_LENGTH];
#ifdef USB_HOST_FOR_FRONTEND
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t Bootloader_Rx_Buffer[USB_HID_GENERIC_OUT_BUFFER_LENGTH];
#endif

static uint16_t crc_table[16] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

static uint8_t s_prog_buffer[FLASH_PROG_BUFF_SIZE];
#if (DEBUG_VERIFY_PROGRAM == 1)
static uint8_t s_read_buffer[FLASH_READ_BUFF_SIZE];
#endif

//internal QSPI flash
static flexspi_nor_config_t flashConfig;
static serial_nor_config_option_t flashOption;
static uint32_t flashInstance;

//static void delay_ms(uint32_t ms);
//static void ConvertAsciiToHex(uint8_t* asciiRec, uint8_t* hexRec);
static uint16_t Bootloader_CalculateCrc(uint8_t *data, uint32_t len);
static void Bootloader_Program_Init (void);
static void Bootloader_Program_BinInit(uint32_t *totLen);
static void check_pc_connected(int ready);
static int Bootloader_ProgramPage(uint32_t address, uint8_t *buff);
//static int Bootloader_FlashErase_All(void);
static int Bootloader_FlashErase_All(int bank);
static int Bootloader_FlashErase_Boot(void);
static int Bootloader_FlashErase(uint32_t secNum);
static int Bootloader_WriteHexRecordRemained(void);
static int Bootloader_WriteEeprom(int erase);
//static void Bootloader_SoftwareReset(void);
//static void Bootloader_Interrupt_enable(bool enable);
static void Bootloader_ProcessBuffer(BOOTLOADER_DATA *handle);
static int Bootloader_BufferEventHandler(DATASTREAM_BUFFER_EVENT buffEvent, uint16_t context);
static status_t Bootloader_FlashInit(void);

#if PROGRAM_BIN_FILE //nsmoon@191118
static int Bootloader_ProgramBinFile(uint8_t* BinRecord, int32_t totalLen);
#endif


void Bootloader_Initialize();
int Bootloader_Task();
extern void MY_WDOG_soft_reset(); //app.c
extern void s_frontend_Reset(); //app.c

#if 0 //for test
ATTRIBUTE_RAMFUNC
static void delay_ms(uint32_t ms)
{
    volatile uint32_t i, j;
    for (j = 0; j < ms; ++j) {
        for (i = 0; i < 66000; ++i) {
            __asm("NOP"); /* delay */
        }
    }
}
#endif

#if 0 //for testing
ATTRIBUTE_RAMFUNC
static void ConvertAsciiToHex(uint8_t* asciiRec, uint8_t* hexRec)
{
    uint8_t i = 0;
    uint8_t k = 0;
    uint8_t hex;

    while((asciiRec[i] >= 0x30) && (asciiRec[i] <= 0x66))
    {
            // Check if the ASCII values are in alpha numeric range.
            if(asciiRec[i] < 0x3A) {
                    // Numerical representation in ASCII found.
                    hex = asciiRec[i] & 0x0F;
            }
            else {
                    // Alphabetical value.
                    hex = 0x09 + (asciiRec[i] & 0x0F);
            }

            // Following logic converts 2 bytes of ASCII to 1 byte of hex.
            k = i%2;

            if(k) {
                    hexRec[i>>1] |= hex;

            }
            else {
                    hexRec[i>>1] = (hex << 4) & 0xF0;
            }
            i++;
    }
}
#endif

ATTRIBUTE_RAMFUNC
static uint16_t Bootloader_CalculateCrc(uint8_t *data, uint32_t len)
{
	uint32_t i = 0, j;
	uint16_t crc = 0;

	for (j = 0; j < len; j++)
	{
		i = (crc >> 12) ^ ((data[j] >> 4) & 0x0F);
		crc = crc_table[i & 0x0F] ^ (crc << 4);
		i = (crc >> 12) ^ ((data[j] >> 0) & 0xFF);
		crc = crc_table[i & 0x0F] ^ (crc << 4);
	}
    return (crc & 0xFFFF);
}


ATTRIBUTE_RAMFUNC
static void Bootloader_Program_Init(void)
{
	//bootloaderData init
	bootloaderData.preSectNum = 0;
	bootloaderData.curSectNum = 0;
	bootloaderData.preVirAddr = 0; //init
	bootloaderData.sectStartVirAddr = 0; //init
	bootloaderData.appLen = 0;
	bootloaderData.prgLen = 0;

	memset(&s_prog_buffer[0], 0xFF, FLASH_PROG_BUFF_SIZE);
}

ATTRIBUTE_RAMFUNC
static void Bootloader_Program_BinInit(uint32_t *totLen)
{
	bootloaderData.writeCode = true;
	//PRINTF("(2)writeCode=%d\r\n", bootloaderData.writeCode);

#if PROGRAM_BIN_FILE //nsmoon@191118
	BinRecordSt.seqNoPkt = 0U;
	BinRecordSt.totalPkt = 0U;
	BinRecordSt.binProgAddr = 0U;
	if (totLen == 0) {
		BinRecordSt.binProgLen = 0;
	}
	else {
	    BinRecordSt.binProgLen = *totLen;
	}
	BinRecordSt.hdIdx = 0U; //init
	memset((void *)&header[0], 0, MAX_HEADER_SIZE * 2);
#endif
}

#ifndef FLASH_DUAL_BANK //nsmoon@200717
ATTRIBUTE_RAMFUNC
static int IsPrgExists(void)
{
    //return 0; //ng
#if 1 //nsmoon@200608
	if (0xFFFFFFFF != *(unsigned int *)APPLICATION_ADDRESS) {
		if (0xFFFFFFFF != *(unsigned int *)BOOTLOADER_EEPROM_BASE_ADDRESS) {
			return 1; //ok
		}
	}
    return 0; //ng
#endif
#if 0 //nsmoon@200608
	if (0xFFFFFFFF != *(unsigned int *)APPLICATION_ADDRESS) {
		memcpy(&header[0], (uint8_t *)BOOTLOADER_EEPROM_BASE_ADDRESS, HD_0_SIZE + HD_1_SIZE);
		memcpy(&header0.all[0], &header[0], HD_0_SIZE);
		memcpy(&header1.all[0], &header[HD_0_SIZE], HD_1_SIZE);
		uint16_t crcHd = Bootloader_CalculateCrc(header, (HD_0_SIZE + HD_1_SIZE - 2));
		if (header0.ind.code_size <= APP_PRG_SIZE) {
			uint16_t crcCode = Bootloader_CalculateCrc((uint8_t *)BOOTLOADER_FLASH_BASE_ADDRESS, header0.ind.code_size);
			PRINTF("\r\nBOOT %X/%X %X/%X\r\n", crcCode, header0.ind.code_crc, crcHd, header1.ind.hd01_crc);
			if (crcCode == header0.ind.code_crc && crcHd == header1.ind.hd01_crc) {
				return 1; //ok
			}
		}
	}
    return 0; //ng
#endif
#if 0
    return (0xFFFFFFFF != *(unsigned int *)APPLICATION_ADDRESS);
#endif
}
#endif

#ifdef FLASH_DUAL_BANK //nsmoon@200717
ATTRIBUTE_RAMFUNC
static uint8_t get_eeprom_dev(int bankNo)
{
    __disable_irq();
	//SCB_CleanDCache();
	SCB_CleanInvalidateDCache();
	if (bankNo == FLASH_BANK_B) {
    	memcpy(&header2[0], (uint8_t *)EEPROM_BASE_ADDRESS_B, HD_SIZE_TOT);
	}
	else { //FLASH_BANK_A
    	memcpy(&header2[0], (uint8_t *)EEPROM_BASE_ADDRESS_A, HD_SIZE_TOT);
	}
    __enable_irq();
    memcpy(&header20.all[0], &header2[0], HD_0_SIZE);
	memcpy(&header21.all[0], &header2[HD_0_SIZE], HD_1_SIZE);
#if 1 //nsmoon@230503
	memcpy(&header22[0], &header2[HD_0_SIZE+HD_1_SIZE], HD_2_SIZE); //sn
	memcpy(&header23[0], &header2[HD_0_SIZE+HD_1_SIZE+HD_2_SIZE], HD_3_SIZE); //fe eeprom
#endif
    return (uint8_t)header20.ind.code_misc;
}


#if 1 //nsmoon@230503
ATTRIBUTE_RAMFUNC
static void copy_eeprom_bin(int hdNo)
{
	int idx = 0, len = 0;

	switch (hdNo) {
	case 0:
		idx = 0;
		len = HD_0_SIZE;
		break;
	case 1:
		idx = HD_0_SIZE;
		len = HD_1_SIZE;
		break;
	case 2:
		idx = HD_0_SIZE + HD_1_SIZE;
		len = HD_2_SIZE;
		break;
	case 3:
		idx = HD_0_SIZE + HD_1_SIZE + HD_2_SIZE;
		len = HD_3_SIZE;
		break;
	}

	memcpy(&header[idx], &header2[idx], len);
}
#endif


#if 0 //for test //nsmoon@200721
static int reset_version_number(void)
{
	int bank;
	uint8_t flash_bank_sav = flash_bank;
	uint8_t verNo;
	int ret = BT_OK;

	for (bank = FLASH_BANK_A; bank < FLASH_BANK_END; bank++) {
		verNo = get_eeprom_dev(bank);
	    header20.ind.code_misc = 0;
	    memcpy(&header2[0], &header20.all[0], HD_0_SIZE);
		memcpy(&header2[HD_0_SIZE], header21.all, HD_1_SIZE);
    	memcpy((void *)&header[0], (void *)&header2[0], HD_SIZE_TOT);
		flash_bank = bank;
		PRINT_DFD("bank,verNo= %d %d (%d %d)", bank, verNo, flash_bank, header0.ind.code_misc);
	    ret = Bootloader_WriteEeprom(1); //erase
	    if (BT_OK == ret) {
	        ret = Bootloader_WriteEeprom(0); //write
	    }
	}
	flash_bank = flash_bank_sav;
	return ret;
}
#endif

ATTRIBUTE_RAMFUNC
int check_hd_crc_flash_bank(int bankNo)
{
	uint8_t *eeprom_addr;
	PRINT_DFD("check_hd_crc_flash_bank...%d", bankNo);

	if (bankNo == FLASH_BANK_B) {
		eeprom_addr = (uint8_t *)EEPROM_BASE_ADDRESS_B;
	}
	else { //FLASH_BANK_A
		eeprom_addr = (uint8_t *)EEPROM_BASE_ADDRESS_A;
	}
	uint16_t crcHd = Bootloader_CalculateCrc(eeprom_addr, (HD_0_SIZE + HD_1_SIZE - 2));
	PRINT_DFD("check_hd_crc_flash_bank..(%d)%X %X", bankNo, header21.ind.hd01_crc, crcHd);
	if (crcHd == header21.ind.hd01_crc) {
		return 1; //crc-ok
	}

    return 0; //crc-mismatched
}

ATTRIBUTE_RAMFUNC
int check_code_crc_flash_bank(int bankNo)
{
	uint8_t *code_addr;
	uint32_t code_size = header20.ind.code_size;

	if (bankNo == FLASH_BANK_B) {
		code_addr = (uint8_t *)FLASH_BANK_B_ADDR;
	}
	else { //FLASH_BANK_A
		code_addr = (uint8_t *)FLASH_BANK_A_ADDR;
	}

	if (code_size > 10 && code_size <= APP_PRG_SIZE) {
		uint16_t crcCode = Bootloader_CalculateCrc(code_addr, code_size);
		PRINT_DFD("check_code_crc_flash_bank= (%d) %X %X", bankNo, crcCode, header20.ind.code_crc);
		if (crcCode == header20.ind.code_crc) {
			return 1; //crc-ok
		}
	}
	else {
        PRINT_ERR("ERROR! check_code_crc_flash_bank..invalid code_size %d", code_size);
	}
    return 0; //crc-mismatched
}

ATTRIBUTE_RAMFUNC
static int IsPrgExistsA(void)
{
	PRINT_DFD("APP_A= %x,%x", (unsigned int *)APP_ADDRESS_A, *(unsigned int *)APP_ADDRESS_A);
	PRINT_DFD("EEPROM_A= %x,%x", (unsigned int *)EEPROM_BASE_ADDRESS_A, *(unsigned int *)EEPROM_BASE_ADDRESS_A);
	program_ver_A = program_is_existed_A = 0;
    if (0xFFFFFFFF != *(unsigned int *)APP_ADDRESS_A) {
        if (0xFFFFFFFF != *(unsigned int *)EEPROM_BASE_ADDRESS_A) {
        	program_ver_A = get_eeprom_dev(FLASH_BANK_A); //odd version
        	PRINT_DFD("program_ver_A= %d", program_ver_A);
#if (FLASH_DUAL_BANK_TYPE == 1) //nsmoon@200717
        	if ((program_ver_A % 2) == 0) {
        		return 0; //invalid version
        	}
#endif
            if (check_hd_crc_flash_bank(FLASH_BANK_A)) {
                if (check_code_crc_flash_bank(FLASH_BANK_A)) {
                	program_is_existed_A = 1; //exist
                    return 1; //exist
                }
            }
        }
    }
    return 0;
}

ATTRIBUTE_RAMFUNC
static int IsPrgExistsB(void)
{
	PRINT_DFD("APP_B= %x %x", (unsigned int *)APP_ADDRESS_B, *(unsigned int *)APP_ADDRESS_B);
	PRINT_DFD("EEPROM_B= %x %x", (unsigned int *)EEPROM_BASE_ADDRESS_B, *(unsigned int *)EEPROM_BASE_ADDRESS_B);
	program_ver_B = program_is_existed_B = 0;
    if (0xFFFFFFFF != *(unsigned int *)APP_ADDRESS_B) {
        if (0xFFFFFFFF != *(unsigned int *)EEPROM_BASE_ADDRESS_B) {
        	program_ver_B = get_eeprom_dev(FLASH_BANK_B); //even version
        	PRINT_DFD("program_ver_B= %d", program_ver_B);
#if (FLASH_DUAL_BANK_TYPE == 1) //nsmoon@200717
        	if ((program_ver_B % 2) != 0) {
        		return 0; //invalid version
        	}
#endif
            if (check_hd_crc_flash_bank(FLASH_BANK_B)) {
                if (check_code_crc_flash_bank(FLASH_BANK_B)) {
                	program_is_existed_B = 1; //exist
                    return 1; //exist
                }
            }
        }
    }
    return 0;
}

#if (FLASH_DUAL_BANK_TYPE == 1)
void SelectFlashBank(int mode)
{
    IsPrgExistsA();
    IsPrgExistsB();

	if (!program_is_existed_A && !program_is_existed_B) {
	    flash_bank = FLASH_BANK_END;
	}
	else if (program_is_existed_A && !program_is_existed_B) {
	    flash_bank = FLASH_BANK_A;
    }
    else if (!program_is_existed_A && program_is_existed_B) {
	    flash_bank = FLASH_BANK_B;
    }
    else {
        if (program_ver_A < program_ver_B) {
    	    flash_bank = FLASH_BANK_B;
        }
        else {
    	    flash_bank = FLASH_BANK_A;
        }
    }

	if (FLASH_BANK_FOR_PROGRAM) {
	    if (FLASH_BANK_A == flash_bank) {
	    	flash_bank = FLASH_BANK_B;
	    }
	    else if (FLASH_BANK_END == flash_bank) {
			flash_bank = FLASH_BANK_A;
		}
	    PRINT_DFD("(P=%d", flash_bank);
	}
	else {
	    PRINT_DFD("(B=%d", flash_bank);
	}
}
#elif (FLASH_DUAL_BANK_TYPE == 2)
#define GET_VIRT_ADDR(a)	((a) - FLASH_BASE_ADDR)
#define GET_PHYS_ADDR(a)	((a) + FLASH_BASE_ADDR)
#define SRC_V_ADDR_CODE		GET_VIRT_ADDR(FLASH_BANK_B_ADDR)
#define DST_V_ADDR_CODE		GET_VIRT_ADDR(FLASH_BANK_A_ADDR)
#define SRC_V_ADDR_EEPROM	GET_VIRT_ADDR(EEPROM_BASE_ADDRESS_B)
#define DST_V_ADDR_EEPROM	GET_VIRT_ADDR(EEPROM_BASE_ADDRESS_A)
#define GET_SECT_NUM(a)		((a) / FLASH_SECTOR_SIZE)

ATTRIBUTE_RAMFUNC
int CopyFlashBank(int mode)
{
	//mode: 1 (eeprom + code), 0 (eeprom)
	uint32_t code_size = header20.ind.code_size;
	int code_sect_size = (code_size + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE;
	int pageNo, sectNo, dst_sect;
	uint32_t src_vaddr, dst_vaddr;
	int sect_offset = 0, page_offset = 0;
	uint8_t *buffProg = (uint8_t *)&s_prog_buffer[0];

	PRINT_DFD("CopyFlashBank.. %x 0x%x", code_size, code_sect_size);
	if (code_size == 0 || code_size > (uint32_t)APP_PRG_SIZE) {
		if(code_size == 0xffffffff)
		{
			PRINT_ERR("Flash bank is empty first time write => code_size %x", code_size);
		}
		else
		{
			PRINT_ERR("ERROR! invalid code_size %x", code_size);
			return BT_ERR; //error
		}
	}

	if (Bootloader_FlashInit() != kStatus_Success) {
		PRINT_ERR("ERROR! CopyFlashBank..Bootloader_FlashInit");
		return BT_ERR; //error
	}
	PRINTF("=C");

	//erase eeprom A
	PRINT_DFD("DST_V_ADDR_EEPROM= %x %x", DST_V_ADDR_EEPROM, GET_SECT_NUM(DST_V_ADDR_EEPROM));
	if (Bootloader_FlashErase(GET_SECT_NUM(DST_V_ADDR_EEPROM)) != BT_OK) {
		PRINT_ERR("ERROR! CopyFlashBank..Bootloader_FlashErase EEPROM");
		return BT_ERR; //error
	}

	flash_bank = FLASH_BANK_A;

	//copy code from B to A
	if (mode) {
	    for (sectNo = 0; sectNo < code_sect_size; sectNo++) {
	    	MY_WDOG_Refresh(); //FIXME
	    	sect_offset = sectNo * FLASH_SECTOR_SIZE;
	    	dst_vaddr = DST_V_ADDR_CODE + sect_offset;
	    	dst_sect = GET_SECT_NUM((uint32_t)dst_vaddr);
	    	PRINT_DFD("erase: (%x) %x %x", sectNo, (uint32_t)dst_vaddr, dst_sect);
	    	if (Bootloader_FlashErase(dst_sect) != BT_OK) {
	    		PRINT_ERR("ERROR! CopyFlashBank..Bootloader_FlashErase");
	    		return BT_ERR; //error
	    	}

	        for (pageNo = 0; pageNo < (FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE); pageNo++) {
	        	page_offset = pageNo * FLASH_PAGE_SIZE;
	        	src_vaddr = SRC_V_ADDR_CODE + sect_offset + page_offset;
	        	dst_vaddr = DST_V_ADDR_CODE + sect_offset + page_offset;
	        	PRINT_DFD("..program: (%d) %x %x", pageNo, dst_vaddr, src_vaddr);
	            __disable_irq();
	        	SCB_CleanDCache();
	        	memcpy((void *)&buffProg[0], (void *)GET_PHYS_ADDR(src_vaddr), FLASH_PAGE_SIZE);
	            __enable_irq();
	        	if (Bootloader_ProgramPage((uint32_t)dst_vaddr, (uint8_t *)&buffProg[0]) != BT_OK) {
	        		PRINT_ERR("ERROR! CopyFlashBank..Bootloader_ProgramPage");
	        		return BT_ERR; //error
	        	}
	        }
	    }
	}

	//copy eeprom from B to A
	src_vaddr = SRC_V_ADDR_EEPROM;
	dst_vaddr = DST_V_ADDR_EEPROM;
	PRINT_DFD("eeprom: %x %x", src_vaddr, dst_vaddr);
    __disable_irq();
	//SCB_CleanDCache();
	SCB_CleanInvalidateDCache();
	memcpy((void *)&buffProg[0], (void *)GET_PHYS_ADDR(src_vaddr), FLASH_PAGE_SIZE);
    __enable_irq();
	if (Bootloader_ProgramPage((uint32_t)dst_vaddr, (uint8_t *)&buffProg[0]) != BT_OK) {
		PRINT_ERR("ERROR! CopyFlashBank..Bootloader_ProgramPage EEPROM");
		return BT_ERR; //error
	}

    return BT_OK; //no-error
}

#if 1 //nsmoon@230503
#define SRC_V_ADDR_CODE2	GET_VIRT_ADDR(FLASH_BANK_A_ADDR)
#define DST_V_ADDR_CODE2	GET_VIRT_ADDR(FLASH_BANK_B_ADDR)

ATTRIBUTE_RAMFUNC
int CopyFlashBankCode(void)
{
	uint32_t code_size, src_vaddr, dst_vaddr;
	int code_sect_size, pageNo, sectNo, dst_sect;
	int sect_offset = 0, page_offset = 0;
	uint8_t *buffProg = (uint8_t *)&s_prog_buffer[0];

	get_eeprom_dev(FLASH_BANK_A);
	code_size = header20.ind.code_size;
	code_sect_size = (code_size + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE;

	PRINT_DFD("CopyFlashBankCode.. %x 0x%x", code_size, code_sect_size);
	if (code_size == 0 || code_size > (uint32_t)APP_PRG_SIZE) {
		if(code_size == 0xffffffff)
		{
			PRINT_ERR("Flash bank is empty first time write => code_size %x", code_size);
		}
		else
		{
			PRINT_ERR("ERROR! invalid code_size %x", code_size);
			return BT_ERR; //error
		}
	}

	if (Bootloader_FlashInit() != kStatus_Success) {
		PRINT_ERR("ERROR! CopyFlashBank..Bootloader_FlashInit");
		return BT_ERR; //error
	}
	PRINTF("=C2");

	//copy code from A to B
	flash_bank = FLASH_BANK_B;

    for (sectNo = 0; sectNo < code_sect_size; sectNo++) {
    	MY_WDOG_Refresh(); //FIXME
    	sect_offset = sectNo * FLASH_SECTOR_SIZE;
    	dst_vaddr = DST_V_ADDR_CODE + sect_offset;
    	dst_sect = GET_SECT_NUM((uint32_t)dst_vaddr);
    	PRINT_DFD("erase: (%x) %x %x", sectNo, (uint32_t)dst_vaddr, dst_sect);
    	if (Bootloader_FlashErase(dst_sect) != BT_OK) {
    		PRINT_ERR("ERROR! CopyFlashBank..Bootloader_FlashErase");
    		return BT_ERR; //error
    	}

        for (pageNo = 0; pageNo < (FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE); pageNo++) {
        	page_offset = pageNo * FLASH_PAGE_SIZE;
        	src_vaddr = SRC_V_ADDR_CODE + sect_offset + page_offset;
        	dst_vaddr = DST_V_ADDR_CODE + sect_offset + page_offset;
        	PRINT_DFD("..program: (%d) %x %x", pageNo, dst_vaddr, src_vaddr);
            __disable_irq();
        	SCB_CleanDCache();
        	memcpy((void *)&buffProg[0], (void *)GET_PHYS_ADDR(src_vaddr), FLASH_PAGE_SIZE);
            __enable_irq();
        	if (Bootloader_ProgramPage((uint32_t)dst_vaddr, (uint8_t *)&buffProg[0]) != BT_OK) {
        		PRINT_ERR("ERROR! CopyFlashBank..Bootloader_ProgramPage");
        		return BT_ERR; //error
        	}
        }
    }

    return BT_OK; //no-error
}
#endif
#endif
#endif

#if 0 //for test //nsmoon@200720
void SelectFlashBankB()
{
    bool swap = 0; //PLIB_NVM_ProgramFlashBank2IsLowerRegion(NVM_ID_0);

    if (swap) {
        //bank1 is upper
        IOMUXC_GPR->GPR30 = 0;
        IOMUXC_GPR->GPR31 = 0;
        IOMUXC_GPR->GPR32 = 0;
        flash_bank = FLASH_BANK_A;
        PRINT_DFD("<S1>");
    }
    else {
        //bank2 is upper
        IOMUXC_GPR->GPR30 = FLASH_BANK_A_ADDR;
        IOMUXC_GPR->GPR31 = FLASH_BANK_B_ADDR;
        IOMUXC_GPR->GPR32 = MY_FLASH_BANK_SIZE;
        flash_bank = FLASH_BANK_B;
        PRINT_DFD("<S2>");
    }
    PRINT_DFD("GPR30= %X %X %X",IOMUXC_GPR->GPR30, IOMUXC_GPR->GPR31, IOMUXC_GPR->GPR32);
}
#endif //0

#if 1 //nsmoon@200807
ATTRIBUTE_RAMFUNC
static void DoSoftReset(int cnt)
{
	s_reset_delay = cnt;
}

ATTRIBUTE_RAMFUNC
static void check_reset_delay(void)
{
   if (s_reset_delay > 0) {
	   if ((s_reset_delay_dot++ % RESET_DELAY_DOT_CNT) == 0) {
   	       PRINTF(".");
		   s_reset_delay--;
	   }
   }
   else if (s_reset_delay == 0) {
		MY_WDOG_soft_reset();
		while(1);
   }
}
#endif

ATTRIBUTE_RAMFUNC
static void check_pc_connected(int ready)
{
#if 0		//YJ@220207
	if (ready == USB_OK)
#endif
	{
		if (bootloaderData.connected_pc_tool != PC_CONN)
	    {
#ifdef FLASH_DUAL_BANK //nsmoon@200717
    #if (FLASH_DUAL_BANK_TYPE == 1)
			if (bootloaderData.bootloader_wait_cnt >= PC_WAIT_TIMEOUT_CNT &&
					(program_is_existed_A || program_is_existed_B)) {
				SelectFlashBank(FLASH_BANK_FOR_BOOT);
				PRINTF("<%d", flash_bank);
    #elif (FLASH_DUAL_BANK_TYPE == 2)
			if (bootloaderData.bootloader_wait_cnt >= PC_WAIT_TIMEOUT_CNT) {
				if (!program_is_existed_A && program_is_existed_B) {
					//bank-B exist and crc-ok, copy from B to A
					get_eeprom_dev(FLASH_BANK_B);
					CopyFlashBank(1);
#if 1 //nsmoon200807
					DoSoftReset(3);
#else
                    s_reset_delay = 100;
					bootloaderData.currentState = BOOTLOADER_RESET;
#endif
					flash_bank = FLASH_BANK_END;
					bootloaderData.bootloader_wait_cnt = 0;
				}
				else if (!program_is_existed_A && !program_is_existed_B) {
				    //bank-A and B are blank
					flash_bank = FLASH_BANK_END;
					bootloaderData.bootloader_wait_cnt = 0;
				}
    #endif
				if (flash_bank != FLASH_BANK_END)
#else
			if (bootloaderData.bootloader_wait_cnt >= PC_WAIT_TIMEOUT_CNT && program_is_existed)
#endif
				{
	#if (FORCED_BOOTLOAD_MODE == 1) //nsmoon@170614a
					//force to close datastream
					bootloaderData.currentState = BOOTLOADER_CLOSE_DATASTREAM;
	#endif //FORCED_BOOTLOAD_MODE
					PRINTF("#\r\n");
					return;
				}
            }
            else {
                if ((bootloaderData.bootloader_wait_cnt % BOOTLOADER_WAIT_DOT_CNT) == 0) {
            	    PRINTF(".");
                }
                bootloaderData.bootloader_wait_cnt++;
            }
	        bootloaderData.sent_ver_cmd = 0; //reset
	        fe_wait_dot = fe_wait_cnt = 0; //reset
        }
#if USB_HOST_FOR_FRONTEND
	    else {
            if (bootloaderData.sent_ver_cmd == 0) {
            	if (USB_HostCmd(READ_BOOT_INFO) == USB_HOST_OK) { //nsmoon@200603
        		    bootloaderData.sent_ver_cmd =  1;
        	    	PRINTF("*");
        	    }
        	    else {
            	    if ((fe_wait_cnt % MAX_FE_WAIT_CNT) == 0) {
        	    		fe_wait_cnt = 1; //reset
                	    PRINTF("&");
        	    		s_frontend_Reset();
                    }
                    if ((fe_wait_dot % FE_WAIT_DOT_CNT) == 0) {
                    	fe_wait_dot = 0; //reset
                    	fe_wait_cnt++;
                	    PRINTF("^");
                    }
    	    		fe_wait_dot++;
        	    }
	    	}
	    }
#endif
	}
#if 0	//YJ@220207
	else {
		bootloaderData.bootloader_wait_cnt = 0;
		bootloaderData.connected_pc_tool = PC_NOTCONN;
        bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
	}
#endif
}

ATTRIBUTE_RAMFUNC
static int Bootloader_ProgramPage(uint32_t virAddr, uint8_t *buff)
{
	uint8_t *buffProg = buff;
	int retry, ret = BT_OK;
	PRINT_DFD("..ProgramPage: %x %x", virAddr, (uint32_t)buff);

    for (retry = 0; retry < 1/*3*/; retry++) {
    	/* Program Page */
        __disable_irq();
        status_t status = flexspi_nor_flash_page_program(flashInstance, &flashConfig, virAddr, (uint32_t *)buffProg); // program 1 page
        __enable_irq();
        if (status != kStatus_Success)
        {
        	PRINT_ERR("ERROR! Bootloader_ProgramPage..flexspi_nor_flash_page_program");
            //assert(0);
            return BT_ERR;
        }
        //PRINT_BLT("Page program successfully !");

    #if (DEBUG_VERIFY_PROGRAM == 1)
    	uint8_t *buffRead = (uint8_t *)&s_read_buffer[0];
    	memset(buffRead, 0, FLASH_PAGE_SIZE);
    	uint32_t phyAddr = FLASH_BASE_ADDR + virAddr;

        __disable_irq();
        /* Read Page */
   	    //SCB_CleanDCache(); //clean the D cache before reading data from QSPI Flash //bug!! do not use
   	    //SCB_CleanDCache_by_Addr((uint32_t *)phyAddr, FLASH_PAGE_SIZE); //bug!! do not use
   	    SCB_CleanInvalidateDCache(); //nsmoon@200703, OK
   	    //SCB_InvalidateDCache(); //nsmoon@200703, OK
        memcpy(&buffRead[0], (void *)phyAddr, FLASH_PAGE_SIZE);
        __enable_irq();

        if (memcmp(&buffProg[0], &buffRead[0], FLASH_PAGE_SIZE)) {
            PRINT_ERR("ERROR! Bootloader_ProgramPage..data incorrect! %x", phyAddr);
#if 0 //for debugging
            int i;
            for (i = 0; i < FLASH_PAGE_SIZE; i++) {
                if ((i % 16) == 0) {
                	PRINTF("\r\n%02x: ", i);
                }
                PRINTF("%02x ", buffProg[i]);
            }
            for (i = 0; i < FLASH_PAGE_SIZE; i++) {
                if ((i % 16) == 0) {
                	PRINTF("\r\n%02x: ", i);
                }
                PRINTF("%02x ", buffRead[i]);
            }
            PRINTF("\r\n");
#endif
            ret = BT_ERR;
        }
        else {
        	PRINT_HEX(".");
            ret = BT_OK;
            break;
        }
    #else
        break;
    #endif
    }

    return ret;
}

#if 0 //for test
ATTRIBUTE_RAMFUNC
static int Bootloader_ReadPage(uint32_t virAddr, uint8_t *buff)
{
	uint8_t *buffRead = buff;
	int ret = BT_OK;

	/* Read Page */
    __disable_irq();
    status_t status = flexspi_nor_flash_read(flashInstance, &flashConfig, (uint32_t *)buffRead, virAddr, FLASH_PAGE_SIZE);
    __enable_irq();

    if (status != kStatus_Success)
    {
    	PRINT_ERR("ERROR! Page read failure !");
        return BT_ERR;
    }

    return ret;
}
#endif

ATTRIBUTE_RAMFUNC
static int Bootloader_FlashErase_All(int bank)
{
	int secLen;
	uint32_t address; //virtual address

#ifdef FLASH_DUAL_BANK //nsmoon@200717
	if (bank == FLASH_BANK_A) { //nsmoon@230503
		//erase bank A only, for recover test
		address = BANK_A_SECTOR_START * FLASH_SECTOR_SIZE;
	}
	else {
		address = BANK_B_SECTOR_START * FLASH_SECTOR_SIZE;
	}
    secLen = NUM_OF_BANK_SECTOR; //sector length
#else
	address = APP_SECTOR_START * FLASH_SECTOR_SIZE;
    secLen = NUM_OF_SECTOR - APP_SECTOR_START;
#endif
	//PRINT_BLT("address,secLen= %x %x", address, secLen);

    __disable_irq();
	status_t status = flexspi_nor_flash_erase(flashInstance, &flashConfig, address, secLen); // Erase sectors
    __enable_irq();
    if (status != kStatus_Success)
    {
    	PRINT_ERR("ERROR! Bootloader_FlashErase_All.. %d %d (%x)", bank, secLen, status);
        assert(0);
        return BT_ERR;
    }

    //PRINT_BLT("Erase - successfully.");
    return BT_OK;
}



ATTRIBUTE_RAMFUNC
static int Bootloader_FlashErase_Boot(void)
{
	//PRINT_BLT("Erasing Serial NOR over FlexSPI...");

    /* Erase all sector. */
    __disable_irq();
    status_t status = flexspi_nor_flash_erase_all(flashInstance, &flashConfig);
    __enable_irq();

    if (status != kStatus_Success)
    {
    	PRINT_ERR("ERROR! Bootloader_FlashErase_Boot.. %d (%x)", status, status);
        assert(0);
        return BT_ERR;
    }

    //PRINT_BLT("Erase Boot - successfully.");
    return BT_OK;
}

ATTRIBUTE_RAMFUNC
static status_t Bootloader_FlashInit(void)
{
    //internal QSPI flash init
    flashOption.option0.U = 0xC0000008; // QuadSPI NOR, Frequency: 133MHz
	flashInstance = 1; //Should identify NOR memory instance

    __disable_irq();
	status_t status = flexspi_nor_get_config(flashInstance, &flashConfig, &flashOption);
    if (status != kStatus_Success) {
    	PRINT_ERR("ERROR! flexspi_nor_get_config().. %d", status);
    }
    status = flexspi_nor_flash_init(flashInstance, &flashConfig);
    __enable_irq();
    if (status != kStatus_Success) {
    	PRINT_ERR("ERROR! flexspi_nor_flash_init().. %d", status);
    }
    return status;
}

ATTRIBUTE_RAMFUNC
static int Bootloader_FlashErase(uint32_t secNum)
{
    /* Erase one sector. */
	status_t status;
	uint32_t address = secNum * FLASH_SECTOR_SIZE;
	PRINT_DFD("..address,secNum= %x %x", address, secNum);

	__disable_irq();
	status = flexspi_nor_flash_erase(flashInstance, &flashConfig, address, 1); // Erase 1 sector
    __enable_irq();
    if (status != kStatus_Success)
    {
    	PRINT_ERR("ERROR! Erase sector failure %d (%x) !", secNum, status);
        //assert(0);
        return BT_ERR;
    }
    //PRINT_BLT("Erase - successfully.");

#if 0 //for verification
	uint8_t *addr= (uint8_t *)(FLASH_BASE_ADDR + address);
	uint8_t *buffProg = (uint8_t *)&s_prog_buffer[0];
	uint8_t *buffRead = (uint8_t *)&s_read_buffer[0];

	memset(buffProg, 0xFF, FLASH_SECTOR_SIZE);

	/* clean the D cache before reading data from QSPI Flash */
	//SCB_CleanDCache();
	//SCB_InvalidateDCache();

    /* Read Page */
    memcpy(&buffRead[0], (void *)addr, FLASH_SECTOR_SIZE);
    if (memcmp(&buffProg[0], &buffRead[0], FLASH_SECTOR_SIZE)) {
        PRINT_ERR("ERROR! Bootloader_FlashErase..data incorrect! %x", addr);
    }
#endif
    return BT_OK;
}

ATTRIBUTE_RAMFUNC
static int Bootloader_WriteHexRecordRemained(void)
{
	int i = 0;
	uint32_t progAddr;
	uint8_t *buffProg = (uint8_t *)&s_prog_buffer[0];
	int ret = BT_OK;
	int len = bootloaderData.appLen - bootloaderData.prgLen;

	if (len > 0) {
		PRINT_HEX("Write.2. %x (%d)", bootloaderData.sectStartVirAddr, len);
		progAddr = bootloaderData.sectStartVirAddr;
#ifdef FLASH_DUAL_BANK //nsmoon@200717
		if (flash_bank == FLASH_BANK_B) {
			progAddr += MY_FLASH_BANK_SIZE;
			if (progAddr >= (GET_VIRTUAL_ADDRESS(FLASH_BANK_B_ADDR)+APP_PRG_SIZE) || progAddr < GET_VIRTUAL_ADDRESS(FLASH_BANK_B_ADDR)) {
				PRINT_ERR("ERROR!! Bootloader_WriteBinRecord..invalid progAddr %X", progAddr);
				return BT_ERR;
			}
		}
#endif
		for (i = 0; i < FLASH_SECTOR_SIZE; i += FLASH_PAGE_SIZE) {
		    ret = Bootloader_ProgramPage((progAddr + i), &buffProg[i]);
			if (ret != BT_OK) {
	        	PRINT_ERR("ERROR! Bootloader_WriteHexRecordRemained..Bootloader_ProgramPage %x", bootloaderData.sectStartVirAddr);
			}
	        bootloaderData.prgLen += FLASH_PAGE_SIZE;
	        if (bootloaderData.prgLen >= bootloaderData.appLen) {
	        	break;
	        }
		}
	}
#if 1 //bug fix, nsmoon@200527
    if (bootloaderData.prgLen < bootloaderData.appLen) {
    	progAddr = bootloaderData.sectStartVirAddr + FLASH_SECTOR_SIZE;
    	bootloaderData.curSectNum = progAddr / FLASH_SECTOR_SIZE;

    	uint32_t curSectNum = bootloaderData.curSectNum;
		PRINT_HEX("Erase.1. %x (%x)\r\n", (unsigned int)curSectNum, (unsigned int)progAddr);
#ifdef FLASH_DUAL_BANK //nsmoon@200717
		if (flash_bank == FLASH_BANK_B) {
			curSectNum += NUM_OF_BANK_SECTOR;
			if (curSectNum >= (BANK_B_SECTOR_START + NUM_OF_APP_SECTOR) || curSectNum < BANK_B_SECTOR_START) {
				PRINT_ERR("ERROR!! Bootloader_WriteBinRecord..invalid curSectNum %x", curSectNum);
				return BT_ERR;
			}
		}
#endif
    	ret = Bootloader_FlashErase(curSectNum); //nsmoon@200724
    	if (ret != BT_OK) {
            PRINT_ERR("ERROR! Bootloader_FlashErase.. %d (%x)", (unsigned int)bootloaderData.curSectNum, (unsigned int)progAddr);
    	}
    	len = bootloaderData.appLen - bootloaderData.prgLen;
    	PRINT_HEX("Write.2. %x (%d)", progAddr, len);
    	while (len > 0) {
#ifdef FLASH_DUAL_BANK //nsmoon@200717
			if (flash_bank == FLASH_BANK_B) {
				progAddr += MY_FLASH_BANK_SIZE;
				if (progAddr >= (GET_VIRTUAL_ADDRESS(FLASH_BANK_B_ADDR)+APP_PRG_SIZE) || progAddr < GET_VIRTUAL_ADDRESS(FLASH_BANK_B_ADDR)) {
					PRINT_ERR("ERROR!! Bootloader_WriteBinRecord..invalid progAddr %X", progAddr);
					return BT_ERR;
				}
			}
#endif
    		PRINT_HEX("..Write.2a. %x (%d)", progAddr, len);
    	    ret = Bootloader_ProgramPage(progAddr, &buffProg[i]);
    		if (ret != BT_OK) {
            	PRINT_ERR("ERROR! Bootloader_WriteHexRecordRemained..Bootloader_ProgramPage %x", bootloaderData.sectStartVirAddr);
    		}
            len -= FLASH_PAGE_SIZE;
    		progAddr += FLASH_PAGE_SIZE;
    		i += FLASH_PAGE_SIZE;
    	}
    }
#endif
	return ret;
}

ATTRIBUTE_RAMFUNC
static int Bootloader_WriteBinRecord(uint32_t curVirAddr__, uint8_t *Data, int len)
{
	uint8_t *buffProg = (uint8_t *)&s_prog_buffer[0];
	//uint32_t curVirAddr = phyAddr - FLASH_BASE_ADDR;
	uint32_t curVirAddr = BOOTLOADER_SIZE + curVirAddr__;
	//uint32_t curSectNum = curVirAddr / FLASH_SECTOR_SIZE;
	uint32_t curSectIdx, curVirAddr2;
	bool ret = BT_OK;
	int i, diffVirAddr;
	uint32_t progAddr;

    if (curVirAddr != 0 && curVirAddr <= bootloaderData.preVirAddr) {
		PRINT_ERR("ERROR! Bootloader_WriteHexRecord..invalid curAddress %x (%x)", (unsigned int)curVirAddr, (unsigned int)bootloaderData.preVirAddr);
    }
	bootloaderData.preVirAddr = curVirAddr;
	bootloaderData.curSectNum = curVirAddr / FLASH_SECTOR_SIZE;
	curSectIdx = curVirAddr % FLASH_SECTOR_SIZE;

	if (bootloaderData.curSectNum > bootloaderData.preSectNum) {
		uint32_t curSectNum = bootloaderData.curSectNum;
		PRINT_HEX("Erase.2. %x (%x)\r\n", (unsigned int)curSectNum, (unsigned int)curVirAddr);
#ifdef FLASH_DUAL_BANK //nsmoon@200717
		if (flash_bank == FLASH_BANK_B) {
			curSectNum += NUM_OF_BANK_SECTOR;
			if (curSectNum >= (BANK_B_SECTOR_START + NUM_OF_APP_SECTOR) || curSectNum < BANK_B_SECTOR_START) {
				PRINT_ERR("ERROR!! Bootloader_WriteBinRecord..invalid curSectNum %x", curSectNum);
				return BT_ERR;
			}
		}
#endif
		ret = Bootloader_FlashErase(curSectNum); //nsmoon@200724
		if (ret != BT_OK) {
        	PRINT_ERR("ERROR! Bootloader_FlashErase.. %d (%x)", (unsigned int)bootloaderData.curSectNum, (unsigned int)curVirAddr);
		}

		if (bootloaderData.sectStartVirAddr > 0) {
			PRINT_HEX("Write.1. %x (%d)", bootloaderData.sectStartVirAddr, FLASH_SECTOR_SIZE);
			progAddr = bootloaderData.sectStartVirAddr;
#ifdef FLASH_DUAL_BANK //nsmoon@200717
			if (flash_bank == FLASH_BANK_B) {
				progAddr += MY_FLASH_BANK_SIZE;
				if (progAddr >= (GET_VIRTUAL_ADDRESS(FLASH_BANK_B_ADDR)+APP_PRG_SIZE) || progAddr < GET_VIRTUAL_ADDRESS(FLASH_BANK_B_ADDR)) {
					PRINT_ERR("ERROR!! Bootloader_WriteBinRecord..invalid progAddr %X", progAddr);
					return BT_ERR;
				}
			}
#endif
			for (i = 0; i < FLASH_SECTOR_SIZE; i += FLASH_PAGE_SIZE) {
			    ret = Bootloader_ProgramPage((progAddr + i), &buffProg[i]);
				if (ret != BT_OK) {
		        	PRINT_ERR("ERROR! Bootloader_ProgramPage.. %x", (progAddr + i));
				}
			}
	        bootloaderData.prgLen += FLASH_SECTOR_SIZE;
		}
		memset(&buffProg[0], 0xFF, FLASH_SECTOR_SIZE);
		bootloaderData.preSectNum = bootloaderData.curSectNum;
		curVirAddr2 = (bootloaderData.curSectNum * FLASH_SECTOR_SIZE);
		if (curVirAddr2 != curVirAddr) {
			PRINT_HEX("curVirAddr is not aligned: %x(%x)", curVirAddr, curVirAddr2);
        	diffVirAddr = curVirAddr - curVirAddr2;
        	if (diffVirAddr > 0) {
        		memcpy(&buffProg[0], &buffProg[FLASH_SECTOR_SIZE], diffVirAddr);
        		memset(&buffProg[FLASH_SECTOR_SIZE], 0xFF, diffVirAddr);
        	}
        	bootloaderData.sectStartVirAddr = curVirAddr2;
		}
		else {
		    bootloaderData.sectStartVirAddr = curVirAddr;
		}
	}
	memcpy(&buffProg[curSectIdx], (void *)Data, len);
	bootloaderData.appLen += len;
	return ret;
}

ATTRIBUTE_RAMFUNC
static int Bootloader_WriteEeprom(int erase)
{
	int ret = BT_OK;
	uint8_t *buffProg = (uint8_t *)&header[0];
#ifdef FLASH_DUAL_BANK //nsmoon@200717
	uint32_t phyAddr;
	if (flash_bank != FLASH_BANK_B) {
		PRINT_ERR("ERROR! Bootloader_WriteEeprom..invalid flash_bank %d", flash_bank);
		return BT_ERR;
	}
    phyAddr = EEPROM_BASE_ADDRESS_B;
#else
	uint32_t phyAddr = BOOTLOADER_EEPROM_BASE_ADDRESS;
#endif
	uint32_t curVirAddr = phyAddr - FLASH_BASE_ADDR;
	uint32_t curSectNum = curVirAddr / FLASH_SECTOR_SIZE;

	if (erase) {
		get_eeprom_dev(FLASH_BANK_A); //nsmoon@230503 //copy header info
		PRINT_EEPROM("Erase.3. %d %d (%x/%x)\r\n", flash_bank, (unsigned int)curSectNum, (unsigned int)curVirAddr, (unsigned int)phyAddr);
		ret = Bootloader_FlashErase(curSectNum); //only 1-page is used, even if erase sector
		if (ret != BT_OK) {
			PRINT_ERR("ERROR! Bootloader_WriteEeprom..Bootloader_FlashErase %d", curSectNum);
		}
	}
	else {
		PRINT_EEPROM("Write.3. %x\r\n", curVirAddr);
#if 1 //nsmoon@230503
		if (bootloaderData.modeChange == BE_BT_MODE) {
			copy_eeprom_bin(3); //copy frontend header
		}
		else {
			copy_eeprom_bin(0); //copy backend header
			copy_eeprom_bin(1);
			copy_eeprom_bin(2);
		}
#endif
		ret = Bootloader_ProgramPage(curVirAddr, buffProg);
		if (ret != BT_OK) {
			PRINT_ERR("ERROR! Bootloader_WriteEeprom..Bootloader_ProgramPage %x", curVirAddr);
		}
	}

	return ret;
}


#if 0 //for test
static void Bootloader_SoftwareReset(void)
{
  /* Generic way to request a reset from software for ARM Cortex */
  /* See https://community.freescale.com/thread/99740
     To write to this register, you must write 0x5FA to the VECTKEY field, otherwise the processor ignores the write.
     SYSRESETREQ will cause a system reset asynchronously, so need to wait afterwards.
   */
    NVIC_SystemReset();
}

static void Bootloader_Interrupt_enable(bool enable)
{
    if (enable) {
    	__enable_irq();
    }
    else {
    	__disable_irq();
    }
}
#endif

#if USE_FW_KEY_VALIDATION
ATTRIBUTE_RAMFUNC
static int is_key_matched(header0_t *hd0)
{
	//check key0, key1
	encrypt_ecb(&tmpKey0[0], &hd0->all[0], HD_KEY0_SIZE);
	encrypt_ecb(&tmpKey1[0], &hd0->all[HD_KEY0_SIZE], HD_KEY1_SIZE);
	if (memcmp(&tmpKey0[0], &hd0->ind.key0[0], HD_KEY0_SIZE) == 0 &&
		memcmp(&tmpKey1[0], &hd0->ind.key1[0], HD_KEY1_SIZE) == 0) {
		return 1; //matched
	}
	return 0; //mismatched
}
#endif

#if PROGRAM_BIN_FILE //nsmoon@191118
ATTRIBUTE_RAMFUNC
static int Bootloader_ProgramBinFile(uint8_t* BinRecord, int32_t totalLen)
{
	uint16_t *seq = (uint16_t *)&BinRecord[1];
	//PRINT_HEX("seqNoPkt: %d", *seq);
	if (*seq != BinRecordSt.seqNoPkt) {
		PRINT_ERR("ERROR! invalid *seq: %d %d", *seq, BinRecordSt.seqNoPkt);
		return BIN_INVALID_SEQ;
	}

	if (BinRecordSt.hdIdx < MAX_HEADER_SIZE) {
		//header packet
		memcpy((void *)&header[BinRecordSt.hdIdx], (void *)&BinRecord[3], PROGRAM_PACKET_SIZE);
		BinRecordSt.hdIdx += PROGRAM_PACKET_SIZE;
		if (BinRecordSt.hdIdx >= MAX_HEADER_SIZE) {
#if 0 //for debugging
		    int i;
		    PRINTF("BT_Header... %d \r\n", BinRecordSt.hdIdx);
		    //for (i = 0; i < HD_0_SIZE + HD_1_SIZE; i++)
		    for (i = 0; i < MAX_HEADER_SIZE; i++)
		    {
			    PRINTF("%02X ", header[i]);
			    if (((i+1)%16) == 0) PRINTF("\r\n");
		    }
		    PRINTF("\r\n");
#endif
            memcpy((void *)&header0.all[0], (void *)&header[0], HD_0_SIZE);
            memcpy((void *)&header1.all[0], (void *)&header[HD_0_SIZE], HD_1_SIZE);
            if (strcmp((char *)&header0.ind.modelName[0], (char *)MODEL_NAME) == 0) {
            	BinRecordSt.modelOk = BT_OK;
            }
            else {
            	PRINT_ERR("ERROR! invalid modelName: (%s)(%s)", (char *)&header0.ind.modelName[0], (char *)MODEL_NAME);
            	BinRecordSt.modelOk = BT_ERR;
        		return BIN_INVALID_MODEL;
            }
            uint16_t crc = Bootloader_CalculateCrc((uint8_t *)&header[0], (HD_0_SIZE + HD_1_SIZE - 2));
            if (BinRecordSt.modelOk == BT_OK && crc == header1.ind.hd01_crc) {
            	BinRecordSt.hdCrcOk = BT_OK;
#ifdef FLASH_DUAL_BANK //nsmoon@200717
#if (FLASH_DUAL_BANK_TYPE == 1)
				uint8_t verNo = (uint8_t)header0.ind.code_misc;
                if ((verNo % 2) == 0) {
                	flash_bank = FLASH_BANK_B;
                }
                else {
                	flash_bank = FLASH_BANK_A;
                }
#elif (FLASH_DUAL_BANK_TYPE == 2)
            	flash_bank = FLASH_BANK_B; //always
#endif
#endif
    	        Bootloader_WriteEeprom(1); //1:erase-only
            	if (BinRecordSt.binProgLen == MAX_HEADER_SIZE) {
        	        //write eeprom only
            		Bootloader_WriteEeprom(0); //0:write
#ifdef FLASH_DUAL_BANK //nsmoon@200717
#if (FLASH_DUAL_BANK_TYPE == 2)
					get_eeprom_dev(FLASH_BANK_B);
					CopyFlashBank(0); //0:eeprom only
					flash_bank = FLASH_BANK_B;
#endif
#endif
        		    bootloaderData.progStart = true; //reset
        		    bootloaderData.writeCode = false; //reset
            	}
            	else {
	                Bootloader_WriteBinRecord(BinRecordSt.binProgAddr, &header[MAX_HEADER_SIZE], (BinRecordSt.hdIdx - MAX_HEADER_SIZE + 1));
            	}
            }
            else {
            	PRINT_ERR("ERROR! invalid hd01_crc: %x %x", crc, header1.ind.hd01_crc);
            	BinRecordSt.hdCrcOk = BT_ERR;
        		return BIN_REC_CRC_ERROR;
            }
#if USE_FW_KEY_VALIDATION
            if (is_key_matched(&header0) == 0) {
            	PRINT_ERR("ERROR! MISMATCH KEY~~~~");
            	BinRecordSt.hdCrcOk = BT_ERR;
            	return BIN_MISMATCH_KEY;
            }
#endif
        	BinRecordSt.binProgAddr += BinRecordSt.hdIdx - MAX_HEADER_SIZE;
		}
	}
	else {
		//code packet
		if (BinRecordSt.modelOk == BT_OK && BinRecordSt.hdCrcOk == BT_OK) {
    	    Bootloader_WriteBinRecord(BinRecordSt.binProgAddr, (void *)&BinRecord[3], PROGRAM_PACKET_SIZE);
		}
    	BinRecordSt.binProgAddr += PROGRAM_PACKET_SIZE;
		if (BinRecordSt.binProgAddr >= header0.ind.code_size) {
			if (BinRecordSt.modelOk == BT_OK && BinRecordSt.hdCrcOk == BT_OK) {
			    Bootloader_WriteHexRecordRemained();
			    Bootloader_WriteEeprom(0); //0:write-only //nsmoon@200609
#ifdef FLASH_DUAL_BANK //nsmoon@200717
#if (FLASH_DUAL_BANK_TYPE == 2)
			    get_eeprom_dev(FLASH_BANK_B);
			    CopyFlashBank(1);
				flash_bank = FLASH_BANK_B;
#endif
#endif
			}
		    bootloaderData.progStart = true; //reset
		    bootloaderData.writeCode = false; //reset
		    PRINT_HEX("(3)writeCode=%d\r\n", bootloaderData.writeCode);
		}
	}

    BinRecordSt.seqNoPkt = *seq + 1;
    return BIN_REC_NORMAL;
}
#endif

#if (APP_TEST_FLASH_ENABLE == 1)
ATTRIBUTE_RAMFUNC
void Bootloader_test_flash(void)
{
    PRINT_ERR("Bootloader_test_flash..");
    testIAP();
}
#endif

ATTRIBUTE_RAMFUNC
static void Bootloader_ProcessBuffer(BOOTLOADER_DATA *handle)
{
    uint8_t Cmd;
    uint32_t Address;
    uint32_t Length;
    uint16_t crcHd, crcCode;
    int ret;

#if 0 //for debugging
    int i;
    PRINTF("Bootloader_ProcessBuffer...\r\n");
    for (i = 0; i < USB_HID_GENERIC_OUT_BUFFER_LENGTH; i++) {
	    PRINTF("%02X ", bootloaderData.buff2[i]);
	    if (((i+1)%16) == 0) PRINTF("\r\n");
    }
    PRINTF("\r\n");
#endif

    /* First, check that we have a valid command. */
    Cmd = handle->buff2[0];

    /* Build the response frame from the command. */
    handle->bufferSize = 0;
    handle->buff1[handle->bufferSize++] = handle->buff2[0];
    bootloaderData.readRaw = false;
    //bootloaderData.writeRaw = false;

    switch (Cmd)
    {
        case MODE_CHANGE:
        {
        	PRINT_CMD("MODE_CHANGE..%d", handle->buff2[1]);
#ifdef USB_HOST_FOR_FRONTEND
            if (handle->buff2[1] == FE_BT_MODE && USB_HostIsReady() != USB_HOST_OK) {
                handle->buff1[handle->bufferSize++] = BT_ERR;
            }
            else {
                handle->buff1[handle->bufferSize++] = BT_OK;
            }
#else //nsmoon@201221
#if 0
        	if (handle->buff2[1] == FE_BT_MODE) {
        		handle->buff1[handle->bufferSize++] = BT_ERR;
        	}
        	else {
                handle->buff1[handle->bufferSize++] = BT_OK;
        	}
#else
        	handle->buff1[handle->bufferSize++] = BT_OK;
#endif
#endif
            handle->buff1[handle->bufferSize++] = handle->buff2[1];
        }
            break;

        case MCU_RESET:
        {
        	PRINT_INFO("~%d", bootloaderData.modeChange);
#ifdef USB_HOST_FOR_FRONTEND
#if defined(FRONTEND_RESET_ENABLE)
#if 1 //nsmoon200807
            handle->buff1[handle->bufferSize++] = BT_OK;
            DoSoftReset(3);
#else
            //s_frontend_Reset(); //nsmoon@200728 move to main
            MY_WDOG_soft_reset();
            while(1);
#endif
#else
            USB_HostCmd(MCU_RESET_REAL);
            g_HostHidGeneric.softReset = 1;
            handle->buff1[handle->bufferSize++] = BT_OK;
#endif
#else
            handle->buff1[handle->bufferSize++] = BT_OK;
            DoSoftReset(3);
#endif
        }
            break;

        case READ_BOOT_INFO:
        {
        	PRINT_CMD("READ_BOOT_INFO");
#if USB_HOST_FOR_FRONTEND
            snprintf(BootInfo, MAX_BOOT_INFO, "%s (%s)", BOOTLOADER_VERSION, MODEL_NAME);
#else
            snprintf(BootInfo, MAX_BOOT_INFO, "%s (%s)", BOOTLOADER_VERSION, MODEL_NAME_VIEW);
#endif
            int tmpLen = strlen(BootInfo);
            bootloaderData.connected_pc_tool = PC_CONN;
#ifdef CYCCNT_ENABLE			//YJ@230703
            APP_TIME_ADD(0);
            APP_TIME_PRINT();
#endif
            handle->buff1[handle->bufferSize++] = BT_OK;
#if USB_HOST_FOR_FRONTEND
            handle->buff1[handle->bufferSize++] = bootloaderData.modeChange;
#else
            handle->buff1[handle->bufferSize++] = 0;
#endif
            handle->buff1[handle->bufferSize++] = tmpLen;
            memcpy(&handle->buff1[handle->bufferSize], &BootInfo[0], tmpLen);
            handle->bufferSize += tmpLen;
#ifdef FLASH_DUAL_BANK //nsmoon@200717
#if (FLASH_DUAL_BANK_TYPE == 1)
        	SelectFlashBank(FLASH_BANK_FOR_BOOT);
#elif (FLASH_DUAL_BANK_TYPE == 2)
            flash_bank = FLASH_BANK_A; //always
#endif
        	//PRINTF("[%d", flash_bank);
#endif
        }
            break;

        case ERASE_FLASH:
        {
        	PRINT_CMD("ERASE_FLASH");
#if 1 //nsmoon@200703
            Bootloader_FlashInit();
#endif
#ifdef FLASH_DUAL_BANK //nsmoon@200717
#if (FLASH_DUAL_BANK_TYPE == 1)
            SelectFlashBank(FLASH_BANK_FOR_PROGRAM);
#elif (FLASH_DUAL_BANK_TYPE == 2)
            flash_bank = FLASH_BANK_B; //always
#endif
        	PRINTF("{%d", flash_bank);
#endif
            //ret = Bootloader_FlashErase_All();
        	ret = Bootloader_FlashErase_All(FLASH_BANK_A); //erase bank A only, for recover test

            handle->buff1[handle->bufferSize++] = (uint8_t)ret;
        }
            break;

        case ERASE_BE_BOOT:
        {
        	PRINT_CMD("ERASE_BE_BOOT");
#if 1 //nsmoon@200703
            Bootloader_FlashInit();
#endif
            ret = Bootloader_FlashErase_Boot();

            handle->buff1[handle->bufferSize++] = (uint8_t)ret;
        }
            break;

        case PROGRAM_FLASH:
        {
#if PROGRAM_BIN_FILE //nsmoon@191118
        	//PRINT_CMD("PROGRAM_FLASH");
        	ret = 0;
        	if (bootloaderData.progStart) {
        		bootloaderData.progStart = false;
                Bootloader_Program_Init();
                Bootloader_Program_BinInit((uint32_t *)&handle->buff2[1]);
#if 1 //nsmoon@200703
                Bootloader_FlashInit();
#endif
                PRINT_HEX("BinRecordSt.binProgLen=%d", BinRecordSt.binProgLen);
                handle->buff1[handle->bufferSize++] = BT_OK;
        	}
        	else {
        		ret = Bootloader_ProgramBinFile(&handle->buff1[0], USB_HID_GENERIC_OUT_BUFFER_LENGTH);
                if(ret != BIN_REC_NORMAL)
                {
                	PRINT_ERR("ERROR! PROGRAM_FLASH.. %d", ret);
        		    bootloaderData.progStart = true; //reset
        		    bootloaderData.writeCode = false; //reset
                    handle->buff1[handle->bufferSize++] = BT_ERR;
                }
                else {
                    handle->buff1[handle->bufferSize++] = BT_OK;
                }
        	}
        	handle->buff1[handle->bufferSize++] = (uint8_t)ret;
            handle->currentState = BOOTLOADER_SEND_RESPONSE;
#endif
        }
            break;

        case READ_CRC:
        {
        	PRINT_CMD("READ_CRC");
#ifdef FLASH_DUAL_BANK //nsmoon@200717
        	//uint8_t verNo = get_eeprom_dev(flash_bank);
        	get_eeprom_dev(flash_bank);
            crcHd = Bootloader_CalculateCrc(header2, (HD_0_SIZE + HD_1_SIZE - 2));
            if (header20.ind.code_size < APP_PRG_SIZE) {
                __disable_irq();
            	//SCB_CleanDCache();
            	SCB_CleanInvalidateDCache();
        	    if (flash_bank == FLASH_BANK_B) {
                    crcCode = Bootloader_CalculateCrc((uint8_t *)FLASH_BANK_B_ADDR, header20.ind.code_size);
        	    }
        	    else {
                    crcCode = Bootloader_CalculateCrc((uint8_t *)FLASH_BANK_A_ADDR, header20.ind.code_size);
        	    }
        	    __enable_irq();
            }
            else {
            	crcCode = crcHd + 0x10; //make crc error
            }
            PRINT_CMD("READ_CRC= %X/%X %X/%X (%d)\r\n", crcCode, header20.ind.code_crc, crcHd, header21.ind.hd01_crc, header20.ind.code_size);
#else
        	memcpy(&header[0], (uint8_t *)BOOTLOADER_EEPROM_BASE_ADDRESS, HD_0_SIZE + HD_1_SIZE);
            memcpy(&header0.all[0], &header[0], HD_0_SIZE);
        	memcpy(&header1.all[0], &header[HD_0_SIZE], HD_1_SIZE);
            crcHd = Bootloader_CalculateCrc(header, (HD_0_SIZE + HD_1_SIZE - 2));
            if (header0.ind.code_size < APP_PRG_SIZE) {
        	    SCB_CleanDCache();
                crcCode = Bootloader_CalculateCrc((uint8_t *)BOOTLOADER_FLASH_BASE_ADDRESS, header0.ind.code_size);
            }
            else {
            	crcCode = crcHd + 0x10; //make crc error
            }
            PRINT_CMD("READ_CRC %X/%X %X/%X (%d)\r\n", crcCode, header0.ind.code_crc, crcHd, header1.ind.hd01_crc, header0.ind.code_size);
#endif
       	    handle->buff1[handle->bufferSize++] = BT_OK;
            *(uint16_t *)&handle->buff1[handle->bufferSize] = crcHd;
            handle->bufferSize += 2;
            *(uint16_t *)&handle->buff1[handle->bufferSize] = crcCode;
            handle->bufferSize += 2;
        }
            break;

        case JMP_TO_APP:
        {
        	PRINT_CMD("JMP_TO_APP");
            handle->currentState = BOOTLOADER_CLOSE_DATASTREAM;
        }
            break;

        case READ_FLASH:
        {
        	PRINT_CMD("READ_FLASH");
        	Address = *(uint32_t *)&handle->buff2[1];
        	Length = *(uint32_t *)&handle->buff2[5];
#ifdef FLASH_DUAL_BANK //nsmoon@200717
#if (FLASH_DUAL_BANK_TYPE == 1)
        	if (flash_bank == FLASH_BANK_B) {
        		Address += MY_FLASH_BANK_SIZE;
        	}
#endif
        	PRINT_DFD("READ_FLASH.. %d,%x,%x", flash_bank, Address, Length);
#endif
            __disable_irq();
        	//SCB_CleanDCache();
        	SCB_CleanInvalidateDCache();
            memcpy((uint8_t *)&handle->buff1[0], (uint8_t *)Address, Length);
            __enable_irq();
            handle->bufferSize = Length;
            bootloaderData.readRaw = true;
        }
            break;

        default:
        {
        	PRINT_CMD("UNKNOWN CMD");
#define UNKNOWN_CMD_RES_SIZE	5
            memset((void *)&handle->buff1[handle->bufferSize], 0xFF, UNKNOWN_CMD_RES_SIZE);
            handle->bufferSize += UNKNOWN_CMD_RES_SIZE;
        }
            break;
    }
    handle->currentState = BOOTLOADER_SEND_RESPONSE;
}

#if 0
static int StringToHexa(const char *param)
{
    int hex = 0;
    int count = strlen(param), i = 0;

    for(i = 0; i < count; i++){
        if(*param>= '0' && *param<= '9') hex = hex *16 + *param- '0';
        else if(*param>= 'A' && *param<= 'F') hex = hex *16 + *param- 'A' + 10;
        else if(*param>= 'a' && *param<= 'f') hex = hex *16 + *param- 'a' + 10;
        param++;
    }
    return hex;
}
#endif

ATTRIBUTE_RAMFUNC
static char frontend_program_hexrecord(uint8_t* HexRecord, int32_t totalLen)
{
    static T_HEX_RECORD HexRecordSt;
    uint8_t Checksum = 0;
    uint32_t i;
    uint32_t WrData;
    void* ProgAddress;
    uint32_t nextRecStartPt = 0;

    // A hex record must be at least 5 bytes. (1 Data Len byte + 1 rec type byte+ 2 address bytes + 1 crc)
    while(totalLen>=5)
    {
        HexRecord = &HexRecord[nextRecStartPt];
        HexRecordSt.RecDataLen = HexRecord[0];
        HexRecordSt.RecType = HexRecord[3];
        HexRecordSt.Data = &HexRecord[4];
        nextRecStartPt = HexRecordSt.RecDataLen + 5; //Determine next record starting point.
        totalLen = totalLen - nextRecStartPt; // Decrement total hex record length by length of current record.

        // Hex Record checksum check.
        Checksum = 0;
        for (i = 0; i < HexRecordSt.RecDataLen + 5; i++) {
            Checksum += HexRecord[i];
        }

        if (Checksum != 0) {
            return HEX_REC_CRC_ERROR;
        }
        else {
            // Hex record checksum OK.
            switch (HexRecordSt.RecType)
            {
                case DATA_RECORD:  //Record Type 00, data record.
                    HexRecordSt.Address = (HexRecord[1]<<8) + HexRecord[2];
                    HexRecordSt.Address = HexRecordSt.Address + HexRecordSt.ExtLinAddress + HexRecordSt.ExtSegAddress; // Derive the address.
                    while(HexRecordSt.RecDataLen) //Loop till all bytes are done.
                    {
                    	// Convert the Physical address to Virtual address.
                        ProgAddress = (void *)FE_PA_TO_KVA0(HexRecordSt.Address);
                        // Make sure we are not writing boot area and device configuration bits.
                        if(((ProgAddress >= (void *)FE_APPLICATION_START) && (ProgAddress < (void *)FE_APPLICATION_END)))
                        {
                            if(HexRecordSt.RecDataLen < 4)
                            {
                                // Sometimes record data length will not be in multiples of 4. Appending 0xFF will make sure that..
                                // we don't write junk data in such cases.
                                WrData = 0xFFFFFFFF;
                                memcpy(&WrData, HexRecordSt.Data, HexRecordSt.RecDataLen);
                            }
                            else
                            {
                                memcpy(&WrData, HexRecordSt.Data, 4);
                            }
                            // Write the data into flash.
                            //APP_NVMWordWrite(ProgAddress, WrData);
                            HexRecordSt.Address += 4; //Increment the address.
                            HexRecordSt.Data += 4; //Increment the data pointer.

                            if(HexRecordSt.RecDataLen > 3) {
                                HexRecordSt.RecDataLen -= 4; //Decrement data len.
                            }
                            else {
                                HexRecordSt.RecDataLen = 0;
                            }
                        }
                        else {   //Out of boundaries. Adjust and move on.
                            HexRecordSt.Address += 4; //Increment the address.
                            HexRecordSt.Data += 4; //Increment the data pointer.
                            if(HexRecordSt.RecDataLen > 3) {
                                HexRecordSt.RecDataLen -= 4; //Decrement data len.
                            }
                            else {
                                HexRecordSt.RecDataLen = 0;
                            }
                        }
                    }
                    break;

                case EXT_SEG_ADRS_RECORD:  // Record Type 02, defines 4th to 19th bits of the data address.
                    HexRecordSt.ExtSegAddress = (HexRecordSt.Data[0]<<12) + (HexRecordSt.Data[1]<<4);
                    HexRecordSt.ExtLinAddress = 0; // Reset linear address.
                    break;

                case EXT_LIN_ADRS_RECORD:   // Record Type 04, defines 16th to 31st bits of the data address.
                    HexRecordSt.ExtLinAddress = (HexRecordSt.Data[0]<<24) + (HexRecordSt.Data[1]<<16);
                    HexRecordSt.ExtSegAddress = 0; // Reset segment address.
                    break;

                case END_OF_FILE_RECORD:  //Record Type 01, defines the end of file record.
                    HexRecordSt.ExtSegAddress = 0;
                    HexRecordSt.ExtLinAddress = 0;
                    break;

                default:
                    HexRecordSt.ExtSegAddress = 0;
                    HexRecordSt.ExtLinAddress = 0;
                    break;
            }
        }
    } //while(1)

    if ( (HexRecordSt.RecType == DATA_RECORD) || (HexRecordSt.RecType == EXT_SEG_ADRS_RECORD)
            || (HexRecordSt.RecType == EXT_LIN_ADRS_RECORD) || (HexRecordSt.RecType == END_OF_FILE_RECORD) ) {
        return HEX_REC_NORMAL;
    }
    else {
        return HEX_REC_UNKNOW_TYPE;
    }
}

static void get_fw_version(char *src, char *dst, int len)
{
    int i, found, cnt;

    cnt = found = 0;
    for (i = 0; i <= strlen(src); i++)
    {
        if (src[i] == '_') {
            found++;
            if (found == 2) {
                if (cnt >= len) {
                    cnt = len - 1;
                }
                dst[cnt] = '\0';
                break;
            }
            continue;
        }
        if (found) {
            if (cnt < len) {
                dst[cnt++] = src[i];
            }
        }
    }
}

static void frontend_fwinfo(uint8_t *buff)
{
    char *pCur, *pPrev;
    int idx = 0, len = 0, lenTot = 0, lenTmp;
    char *fwfileText;
    char *timestampText;
    char *modelText;
    char *prgLenText;
    char *prgCrcText;
    char *fwVer;
    unsigned short *fe_crc;
    unsigned int *fe_len;

    pCur = (char *)buff;
    fwfileText = &frontend_fwfileText[0];
    timestampText = &frontend_timestampText[0];
    modelText = &frontend_modelText[0];
    prgLenText = &frontend_prgLenText[0];
    prgCrcText = &frontend_prgCrcText[0];
    fwVer = &frontend_fwVersion[0];
    fe_crc = &frontend_hexCrc;
    fe_len = &frontend_hexProgLen;

    buff[FE_HEADER_SIZE-1] = '\0'; //nsmoon@200717
    PRINT_CMD("buff=[%s]", buff);

    fwfileText[0] = '\0';
    timestampText[0] = '\0';
    modelText[0] = '\0';
    prgLenText[0] = '\0';
    prgCrcText[0] = '\0';

    while (1)
    {
        if (lenTot > 0) {
            pCur++; //skip ','
        }
        pPrev = pCur;
        pCur = strchr((const char *)&buff[lenTot], ',');
        if (pCur == NULL) {
                break;
        }
        len = (int)(pCur - pPrev);
        lenTot += len + 1;

        switch (idx) {
        case 0: //Firmware File Name
        {
            if (len > (FE_FWFILE_SIZE - 1)) {
            	PRINT_CMD("ERROR! show_frontend_fwinfo..invalid fwfileText[] len: %d", len);
                lenTmp = (FE_FWFILE_SIZE - 1);
            }
            else {
                lenTmp = len;
            }
            memcpy((char *)&fwfileText[0], (char *)(pPrev), lenTmp);
            fwfileText[lenTmp] = '\0';
        }
        break;

        case 1: //Timestamp
        {
            if (len > (FE_TIMESTAMP_SIZE - 1)) {
            	PRINT_CMD("ERROR! show_frontend_fwinfo..invalid timestampText[] len: %d", len);
                lenTmp = (FE_TIMESTAMP_SIZE - 1);
            }
            else {
                lenTmp = len;
            }
            memcpy((char *)&timestampText[0], (char *)(pPrev), lenTmp);
            timestampText[lenTmp] = '\0';
        }
        break;

        case 2: //FE Model Name
        {
            if (len > (FE_MODEL_SIZE - 1)) {
            	PRINT_CMD("ERROR! show_frontend_fwinfo..invalid modelText[] len: %d", len);
                lenTmp = (FE_MODEL_SIZE - 1);
            }
            else {
                lenTmp = len;
            }
            memcpy((char *)&modelText[0], (char *)(pPrev), lenTmp);
            modelText[lenTmp] = '\0';
        }
        break;

        case 3: //FE Program Len
        {
            if (len > (FE_PRGLEN_SIZE - 1)) {
            	PRINT_CMD("ERROR! show_frontend_fwinfo..invalid prgLenText[] len: %d", len);
                lenTmp = (FE_PRGLEN_SIZE - 1);
            }
            else {
                lenTmp = len;
            }
            memcpy((char *)&prgLenText[0], (char *)(pPrev), lenTmp);
            prgLenText[lenTmp] = '\0';
        }
        break;

        case 4: //FE Program CRC
        {
            if (len > (FE_PRGCRC_SIZE - 1)) {
            	PRINT_CMD("ERROR! show_frontend_fwinfo..invalid prgCrcText[] len: %d", len);
                lenTmp = (FE_PRGCRC_SIZE - 1);
            }
            else {
                lenTmp = len;
            }
            memcpy((char *)&prgCrcText[0], (char *)(pPrev), lenTmp);
            prgCrcText[lenTmp] = '\0';
        }
        break;
        default:
        	PRINT_CMD("ERROR! show_frontend_fwinfo..invalid idx: %d", idx);
            break;
        }
        idx++;
    }

    PRINT_CMD(".File Name= [%s]", fwfileText);
    PRINT_CMD(".Timestamp= [%s]", timestampText);
    PRINT_CMD(".Model= [%s]", modelText);
    PRINT_CMD(".Prg Len= [%s]", prgLenText);
    PRINT_CMD(".Prg CRC= [%s]", prgCrcText);

    *fe_len = (uint32_t)strtol(prgLenText, NULL, 16);
    *fe_crc = (unsigned short)strtol(prgCrcText, NULL, 16);
    PRINT_CMD("fe_len,fe_crc= %X, %X", *fe_len, *fe_crc);

    get_fw_version(fwfileText, fwVer, MAX_VERSION_LEN);
    PRINT_CMD("...FE Version= [%s]", fwVer);
}


ATTRIBUTE_RAMFUNC
static void Bootloader_ProcessBuffer_FE(BOOTLOADER_DATA *handle)
{
    uint8_t Cmd;
    //uint16_t crcCode;

#if 0 //for debugging
    int i;
    PRINTF("Bootloader_ProcessBuffer...\r\n");
    for (i = 0; i < USB_HID_GENERIC_OUT_BUFFER_LENGTH; i++) {
	    PRINTF("%02X ", bootloaderData.buff2[i]);
	    if (((i+1)%16) == 0) PRINTF("\r\n");
    }
    PRINTF("\r\n");
#endif

    /* First, check that we have a valid command. */
    Cmd = handle->buff2[0];

    /* Build the response frame from the command. */
    handle->bufferSize = 0;
    handle->buff1[handle->bufferSize++] = handle->buff2[0];
    bootloaderData.readRaw = false;
    //bootloaderData.writeRaw = false;

    switch (Cmd)
    {
        case MODE_CHANGE:
        {
        	PRINT_CMD("FE_MODE_CHANGE..%d", handle->buff2[1]);
        	handle->buff1[handle->bufferSize++] = BT_OK;
            handle->buff1[handle->bufferSize++] = handle->buff2[1];
        }
            break;

        case MCU_RESET:
        {
        	PRINT_INFO("~%d", bootloaderData.modeChange);
            handle->buff1[handle->bufferSize++] = BT_OK;
            DoSoftReset(3);
        }
            break;

        case READ_BOOT_INFO:
        {
        	PRINT_CMD("READ_BOOT_INFO");
            snprintf(BootInfo, MAX_BOOT_INFO, "%s", MODEL_NAME);
            int tmpLen = strlen(BootInfo);
            bootloaderData.connected_pc_tool = PC_CONN;
            handle->buff1[handle->bufferSize++] = BT_OK;
            handle->buff1[handle->bufferSize++] = 0;
            handle->buff1[handle->bufferSize++] = tmpLen;
            memcpy(&handle->buff1[handle->bufferSize], &BootInfo[0], tmpLen);
            handle->bufferSize += tmpLen;
        }
            break;

        case ERASE_FLASH:
        {
        	PRINT_CMD("FE ERASE_FLASH..");
            handle->buff1[handle->bufferSize++] = BT_OK;
        }
        	break;

        case PROGRAM_FLASH:
        {
#if 0
            PRINT_CMD("FE PROGRAM_FLASH..");
            handle->buff1[handle->bufferSize++] = BT_OK;
#else
        	//PRINT_CMD("FE PROGRAM_FLASH..");
            if (frontend_program_hexrecord(&handle->buff2[1], handle->cmdBufferLength-3) != HEX_REC_NORMAL) {
            	PRINT_CMD("ERROR! APP_ProgramHexRecord..\r\n");
                handle->buff1[handle->bufferSize++] = BT_ERR;
            }
            else {
                handle->buff1[handle->bufferSize++] = BT_OK;
            }
#endif
        }
        	break;

        case READ_CRC:
        {
#if 0
        	PRINT_CMD("READ_CRC");
        	crcCode = StringToHexa(FE_FIXED_INFO_CRC);

            PRINT_CMD("FE READ_CRC= %X\r\n", crcCode);

       	    handle->buff1[handle->bufferSize++] = BT_OK;
            *(uint16_t *)&handle->buff1[handle->bufferSize] = crcCode;
            handle->bufferSize += 2;
#else
        	get_eeprom_dev(flash_bank);
			frontend_fwinfo(&header23[0]);
			handle->buff1[handle->bufferSize++] = BT_OK;
			memcpy(&handle->buff1[handle->bufferSize], &frontend_hexCrc, 2);
			handle->bufferSize += 2;
			PRINT_CMD("FE READ_CRC..%04x", frontend_hexCrc);
#endif
        }
            break;

        case WRITE_EEPROM:
		{
			handle->writeRaw = true;
			handle->eepromBlockNo = handle->buff2[1];
			handle->buff1[handle->bufferSize++] = BT_OK;
			PRINT_CMD("FE WRITE_EEPROM.. %d", handle->eepromBlockNo);
		}
        break;

        case READ_EEPROM :
        {
#if 0
           	PRINT_CMD("READ_EEPROM CMD	 : %d, %d", Cmd, bootloaderData.modeChange);
            snprintf(BootInfoFE, MAX_FE_BOOT_INFO, "%s,%s,", FE_FIXED_INFO,FE_FIXED_INFO_CRC);
             int tmpLen = strlen(BootInfoFE);
             memcpy(&handle->buff1[handle->bufferSize], &BootInfoFE[0], tmpLen);
             handle->bufferSize += tmpLen;
             bootloaderData.readRaw = true;
#else
            PRINT_CMD("FE READ_EEPROM CMD..");
			handle->readRaw = true;
			get_eeprom_dev(flash_bank);
			//frontend_fwinfo(&header23[0]);
			memcpy(&handle->buff1[handle->bufferSize], &header23[0], HD_FE_EEPROM_SIZE);
			handle->bufferSize += HD_FE_EEPROM_SIZE;
#endif

        }
        break;

        default:
        {
        	PRINT_CMD("UNKNOWN CMD	 : %d", Cmd);
#define UNKNOWN_CMD_RES_SIZE	5
            memset((void *)&handle->buff1[handle->bufferSize], 0xFF, UNKNOWN_CMD_RES_SIZE);
            handle->bufferSize += UNKNOWN_CMD_RES_SIZE;
        }
            break;
    }
    handle->currentState = BOOTLOADER_SEND_RESPONSE;
}

static int Bootloader_WriteErprom_FE(BOOTLOADER_DATA *handle)
{
	int crc, crc2, ret = BT_OK;
	uint8_t *header3 = &header[HD_0_SIZE + HD_1_SIZE + HD_2_SIZE];
	uint8_t Cmd = handle->buff1[0];

	if (handle->modeChange != FE_BT_MODE){
    		//|| handle->hw_board_old != HW_BOARD_VE) {
    	//PRINT_CMD("ERROR! Bootloader_WriteErprom_FE.. %d %d", handle->modeChange, handle->hw_board_old);
		PRINT_CMD("ERROR! Bootloader_WriteErprom_FE.. %d", handle->modeChange);
    	return BT_ERR;
	}
	//copy header first
    memcpy(header3, &handle->buff1[1], HD_FE_EEPROM_SIZE);

	handle->bufferSize = 0;
	handle->buff1[handle->bufferSize++] = Cmd;
	handle->readRaw = false;

	crc = header3[HD_FE_EEPROM_SIZE-2];
    crc += ((header3[HD_FE_EEPROM_SIZE-1]) << 8);
    crc2 = Bootloader_CalculateCrc(header3, HD_FE_EEPROM_SIZE-2);
	PRINT_CMD("fe eeprom crc= %x %x", crc, crc2);
	if (crc != crc2) {
    	PRINT_ERR("ERROR! Bootloader_WriteErprom_FE..invalid crc %x %x", crc, crc2);
    	ret = BT_ERR;
    	goto L_Bootloader_WriteErprom_FE_exit;
    }

    /* Build the response frame from the command. */
    Bootloader_FlashInit();
	flash_bank = FLASH_BANK_B;

	ret = Bootloader_WriteEeprom(1); //1:erase-only
	if (ret != BT_OK) {
    	PRINT_ERR("ERROR! Bootloader_WriteErprom_FE..Bootloader_WriteErprom.1.");
    	goto L_Bootloader_WriteErprom_FE_exit;
    }

	ret = Bootloader_WriteEeprom(0); //0:write
	if (ret != BT_OK) {
    	PRINT_ERR("ERROR! Bootloader_WriteErprom_FE..Bootloader_WriteErprom.2.");
    	goto L_Bootloader_WriteErprom_FE_exit;
    }

#ifdef FLASH_DUAL_BANK
	ret = CopyFlashBankCode();
	if (ret == BT_OK) {
	    ret = Bootloader_FlashErase_All(FLASH_BANK_A);
    	if (ret != BT_OK) {
        	PRINT_ERR("ERROR! Bootloader_WriteErprom_FE..Bootloader_FlashErase_All");
        	goto L_Bootloader_WriteErprom_FE_exit;
        }
	    CopyFlashBank(1); //1:all
	}
	else {
    	PRINT_ERR("ERROR! Bootloader_WriteErprom_FE..CopyFlashBankCode");
    	goto L_Bootloader_WriteErprom_FE_exit;
	}
	flash_bank = FLASH_BANK_B;
#endif

L_Bootloader_WriteErprom_FE_exit:
    handle->buff1[handle->bufferSize++] = (uint8_t)ret;
    handle->currentState = BOOTLOADER_SEND_RESPONSE;
    return ret;
}

ATTRIBUTE_RAMFUNC
static int Bootloader_BufferEventHandler(DATASTREAM_BUFFER_EVENT buffEvent, uint16_t context)
{
    static bool Escape = false;
    uint16_t crc, crc2;
    int i = 0;

#if 0 //for testing
    PRINTF("Bootloader_BufferEventHandler...\r\n");
    while(i < context)
    {
        if ((i % 16) == 0) {
        	PRINTF("\r\n%02x: ", i);
        }
        PRINTF("%02x ", bootloaderData.buff1[i]);
        i++;
    }
    PRINTF("\r\n");
    PRINTF("buffEvent %d \r\n", buffEvent);
    i = 0;
#endif

    while (i < context)
    {
        switch (buffEvent)
        {
            /* Buffer event is completed successfully */
            case DATASTREAM_BUFFER_EVENT_COMPLETE:
            {
            	PRINT_ESC("buff1[%d]=%02x ", i, bootloaderData.buff1[i]);
                /* Check previous state to know what to check */
                // If we were in an Escape sequence, copy the data on and reset the flag.
                if (Escape)
                {
                    PRINT_ESC("ESC buff2[%d]=buff1[%d]=%02x", bootloaderData.cmdBufferLength, i, bootloaderData.buff1[i]);
                    bootloaderData.buff2[bootloaderData.cmdBufferLength++] = bootloaderData.buff1[i];
                    Escape = false;
                }
                else
                {
                    switch (bootloaderData.buff1[i])
                    {
                    case SOH:   // Start of header
                    	PRINT_ESC("SOH");
                        bootloaderData.cmdBufferLength = 0;
                        break;

                    case EOT:   // End of transmission
                        // Calculate CRC and see if this is valid
                    	PRINT_ESC("EOT");
                        if (bootloaderData.cmdBufferLength > 2)
                        {
                            crc = bootloaderData.buff2[bootloaderData.cmdBufferLength-2];
                            crc += ((bootloaderData.buff2[bootloaderData.cmdBufferLength-1])<<8);
                            crc2 = Bootloader_CalculateCrc(bootloaderData.buff2, bootloaderData.cmdBufferLength-2);
                            PRINT_ESC("crc %04x crc2 %04x", crc, crc2);
                            if (crc != crc2) {
                            	PRINT_ERR("ERROR! crc %04x crc2 %04x", crc, crc2);
                            	//return BT_ERR; //FIXME nsmoon@170427a
                            }
                            // CRC matches so frame is valid.
                            bootloaderData.usrBufferEventComplete = true;
                            return BT_OK;
                        }
                        break;

                    case DLE:   // Escape sequence
                        PRINT_ESC("DLE");
                        Escape = true;
                        break;

                    default:
                        PRINT_ESC("default buff2[%d]=buff1[%d]=%02x", bootloaderData.cmdBufferLength, i, bootloaderData.buff1[i]);
                        bootloaderData.buff2[bootloaderData.cmdBufferLength++] = bootloaderData.buff1[i];
                        break;
                  }
              }
              i++;
          }
              break;

          /* Buffer event has some error */
          case DATASTREAM_BUFFER_EVENT_ERROR:
          case DATASTREAM_BUFFER_EVENT_ABORT:
              PRINT_ERR("DATASTREAM_BUFFER_EVENT_ERROR");
              break;

          default:
        	  PRINT_ERR("ERROR! unknown buffEvent...");
        	  break;
        } //switch (buffEvent)
    } //while (i < context)
    if (bootloaderData.modeChange == BE_BT_MODE) {
		PRINT_ERR("ERROR! unknown cmd");
	#if 0 //for testing
		for (i = 0; i < context; i++)
		{
			PRINTF("%02x ", bootloaderData.buff1[i]);
			if (((i+1) % 16) == 0) PRINTF("\r\n");
		}
		PRINTF(" (%d)\r\n", context);
	#endif
    }
    bootloaderData.cmdBufferLength = 0;
    bootloaderData.buff2[bootloaderData.cmdBufferLength++] = UNKNOWN_CMD;
    return BT_ERR;
}

ATTRIBUTE_RAMFUNC
void Bootloader_Initialize (void)
{
    /* Place the App state machine in it's initial state. */
    bootloaderData.currentState = BOOTLOADER_CHECK_FOR_TRIGGER;
    bootloaderData.cmdBufferLength = 0;
    bootloaderData.usrBufferEventComplete = false;
    bootloaderData.FlashEraseFunc = (BOOTLOADER_CALLBACK)NULL;
    bootloaderData.StartAppFunc = (BOOTLOADER_CALLBACK)NULL;
    bootloaderData.BlankCheckFunc = (BOOTLOADER_CALLBACK)NULL;
    bootloaderData.ProgramCompleteFunc = (BOOTLOADER_CALLBACK)NULL;
    bootloaderData.ForceBootloadFunc = (BOOTLOADER_CALLBACK)NULL;
    //bootloaderData.soft_reset = false;
    bootloaderData.bufferSize = 512;

    bootloaderData.bootloader_wait_cnt = 0;
    bootloaderData.connected_pc_tool = PC_NOTCONN;
    bootloaderData.buff1 = g_UsbDeviceHidGeneric.buffRx; //RX
    bootloaderData.buff2 = &Bootloader_Tx_Buffer[0]; //TX
    //bootloaderData.forceBootloadMode = true;
    bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
    bootloaderData.progStart = true; //init
#if USB_HOST_FOR_FRONTEND
    bootloaderData.sent_ver_cmd = 0; //init
    bootloaderData.device_not_supported = 0;
#endif
    bootloaderData.modeChange = BE_BT_MODE; //init
    bootloaderData.writeRaw = false; //init
    bootloaderData.writeCode = false; //init

#if 0 //for test
    //internal QSPI flash init
    flashOption.option0.U = 0xC0000008; // QuadSPI NOR, Frequency: 133MHz
	flashInstance = 1; //Should identify NOR memory instance
	status_t status = flexspi_nor_get_config(flashInstance, &flashConfig, &flashOption);
    if (status != kStatus_Success) {
    	PRINT_ERR("ERROR! flexspi_nor_get_config().. %d", status);
    }
    status = flexspi_nor_flash_init(flashInstance, &flashConfig);
    if (status != kStatus_Success) {
    	PRINT_ERR("ERROR! flexspi_nor_flash_init().. %d", status);
    }
#endif

    Bootloader_Program_Init(); //should be called get_version_info first
#ifdef FLASH_DUAL_BANK //nsmoon@200717
	flash_bank = FLASH_BANK_A; //init
    fe_wait_dot = fe_wait_cnt = 0;
    IsPrgExistsA();
    IsPrgExistsB();
#else
    program_is_existed = IsPrgExists();
#endif
#if 1 //nsmoon@200807
    s_reset_delay_dot = 0; //init
    s_reset_delay = -1; //init
#endif
}

ATTRIBUTE_RAMFUNC
int Bootloader_Task(void)
{
    size_t BuffLen=0;
    uint16_t crc;
    unsigned int i;
    int devReady = USB_DeviceReady();
#ifdef CYCCNT_ENABLE			//YJ@230703
    static uint8_t flag =0;

	if(flag == 0)
	{
		flag = 1;
	    APP_TIME_INIT();
	}
#endif
    check_pc_connected(devReady);
#if 1 //nsmoon@200807
    check_reset_delay();
#endif
	//PRINT_INFO("currentState= %d", bootloaderData.currentState);

    /* Check the application state*/
    switch ( bootloaderData.currentState )
    {
#if 0 //nsmoon@200807 not-use
        case BOOTLOADER_RESET:
        {
        	if (s_reset_delay-- > 0) {
    			bootloaderData.currentState = BOOTLOADER_RESET;
        	}
        	else {
    			//s_frontend_Reset(); //nsmoon@200728 move to main
    			MY_WDOG_soft_reset();
    			while(1);
        	}
        }
        	break;
#endif

        case BOOTLOADER_CHECK_FOR_TRIGGER:
        	break;

        case BOOTLOADER_OPEN_DATASTREAM:
        {
            //PRINT_BLT("BOOTLOADER_OPEN_DATASTREAM...");
            if (devReady == USB_OK) {
                bootloaderData.currentState = BOOTLOADER_GET_COMMAND;
            }
        }
            break;

        case BOOTLOADER_GET_COMMAND:
        {
            if (devReady != USB_OK) {
                bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
                break;
            }
#if USB_HOST_FOR_FRONTEND
#ifndef FRONTEND_RESET_ENABLE
            if (g_HostHidGeneric.softReset == 1) {
            	USB_HostCmd(MCU_RESET_REAL);
            }
#endif
#endif

            if (DATASTREAM_Ready() == USB_OK) {
                DATASTREAM_RecvClear();
#if USB_HOST_FOR_FRONTEND
                PRINT_BLT("BOOTLOADER_GET_COMMAND...%d", bootloaderData.modeChange);
                if (bootloaderData.modeChange == FE_BT_MODE) {
                	int res;
#ifdef FRONTEND_RESET_ENABLE
                	bootloaderData.usrBufferEventComplete = false;
                    res = Bootloader_BufferEventHandler(DATASTREAM_BUFFER_EVENT_COMPLETE, USB_HID_GENERIC_OUT_BUFFER_LENGTH);
                    if (res == BT_OK) {
                    	//PRINT_INFO("frontend cmd=%x", bootloaderData.buff2[0]);
                    	if (bootloaderData.buff2[0] == MCU_RESET) {
                    		PRINT_INFO("~~%d", bootloaderData.modeChange);
#if 1 //nsmoon@200728
#if 1 //nsmoon@200807
                    		DoSoftReset(3);
#if 1 //nsmoon@211203
                        	bootloaderData.modeChange = BE_BT_MODE; //reset
                        	bootloaderData.buff1[0] = bootloaderData.buff2[0];
    						bootloaderData.buff1[1] = BT_OK;
    						bootloaderData.bufferSize = 2;
    						bootloaderData.prevState    = BOOTLOADER_GET_COMMAND;
                            bootloaderData.currentState = BOOTLOADER_SEND_RESPONSE;
                            break;
#endif
#else
        					s_reset_delay = 100;
        			bootloaderData.currentState = BOOTLOADER_RESET;
#endif
#else
        	                s_frontend_Reset();
                            g_HostHidGeneric.hostWaitCnt = 0;
                            bootloaderData.prevState    = BOOTLOADER_GET_COMMAND;
                            bootloaderData.currentState = BOOTLOADER_WAIT_FOR_RESET_DEVICE;
#endif
        	                break;
                    	}
                    }
#endif
					res = USB_HostSendPacket(bootloaderData.buff1, USB_HID_GENERIC_OUT_BUFFER_LENGTH);
					if (res != USB_HOST_OK) {
#if 1 //nsmoon@211203
						//bootloaderData.feResCnt = FE_RES_CNT_ERROR; //error
						bootloaderData.modeChange = BE_BT_MODE; //reset
						bootloaderData.buff1[0] = bootloaderData.buff2[0];
						bootloaderData.buff1[1] = BT_ERR;
						bootloaderData.bufferSize = 2;
#else
						bootloaderData.prevState    = BOOTLOADER_GET_COMMAND;
						bootloaderData.currentState = BOOTLOADER_SEND_RESPONSE;
						break;
#endif
					}
#if 1 //nsmoon@211203
					else {
					    bootloaderData.feResCnt = 0; //init
					    memcpy(&Bootloader_Rx_Buffer[0], bootloaderData.buff1, USB_HID_GENERIC_OUT_BUFFER_LENGTH); //nsmoon@211220
					}
#endif
					bootloaderData.prevState    = BOOTLOADER_GET_COMMAND;
                    bootloaderData.currentState = BOOTLOADER_SEND_RESPONSE;
                }
                else
#else
                PRINT_BLT("BOOTLOADER_GET_COMMAND..%d,%d", bootloaderData.writeRaw, bootloaderData.writeCode);

#endif
                {
                    //PRINT_BLT("writeRaw,writeCode= %d %d", bootloaderData.writeRaw, bootloaderData.writeCode);
                    if (!bootloaderData.writeRaw && !bootloaderData.writeCode) {
                	    bootloaderData.usrBufferEventComplete = false;
                        int res = Bootloader_BufferEventHandler(DATASTREAM_BUFFER_EVENT_COMPLETE, USB_HID_GENERIC_OUT_BUFFER_LENGTH);
                        if (res != BT_OK) {
                        	bootloaderData.buff1[0] = bootloaderData.buff2[0];
                       		bootloaderData.buff1[1] = BT_ERR;
                          	bootloaderData.bufferSize = 2;
                            bootloaderData.prevState    = BOOTLOADER_GET_COMMAND;
                           	bootloaderData.currentState = BOOTLOADER_SEND_RESPONSE;
                           	break;
                       	}
                    }
                    else if (bootloaderData.writeCode) {
                        memcpy(&bootloaderData.buff2[0], &bootloaderData.buff1[0], USB_HID_GENERIC_OUT_BUFFER_LENGTH);
                    }
                    bootloaderData.prevState    = BOOTLOADER_GET_COMMAND;
                    bootloaderData.currentState = BOOTLOADER_PROCESS_COMMAND;
                }
            }
        }
            break;

        case BOOTLOADER_PROCESS_COMMAND:
        {
        	PRINT_BLT("BOOTLOADER_PROCESS_COMMAND... %d %d %d", devReady, bootloaderData.writeRaw, bootloaderData.writeCode);
            if (devReady != USB_OK) {
                bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
                break;
            }
            if (bootloaderData.writeRaw) {
            	bootloaderData.writeRaw = false;
#if DEBUG_EEPROM //for debugging
                int i;
                for (i = 0; i < USB_HID_GENERIC_OUT_BUFFER_LENGTH; i++) {
            	    PRINT_EEPROM("%02X ", bootloaderData.buff1[i]);
            	    if (((i+1)%16) == 0) PRINT_EEPROM("\r\n");
                }
                PRINT_EEPROM("\r\n");
#endif
#if 1 //nsmoon@230503
    			Bootloader_WriteErprom_FE(&bootloaderData);
#endif
            }
            else {
#ifdef FE_FIXED_INFO 	//YJ@230223
            	if (bootloaderData.modeChange == BE_BT_MODE)
				{
					Bootloader_ProcessBuffer(&bootloaderData);
				}
				else
				{
					Bootloader_ProcessBuffer_FE(&bootloaderData);
				}
#else

            	Bootloader_ProcessBuffer(&bootloaderData);
#endif
            }
        }
            break;

        case BOOTLOADER_RESEND_COMMAND:
        {
#if USB_HOST_FOR_FRONTEND		//YJ@230215
        	if (bootloaderData.feResCnt == 0) {
	    	    s_frontend_Reset();
	    	    bootloaderData.feResCnt++;
        	}
        	else if (bootloaderData.feResCnt == 1) {
            	if (USB_HostCmd(READ_BOOT_INFO) == USB_HOST_OK) {
            		bootloaderData.feResCnt++;
            	}
            	else {
    				PRINTF("-");
            		//delay_ms(1);
            	}
        	}
        	else {
				int res = USB_HostSendPacket(&Bootloader_Rx_Buffer[0], USB_HID_GENERIC_OUT_BUFFER_LENGTH);
				if (res == USB_HOST_OK) {
					bootloaderData.prevState = BOOTLOADER_RESEND_COMMAND;
					bootloaderData.currentState = BOOTLOADER_SEND_RESPONSE;
            		bootloaderData.feResCnt++;
				}
				else {
					PRINT_BLT("ERROR! ...USB_HostSendPacket\r\n");
					bootloaderData.feResCnt = 0;
				}
        	}
#else
        bootloaderData.prevState = BOOTLOADER_RESEND_COMMAND;
        bootloaderData.currentState = BOOTLOADER_SEND_RESPONSE;
#endif
        }
        	break;

        case BOOTLOADER_SEND_RESPONSE:
        {
            if (devReady != USB_OK) {
                bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
                break;
            }
#if USB_HOST_FOR_FRONTEND
            PRINT_BLT("BOOTLOADER_SEND_RESPONSE...%d", bootloaderData.modeChange);
            if (bootloaderData.modeChange == FE_BT_MODE) {
#if 0 //nsmoon@211203
            	if (bootloaderData.feResCnt > 0x1000000)
            	{
            		PRINTF("ERROR! bootloaderData.feResCnt..(%x)\r\n", bootloaderData.feResCnt);
            		DoSoftReset(3);
            		bootloaderData.feResCnt = 0;
                    break;
            	}
            	else
#endif 
                {
					BuffLen = USB_HostRecvPacket(bootloaderData.buff2);
					if (BuffLen > 0) {
						if (DATASTREAM_Send_init(bootloaderData.buff2, BuffLen) == BT_OK) {
		            		//PRINTF("bootloaderData.feResCnt..(%x)\r\n", bootloaderData.feResCnt);
							bootloaderData.prevState = BOOTLOADER_SEND_RESPONSE;
							bootloaderData.currentState = BOOTLOADER_WAIT_FOR_SEND;
							break;
						}
					}
#if 1 //nsmoon@211203
#define FE_RES_CNT_MAX    0x100000
					if (bootloaderData.feResCnt < FE_RES_CNT_ERROR) {
					    bootloaderData.feResCnt++;
					    if (bootloaderData.feResCnt > FE_RES_CNT_MAX) {
							bootloaderData.feResCnt = 0;
							bootloaderData.prevState = BOOTLOADER_SEND_RESPONSE;
							bootloaderData.currentState = BOOTLOADER_RESEND_COMMAND;
					    }
					}
#endif //1
            	}
            }
            else
#else
            PRINT_BLT("BOOTLOADER_SEND_RESPONSE...");
#endif
            {
                if (bootloaderData.bufferSize)
                {
                	if (bootloaderData.readRaw) {
                        BuffLen = bootloaderData.bufferSize;
                        if (DATASTREAM_Send_init(bootloaderData.buff1, BuffLen) == BT_OK) {
                            bootloaderData.prevState = BOOTLOADER_SEND_RESPONSE;
                            bootloaderData.currentState = BOOTLOADER_WAIT_FOR_SEND;
                            break;
                        }
                	}
                	else {
//#if USB_HOST_FOR_FRONTEND
                		if (bootloaderData.buff1[0] == MODE_CHANGE && bootloaderData.buff1[1] == BT_OK) {
                			bootloaderData.modeChange = bootloaderData.buff1[2];
                			PRINTF("=%d", bootloaderData.modeChange);
                		}
//#endif

                        /* Calculate the CRC of the response*/
                        crc = Bootloader_CalculateCrc(bootloaderData.buff1, bootloaderData.bufferSize);
                        bootloaderData.buff1[bootloaderData.bufferSize++] = (uint8_t)crc;
                        bootloaderData.buff1[bootloaderData.bufferSize++] = (crc>>8);
        #if 0  //for testing
                        for (i = 0; i < bootloaderData.bufferSize; i++)
                        {
                            if ((i % 16) == 0) {
                            	PRINTNR_ESC("\r\n%02x: ", i);
                            }
                            PRINTNR_ESC("%02x ", bootloaderData.buff1[i]);
                        }
                        PRINTNR_ESC("\r\n");
        #endif

                        BuffLen = 0;
                        bootloaderData.buff2[BuffLen++] = SOH;

                        for (i = 0; i < bootloaderData.bufferSize; i++)
                        {
                        	PRINTNR_ESC("(%d)%02x", i, bootloaderData.buff1[i]);
                        if ((bootloaderData.buff1[i] == EOT) || (bootloaderData.buff1[i] == SOH)
                                || (bootloaderData.buff1[i] == DLE)) {
                                bootloaderData.buff2[BuffLen++] = DLE;
                                PRINTNR_ESC("[%d]%02x", BuffLen-1, bootloaderData.buff2[BuffLen-1]);
                            }
                            bootloaderData.buff2[BuffLen++] = bootloaderData.buff1[i];
                            PRINT_ESC("[%d]%02x", BuffLen-1, bootloaderData.buff2[BuffLen-1]);
                        }

                        bootloaderData.buff2[BuffLen++] = EOT;
                        bootloaderData.bufferSize = 0;
    #if 0  //for testing
                        for (i = 0; i < BuffLen; i++)
                        {
                            if ((i % 16) == 0) {
                            	PRINTNR_ESC("\r\n%02x: ", i);
                            }
                            PRINTNR_ESC("%02x ", bootloaderData.buff2[i]);
                        }
                        PRINTNR_ESC("\r\n");
        #endif
                        if (DATASTREAM_Send_init(bootloaderData.buff2, BuffLen) == BT_OK) {
                        	bootloaderData.prevState = BOOTLOADER_SEND_RESPONSE;
                            //bootloaderData.currentState = BOOTLOADER_WAIT_FOR_DONE;
                            bootloaderData.currentState = BOOTLOADER_WAIT_FOR_SEND;
                            break;
                        }
                	}
                }
                else {
                    PRINT_ERR("ERROR! invalid bufferSize");
                    bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
                }
            }
        }
            break;

        case BOOTLOADER_WAIT_FOR_SEND:
        {
            if (devReady != USB_OK) {
                bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
                break;
            }
        	if (DATASTREAM_Send() == USB_OK) {
        		PRINT_BLT("..send\r\n");
#if USB_HOST_FOR_FRONTEND
#ifndef FRONTEND_RESET_ENABLE
                if (bootloaderData.buff1[0] == MCU_RESET && g_HostHidGeneric.softReset == 1 ) {
                    bootloaderData.currentState = BOOTLOADER_WAIT_FOR_RESET_DEVICE;
                    g_HostHidGeneric.hostWaitCnt = 0;
                    break;
                }
#endif
                if (bootloaderData.modeChange == FE_BT_MODE && g_HostHidGeneric.modeChange == 1) {
                	g_HostHidGeneric.modeChange = 0;
            		bootloaderData.modeChange = BE_BT_MODE;
            		PRINTF("==%d", bootloaderData.modeChange);
                }
#endif
                //bootloaderData.currentState = BOOTLOADER_WAIT_FOR_DONE;
                bootloaderData.currentState = BOOTLOADER_GET_COMMAND;
        	}
        }
            break;

#if USB_HOST_FOR_FRONTEND
#ifndef FRONTEND_RESET_ENABLE
        case BOOTLOADER_WAIT_FOR_RESET_DEVICE:
        {
            if (g_HostHidGeneric.configHandle == NULL) {
            	//detached
            	//PRINTF("=");
                MY_WDOG_soft_reset();
                while(1);
            }
            else if (g_HostHidGeneric.hostWaitCnt++ > 5000) {
            	//wait timeout
            	PRINTF("..timeout\r\n");
                bootloaderData.currentState = BOOTLOADER_GET_COMMAND;
            }
        }
            break;
#endif
#endif

        case BOOTLOADER_CLOSE_DATASTREAM:
            //PRINTF("BOOTLOADER_CLOSE_DATASTREAM...");
            //DATASTREAM_Close();
        case BOOTLOADER_ENTER_APPLICATION:
            return BT_OK;

        case BOOTLOADER_ERROR:
        {
            if (devReady != USB_OK) {
                bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
                break;
            }
        	PRINT_ERR("BOOTLOADER_ERROR...");
            /* The application comes here when the demo
             * has failed. Switch on the LED 9.*/
            //BSP_LEDOn(BSP_LED_3);
            //BSP_LEDOn(BSP_LED_2);
            //BSP_LEDOn(BSP_LED_1);
        }
            break;

        default:
        {
            if (devReady != USB_OK) {
                bootloaderData.currentState = BOOTLOADER_OPEN_DATASTREAM;
                break;
            }
        	PRINT_ERR("BOOTLOADER UNKNOWN STAE... %d", bootloaderData.currentState);
            bootloaderData.currentState = BOOTLOADER_ERROR;
        }
            break;
    }
    return BT_ERR;
}
/*end of file*/
