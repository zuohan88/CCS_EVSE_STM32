################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/ccs/connMgr.c \
../Core/ccs/evsePredefine.c \
../Core/ccs/evseStateMachine.c \
../Core/ccs/homeplug.c \
../Core/ccs/myHelpers.c \
../Core/ccs/myScheduler.c \
../Core/ccs/qca7000.c \
../Core/ccs/tcp.c \
../Core/ccs/udp.c \
../Core/ccs/udpChecksum.c 

OBJS += \
./Core/ccs/connMgr.o \
./Core/ccs/evsePredefine.o \
./Core/ccs/evseStateMachine.o \
./Core/ccs/homeplug.o \
./Core/ccs/myHelpers.o \
./Core/ccs/myScheduler.o \
./Core/ccs/qca7000.o \
./Core/ccs/tcp.o \
./Core/ccs/udp.o \
./Core/ccs/udpChecksum.o 

C_DEPS += \
./Core/ccs/connMgr.d \
./Core/ccs/evsePredefine.d \
./Core/ccs/evseStateMachine.d \
./Core/ccs/homeplug.d \
./Core/ccs/myHelpers.d \
./Core/ccs/myScheduler.d \
./Core/ccs/qca7000.d \
./Core/ccs/tcp.d \
./Core/ccs/udp.d \
./Core/ccs/udpChecksum.d 


# Each subdirectory must supply rules for building sources it contributes
Core/ccs/%.o Core/ccs/%.su Core/ccs/%.cyclo: ../Core/ccs/%.c Core/ccs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/canbus/qpc -I../Core/canbus/ring -I../Core/canbus/ring/et -I../Core/canbus -I../Core/ring/et -I../Core/ring -I../Core/TM -I../Core/TM/Low -I../Core/qca_spi -I../Core/exi -I../Core/ccs -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-ccs

clean-Core-2f-ccs:
	-$(RM) ./Core/ccs/connMgr.cyclo ./Core/ccs/connMgr.d ./Core/ccs/connMgr.o ./Core/ccs/connMgr.su ./Core/ccs/evsePredefine.cyclo ./Core/ccs/evsePredefine.d ./Core/ccs/evsePredefine.o ./Core/ccs/evsePredefine.su ./Core/ccs/evseStateMachine.cyclo ./Core/ccs/evseStateMachine.d ./Core/ccs/evseStateMachine.o ./Core/ccs/evseStateMachine.su ./Core/ccs/homeplug.cyclo ./Core/ccs/homeplug.d ./Core/ccs/homeplug.o ./Core/ccs/homeplug.su ./Core/ccs/myHelpers.cyclo ./Core/ccs/myHelpers.d ./Core/ccs/myHelpers.o ./Core/ccs/myHelpers.su ./Core/ccs/myScheduler.cyclo ./Core/ccs/myScheduler.d ./Core/ccs/myScheduler.o ./Core/ccs/myScheduler.su ./Core/ccs/qca7000.cyclo ./Core/ccs/qca7000.d ./Core/ccs/qca7000.o ./Core/ccs/qca7000.su ./Core/ccs/tcp.cyclo ./Core/ccs/tcp.d ./Core/ccs/tcp.o ./Core/ccs/tcp.su ./Core/ccs/udp.cyclo ./Core/ccs/udp.d ./Core/ccs/udp.o ./Core/ccs/udp.su ./Core/ccs/udpChecksum.cyclo ./Core/ccs/udpChecksum.d ./Core/ccs/udpChecksum.o ./Core/ccs/udpChecksum.su

.PHONY: clean-Core-2f-ccs

