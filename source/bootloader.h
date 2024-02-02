/*
 * bootloader.h
 *
 *  Created on: Feb 28, 2019
 *      Author: user
 */
#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_
#include "hid_generic.h"
#include "rt1064_iap.h"

////////////////////////////////////////////////////////////////////
//#define CUSTOMER_NO		3 //CTSO-0850 samsung
//#define CUSTOMER_NO		4 //CTSO-0750 samsung
//#define CUSTOMER_NO		5 //CTSI-0650 samsung
//#define CUSTOMER_NO		6 //CTSI-0550 samsung
//#define CUSTOMER_NO		10 //nohost samsung

#define NON_HOST_SAMSUNG		0010 //nohost samsung
#define NON_HOST_N65_V100		6551 //nohost N65 nxp only		//YJ@230215
#define NON_HOST_N75_V100		7551 //nohost N75 nxp only		//YJ@230223
#define NON_HOST_N85_V100		8551 //nohost N85 nxp only		//YJ@230223
#define MODEL_CTKS_750_V140		7503 //samsung V120, nxp+pic, frontend_reset
#define MODEL_CTSK_850_V100     8501 //samsung, nxp+pic
#define MODEL_CTSK_650_V100     6501 //samsung, nxp+pic
#define MODEL_CTSK_550_V100     5501 //samsung, nxp+pic
#define CUSTOMER_NO				NON_HOST_N85_V100

#if (CUSTOMER_NO == 1)
#define MODEL_NAME	"IRT750SS"
#define BOOTLOADER_VERSION	"NXPBT4@200807A" //sdk2.6.1
#define USB_HOST_FOR_FRONTEND	(1U) //nxp+pic
#include "host_hid_generic.h"
#elif (CUSTOMER_NO == 2)
#define MODEL_NAME	"IRT750CN"
#define BOOTLOADER_VERSION	"NXPBT4@200807A" //sdk2.6.1
#define USB_HOST_FOR_FRONTEND	(0U) //nxp
#elif (CUSTOMER_NO == MODEL_CTSK_850_V100)
///////////////////////////	"12345678901234"
//#define BOOTLOADER_VERSION	"NXPBT4@200807A" //sdk2.6.1
//#define BOOTLOADER_VERSION	"NXPBT4@220105A" //sdk2.6.1
#define BOOTLOADER_VERSION	"NXP761@220517A" //sdk2.6.1
#define MODEL_NAME	"CTSO-0850"
#define USB_HOST_FOR_FRONTEND	(1U) //nxp+pic
#define FRONTEND_0512
#include "host_hid_generic.h"
#elif (CUSTOMER_NO == MODEL_CTKS_750_V140) //nsmoon@201208
///////////////////////////	"12345678901234"
//#define BOOTLOADER_VERSION	"NXP751@201208A" //sdk2.6.1
#define BOOTLOADER_VERSION	"NXP761@220513A" //sdk2.6.1
#define MODEL_NAME	"CTSO-0850"
#define USB_HOST_FOR_FRONTEND	(1U) //nxp+pic
#define FRONTEND_0512
#include "host_hid_generic.h"
#elif (CUSTOMER_NO == MODEL_CTSK_650_V100) //yj@210709
///////////////////////////	"12345678901234"
//#define BOOTLOADER_VERSION	"NXP753@210709A" //sdk2.6.1
//#define BOOTLOADER_VERSION	"NXP753@220111A" //sdk2.6.1
#define BOOTLOADER_VERSION	"NXP753@220217A" //sdk2.6.1
#define MODEL_NAME	"CTSO-0850"
#define USB_HOST_FOR_FRONTEND	(1U) //nxp+pic
#include "host_hid_generic.h"
#elif (CUSTOMER_NO == MODEL_CTSK_550_V100) //yj@210913
///////////////////////////	"12345678901234"
//#define BOOTLOADER_VERSION	"NXP754@211222A" //sdk2.6.1
//#define BOOTLOADER_VERSION	"NXP754@220111A" //sdk2.6.1
//#define BOOTLOADER_VERSION	"NXP754@220208A" //sdk2.6.1
#define BOOTLOADER_VERSION	"NXP754@220303A" //sdk2.6.1
#define MODEL_NAME	"CTSO-0850"
#define USB_HOST_FOR_FRONTEND	(1U) //nxp+pic
#include "host_hid_generic.h"
#elif (CUSTOMER_NO == NON_HOST_SAMSUNG) //nsmoon@201221
///////////////////////////	"12345678901234"
#define BOOTLOADER_VERSION	"NXPBT5@201221A" //sdk2.6.1
#define MODEL_NAME	"NXP-NOHOST"
#include "board.h" //nsmoon@201221
#elif (CUSTOMER_NO == NON_HOST_N65_V100) //YJ@230601
#define BOOTLOADER_VERSION	"NXPN650@230703B" //sdk2.6.1
#define MODEL_NAME	"NXP-NOHOST"
#define MODEL_NAME_VIEW	"CTSI-N650"
//#define FE_FIXED_INFO_CRC	"72D2"	//"0000"	// for version info mfg or Sn Test
#define FE_FIXED_INFO		"CTSI-0650_30_F0308C,230601123323,650,10"
#include "board.h" //nsmoon@201221
#elif (CUSTOMER_NO == NON_HOST_N75_V100) //YJ@230223
#define BOOTLOADER_VERSION	"NXPN750@230822A" //sdk2.13.0
#define MODEL_NAME	"NXP-NOHOST"
#define MODEL_NAME_VIEW	"CTSI-N750"
//#define FE_FIXED_INFO_CRC	"0000"
#define FE_FIXED_INFO		"CTSI-0750_20_F0531A,230230531182823,750,10"
#include "board.h" //nsmoon@201221
#elif (CUSTOMER_NO == NON_HOST_N85_V100) //YJ@230223
#define BOOTLOADER_VERSION	"NXPN850@230831A" //sdk2.13.0
#define MODEL_NAME	"NXP-NOHOST"
#define MODEL_NAME_VIEW	"CTSI-N850"
//#define FE_FIXED_INFO_CRC	"0000"
#define FE_FIXED_INFO		"CTSI-0850_10_F0531A,230223182831,850,10"
#include "board.h"
#else
#error "CUSTOMER_NO is not defined..."
#endif

