################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/canbus/ring/et/et.c \
../Core/canbus/ring/et/et_host.c 

OBJS += \
./Core/canbus/ring/et/et.o \
./Core/canbus/ring/et/et_host.o 

C_DEPS += \
./Core/canbus/ring/et/et.d \
./Core/canbus/ring/et/et_host.d 


# Each subdirectory must supply rules for building sources it contributes
Core/canbus/ring/et/%.o Core/canbus/ring/et/%.su Core/canbus/ring/et/%.cyclo: ../Core/canbus/ring/et/%.c Core/canbus/ring/et/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/canbus/qpc -I../Core/canbus/ring -I../Core/canbus/ring/et -I../Core/canbus -I../Core/ring/et -I../Core/ring -I../Core/TM -I../Core/TM/Low -I../Core/qca_spi -I../Core/exi -I../Core/ccs -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-canbus-2f-ring-2f-et

clean-Core-2f-canbus-2f-ring-2f-et:
	-$(RM) ./Core/canbus/ring/et/et.cyclo ./Core/canbus/ring/et/et.d ./Core/canbus/ring/et/et.o ./Core/canbus/ring/et/et.su ./Core/canbus/ring/et/et_host.cyclo ./Core/canbus/ring/et/et_host.d ./Core/canbus/ring/et/et_host.o ./Core/canbus/ring/et/et_host.su

.PHONY: clean-Core-2f-canbus-2f-ring-2f-et

