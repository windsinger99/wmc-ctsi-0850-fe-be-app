################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/CTSI_0650_NXP_scan_table.c \
../source/Scan.c \
../source/adc.c \
../source/aes.c \
../source/app.c \
../source/backend_postp.c \
../source/bezier.c \
../source/composite.c \
../source/delay.c \
../source/dlt.c \
../source/gpio.c \
../source/gpt.c \
../source/hid_generic.c \
../source/hid_mouse.c \
../source/pitch_table.c \
../source/scan_table.c \
../source/semihost_hardfault.c \
../source/speed_test_float.c \
../source/uartCommand.c \
../source/usb_device_descriptor.c \
../source/virtual_com.c 

C_DEPS += \
./source/CTSI_0650_NXP_scan_table.d \
./source/Scan.d \
./source/adc.d \
./source/aes.d \
./source/app.d \
./source/backend_postp.d \
./source/bezier.d \
./source/composite.d \
./source/delay.d \
./source/dlt.d \
./source/gpio.d \
./source/gpt.d \
./source/hid_generic.d \
./source/hid_mouse.d \
./source/pitch_table.d \
./source/scan_table.d \
./source/semihost_hardfault.d \
./source/speed_test_float.d \
./source/uartCommand.d \
./source/usb_device_descriptor.d \
./source/virtual_com.d 

OBJS += \
./source/CTSI_0650_NXP_scan_table.o \
./source/Scan.o \
./source/adc.o \
./source/aes.o \
./source/app.o \
./source/backend_postp.o \
./source/bezier.o \
./source/composite.o \
./source/delay.o \
./source/dlt.o \
./source/gpio.o \
./source/gpt.o \
./source/hid_generic.o \
./source/hid_mouse.o \
./source/pitch_table.o \
./source/scan_table.o \
./source/semihost_hardfault.o \
./source/speed_test_float.o \
./source/uartCommand.o \
./source/usb_device_descriptor.o \
./source/virtual_com.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MIMXRT1064DVL6A -DCPU_MIMXRT1064DVL6A_cm7 -D_DEBUG=0 -DSDK_OS_BAREMETAL -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DUSB_STACK_BM -DSERIAL_PORT_TYPE_UART=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -DSDK_DEBUGCONSOLE=1 -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\drivers" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\xip" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\device" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\CMSIS" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\serial_manager" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\uart" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\utilities" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\lists" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\board" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\osa" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\drivers" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\xip" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\device" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\CMSIS" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\serial_manager" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\uart" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\utilities" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\lists" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\board" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\source" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\class" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\class\cdc" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\class\hid" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\include" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\source" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\source\ehci" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\phy" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\include" -O3 -fno-common -g -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/CTSI_0650_NXP_scan_table.d ./source/CTSI_0650_NXP_scan_table.o ./source/Scan.d ./source/Scan.o ./source/adc.d ./source/adc.o ./source/aes.d ./source/aes.o ./source/app.d ./source/app.o ./source/backend_postp.d ./source/backend_postp.o ./source/bezier.d ./source/bezier.o ./source/composite.d ./source/composite.o ./source/delay.d ./source/delay.o ./source/dlt.d ./source/dlt.o ./source/gpio.d ./source/gpio.o ./source/gpt.d ./source/gpt.o ./source/hid_generic.d ./source/hid_generic.o ./source/hid_mouse.d ./source/hid_mouse.o ./source/pitch_table.d ./source/pitch_table.o ./source/scan_table.d ./source/scan_table.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o ./source/speed_test_float.d ./source/speed_test_float.o ./source/uartCommand.d ./source/uartCommand.o ./source/usb_device_descriptor.d ./source/usb_device_descriptor.o ./source/virtual_com.d ./source/virtual_com.o

.PHONY: clean-source

