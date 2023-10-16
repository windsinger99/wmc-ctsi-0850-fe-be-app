################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../xip/evkmimxrt1064_flexspi_nor_config.c \
../xip/fsl_flexspi_nor_boot.c 

C_DEPS += \
./xip/evkmimxrt1064_flexspi_nor_config.d \
./xip/fsl_flexspi_nor_boot.d 

OBJS += \
./xip/evkmimxrt1064_flexspi_nor_config.o \
./xip/fsl_flexspi_nor_boot.o 


# Each subdirectory must supply rules for building sources it contributes
xip/%.o: ../xip/%.c xip/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MIMXRT1064DVL6A -DCPU_MIMXRT1064DVL6A_cm7 -D_DEBUG=0 -DSDK_OS_BAREMETAL -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DUSB_STACK_BM -DSERIAL_PORT_TYPE_UART=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -DSDK_DEBUGCONSOLE=1 -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\drivers" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\xip" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\device" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\CMSIS" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\serial_manager" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\uart" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\utilities" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\lists" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\board" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\osa" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\drivers" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\xip" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\device" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\CMSIS" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\serial_manager" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\uart" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\utilities" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\component\lists" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\board" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\source" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\class" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\class\cdc" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\class\hid" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\include" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\source" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\device\source\ehci" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\phy" -I"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\usb\include" -O3 -fno-common -g -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-xip

clean-xip:
	-$(RM) ./xip/evkmimxrt1064_flexspi_nor_config.d ./xip/evkmimxrt1064_flexspi_nor_config.o ./xip/fsl_flexspi_nor_boot.d ./xip/fsl_flexspi_nor_boot.o

.PHONY: clean-xip

