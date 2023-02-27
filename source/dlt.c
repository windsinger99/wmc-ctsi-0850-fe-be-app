/*
 * dlt.c
 *
 *  Created on: Jul 19, 2019
 *      Author: user
 */

#include "common.h"
#include "macro.h"
#include "scan.h"
#include "TSP_Layout.h"
#include "dlt.h"
#include "app.h"
#include "usb_misc.h"
#include "usb_device_descriptor.h"
#include "pitch_table.h"
#include <stddef.h>
#include <string.h>
//#include <stdarg.h>

#define NUM_ADC_BUFF	3
#define ADC_BUFF_CNT	0
#define ADC_BUFF_LFT	1
#define ADC_BUFF_RHT	2

//------------------------------------------------------------------------------

#if ENABLE_EXB_BUF_BANKING == TRUE
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t Exb_Send_Buf__[2][EXB_XFER_BUF_SIZE];
uint8_t Exb_Send_Buf_Idx;
uint8_t *Exb_Send_Buf;
#else
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) uint8_t Exb_Send_Buf[EXB_XFER_BUF_SIZE];
#endif


typedef enum
{
    DLT_MODE_ALL = 0x01,
    DLT_MODE_IND = 0x02,
    DLT_MODE_PD = 0x10,
    DLT_MODE_LED = 0x20,
    DLT_MODE_MM = 0x40,
    SET_THRESHOLD_DATA = 0x80,
} usb_sub_cmd_t;


DLT_SCAN_INFO_T DLT_scan;
static DLT_mode_t DLT_mode;
int slope_byte_x, slope_byte_y;
volatile uint32_t total_send_cnt;

#ifdef ENABLE_CHECK_ADC_HEADER //nsmoon@230118
static uint8_t DLT_adc_header[CHK_ADC_HDR_SIZE];
#endif
#define USE_GET_VERSION //nsmoon@200706
#define FLASH_DUAL_BANK //nsmoon@200717

#ifdef USE_GET_VERSION
#define BOOT_FLASH_BASE_ADDRESS         (0x9FC00000) //nsmoon@200727
#define APP_FLASH_BASE_ADDRESS          (0x9D000000)
#if (FLASH_MEM_SIZE == 2048)
#define DEVICE_FLASH_SIZE               (0x200000)
#elif (FLASH_MEM_SIZE == 512)
#define DEVICE_FLASH_SIZE               (0x80000)
#else
#error "FLASH_MEM_SIZE is not defined!!!"
#endif
#define APP_FLASH_END_ADDRESS           (0x9D000000 + DEVICE_FLASH_SIZE - 1)
#ifdef FLASH_DUAL_BANK //nsmoon@200717
#define FLASH_SIZE          (APP_FLASH_END_ADDRESS - APP_FLASH_BASE_ADDRESS + 1) //0x80000
#define FLASH_SIZE_HALF     (FLASH_SIZE / 2) //0x40000
#define EEPROM_FLASH_SIZE   (0x4000) //(NVM_PAGE_SIZE) //0x4000
#define APP_FLASH_SIZE      (FLASH_SIZE_HALF - EEPROM_FLASH_SIZE) //0x3C000=(0x40000-0x4000)
#define MY_USB_PACKET_SIZE              USB_DEVICE_EP0_BUFFER_SIZE
#define BOOTLOADER_EEPROM_BASE_ADDRESS  (APP_FLASH_BASE_ADDRESS + APP_FLASH_SIZE) //0x9D03C000
#define MY_APP_FLASH_SIZE               (FLASH_SIZE_HALF - EEPROM_FLASH_SIZE) //0x3C000=(0x40000-0x4000)
#define MY_EEPROM_ADDR_0609             (APP_FLASH_BASE_ADDRESS + FLASH_SIZE - EEPROM_FLASH_SIZE)
#define MY_EEPROM_ADDR                  (BOOTLOADER_EEPROM_BASE_ADDRESS)    //0x9D03C000
#define MY_EEPROM_SIZE                  (MY_USB_PACKET_SIZE-1)  //65
#else
#define MY_NVM_PAGE_SIZE                (0x4000)
#define MY_USB_PACKET_SIZE              64
#define BOOTLOADER_EEPROM_BASE_ADDRESS  (APP_FLASH_END_ADDRESS + 1 - (MY_NVM_PAGE_SIZE * 2))
#define MY_APP_FLASH_SIZE               ((BOOTLOADER_EEPROM_BASE_ADDRESS - APP_FLASH_BASE_ADDRESS) / 2) //0xFC000 = (0x200000-0x8000)/2
#define MY_APP_FLASH_ADDR               (APP_FLASH_BASE_ADDRESS + MY_APP_FLASH_SIZE)
#define MY_APP_FLASH_END_ADDR           (MY_APP_FLASH_ADDR + MY_APP_FLASH_SIZE)
#define MY_EEPROM_ADDR                  (BOOTLOADER_EEPROM_BASE_ADDRESS + MY_NVM_PAGE_SIZE)
#define MY_EEPROM_SIZE                  MY_USB_PACKET_SIZE
#endif

#define FE_HEADER_SIZE		    (MY_USB_PACKET_SIZE -1 -2) //-(cmd+crc)
#define FE_EEPROM_SIZE          FE_HEADER_SIZE
#define FE_FWFILE_SIZE		    (27) //+\0
#define FE_TIMESTAMP_SIZE	    (13) //+\0 200618113132
#define FE_MODEL_SIZE		    (10) //+\0
#define FE_PRGLEN_SIZE          (6) //+\0
#define FE_PRGCRC_SIZE          (5) //+\0
#define MAX_VERSION_LEN         (3) //+\0