//#define CYCCNT_ENABLE			//YJ@230703

#if 1 //nsmoon@200715
#define FRONTEND_RESET_VAL(a)	(GPIO_PinWrite(BOARD_FRONT_RESET_GPIO, BOARD_FRONT_RESET_GPIO_PIN, (a)))
#endif

#define APP_TEST_FLASH_ENABLE	(0U) //for test
#define FORCED_BOOTLOAD_MODE	(1U)
#define DEBUG_VERIFY_PROGRAM	(1U) //nsmoon@200703, //nsmoon@200703 disable
#define PROGRAM_BIN_FILE		(1U) //nsmoon@191118
#define USE_FW_KEY_VALIDATION	(0U) //nsmoon@191010
#if USB_HOST_FOR_FRONTEND
#define FRONTEND_RESET_ENABLE //nsmoon@200608
#endif
#define FLASH_DUAL_BANK //nsmoon@200717
#define FLASH_DUAL_BANK_TYPE    (2U) //1:bank-a & -b, 2:bank-a & copy

#define BOOTLOADER_SIZE			0x20000
#define FLASH_SECTOR_SIZE		0x1000	/*4KB*/
#define FLASH_PAGE_SIZE			0x100   /*256*/
#define EFUSE_KEY_LEN			0x10
#define FLASH_READ_BUFF_SIZE	FLASH_PAGE_SIZE
#define FLASH_PROG_BUFF_SIZE	(FLASH_SECTOR_SIZE * 2)
#define BOOTLOADER_EEPROM_SIZE	(USB_HID_GENERIC_OUT_BUFFER_LENGTH - 1)

