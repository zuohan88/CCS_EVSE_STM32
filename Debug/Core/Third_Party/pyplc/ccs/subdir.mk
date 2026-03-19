################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Third_Party/pyplc/ccs/evsePredefine.c \
../Core/Third_Party/pyplc/ccs/homeplug.c \
../Core/Third_Party/pyplc/ccs/myHelpers.c \
../Core/Third_Party/pyplc/ccs/qca7000.c \
../Core/Third_Party/pyplc/ccs/tcp.c \
../Core/Third_Party/pyplc/ccs/udp.c \
../Core/Third_Party/pyplc/ccs/udpChecksum.c 

OBJS += \
./Core/Third_Party/pyplc/ccs/evsePredefine.o \
./Core/Third_Party/pyplc/ccs/homeplug.o \
./Core/Third_Party/pyplc/ccs/myHelpers.o \
./Core/Third_Party/pyplc/ccs/qca7000.o \
./Core/Third_Party/pyplc/ccs/tcp.o \
./Core/Third_Party/pyplc/ccs/udp.o \
./Core/Third_Party/pyplc/ccs/udpChecksum.o 

C_DEPS += \
./Core/Third_Party/pyplc/ccs/evsePredefine.d \
./Core/Third_Party/pyplc/ccs/homeplug.d \
./Core/Third_Party/pyplc/ccs/myHelpers.d \
./Core/Third_Party/pyplc/ccs/qca7000.d \
./Core/Third_Party/pyplc/ccs/tcp.d \
./Core/Third_Party/pyplc/ccs/udp.d \
./Core/Third_Party/pyplc/ccs/udpChecksum.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Third_Party/pyplc/ccs/%.o Core/Third_Party/pyplc/ccs/%.su Core/Third_Party/pyplc/ccs/%.cyclo: ../Core/Third_Party/pyplc/ccs/%.c Core/Third_Party/pyplc/ccs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"C:/Users/zuoha/STM32CubeIDE/workspace_1.14.1/g474re_EVSE4/Core/Third_Party/lwjson" -I../Core/Third_Party/pyplc/canbus/qpc -I../Core/Third_Party/pyplc/canbus/ring -I../Core/Third_Party/pyplc/canbus/ring/et -I../Core/Third_Party/pyplc/canbus -I../Core/Third_Party/pyplc/ring/et -I../Core/Third_Party/pyplc/ring -I../Core/Third_Party/pyplc/qca_spi -I../Core/Third_Party/pyplc/exi -I../Core/Third_Party/pyplc/exi/appHandshake -I../Core/Third_Party/pyplc/exi/codec -I../Core/Third_Party/pyplc/exi/din -I../Core/Third_Party/pyplc/ccs -I../Core/Src/App -I../Core/Third_Party/TM -I../Core/Third_Party/TM/Low -I../Core/Third_Party/AL94_USB_Composite -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Target -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Third_Party-2f-pyplc-2f-ccs

clean-Core-2f-Third_Party-2f-pyplc-2f-ccs:
	-$(RM) ./Core/Third_Party/pyplc/ccs/evsePredefine.cyclo ./Core/Third_Party/pyplc/ccs/evsePredefine.d ./Core/Third_Party/pyplc/ccs/evsePredefine.o ./Core/Third_Party/pyplc/ccs/evsePredefine.su ./Core/Third_Party/pyplc/ccs/homeplug.cyclo ./Core/Third_Party/pyplc/ccs/homeplug.d ./Core/Third_Party/pyplc/ccs/homeplug.o ./Core/Third_Party/pyplc/ccs/homeplug.su ./Core/Third_Party/pyplc/ccs/myHelpers.cyclo ./Core/Third_Party/pyplc/ccs/myHelpers.d ./Core/Third_Party/pyplc/ccs/myHelpers.o ./Core/Third_Party/pyplc/ccs/myHelpers.su ./Core/Third_Party/pyplc/ccs/qca7000.cyclo ./Core/Third_Party/pyplc/ccs/qca7000.d ./Core/Third_Party/pyplc/ccs/qca7000.o ./Core/Third_Party/pyplc/ccs/qca7000.su ./Core/Third_Party/pyplc/ccs/tcp.cyclo ./Core/Third_Party/pyplc/ccs/tcp.d ./Core/Third_Party/pyplc/ccs/tcp.o ./Core/Third_Party/pyplc/ccs/tcp.su ./Core/Third_Party/pyplc/ccs/udp.cyclo ./Core/Third_Party/pyplc/ccs/udp.d ./Core/Third_Party/pyplc/ccs/udp.o ./Core/Third_Party/pyplc/ccs/udp.su ./Core/Third_Party/pyplc/ccs/udpChecksum.cyclo ./Core/Third_Party/pyplc/ccs/udpChecksum.d ./Core/Third_Party/pyplc/ccs/udpChecksum.o ./Core/Third_Party/pyplc/ccs/udpChecksum.su

.PHONY: clean-Core-2f-Third_Party-2f-pyplc-2f-ccs

