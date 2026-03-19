################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/TM/Low/McuLib.c \
../Core/TM/Low/McuUtility.c \
../Core/TM/Low/McuXFormat.c \
../Core/TM/Low/flash_if.c \
../Core/TM/Low/ringbuff.c 

OBJS += \
./Core/TM/Low/McuLib.o \
./Core/TM/Low/McuUtility.o \
./Core/TM/Low/McuXFormat.o \
./Core/TM/Low/flash_if.o \
./Core/TM/Low/ringbuff.o 

C_DEPS += \
./Core/TM/Low/McuLib.d \
./Core/TM/Low/McuUtility.d \
./Core/TM/Low/McuXFormat.d \
./Core/TM/Low/flash_if.d \
./Core/TM/Low/ringbuff.d 


# Each subdirectory must supply rules for building sources it contributes
Core/TM/Low/%.o Core/TM/Low/%.su Core/TM/Low/%.cyclo: ../Core/TM/Low/%.c Core/TM/Low/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/canbus/qpc -I../Core/canbus/ring -I../Core/canbus/ring/et -I../Core/canbus -I../Core/ring/et -I../Core/ring -I../Core/TM -I../Core/TM/Low -I../Core/qca_spi -I../Core/exi -I../Core/ccs -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-TM-2f-Low

clean-Core-2f-TM-2f-Low:
	-$(RM) ./Core/TM/Low/McuLib.cyclo ./Core/TM/Low/McuLib.d ./Core/TM/Low/McuLib.o ./Core/TM/Low/McuLib.su ./Core/TM/Low/McuUtility.cyclo ./Core/TM/Low/McuUtility.d ./Core/TM/Low/McuUtility.o ./Core/TM/Low/McuUtility.su ./Core/TM/Low/McuXFormat.cyclo ./Core/TM/Low/McuXFormat.d ./Core/TM/Low/McuXFormat.o ./Core/TM/Low/McuXFormat.su ./Core/TM/Low/flash_if.cyclo ./Core/TM/Low/flash_if.d ./Core/TM/Low/flash_if.o ./Core/TM/Low/flash_if.su ./Core/TM/Low/ringbuff.cyclo ./Core/TM/Low/ringbuff.d ./Core/TM/Low/ringbuff.o ./Core/TM/Low/ringbuff.su

.PHONY: clean-Core-2f-TM-2f-Low

