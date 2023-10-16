################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include xip/subdir.mk
-include utilities/subdir.mk
-include usb/phy/subdir.mk
-include usb/device/source/ehci/subdir.mk
-include usb/device/source/subdir.mk
-include usb/device/class/hid/subdir.mk
-include usb/device/class/cdc/subdir.mk
-include usb/device/class/subdir.mk
-include startup/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/serial_manager/subdir.mk
-include component/osa/subdir.mk
-include component/lists/subdir.mk
-include board/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := wmb-ctsi-0650-ve
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: wmb-ctsi-0650-ve.axf

# Tool invocations
wmb-ctsi-0650-ve.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"D:\FW\wmb85_ve\wmc-ctsi-0850-fe-be-app\lib" -Xlinker -Map="wmb-ctsi-0650-ve.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -T mimxrt1064_linker_script.ld -L ../source -o "wmb-ctsi-0650-ve.axf" $(OBJS) $(USER_OBJS) $(LIBS) -l:libbackend_irt650_nxp2.a
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) wmb-ctsi-0650-ve.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "wmb-ctsi-0650-ve.axf" ; arm-none-eabi-objcopy -v -O ihex "wmb-ctsi-0650-ve.axf" "wmb-ctsi-0650-ve.hex" ; # arm-none-eabi-objcopy -v -O binary "wmb-ctsi-0650-ve.axf" "wmb-ctsi-0650-ve.bin" ; # checksum -p MIMXRT1064xxxxA -d "wmb-ctsi-0650-ve.bin"
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
