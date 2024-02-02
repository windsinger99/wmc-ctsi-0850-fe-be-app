#ifndef UARTCOMMAND_H_
#define UARTCOMMAND_H_

#if (ENABLE_UART_CMD_PROCESS == DEBUG_UART_MOME)

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

extern void uartReceiveHandler(void);
extern uint16_t uartCmdProcess(void);
extern int16_t uartScanTest(void);
extern int16_t uartScanSeqTest(void);
extern uint32_t uartScanSeqTestCnt;
extern void uartCmdProcessInit(void); //nsmoon@210914
extern uint8_t ledIdx_test, pdIdx_test, showCnt_test, scan_delay_test, scan_axis_test; //nsmoon@211213
extern void print_scan_threshold(const char *dbgStr, uint8_t axis, int16_t ledIdx, int16_t pdIdx);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif


#elif(ENABLE_UART_CMD_PROCESS == DEBUG_VCOM_MODE)
#define TRACE_VCOM(...)    DbgConsole_Printf(__VA_ARGS__);
#define TRACE_MENU(...)    DbgConsole_Printf(__VA_ARGS__);vcom_delay(100);		//for usb tx delay
#define DEBUG_BUF_MAX					256
#define HIS_MAX							3
#define CMD_MAX							30
#define WRONG							"\n\r\033[1;31m Wrong Command...Try again...\033[0m \n\r"
#define VCOM_SHELL_MODE					0
#define VCOM_DAC_MODE					1

extern int vcom_delay(uint32_t cnt);
extern void vcom_init(void);
extern void shellTask(uint8_t *vcomRecvBuf, uint32_t revsize );
extern void Current_adj_mode(uint8_t val);
extern void display_menu(void);

extern uint8_t vcom_mode, nr_option;
extern uint8_t testFlag, testblockedlinecnt;

int vcom_delay(uint32_t cnt);

#endif  // ENABLE_UART_CMD_PROCESS
#endif /* UARTCOMMAND_H_ */
