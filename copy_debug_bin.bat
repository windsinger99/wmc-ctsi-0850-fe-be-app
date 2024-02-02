@ECHO OFF
@SET SRC_PATH=Debug
@SET SRC_NAME=nxp_bootloader2_ss_.bin
@SET DST_PATH="..\Flashloader_RT1064_test\mfgtools-rel\Profiles\MXRT106X\OS Firmware"
@SET DST_NAME=boot_image.bin
@SET SRC_FILE=%SRC_PATH%\%SRC_NAME%
@SET DST_FILE=%DST_PATH%\%DST_NAME%

@dir /n %SRC_FILE%
@copy /y/v/b %SRC_FILE% %DST_FILE% 
@echo ---------------------------------------------------------------------
@dir /n %DST_FILE% 
@echo ---------------------------------------------------------------------
@pause