#define FLASH_BASE_ADDR			FlexSPI2_AMBA_BASE
#ifdef FLASH_DUAL_BANK //nsmoon@200717
#define MY_FLASH_SIZE			(BOARD_FLASH_SIZE / 2) /*2MB*/
#else
#define MY_FLASH_SIZE			BOARD_FLASH_SIZE /*4MB*/
#endif
#define NUM_OF_SECTOR			(MY_FLASH_SIZE / FLASH_SECTOR_SIZE)

#define BOOTLOADER_FLASH_BASE_ADDRESS	(FLASH_BASE_ADDR + BOOTLOADER_SIZE)
#define APPLICATION_ADDRESS				(BOOTLOADER_FLASH_BASE_ADDRESS + 0x2000) //boot_hdr, 0x70022000

#ifdef FLASH_DUAL_BANK //nsmoon@200717
#define MY_FLASH_BANK_SIZE		((BOARD_FLASH_SIZE - BOOTLOADER_SIZE) / 2) //0x1F0000=(0x400000-0x20000)/2
#define NUM_OF_BANK_SECTOR		(MY_FLASH_BANK_SIZE / FLASH_SECTOR_SIZE) //0x1F0
#define MY_FLASH_APP_SIZE		(MY_FLASH_BANK_SIZE - FLASH_SECTOR_SIZE) //0x1EF000=0x1F0000-0x1000
#define NUM_OF_APP_SECTOR		(MY_FLASH_APP_SIZE / FLASH_SECTOR_SIZE) //0x1EF=0x1EF000/0x1000
#define FLASH_BANK_A_ADDR		(BOOTLOADER_FLASH_BASE_ADDRESS)	//0x70020000=0x70000000+0x20000
#define FLASH_BANK_B_ADDR		(FLASH_BANK_A_ADDR + MY_FLASH_BANK_SIZE) //0x70210000=0x70020000+0x1F0000
#define APP_ADDRESS_A			(APPLICATION_ADDRESS)
#define APP_ADDRESS_B			(FLASH_BANK_B_ADDR + 0x2000)
#define APP_PRG_SIZE 			(MY_FLASH_APP_SIZE)
#define GET_VIRTUAL_ADDRESS(a)	((a) - FLASH_BASE_ADDR)
#define BANK_A_SECTOR_START		(BOOTLOADER_SIZE / FLASH_SECTOR_SIZE) //0x20
#define BANK_B_SECTOR_START		(BANK_A_SECTOR_START + NUM_OF_BANK_SECTOR) //0x210
#define MAGIC_VERSION_NO		(99)
#if 1 //nsmoon@200724, for backward compatibility
#define EEPROM_BASE_ADDRESS_B	(FLASH_BANK_A_ADDR + MY_FLASH_APP_SIZE)	//0x7020F000=0x70020000+0x1EF000
#define EEPROM_BASE_ADDRESS_A	(FLASH_BANK_B_ADDR + MY_FLASH_APP_SIZE)	//0x703FF000=0x70210000+0x1EF000
#else
#define EEPROM_BASE_ADDRESS_A	(FLASH_BANK_A_ADDR + MY_FLASH_APP_SIZE)	//0x7020F000=0x70020000+0x1EF000
#define EEPROM_BASE_ADDRESS_B	(FLASH_BANK_B_ADDR + MY_FLASH_APP_SIZE)	//0x703FF000=0x70210000+0x1EF000
#endif
#else
#define APP_PRG_SIZE 			(MY_FLASH_SIZE - BOOTLOADER_SIZE - FLASH_SECTOR_SIZE)
#define BOOTLOADER_EEPROM_BASE_ADDRESS  (FLASH_BASE_ADDR + MY_FLASH_SIZE - FLASH_SECTOR_SIZE)
//#define BOOTLOADER_EEPROM_BASE_ADDRESS  (FLASH_BASE_ADDR + (BOOTLOADER_SIZE * 2)) //for test
//#define BOOTLOADER_FLASH_END_ADDRESS	(BOOTLOADER_EEPROM_BASE_ADDRESS - 1) //(FLASH_BASE_ADDR + MY_FLASH_SIZE)
#define APP_SECTOR_START		(BOOTLOADER_SIZE / FLASH_SECTOR_SIZE)
#endif

