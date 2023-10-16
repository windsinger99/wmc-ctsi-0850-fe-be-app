################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_adc.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_gpio.c \
../drivers/fsl_gpt.c \
../drivers/fsl_lpuart.c \
../drivers/fsl_qtmr.c \
../drivers/fsl_wdog.c \
../drivers/fsl_xbara.c 

C_DEPS += \
./drivers/fsl_adc.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_gpio.d \
./drivers/fsl_gpt.d \
./drivers/fsl_lpuart.d \
./drivers/fsl_qtmr.d \
./drivers/fsl_wdog.d \
./drivers/fsl_xbara.d 

OBJS += \
./drivers/fsl_adc.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_gpio.o \
./drivers/fsl_gpt.o \
./drivers/fsl_lpuart.o \
./drivers/fsl_qtmr.o \
./drivers/fsl_wdog.o \
./drivers/fsl_xbara.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MIMXRT1064DVL6A -DCPU_MIMXRT1064DVL6A_cm7 -D_DEBUG=0 -DSDK_OS_BAREMETAL -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DUSB_STACK_BM -DSERIAL_PORT_TYPE_UART=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -DSDK_DEBUGCONSOLE=1 -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\drivers" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\xip" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\device" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\CMSIS" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\serial_manager" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\uart" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\utilities" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\lists" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\board" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\osa" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\drivers" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\xip" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\device" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\CMSIS" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\serial_manager" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\uart" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\utilities" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\lists" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\board" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\source" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\class" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\class\cdc" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\class\hid" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\include" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\source" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\source\ehci" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\phy" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\include" -O3 -fno-common -g -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/fsl_adc.d ./drivers/fsl_adc.o ./drivers/fsl_clock.d ./drivers/fsl_clock.o ./drivers/fsl_common.d ./drivers/fsl_common.o ./drivers/fsl_common_arm.d ./drivers/fsl_common_arm.o ./drivers/fsl_gpio.d ./drivers/fsl_gpio.o ./drivers/fsl_gpt.d ./drivers/fsl_gpt.o ./drivers/fsl_lpuart.d ./drivers/fsl_lpuart.o ./drivers/fsl_qtmr.d ./drivers/fsl_qtmr.o ./drivers/fsl_wdog.d ./drivers/fsl_wdog.o ./drivers/fsl_xbara.d ./drivers/fsl_xbara.o

.PHONY: clean-drivers

