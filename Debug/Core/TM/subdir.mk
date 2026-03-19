################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/TM/app_func.c \
../Core/TM/cdc_serial.c \
../Core/TM/serial.c 

OBJS += \
./Core/TM/app_func.o \
./Core/TM/cdc_serial.o \
./Core/TM/serial.o 

C_DEPS += \
./Core/TM/app_func.d \
./Core/TM/cdc_serial.d \
./Core/TM/serial.d 


# Each subdirectory must supply rules for building sources it contributes
Core/TM/%.o Core/TM/%.su Core/TM/%.cyclo: ../Core/TM/%.c Core/TM/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/canbus/qpc -I../Core/canbus/ring -I../Core/canbus/ring/et -I../Core/canbus -I../Core/ring/et -I../Core/ring -I../Core/TM -I../Core/TM/Low -I../Core/qca_spi -I../Core/exi -I../Core/ccs -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-TM

clean-Core-2f-TM:
	-$(RM) ./Core/TM/app_func.cyclo ./Core/TM/app_func.d ./Core/TM/app_func.o ./Core/TM/app_func.su ./Core/TM/cdc_serial.cyclo ./Core/TM/cdc_serial.d ./Core/TM/cdc_serial.o ./Core/TM/cdc_serial.su ./Core/TM/serial.cyclo ./Core/TM/serial.d ./Core/TM/serial.o ./Core/TM/serial.su

.PHONY: clean-Core-2f-TM