#ifndef ATTRIBUTE_RAMFUNC
#define ATTRIBUTE_RAMFUNC		__attribute__((section(".ramfunc*")))  //FIXME
#endif
//#define PC_WAIT_TIMEOUT_CNT		0xF00000	//0x500000 //0x280000		//YJ@230224		0x500000 -> 0xA00000 for filp app upgrade function
#define PC_WAIT_TIMEOUT_CNT		0xE00000 //0x280000		//YJ@230224		0x500000 -> 0xA00000 for filp app upgrade function
///////////////////////////////////////////////////////////////////
#define SOH		01
#define EOT		04
#define DLE		16

#if 0 //nsmoon@190817
static const uint8_t BootInfo[5] =
{
    MINOR_VERSION,
    MAJOR_VERSION,
#if 1 //nsmoon@170609a
    MODEL_NAME,
    COMP_NAME,
    VER_NAME
#endif
};
#endif

typedef enum
{
    READ_BOOT_INFO = 0x01,
    ERASE_FLASH = 0x02,
    PROGRAM_FLASH = 0x03,
    READ_CRC = 0x04,
    JMP_TO_APP = 0x05,
    READ_FLASH = 0x06,
    READ_EEPROM = 0x07,
	WRITE_EEPROM = 0x08,
	MODE_CHANGE = 0x09,
	MCU_RESET = 0x0A,
	MCU_RESET_REAL = 0x55,
	ERASE_BE_BOOT = 0xAA, //for testing
    UNKNOWN_CMD = 0xFF,
} T_COMMANDS;

typedef enum
{
    /* Application's state machine's initial state. */
    BOOTLOADER_STATE_INIT=0,
    /* Check to see if we need to force the bootloader */
    BOOTLOADER_CHECK_FOR_TRIGGER,
    /* If we need to program, then open the datastream. */
    BOOTLOADER_OPEN_DATASTREAM,
    /* The application gets a command from the host application. */
    BOOTLOADER_GET_COMMAND,
    /* The application processes the command from the host application. */
    BOOTLOADER_PROCESS_COMMAND,
    /* The application waits for the NVM operation to complete. */
    BOOTLOADER_WAIT_FOR_NVM,
    /* The application sends data back to the user. */
    BOOTLOADER_SEND_RESPONSE,
    BOOTLOADER_RESEND_COMMAND, //nsmoon@211220
    /* The application waits in this state for the driver to finish
       sending/receiving the message. */
    BOOTLOADER_WAIT_FOR_DONE,
    BOOTLOADER_WAIT_FOR_SEND,
    BOOTLOADER_CLOSE_DATASTREAM,
    /* The application enters the user application. */
    BOOTLOADER_ENTER_APPLICATION,
    //BOOTLOADER_RESET, //nsmoon@200807 not use
    BOOTLOADER_OPEN_FILE,
    BOOTLOADER_READ_FILE,
	BOOTLOADER_WAIT_FOR_DEVICE_ATTACH,
    BOOTLOADER_WAIT_FOR_HOST_ENABLE,
    BOOTLOADER_DEVICE_CONNECTED,
    BOOTLOADER_UNMOUNT_DISK,
    BOOTLOADER_SWITCH_APPLICATION,
	/* The application enters bypass mode for front-end. */
    BOOTLOADER_BYPASS_FE_DATA,
	BOOTLOADER_WAIT_FOR_SEND_DEVICE,
	BOOTLOADER_WAIT_FOR_RESET_DEVICE,
    /* This state indicates an error has occurred. */
    BOOTLOADER_ERROR,
} BOOTLOADER_STATES;

typedef enum
{
    TYPE_I2C,
    TYPE_USART,
    TYPE_USB_HOST,
    TYPE_USB_DEVICE,
    TYPE_ETHERNET_UDP_PULL,
    TYPE_SD_CARD
} BOOTLOADER_TYPE;

typedef struct
{
    BOOTLOADER_TYPE drvType;
    BOOTLOADER_STATES (*drvTrigger)(void);
} BOOTLOADER_INIT;