static char frontend_eeprom[FE_EEPROM_SIZE]; //from hex file
#if 0 //nsmoon@210428
static char frontend_fwfileText[FE_FWFILE_SIZE];
static char frontend_timestampText[FE_TIMESTAMP_SIZE];
static char frontend_modelText[FE_MODEL_SIZE];
static char frontend_prgLenText[FE_PRGLEN_SIZE];
static char frontend_prgCrcText[FE_PRGCRC_SIZE];
#endif
static char frontend_fwVersion[MAX_VERSION_LEN];
static unsigned int hexProgLen = 0; //from file
static unsigned short hexCrc = 0;


void get_fw_version(char *src, char *dst, int len) {
    int i, found, cnt;

    cnt = found = 0;
    for (i = 0; i <= strlen(src); i++) {
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

#define TRACE_SFFI(...)     TRACE(__VA_ARGS__)
int show_frontend_fwinfo() {
    char *pCur, *pPrev, *buff;
    int idx = 0, len = 0, lenTot = 0, lenTmp;
    char *fwfileText;
    char *timestampText;
    char *modelText;
    char *prgLenText;
    char *prgCrcText;
    char *fwVer;
    unsigned short *fe_crc;
    unsigned int *fe_len;
    char tmpBuff[FE_FWFILE_SIZE] = { 0 }; //nsmoon@210428
    TRACE_SFFI("show_frontend_fwinfo..\r\n");

    pCur = buff = &frontend_eeprom[0];
    fwfileText = &tmpBuff[0];
    timestampText = &tmpBuff[0];
    modelText = &tmpBuff[0];
    prgLenText = &tmpBuff[0];
    prgCrcText = &tmpBuff[0];
    fwVer = &frontend_fwVersion[0];
    fe_crc = &hexCrc;
    fe_len = &hexProgLen;

    buff[FE_EEPROM_SIZE - 1] = '\0';
    TRACE_SFFI("buff=[%s] \r\n", buff);

    while (1) {
        if (lenTot > 0) {
            pCur++; //skip ','
        }
        pPrev = pCur;
        pCur = strchr(&buff[lenTot], ',');
        if (pCur == NULL) {
            break;
        }
        len = pCur - pPrev;
        lenTot += len + 1;

        switch (idx) {
        case 0: //Firmware File Name
        {
            if (len > (FE_FWFILE_SIZE - 1)) {
                TRACE_SFFI("ERROR! show_frontend_fwinfo..invalid fwfileText[] len: %d \r\n", len);
                lenTmp = (FE_FWFILE_SIZE - 1);
            } else {
                lenTmp = len;
            }
            memcpy((char*) &fwfileText[0], (char*) (pPrev), lenTmp);
            fwfileText[lenTmp] = '\0';
            get_fw_version(fwfileText, fwVer, MAX_VERSION_LEN);
            TRACE_SFFI("...File Name= [%s][%s]\r\n", fwfileText, fwVer);
        }
        break;

        case 1: //Timestamp
        {
            if (len > (FE_TIMESTAMP_SIZE - 1)) {
                TRACE_SFFI("ERROR! show_frontend_fwinfo..invalid timestampText[] len: %d \r\n", len);
                lenTmp = (FE_TIMESTAMP_SIZE - 1);
            } else {
                lenTmp = len;
            }
            memcpy((char*) &timestampText[0], (char*) (pPrev), lenTmp);
            timestampText[lenTmp] = '\0';
            TRACE_SFFI("...Timestamp= [%s] \r\n", timestampText);
        }
        break;

        case 2: //FE Model Name
        {
            if (len > (FE_MODEL_SIZE - 1)) {
                TRACE_SFFI("ERROR! show_frontend_fwinfo..invalid modelText[] len: %d \r\n", len);
                lenTmp = (FE_MODEL_SIZE - 1);
            } else {
                lenTmp = len;
            }
            memcpy((char*) &modelText[0], (char*) (pPrev), lenTmp);
            modelText[lenTmp] = '\0';
            TRACE_SFFI("...Model= [%s] \r\n", modelText);
        }
        break;

        case 3: //FE Program Len
        {
            if (len > (FE_PRGLEN_SIZE - 1)) {
                TRACE_SFFI("ERROR! show_frontend_fwinfo..invalid prgLenText[] len: %d \r\n", len);
                lenTmp = (FE_PRGLEN_SIZE - 1);
            } else {
                lenTmp = len;
            }
            memcpy((char*) &prgLenText[0], (char*) (pPrev), lenTmp);
            prgLenText[lenTmp] = '\0';
            *fe_len = (uint32_t) strtol(prgLenText, NULL, 16);
            TRACE_SFFI("...Prg Len= [%s](%d)\r\n", prgLenText, *fe_len);
        }
        break;

        case 4: //FE Program CRC
        {
            if (len > (FE_PRGCRC_SIZE - 1)) {
                TRACE_SFFI("ERROR! show_frontend_fwinfo..invalid prgCrcText[] len: %d \r\n", len);
                lenTmp = (FE_PRGCRC_SIZE - 1);
            } else {
                lenTmp = len;
            }
            memcpy((char*) &prgCrcText[0], (char*) (pPrev), lenTmp);
            prgCrcText[lenTmp] = '\0';
            *fe_crc = (unsigned short) strtol(prgCrcText, NULL, 16);
            TRACE_SFFI("...Prg CRC= [%s] \r\n", prgCrcText);
        }
        break;
        default:
            TRACE_SFFI("ERROR! show_frontend_fwinfo..invalid idx: %d \r\n", idx);
            break;
        }
        idx++;
    }

    return 0;
}

void get_ver_string(void) {
#if 0 //nsmoon@210428
#define BT_VER_LEN  8
    char test_str[BT_VER_LEN+1];
    //memcpy((void *)&test_str[0], (void *)(BOOT_FLASH_BASE_ADDRESS+0xF018), 10);
    //memcpy((void *)&test_str[0], (void *)(BOOT_FLASH_BASE_ADDRESS+0xFA52), BT_VER_LEN);
    memcpy((void *)&test_str[0], (void *)(BOOT_FLASH_BASE_ADDRESS+0xEEDA), BT_VER_LEN);
    test_str[BT_VER_LEN] = '\0';
    //SYS_DEBUG_Print("ver= [%s] \r\n", test_str); //for test

    uint32_t Address;
    if (strcmp(test_str, "@200609A") == 0) {
        Address = MY_EEPROM_ADDR_0609;
    }
    else {
        Address = MY_EEPROM_ADDR;
    }
    TRACE("Address=%x\r\n", Address);
#endif

    memcpy((void*) &frontend_eeprom[0], (void*) MY_EEPROM_ADDR, FE_EEPROM_SIZE); //nsmoon@210908 bug-fix MY_EEPROM_SIZE=>FE_EEPROM_SIZE
    show_frontend_fwinfo();
}
#endif

void get_slope_size(void) {
    slope_byte_x = ((X_TOTAL_OFFSET - 1) / 8 + 1);
    slope_byte_y = ((Y_TOTAL_OFFSET - 1) / 8 + 1);
}

//------------------------------------------------------------------------------
// Make Exb Buf
//  - X_CELL_SIZE, Y_CELL_SIZE, X_MAX_OFFSET, Y_MAX_OFFSET
//  - Dead cell count
//  - X LED Pitch, X PD Pitch, Y LED Pitch, Y PD Pitch
//  - sensor_zero_x, sensor_end_x, sensor_zero_y, sensor_end_y
//  - aarea_zero_x, aarea_end_x, aarea_zero_y, aarea_end_y
//  - X Led dead cell number list, X Pd dead cell list
//  - Y Led dead cell number list, Y Pd dead cell list
//  - F/W Version
// Return xfer buffer length w/o CRC
//------------------------------------------------------------------------------
uint32_t copy_ofst_to_exb(void) {
    uint32_t i, j, idx = 0;

    //header
    Exb_Send_Buf[0] = 0;
    Exb_Send_Buf[1] = 0;
    Exb_Send_Buf[2] = 0;
    Exb_Send_Buf[3] = 0xA0; //a5=>a0

    //Number of sensor
    Exb_Send_Buf[4] = (X_CELL_SIZE >> 8) & 0xff;
    Exb_Send_Buf[5] = X_CELL_SIZE & 0xff;
    Exb_Send_Buf[6] = (Y_CELL_SIZE >> 8) & 0xff;
    Exb_Send_Buf[7] = Y_CELL_SIZE & 0xff;

    //Scan Mode (not-used, reserved)
    Exb_Send_Buf[8] = 2; //FINE_SCAN;

    //Max offset value
    Exb_Send_Buf[9] = X_MAX_OFFSET; //MAX_X_INCLINE_NUMBER;
    Exb_Send_Buf[10] = Y_MAX_OFFSET; //MAX_Y_INCLINE_NUMBER;
    idx = 11;

#ifdef DEAD_CELL_TBL //nsmoon@190926
    //length of X dead cell
    Exb_Send_Buf[idx] = xLedDeadCellCnt;
    Exb_Send_Buf[idx + 1] = xPdDeadCellCnt;
    idx += 2;

    //length of Y dead cell
    Exb_Send_Buf[idx] = yLedDeadCellCnt;
    Exb_Send_Buf[idx + 1] = yPdDeadCellCnt;
    idx += 2;
#endif

#ifdef USE_GET_VERSION //nsmoon@200706
    //Max 2nd offset value
    Exb_Send_Buf[idx] = X_MAX_OFFSET;
    Exb_Send_Buf[idx + 1] = Y_MAX_OFFSET;
    idx += 2;

    //length of version string
    Exb_Send_Buf[idx] = 2; //2-byte
    Exb_Send_Buf[idx + 1] = 0;
    idx += 2;
#else
#ifdef SECOND_OFST_TBL //nsmoon@190624
    //Max 2nd offset value
    Exb_Send_Buf[idx] = X_CENTER_INCLINE_2ND;
    Exb_Send_Buf[idx + 1] = Y_CENTER_INCLINE_2ND;
    idx += 2;

    //length of sensor table for 2nd offset table
    Exb_Send_Buf[idx] = MAX_SEN_TBL_OFST2; //x-size
    Exb_Send_Buf[idx + 1] = MAX_SEN_TBL_OFST2; //y-size
    idx += 2;
#else
    for (i = 0; i < MAX_X_INCLINE_NUMBER; i++) {
        Exb_Send_Buf[idx + i] = *(uint8_t *)&X_Incline_Offset_Table[i];
    }
    idx += MAX_X_INCLINE_NUMBER;

    for (i = 0; i < MAX_Y_INCLINE_NUMBER; i++) {
        Exb_Send_Buf[idx + i] = *(uint8_t *)&Y_Incline_Offset_Table[i];
    }
    idx += MAX_Y_INCLINE_NUMBER;
#endif
#endif

    //position table of X-led
    uint32_t tmpLongX; //, tmpLongY; //nsmoon@170828a
    uint8_t tmpChar;
    //pByte = (unsigned char *)&tmpLong;
    for (i = 0; i < X_CELL_SIZE; i++) {
        tmpLongX = *(uint32_t*) &sensor_XLED_Pos__[i];
        for (j = 0; j < sizeof(float); j++) {
            tmpChar = (unsigned char) (tmpLongX >> (8 * j));
            Exb_Send_Buf[idx + j] = tmpChar;
        }
        idx += 4;
    }

    //position table of X-pd
    for (i = 0; i < X_CELL_SIZE; i++) {
        tmpLongX = *(uint32_t*) &sensor_XPD_Pos__[i];
        for (j = 0; j < sizeof(float); j++) {
            tmpChar = (unsigned char) (tmpLongX >> (8 * j));
            Exb_Send_Buf[idx + j] = tmpChar;
        }
        idx += 4;
    }

    //position table of Y-led
    for (i = 0; i < Y_CELL_SIZE; i++) {
        tmpLongX = *(uint32_t*) &sensor_YLED_Pos__[i];
        for (j = 0; j < sizeof(float); j++) {
            tmpChar = (unsigned char) (tmpLongX >> (8 * j));
            Exb_Send_Buf[idx + j] = tmpChar;
        }
        idx += 4;
    }

    //position table of X-pd
    for (i = 0; i < Y_CELL_SIZE; i++) {
        tmpLongX = *(uint32_t*) &sensor_YPD_Pos__[i];
        for (j = 0; j < sizeof(float); j++) {
            tmpChar = (unsigned char) (tmpLongX >> (8 * j));
            Exb_Send_Buf[idx + j] = tmpChar;
        }
        idx += 4;
    }

#ifdef FRONTEND_AAREA //nsmoon@190624
    //sensor position X-zero
    tmpLongX = *(uint32_t*) &sensor_zero_x__;
    for (j = 0; j < sizeof(float); j++) {
        tmpChar = (unsigned char) (tmpLongX >> (8 * j));
        Exb_Send_Buf[idx + j] = tmpChar;
    }
    idx += 4;

    //sensor position X-end
    tmpLongX = *(uint32_t*) &sensor_end_x__;
    for (j = 0; j < sizeof(float); j++) {
        tmpChar = (unsigned char) (tmpLongX >> (8 * j));
        Exb_Send_Buf[idx + j] = tmpChar;
    }
    idx += 4;

    //sensor position Y-zero
    tmpLongX = *(uint32_t*) &sensor_zero_y__;
    for (j = 0; j < sizeof(float); j++) {
        tmpChar = (unsigned char) (tmpLongX >> (8 * j));
        Exb_Send_Buf[idx + j] = tmpChar;
    }
    idx += 4;

    //sensor position Y-end
    tmpLongX = *(uint32_t*) &sensor_end_y__;
    for (j = 0; j < sizeof(float); j++) {
        tmpChar = (unsigned char) (tmpLongX >> (8 * j));
        Exb_Send_Buf[idx + j] = tmpChar;
    }
    idx += 4;

    //active area position X-zero
    tmpLongX = *(uint32_t*) &aarea_zero_x__;
    for (j = 0; j < sizeof(float); j++) {
        tmpChar = (unsigned char) (tmpLongX >> (8 * j));
        Exb_Send_Buf[idx + j] = tmpChar;
    }
    idx += 4;

    //active area position X-end
    tmpLongX = *(uint32_t*) &aarea_end_x__;
    for (j = 0; j < sizeof(float); j++) {
        tmpChar = (unsigned char) (tmpLongX >> (8 * j));
        Exb_Send_Buf[idx + j] = tmpChar;
    }
    idx += 4;

    //active area position Y-zero
    tmpLongX = *(uint32_t*) &aarea_zero_y__;
    for (j = 0; j < sizeof(float); j++) {
        tmpChar = (unsigned char) (tmpLongX >> (8 * j));
        Exb_Send_Buf[idx + j] = tmpChar;
    }
    idx += 4;

    //active area position Y-end
    tmpLongX = *(uint32_t*) &aarea_end_y__;
    for (j = 0; j < sizeof(float); j++) {
        tmpChar = (unsigned char) (tmpLongX >> (8 * j));
        Exb_Send_Buf[idx + j] = tmpChar;
    }
    idx += 4;

#ifdef DEAD_CELL_TBL //nsmoon@190926
    //X LED dead cell
    for (i = 0; i < X_CELL_SIZE; i++) {
        if (xCellStatus[i].isLedDead) {
            Exb_Send_Buf[idx++] = i;
        }
    }

    //X PD dead cell
    for (i = 0; i < X_CELL_SIZE; i++) {
        if (xCellStatus[i].isPdDead) {
            Exb_Send_Buf[idx++] = i;
        }
    }

    //Y LED dead cell
    for (i = 0; i < Y_CELL_SIZE; i++) {
        if (yCellStatus[i].isLedDead) {
            Exb_Send_Buf[idx++] = i;
        }
    }

    //Y PD dead cell
    for (i = 0; i < Y_CELL_SIZE; i++) {
        if (yCellStatus[i].isPdDead) {
            Exb_Send_Buf[idx++] = i;
        }
    }
#endif
#endif

#ifdef USE_GET_VERSION //nsmoon@200706
    //version string
    for (i = 0; i < 2; i++) {
        Exb_Send_Buf[idx + i] = frontend_fwVersion[i];
    }
    idx += 2;
#else
#ifdef SECOND_OFST_TBL //nsmoon@190624
    //X sensor table for 2nd offset
    for (i = 0; i < MAX_SEN_TBL_OFST2; i++) {
        Exb_Send_Buf[idx + i] = sen_tbl_ofst2_x[i]; //x
    }
    idx += MAX_SEN_TBL_OFST2;

    //Y sensor table for 2nd offset
    for (i = 0; i < MAX_SEN_TBL_OFST2; i++) {
        Exb_Send_Buf[idx + i] = sen_tbl_ofst2_y[i]; //y
    }
    idx += MAX_SEN_TBL_OFST2;
#endif
#endif

    return idx;
}

#if (MODEL_SPT != MODEL_CTSK_N650_V100) && (MODEL_SPT != MODEL_CTSK_N750_V100) && (MODEL_SPT != MODEL_CTSK_N850_V100)
uint32_t Send_ExbBit_Data_to_USBCPU_Init(void) {
    uint32_t idx;

    Exb_Send_Buf[0] = 0;
    Exb_Send_Buf[1] = 0;
    Exb_Send_Buf[2] = 0;
    Exb_Send_Buf[3] = 0xA5; //(*)
    idx = 4;

    memset(&Exb_Send_Buf[idx], 0, EXB_XFER_BUF_SIZE - 4);
    return 0; //nsmoon@210720
}
#else
uint32_t Send_ExbBit_Data_to_USBCPU_Init(void) {
    uint32_t idx;

    g_usb_in_buff[0] = 0;
    g_usb_in_buff[1] = 0;
    g_usb_in_buff[2] = 0;
    g_usb_in_buff[3] = 0xA5; //(*)
    idx = 4;

    memset(&g_usb_in_buff[idx], 0, MAX_USB_IN_BUF_SIZE - idx);

    Exb_Send_Buf[0] = 0;
    Exb_Send_Buf[1] = 0;
    Exb_Send_Buf[2] = 0;
    Exb_Send_Buf[3] = 0xA5; //(*)

    memset(&Exb_Send_Buf[idx], 0, EXB_XFER_BUF_SIZE - idx);

    return 0; //nsmoon@210720
}
#endif
uint32_t Send_ExbBit_Data_to_USBCPU_size(void) {
    int idx; //i
    //uint8_t byte_idx, bit_idx;

    get_slope_size();

    idx = 4 + (X_CELL_SIZE * slope_byte_x) + (Y_CELL_SIZE * slope_byte_y);

    return idx;
}

#if DLT_LED_ON_TIME_ENABLE //nsmoon@220117
uint8_t getLedOnTime(uint8_t axisType, uint8_t pdIdx, uint8_t ledIdx) {
    int16_t offset = ledIdx - pdIdx;
    int16_t offsetMax = (axisType == X_AXIS) ? X_MAX_OFFSET : Y_MAX_OFFSET;
    if (M_ABS(offset) > offsetMax) {
        return 0; //error
    }

    uint16_t seqIdx = findStartScanSequenceIdx(axisType, pdIdx, ledIdx, ledIdx);
    uint16_t seqIdxMax =  (axisType == X_AXIS) ? X_TOTAL_SCAN_STEPS : Y_TOTAL_SCAN_STEPS;
    if (seqIdx < 0 || seqIdx >= seqIdxMax) {
        printf("ERROR! getLedOnTime..invalid seqIdx..%d %d/%d %d/%d \r\n",
               axisType, seqIdx, seqIdxMax, ledIdx, pdIdx);
        return 0; //error
    }
    uint16_t onTime = (axisType == X_AXIS) ? xAxisDacIdx[seqIdx]-LED_ON_DAC_MIN_X : yAxisDacIdx[seqIdx]-LED_ON_DAC_MIN_Y;
    uint16_t onTimeMax = (axisType == X_AXIS) ? LED_ON_DAC_MAX_X-LED_ON_DAC_MIN_X : LED_ON_DAC_MAX_Y-LED_ON_DAC_MIN_Y;
    uint8_t ret = (uint8_t)((onTime * 0xFF) / onTimeMax);
    return ret;
}

uint8_t getLedCurrentIdx(uint8_t axisType, uint8_t pdIdx, uint8_t ledIdx) {
    int16_t offset = ledIdx - pdIdx;
    int16_t offsetMax = (axisType == X_AXIS) ? X_MAX_OFFSET : Y_MAX_OFFSET;
    if (M_ABS(offset) > offsetMax) {
        return 0; //error
    }

    uint16_t seqIdx = findStartScanSequenceIdx(axisType, pdIdx, ledIdx, ledIdx);
    uint16_t seqIdxMax =
        (axisType == X_AXIS) ? X_TOTAL_SCAN_STEPS : Y_TOTAL_SCAN_STEPS;
    if (seqIdx < 0 || seqIdx >= seqIdxMax) {
        printf("ERROR! getLedCurrentIdx..invalid seqIdx..%d %d/%d %d/%d \r\n",
               axisType, seqIdx, seqIdxMax, ledIdx, pdIdx);
        return 0; //error
    }
#if DLT_THRESHOLD_ENABLE  //nsmoon@220119
    uint8_t ret = (axisType == X_AXIS) ? xScanThresholdData[pdIdx][offset + offsetMax] : yScanThresholdData[pdIdx][offset + offsetMax];
#else
    uint8_t curTblIdx = (axisType == X_AXIS) ? xAxisLedCurrentTblIdx[seqIdx] : yAxisLedCurrentTblIdx[seqIdx];
    uint8_t curTblIdxMax = (axisType == X_AXIS) ? X_LED_CURRENT_MAX_INDEX : Y_LED_CURRENT_MAX_INDEX;
    uint8_t ret = (curTblIdxMax == 0) ? 0 : (curTblIdx * 0xFF) / curTblIdxMax;
#endif
    return ret;
}
#endif

uint32_t Send_ADC_Data_to_USBCPU(uint8_t usbReq) {
    uint16_t i, idx; //k
    uint8_t pdIdx, ledIdx, adcResult;
    int8_t offset;

    idx = EXB_X_SUB_DATA_START;

    if (usbReq == USB_REQ_SEN_LIFE_TEST) {
        DLT_mode.dltMode = DLT_MODE_ALL;
    }

    //======================================================================
    // 1. make center offset or device idx @ senX[0]
    //======================================================================
    // X-Axis
    //----------------------------------------------------------------------
    for (i = 0; i < X_CELL_SIZE; i++) {
        if (DLT_mode.dltMode & DLT_MODE_ALL) { // offset mode
            adcResult = getADCResultByPdIdxAndOffset(X_AXIS, i, 0);
        } else { // index mode
            if (DLT_mode.dltMode & DLT_MODE_LED) {
                ledIdx = DLT_mode.senX[0];
                pdIdx = i;
            } else {
                pdIdx = DLT_mode.senX[0];
                ledIdx = i;
            }
            adcResult = getADCResultByDeviceIdx(X_AXIS, pdIdx, ledIdx);
        }

        Exb_Send_Buf[idx++] = adcResult;
    }
    //----------------------------------------------------------------------
    // Y-Axis
    //----------------------------------------------------------------------
    for (i = 0; i < Y_CELL_SIZE; i++) {
        if (DLT_mode.dltMode & DLT_MODE_ALL) { // offset mode
            adcResult = getADCResultByPdIdxAndOffset(Y_AXIS, i, 0);
        } else { // index mode
            if (DLT_mode.dltMode & DLT_MODE_LED) { // LED mode
                ledIdx = DLT_mode.senY[0];
                pdIdx = i;
            } else { // PD mode
                pdIdx = DLT_mode.senY[0];
                ledIdx = i;
            }
            adcResult = getADCResultByDeviceIdx(Y_AXIS, pdIdx, ledIdx);
        }

        Exb_Send_Buf[idx++] = adcResult;
    }

    if (usbReq == USB_REQ_SEN_LIFE_TEST) {
        return idx;
    }

    //======================================================================
    // 2. make left offset or device idx @ senX[1]
    //======================================================================
    // X-Axis
    //----------------------------------------------------------------------
    for (i = 0; i < X_CELL_SIZE; i++) {
        if (DLT_mode.dltMode & DLT_MODE_ALL) { // offset mode
            if (DLT_mode.dltMode & DLT_MODE_MM) { // Min/Max offset mode
                offset = - X_MAX_OFFSET;
            } else {
                offset = DLT_mode.senX[0] - X_MAX_OFFSET;
            }

            if (DLT_mode.dltMode & DLT_MODE_LED) { // LED mode
                adcResult = getADCResultByLedIdxAndOffset(X_AXIS, i, offset);
            } else { // PD mode
                adcResult = getADCResultByPdIdxAndOffset(X_AXIS, i, offset);
            }
        } else { // index mode
#if DLT_LED_ON_TIME_ENABLE //nsmoon@220117
            if (DLT_mode.dltMode & DLT_MODE_LED) {
                ledIdx = DLT_mode.senX[0];
                pdIdx = i;
            } else {
                pdIdx = DLT_mode.senX[0];
                ledIdx = i;
            }
            adcResult = getLedOnTime(X_AXIS, pdIdx, ledIdx);
#else
            if ( DLT_mode.dltMode & DLT_MODE_LED ) {
                ledIdx = DLT_mode.senX[1];
                pdIdx = i;
            } else {
                pdIdx = DLT_mode.senX[1];
                ledIdx = i;
            }
            adcResult = getADCResultByDeviceIdx(X_AXIS, pdIdx, ledIdx);
#endif
        }

        Exb_Send_Buf[idx++] = adcResult;
    }
    //----------------------------------------------------------------------
    // Y-Axis
    //----------------------------------------------------------------------
    for (i = 0; i < Y_CELL_SIZE; i++) {
        if (DLT_mode.dltMode & DLT_MODE_ALL) { // offset mode
            if (DLT_mode.dltMode & DLT_MODE_MM) { // Min/Max offset mode
                offset = - Y_MAX_OFFSET;
            } else {
                offset = DLT_mode.senY[0] - Y_MAX_OFFSET;
            }

            if (DLT_mode.dltMode & DLT_MODE_LED) { // LED mode
                adcResult = getADCResultByLedIdxAndOffset(Y_AXIS, i, offset);
            } else { // PD mode
                adcResult = getADCResultByPdIdxAndOffset(Y_AXIS, i, offset);
            }
        } else { // index mode
#if DLT_LED_ON_TIME_ENABLE //nsmoon@220117
            if (DLT_mode.dltMode & DLT_MODE_LED) {
                ledIdx = DLT_mode.senY[0];
                pdIdx = i;
            } else {
                pdIdx = DLT_mode.senY[0];
                ledIdx = i;
            }
            adcResult = getLedOnTime(Y_AXIS, pdIdx, ledIdx);
#else
            if ( DLT_mode.dltMode & DLT_MODE_LED ) {
                ledIdx = DLT_mode.senY[1];
                pdIdx = i;
            } else {
                pdIdx = DLT_mode.senY[1];
                ledIdx = i;
            }
            adcResult = getADCResultByDeviceIdx(Y_AXIS, pdIdx, ledIdx);
#endif
        }

        Exb_Send_Buf[idx++] = adcResult;
    }

    //======================================================================
    // 3. make right offset or device idx @ senX[2]
    //======================================================================
    // X-Axis
    //----------------------------------------------------------------------
    for (i = 0; i < X_CELL_SIZE; i++) {
        if (DLT_mode.dltMode & DLT_MODE_ALL) { // offset mode
            if (DLT_mode.dltMode & DLT_MODE_MM) { // Min/Max offset mode
                offset = X_MAX_OFFSET;
            } else {
                offset = DLT_mode.senX[1] - X_MAX_OFFSET;
            }

            if (DLT_mode.dltMode & DLT_MODE_LED) { // LED mode
                adcResult = getADCResultByLedIdxAndOffset(X_AXIS, i, offset);
            } else { // PD mode
                adcResult = getADCResultByPdIdxAndOffset(X_AXIS, i, offset);
            }
        } else { // index mode
#if DLT_LED_ON_TIME_ENABLE //nsmoon@220117
            if (DLT_mode.dltMode & DLT_MODE_LED) {
                ledIdx = DLT_mode.senX[0];
                pdIdx = i;
            } else {
                pdIdx = DLT_mode.senX[0];
                ledIdx = i;
            }
            adcResult = getLedCurrentIdx(X_AXIS, pdIdx, ledIdx);
#else
            if ( DLT_mode.dltMode & DLT_MODE_LED ) {
                ledIdx = DLT_mode.senX[2];
                pdIdx = i;
            } else {
                pdIdx = DLT_mode.senX[2];
                ledIdx = i;
            }
            adcResult = getADCResultByDeviceIdx(X_AXIS, pdIdx, ledIdx);
#endif
        }

        Exb_Send_Buf[idx++] = adcResult;
    }
    //----------------------------------------------------------------------
    // Y-Axis
    //----------------------------------------------------------------------
    for (i = 0; i < Y_CELL_SIZE; i++) {
        if (DLT_mode.dltMode & DLT_MODE_ALL) { // offset mode
            if (DLT_mode.dltMode & DLT_MODE_MM) { // Min/Max offset mode
                offset = Y_MAX_OFFSET;
            } else {
                offset = DLT_mode.senY[1] - Y_MAX_OFFSET;
            }

            if (DLT_mode.dltMode & DLT_MODE_LED) { // LED mode
                adcResult = getADCResultByLedIdxAndOffset(Y_AXIS, i, offset);
            } else { // PD mode
                adcResult = getADCResultByPdIdxAndOffset(Y_AXIS, i, offset);
            }
        } else { // index mode
#if DLT_LED_ON_TIME_ENABLE //nsmoon@220117
            if (DLT_mode.dltMode & DLT_MODE_LED) {
                ledIdx = DLT_mode.senY[0];
                pdIdx = i;
            } else {
                pdIdx = DLT_mode.senY[0];
                ledIdx = i;
            }
            adcResult = getLedCurrentIdx(Y_AXIS, pdIdx, ledIdx);
#else
            if ( DLT_mode.dltMode & DLT_MODE_LED ) {
                ledIdx = DLT_mode.senY[2];
                pdIdx = i;
            } else {
                pdIdx = DLT_mode.senY[2];
                ledIdx = i;
            }
            adcResult = getADCResultByDeviceIdx(Y_AXIS, pdIdx, ledIdx);
#endif
        }

        Exb_Send_Buf[idx++] = adcResult;
    }
#ifdef ENABLE_CHECK_ADC_HEADER //nsmoon@230118
    for (i = 0; i < CHK_ADC_HDR_SIZE; i++) {
    	Exb_Send_Buf[idx++] = DLT_adc_header[i];
    }
#endif

    return idx;
}

void dltParmInit(uint8_t usb_rx_cmd, uint8_t *usb_rx_buff)
{
    uint8_t RxSubCmd;
    uint8_t *buff = usb_rx_buff;
    //uint8_t i;

    DLT_mode.dltCmd = usb_rx_cmd;

    RxSubCmd = buff[1];

    switch (DLT_mode.dltCmd) {
    case USB_REQ_LINE_ADC0:
    {
        DLT_mode.dltMode = RxSubCmd;    //1
        if ((DLT_mode.dltMode & DLT_MODE_ALL) != 0) {
            DLT_mode.senX[SLOPE_LEFT] = buff[2];    //2
            DLT_mode.senX[SLOPE_RIGHT] = buff[3];   //3
            DLT_mode.senY[SLOPE_LEFT] = buff[4];    //4
            DLT_mode.senY[SLOPE_RIGHT] = buff[5];   //5
        }
        else {
            DLT_mode.senX[0] = buff[2]; //2
            DLT_mode.senX[1] = buff[3]; //3
            DLT_mode.senX[2] = buff[4]; //4
            DLT_mode.senY[0] = buff[5]; //5
            DLT_mode.senY[1] = buff[6]; //6
            DLT_mode.senY[2] = buff[7]; //7
        }
        TRACE_DLT("USB_REQ_LINE_ADC0.. %x %x D: %d %d %d %d %d %d", DLT_mode.dltCmd, DLT_mode.dltMode, buff[2], buff[3], buff[4], buff[5], buff[6], buff[7]);
#ifdef ENABLE_CHECK_ADC_HEADER //nsmoon@230118
        memcpy(&DLT_adc_header[0], &buff[0], CHK_ADC_HDR_SIZE);
#endif

    }
    break;

    case USB_REQ_SEN_LIFE_TEST:
    {
        DLT_mode.dltMode = DLT_MODE_ALL;
        DLT_mode.senX[SLOPE_LEFT] = 0;
        DLT_mode.senX[SLOPE_RIGHT] = 0;
        DLT_mode.senY[SLOPE_LEFT] = 0;
        DLT_mode.senY[SLOPE_RIGHT] = 0;
    }
    break;

    case USB_REQ_OFST:
        DLT_mode.subThreshold = RxSubCmd;
        break;

    case USB_REQ_LINE:
    case USB_REQ_LINE_THRESHOLD_DATA:
//#if (FORCE_AGC_SET == 1)
    	//g_forcedagc.forced_agc = buff[3];     //YJ@210719
//#endif
        break;

    default:
        break;
    }
}

int dltInit(void) {
#if ENABLE_EXB_BUF_BANKING == TRUE
    Exb_Send_Buf_Idx = 0;
    Exb_Send_Buf = &Exb_Send_Buf__[Exb_Send_Buf_Idx][0];
#endif

#ifdef USE_GET_VERSION //nsmoon@200706
    get_ver_string(); //version string
#endif
    return 0;
}

int dltRxProcess(void) {
    uint8_t RxCmd = DLT_mode.dltCmd;
    switch (RxCmd) {
    case USB_REQ_OFST:
        TRACE_DLT("USB_REQ_OFST");
#if 0
        subThresholdDropPercent = DLT_mode.subThreshold;
        if (subThresholdDropPercent > 0 && subThresholdDropPercent < 100) {
            scanStatus.requestSubThreshold = TRUE;

#if 0 // kjs 0 to use average non-block level
            updateThresholdData(SUB_THRESHOLD_MODE);
#else
            setThresholdDataByAvgNonBlockLevel(SUB_THRESHOLD_MODE);
#endif
        }
#endif
        subThresholdDropPercent = LINE_THRESHOLD_VALUE;
        setThresholdDataByAvgNonBlockLevel(SUB_THRESHOLD_MODE);
        total_send_cnt = copy_ofst_to_exb();
        //TRACE_DLT("total_send_cnt = %d", total_send_cnt); // kjsxx
        break;

    case USB_REQ_LINE:
        TRACE_DLT("USB_REQ_LINE");
        total_send_cnt = EXB_Y_DATA_END + 1; // line data size by main threshold only
        break;

    case USB_REQ_LINE_ADC0:
        TRACE_DLT("USB_REQ_LINE_ADC0");
        total_send_cnt = Send_ADC_Data_to_USBCPU(RxCmd);
//        g_forcedagc.fdltmode = TRUE;
        break;

#if (USE_SEND_THRESHOLD_DATA == 1)
    case USB_REQ_LINE_THRESHOLD_DATA: {
        TRACE_DLT("USB_REQ_LINE_THRESHOLD_DATA\r\n");
        total_send_cnt = (uint32_t) EXB_TOTAL_DATA_SIZE; // line data size
        //TRACE_DLT("EXB_TOTAL_DATA_SIZE = %d, total_send_cnt = %d", EXB_TOTAL_DATA_SIZE, total_send_cnt); // kjsxx
        //TRACE_DLT("EXB_XFER_BUF_SIZE = %d, EXB_SEND_BUF_LEN = %d\r\n", EXB_XFER_BUF_SIZE, EXB_SEND_BUF_LEN); // kjsxx
//        g_forcedagc.fdltmode = TRUE;
    }
    break;
#endif

    case USB_REQ_SEN_LIFE_TEST: {
        TRACE_DLT("USB_REQ_SEN_LIFE_TEST\r\n");
        total_send_cnt = Send_ADC_Data_to_USBCPU(RxCmd);
    }
    break;

    case USB_NORCMD_RESET:
        TRACE_DLT("USB_REQ_RESET");
        //printf("\r\nUSB_REQ_RESET Cmd\r\n");
        total_send_cnt = 0; // kjs add
        break;

    default:
        TRACE_DLT("ERROR! unknown cmd 0x%02x \r\n", RxCmd);
        //printf("\r\nUnknown Cmd\r\n");
        total_send_cnt = 0; // kjs add
        return 0;
    }
//#if 0
//    if(RxCmd != USB_REQ_OFST) g_forcedagc.fdltmode = TRUE;
//    else TRACE_DLT("@");
//#endif


#if 0 // kjs 0
    return 1;
#else
    if (total_send_cnt == 0)
        return 0;
    else
        return 1;
#endif
}

int dltProcess(void) {
    if (dltRxProcess()) {
//		uint16_t crc = USB_CalculateCrc(&Exb_Send_Buf[0], total_send_cnt);
//		Exb_Send_Buf[total_send_cnt] = (uint8_t) crc;
//		Exb_Send_Buf[total_send_cnt + 1] = (uint8_t) (crc >> 8);
        total_send_cnt += 2;
        normal_dlt_send_data(Exb_Send_Buf, total_send_cnt);
    }

#ifdef EXB_DUAL_BUFFER //for test
    Exb_Send_Buf_Idx ^= 0x01;
    Exb_Send_Buf = &Exb_Send_Buf__[Exb_Send_Buf_Idx][0];
#endif

    return 0;
}
/*end of file*/
