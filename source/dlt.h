#ifndef _DLT_H
#define _DLT_H

#include "common.h"
#include "scan.h"
//#include "usb_device_interface_0_hid.h"

#define USB_ENDPOINT_BUFFER_LENGTH  512
#define NUM_PD_DLT                  3
#define USB_CMD_LEN                 10

//==============================================================================
// Transfer Data Block Format for B/E Host
//------------------------------------------------------------------------------
#define EXB_HEADER_SIZE         4
#define EXB_CRC_SIZE            2

#define EXB_X_OFFSET_BYTE_SIZE  ((X_TOTAL_OFFSET - 1) / 8 + 1)  // 11
#define EXB_Y_OFFSET_BYTE_SIZE  ((Y_TOTAL_OFFSET - 1) / 8 + 1)  // 16

#define EXB_X_DATA_SIZE         (X_CELL_SIZE * EXB_X_OFFSET_BYTE_SIZE)  // 1837
#define EXB_Y_DATA_SIZE         (Y_CELL_SIZE * EXB_Y_OFFSET_BYTE_SIZE)  // 1872

#define EXB_X_DATA_START        (EXB_HEADER_SIZE)
#define EXB_X_DATA_END          (EXB_X_DATA_START + EXB_X_DATA_SIZE - 1)
#define EXB_Y_DATA_START        (EXB_X_DATA_END + 1)
#define EXB_Y_DATA_END          (EXB_Y_DATA_START + EXB_Y_DATA_SIZE - 1)

#define EXB_X_SUB_DATA_START    (EXB_Y_DATA_END + 1)
#define EXB_X_SUB_DATA_END      (EXB_X_SUB_DATA_START + EXB_X_DATA_SIZE - 1)
#define EXB_Y_SUB_DATA_START    (EXB_X_SUB_DATA_END + 1)
#define EXB_Y_SUB_DATA_END      (EXB_Y_SUB_DATA_START + EXB_Y_DATA_SIZE - 1)

//
#if (USE_SEND_THRESHOLD_DATA == 1)
#define EXB_TOTAL_DATA_SIZE     (EXB_Y_SUB_DATA_END + 1)
#else
//#define EXB_TOTAL_DATA_SIZE     (EXB_Y_DATA_END + 1)
#endif
#ifdef ENABLE_CHECK_ADC_HEADER //nsmoon@230118
#define CHK_ADC_HDR_SIZE   8 //recv:0~7, send:1~8
#else
#define CHK_ADC_HDR_SIZE   0 
#endif

#if 0 // kjsxx 0
#define EXB_ADC_DLT_SIZE        ((X_CELL_SIZE + Y_CELL_SIZE) * NUM_PD_DLT)
#define EXB_BUF_SIZE_MAX        (EXB_TOTAL_DATA_SIZE + EXB_ADC_DLT_SIZE + EXB_CRC_SIZE)
#else
#define EXB_BUF_SIZE_MAX        (EXB_TOTAL_DATA_SIZE + EXB_CRC_SIZE + CHK_ADC_HDR_SIZE)	//nsmoon@230118
#endif

#define EXB_XFER_PACKET_SIZE    ((EXB_BUF_SIZE_MAX + (USB_ENDPOINT_BUFFER_LENGTH - 1)) / USB_ENDPOINT_BUFFER_LENGTH)
#define EXB_XFER_BUF_SIZE       (EXB_XFER_PACKET_SIZE * USB_ENDPOINT_BUFFER_LENGTH)


#define FE_HEADER_SIZE		    (MY_USB_PACKET_SIZE -1 -2) //-(cmd+crc)
#define FE_EEPROM_SIZE          FE_HEADER_SIZE
#define FE_FWFILE_SIZE		    (27) //+\0
#define FE_TIMESTAMP_SIZE	    (13) //+\0 200618113132
#define FE_MODEL_SIZE		    (10) //+\0
#define FE_PRGLEN_SIZE          (6) //+\0
#define FE_PRGCRC_SIZE          (5) //+\0
#define MAX_VERSION_LEN         (3) //+\0


//==============================================================================
#if 0 // kjsxx 0 to change definition name
#define PD_DATA_IDX2			4 //PD-data start
#define USB_CRC_SIZE			2 //bytes

#if 1   //shj@20191213 add threshold adc data
#if (USE_SEND_THRESHOLD_DATA == 1)
#define MAX_EXB_SEND_DATA_SIZE   2
#else
#define MAX_EXB_SEND_DATA_SIZE   1
#endif
#endif