typedef int ( *BOOTLOADER_CALLBACK ) (void);
typedef int ( *BOOTLOADER_DATA_CALLBACK ) (uint32_t address, uint32_t *data, uint32_t size);

#if 0
typedef enum
{
	DRV_HANDLE_VALID,
	DRV_HANDLE_INVALID,
} drv_handle_t;
#endif

typedef enum
{
    /* Indicates that a driver-specific error has occurred. */
    DRV_CLIENT_STATUS_ERROR_EXTENDED   = -10,

    /* An unspecified error has occurred.*/
    DRV_CLIENT_STATUS_ERROR            =  -1,
    /* The driver is closed, no operations for this client are ongoing,
    and/or the given handle is invalid. */
    DRV_CLIENT_STATUS_CLOSED           =   0,
    /* The driver is currently busy and cannot start additional operations. */
    DRV_CLIENT_STATUS_BUSY             =   1,
    /* The module is running and ready for additional operations */
    DRV_CLIENT_STATUS_READY            =   2,
    /* The module is received packet */
    DRV_CLIENT_STATUS_RECV_START       =   3,

	/* Indicates that the module is in a driver-specific ready/run state. */
    DRV_CLIENT_STATUS_READY_EXTENDED   =  10
} DRV_CLIENT_STATUS;

#if 1 //nsmoon@191010
#define EEPROM_BLOCK_FW_INFO	0
#define EEPROM_BLOCK_FW_KEY		1
#define EEPROM_BLOCK_FW_SN		2
#define EEPROM_BLOCK_FW_MISC	3
#endif

#define FE_BT_MODE	0
#define BE_BT_MODE	1

typedef struct
{
    /* Application current state */
    BOOTLOADER_STATES currentState;

    /* Application previous state */
    BOOTLOADER_STATES prevState;

    /* Datastream buffer size */
    uint16_t bufferSize;

    /* Command buffer length */
    int cmdBufferLength;

    /* Datastream status */
    DRV_CLIENT_STATUS datastreamStatus;

    /* Flag to indicate the user message is been processed */
    bool usrBufferEventComplete;

    /* USB Host Layer Handle */
    uintptr_t hostHandle;

    /* Application data buffer */
    uint8_t *buff1;
    uint8_t *buff2;

    /* Device configured */
    bool deviceConfigured;

    /* HID data received flag*/
    bool DataTransferred;

    /* Callback for Flash Erase function */
    BOOTLOADER_CALLBACK FlashEraseFunc;

    /* Callback for Start App function */
    BOOTLOADER_CALLBACK StartAppFunc;

    /* Callback for Blank Check function */
    BOOTLOADER_CALLBACK BlankCheckFunc;

    /* Callback for Programming Complete function */
    BOOTLOADER_CALLBACK ProgramCompleteFunc;

    /* Callback for Force Bootloader function */
    BOOTLOADER_CALLBACK ForceBootloadFunc;

    /* Determine if we are in the bootloader as a result of a software reset. */
    //bool soft_reset;

#if 1  //nsmoon@170623a
    uint32_t bootloader_wait_cnt;
    uint32_t preSectNum;
    uint32_t curSectNum;
    uint32_t preVirAddr;
    //uint32_t sectStartPhyAddr;
    uint32_t sectStartVirAddr;
    uint32_t appLen;
    uint32_t prgLen;
    uint32_t writeAddr; //for writeRaw
    //bool forceBootloadMode;
    bool connected_pc_tool;
    bool sent_ver_cmd;
    bool device_not_supported;
    bool readRaw;
    bool writeRaw;
    bool writeCode;
    bool progStart;
    uint8_t eepromBlockNo;
	uint8_t modeChange; //0:front-end, back-end
#endif
    uint32_t feResCnt; //nsmoon@211203
} BOOTLOADER_DATA;
#define FE_RES_CNT_ERROR 0xFFFFFFFF

