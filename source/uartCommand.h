#ifndef UARTCOMMAND_H_
#define UARTCOMMAND_H_

#if (ENABLE_UART_CMD_PROCESS)

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

#endif  // ENABLE_UART_CMD_PROCESS


#endif /* UARTCOMMAND_H_ */
