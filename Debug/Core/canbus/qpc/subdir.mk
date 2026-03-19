################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/canbus/qpc/uc_ao.c 

OBJS += \
./Core/canbus/qpc/uc_ao.o 

C_DEPS += \
./Core/canbus/qpc/uc_ao.d 


# Each subdirectory must supply rules for building sources it contributes
Core/canbus/qpc/%.o Core/canbus/qpc/%.su Core/canbus/qpc/%.cyclo: ../Core/canbus/qpc/%.c Core/canbus/qpc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/canbus/qpc -I../Core/canbus/ring -I../Core/canbus/ring/et -I../Core/canbus -I../Core/ring/et -I../Core/ring -I../Core/TM -I../Core/TM/Low -I../Core/qca_spi -I../Core/exi -I../Core/ccs -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-canbus-2f-qpc

clean-Core-2f-canbus-2f-qpc:
	-$(RM) ./Core/canbus/qpc/uc_ao.cyclo ./Core/canbus/qpc/uc_ao.d ./Core/canbus/qpc/uc_ao.o ./Core/canbus/qpc/uc_ao.su

.PHONY: clean-Core-2f-canbus-2f-qpc