typedef enum
{
     /* Client is closed or the specified handle is invalid */
    BOOTLOADER_CLIENT_STATUS_CLOSED
            /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,
    /* Client is ready */
    BOOTLOADER_CLIENT_STATUS_READY
            /*DOM-IGNORE-BEGIN*/ = 1 /*DOM-IGNORE-END*/,
    BOOTLOADER_CLIENT_STATUS_WRITEFAILURE,
    BOOTLOADER_CLIENT_STATUS_WRITESUCCESS
} BOOTLOADER_CLIENT_STATUS;

typedef enum {
    IDLE = 0,
    DIR_RX,
	DIR_TX
} eDIR;

typedef enum
{
    /* All data from or to the buffer was transferred successfully. */
    DATASTREAM_BUFFER_EVENT_COMPLETE,
    /* There was an error while processing the buffer transfer request. */
    DATASTREAM_BUFFER_EVENT_ERROR,
    /* Data transfer aborted (Applicable in DMA mode) */
    DATASTREAM_BUFFER_EVENT_ABORT
} DATASTREAM_BUFFER_EVENT;

#define BT_OK	0
#define BT_ERR	1

#if PROGRAM_BIN_FILE //nsmoon@191118
typedef struct
{
    uint16_t seqNoPkt;
    uint16_t totalPkt;
    uint32_t binProgAddr;
    uint32_t binProgLen;
    uint16_t hdIdx;
    uint8_t modelOk;
    uint8_t hdCrcOk;
} T_BIN_RECORD;

typedef enum
{
	//magic version number
	BIN_MAGIC_VERSION = 99, //nsmoon@200720

	//invalid sequence
	BIN_MISMATCH_KEY = 40,
	BIN_INVALID_MODEL = 30,
	BIN_INVALID_SEQ = 20,

    // indicates that the CRC value between the calculated value and the
    // value received from data stream did not match
    BIN_REC_CRC_ERROR   = 10,
    // programming error
    BIN_REC_PGM_ERROR   = 5,
    // An unspecified hex record tyype is received
    BIN_REC_UNKNOW_TYPE  = 1,
    // the record type is a valid hex record
    BIN_REC_NORMAL    = 0,
} BIN_RECORD_STATUS;

#define PROGRAM_PACKET_SIZE	(USB_HID_GENERIC_OUT_BUFFER_LENGTH -  3) //cmd(1)+seq(2)

#define EEPROM_BLOCK_0		0   //header
#define EEPROM_BLOCK_1		1   //header2
#define EEPROM_BLOCK_2		2   //header3
#define EEPROM_BLOCK_3		3   //header4

//-------------------------------------------------------
#define MAX_HEADER_SIZE			(256) //0x100

//header0_t size
#define HD_0_SIZE				(64)
#define HD_USB_CMD_SIZE			(1)
#define HD_CODE_MISC_SIZE		(1)
#define HD_CODE_CRC_SIZE		(2)
#define HD_CODE_SIZE_SIZE		(4)
#define HD_MODEL_NAME_SIZE		(24)
#define HD_KEY0_SIZE			(16)
#define HD_KEY1_SIZE			(16)

//header1_t size
#define HD_1_SIZE				(64)
#define HD_FW_VERSION_SIZE		(64 - 2) //crc

//header2_t size
#define HD_2_SIZE				(64)
#define HD_SN_SIZE				(64 - 1 - 2) //cmd+crc
//#define HD_SIZE_TOT				(HD_0_SIZE + HD_1_SIZE + HD_2_SIZE)

#if 1 //nsmoon@230503
//header3 size
#define HD_3_SIZE				(64)
#define HD_FE_EEPROM_SIZE		(64-1) //cmd
#define HD_SIZE_TOT				(HD_0_SIZE + HD_1_SIZE + HD_2_SIZE + HD_3_SIZE)
#endif

typedef union {
	uint8_t all[HD_0_SIZE];
	struct {
		uint16_t code_crc;
		uint16_t code_misc;
		uint32_t code_size;
		uint8_t modelName[HD_MODEL_NAME_SIZE];
		uint8_t key0[HD_KEY0_SIZE];
		uint8_t key1[HD_KEY1_SIZE];
	} ind;
} header0_t; //64-bytes, eeprom block no-1

