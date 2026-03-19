################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32G4xx_HAL_Driver/Src/subdir.mk
-include Core/Third_Party/pyplc/qca_spi/subdir.mk
-include Core/Third_Party/pyplc/exi/iso2/subdir.mk
-include Core/Third_Party/pyplc/exi/din/subdir.mk
-include Core/Third_Party/pyplc/exi/codec/subdir.mk
-include Core/Third_Party/pyplc/exi/appHandshake/subdir.mk
-include Core/Third_Party/pyplc/exi/subdir.mk
-include Core/Third_Party/pyplc/ccs/subdir.mk
-include Core/Third_Party/lwjson/subdir.mk
-include Core/Third_Party/TM/Low/subdir.mk
-include Core/Third_Party/TM/subdir.mk
-include Core/Third_Party/AL94_USB_Composite/COMPOSITE/Target/subdir.mk
-include Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Src/subdir.mk
-include Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Src/subdir.mk
-include Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Src/subdir.mk
-include Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/App/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := g474re_EVSE4
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
g474re_EVSE4.elf \

MAP_FILES += \
g474re_EVSE4.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
g474re_EVSE4.list \


# All Target
all: main-build

# Main-build Target
main-build: g474re_EVSE4.elf secondary-outputs

# Tool invocations
g474re_EVSE4.elf g474re_EVSE4.map: $(OBJS) $(USER_OBJS) C:\Users\zuoha\STM32CubeIDE\workspace_1.14.1\g474re_EVSE4\STM32G474RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "g474re_EVSE4.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\zuoha\STM32CubeIDE\workspace_1.14.1\g474re_EVSE4\STM32G474RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="g474re_EVSE4.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

g474re_EVSE4.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "g474re_EVSE4.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout g474re_EVSE4.elf g474re_EVSE4.list g474re_EVSE4.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