#define MAX_SLOPE_BYTE_X        ((X_TOTAL_OFFSET - 1) / 8 + 1)    //11
#define MAX_SLOPE_BYTE_Y        ((Y_TOTAL_OFFSET - 1) / 8 + 1)    //16

#define MAX_EXB_X_BUF_SIZE      (X_CELL_SIZE * MAX_SLOPE_BYTE_X)    //1771
#define MAX_EXB_Y_BUF_SIZE      (Y_CELL_SIZE * MAX_SLOPE_BYTE_Y)    //1712

#define EXB_X_START_BUFF_IDX    (PD_DATA_IDX2)
#define EXB_X_END_BUFF_IDX      (PD_DATA_IDX2 + MAX_EXB_X_BUF_SIZE)
#define EXB_Y_START_BUFF_IDX    (PD_DATA_IDX2 + MAX_EXB_X_BUF_SIZE)
#define EXB_Y_END_BUFF_IDX      (PD_DATA_IDX2 + MAX_EXB_X_BUF_SIZE + MAX_EXB_Y_BUF_SIZE)

#define EXB_SUB_TH_X_START_BUFF_IDX    (PD_DATA_IDX2 + MAX_EXB_X_BUF_SIZE + MAX_EXB_Y_BUF_SIZE)
#define EXB_SUB_TH_Y_START_BUFF_IDX    (PD_DATA_IDX2 + MAX_EXB_X_BUF_SIZE + MAX_EXB_Y_BUF_SIZE + MAX_EXB_X_BUF_SIZE)

#define EXB_SUB_THRESHOLD_BUFF_LEN     (PD_DATA_IDX2 + ((MAX_EXB_X_BUF_SIZE + MAX_EXB_Y_BUF_SIZE)*MAX_EXB_SEND_DATA_SIZE))  //6,848

#define MAX_EXB_SEND_BUF_SIZE   (PD_DATA_IDX2 + ((MAX_EXB_X_BUF_SIZE + MAX_EXB_Y_BUF_SIZE)*MAX_EXB_SEND_DATA_SIZE) + ((X_CELL_SIZE + Y_CELL_SIZE) * NUM_PD_DLT) + USB_CRC_SIZE)
#define MAX_EMPTY_BUFFER_SIZE (USB_ENDPOINT_BUFFER_LENGTH - (MAX_EXB_SEND_BUF_SIZE % USB_ENDPOINT_BUFFER_LENGTH))
#define EXB_SEND_BUF_LEN (MAX_EXB_SEND_BUF_SIZE + MAX_EMPTY_BUFFER_SIZE)
#endif
//==============================================================================

typedef struct {
    uint8_t dltMode;
    uint8_t senX[NUM_PD_DLT]; //X sensor number or slope index
    uint8_t senY[NUM_PD_DLT]; //Y sensor number or slope index
    uint8_t dltCmd; //nsmoon@221116
    uint8_t subThreshold;
} DLT_mode_t;

#define SLOPE_LEFT		0
#define SLOPE_RIGHT		1

typedef struct {
    uint16_t XPD_s, XPD_e, YPD_s, YPD_e;
    uint16_t XLED_s, XLED_e, YLED_s, YLED_e;
} DLT_SCAN_INFO_T;

////////////////////////////////////////
//extern dlt variables and functions
////////////////////////////////////////

#if ENABLE_EXB_BUF_BANKING == TRUE
extern uint8_t	Exb_Send_Buf__[2][EXB_XFER_BUF_SIZE] __attribute__((coherent, aligned(16)));
extern uint8_t Exb_Send_Buf_Idx;
extern uint8_t *Exb_Send_Buf;
#else
extern uint8_t Exb_Send_Buf[EXB_XFER_BUF_SIZE];
#endif

extern char frontend_fwVersion[MAX_VERSION_LEN];	//YJ@230531

extern volatile uint32_t total_send_cnt;
extern DLT_SCAN_INFO_T DLT_scan;

extern int dltInit(void);
extern void dltParmInit(uint8_t usb_rx_cmd, uint8_t *usb_rx_buff);
extern int dltProcess(void);
extern uint32_t copy_ofst_to_exb(void);
extern uint32_t Send_ExbBit_Data_to_USBCPU_Init(void);
extern void get_ver_string(void);
#endif /* DLT_H_ */