typedef union {
	uint8_t all[HD_1_SIZE];
	struct {
		uint8_t fwVersion[HD_FW_VERSION_SIZE];
		uint16_t hd01_crc;
	} ind;
} header1_t; //64-bytes, eeprom block no-1
//----------------------------------------------------------
#endif


#define FE_BOOT_SECTOR_BEGIN	0x9FC00000
#define FE_MY_FLASH_BASE_ADDR	FE_BOOT_SECTOR_BEGIN
#define FE_MY_FLASH_SIZE		0x100000 //2MB/2, 0x200000/2
#define FE_APPLICATION_START	0x9D000000
#define FE_APPLICATION_END		(FE_BOOT_SECTOR_BEGIN + FE_MY_FLASH_SIZE)
//#define FE_MY_FLASH_SECT_SIZE	0x4000  /* 16K */
//#define FE_BT_EEPROM_ADDR		(FE_MY_FLASH_BASE_ADDR + FE_MY_FLASH_SIZE - FE_MY_FLASH_SECT_SIZE)
//#define FE_BT_START_ADDR		FE_APPLICATION_START
//#define FE_BT_END_ADDR		FE_BT_EEPROM_ADDR //(FE_MY_FLASH_BASE_ADDR + FE_MY_FLASH_SIZE)
#define FE_PA_TO_VFA(x)			((x) - FE_APPLICATION_START)
#define FE_PA_TO_KVA0(x)		((x)|0x80000000)

#define DATA_RECORD           0
#define END_OF_FILE_RECORD    1
#define EXT_SEG_ADRS_RECORD   2
#define EXT_LIN_ADRS_RECORD   4
#define START_LIN_ADRS_RECORD 5

typedef enum
{
    // indicates that the CRC value between the calculated value and the
    // value received from data stream did not match
    HEX_REC_CRC_ERROR   = -10,
    // programming error
    HEX_REC_PGM_ERROR   = -5,
    // An unspecified hex record tyype is received
    HEX_REC_UNKNOW_TYPE  = -1,
    // the record type is a valid hex record
    HEX_REC_NORMAL    = 0,
} HEX_RECORD_STATUS;

typedef struct
{
    uint8_t RecDataLen;
    uint32_t Address;
    uint8_t RecType;
    uint8_t* Data;
    uint8_t CheckSum;
    uint32_t ExtSegAddress;
    uint32_t ExtLinAddress;
} T_HEX_RECORD;

#define MAX_VERSION_LEN         16
//#define FE_MODEL_NAME           "FRONT-END" //2048EFH064
#define FE_HEADER_SIZE		    (HD_3_SIZE -1 -2) //-(cmd+crc)
#define FE_FWFILE_SIZE		    (26) //+\0 //nsmoon@200717 27=>26
#define FE_TIMESTAMP_SIZE	    (13) //+\0 200618113132
#define FE_MODEL_SIZE		    (10) //+\0
#define FE_PRGLEN_SIZE          (6) //+\0
#define FE_PRGCRC_SIZE          (5) //\0


///////////////////////////////////////////////////////////////////////////////////////////////
extern BOOTLOADER_DATA bootloaderData;
extern void Bootloader_Initialize();
extern int Bootloader_Task();
#if (APP_TEST_FLASH_ENABLE == 1)
extern void Bootloader_test_flash(void);
#endif
#if PROGRAM_BIN_FILE //nsmoon@191118
//extern uint8_t mcuKey[EFUSE_KEY_LEN];
#endif

#ifdef FLASH_DUAL_BANK //nsmoon@200717
#define FLASH_BANK_FOR_BOOT		0
#define FLASH_BANK_FOR_PROGRAM	1
extern void SelectFlashBank(int mode);
#define FLASH_BANK_A         1
#define FLASH_BANK_B         2
#define FLASH_BANK_END       3
extern int flash_bank;
#endif
extern void MY_WDOG_Refresh();
#endif /* BOOTLOADER_H_ */
