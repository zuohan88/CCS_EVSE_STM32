################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/exi/BitInputStream.c \
../Core/exi/BitOutputStream.c \
../Core/exi/ByteStream.c \
../Core/exi/DecoderChannel.c \
../Core/exi/EXIHeaderDecoder.c \
../Core/exi/EXIHeaderEncoder.c \
../Core/exi/EncoderChannel.c \
../Core/exi/MethodsBag.c \
../Core/exi/appHandEXIDatatypes.c \
../Core/exi/appHandEXIDatatypesDecoder.c \
../Core/exi/appHandEXIDatatypesEncoder.c \
../Core/exi/dinEXIDatatypes.c \
../Core/exi/dinEXIDatatypesDecoder.c \
../Core/exi/dinEXIDatatypesEncoder.c \
../Core/exi/projectExiConnector.c 

OBJS += \
./Core/exi/BitInputStream.o \
./Core/exi/BitOutputStream.o \
./Core/exi/ByteStream.o \
./Core/exi/DecoderChannel.o \
./Core/exi/EXIHeaderDecoder.o \
./Core/exi/EXIHeaderEncoder.o \
./Core/exi/EncoderChannel.o \
./Core/exi/MethodsBag.o \
./Core/exi/appHandEXIDatatypes.o \
./Core/exi/appHandEXIDatatypesDecoder.o \
./Core/exi/appHandEXIDatatypesEncoder.o \
./Core/exi/dinEXIDatatypes.o \
./Core/exi/dinEXIDatatypesDecoder.o \
./Core/exi/dinEXIDatatypesEncoder.o \
./Core/exi/projectExiConnector.o 

C_DEPS += \
./Core/exi/BitInputStream.d \
./Core/exi/BitOutputStream.d \
./Core/exi/ByteStream.d \
./Core/exi/DecoderChannel.d \
./Core/exi/EXIHeaderDecoder.d \
./Core/exi/EXIHeaderEncoder.d \
./Core/exi/EncoderChannel.d \
./Core/exi/MethodsBag.d \
./Core/exi/appHandEXIDatatypes.d \
./Core/exi/appHandEXIDatatypesDecoder.d \
./Core/exi/appHandEXIDatatypesEncoder.d \
./Core/exi/dinEXIDatatypes.d \
./Core/exi/dinEXIDatatypesDecoder.d \
./Core/exi/dinEXIDatatypesEncoder.d \
./Core/exi/projectExiConnector.d 


# Each subdirectory must supply rules for building sources it contributes
Core/exi/%.o Core/exi/%.su Core/exi/%.cyclo: ../Core/exi/%.c Core/exi/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/canbus/qpc -I../Core/canbus/ring -I../Core/canbus/ring/et -I../Core/canbus -I../Core/ring/et -I../Core/ring -I../Core/TM -I../Core/TM/Low -I../Core/qca_spi -I../Core/exi -I../Core/ccs -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-exi

clean-Core-2f-exi:
	-$(RM) ./Core/exi/BitInputStream.cyclo ./Core/exi/BitInputStream.d ./Core/exi/BitInputStream.o ./Core/exi/BitInputStream.su ./Core/exi/BitOutputStream.cyclo ./Core/exi/BitOutputStream.d ./Core/exi/BitOutputStream.o ./Core/exi/BitOutputStream.su ./Core/exi/ByteStream.cyclo ./Core/exi/ByteStream.d ./Core/exi/ByteStream.o ./Core/exi/ByteStream.su ./Core/exi/DecoderChannel.cyclo ./Core/exi/DecoderChannel.d ./Core/exi/DecoderChannel.o ./Core/exi/DecoderChannel.su ./Core/exi/EXIHeaderDecoder.cyclo ./Core/exi/EXIHeaderDecoder.d ./Core/exi/EXIHeaderDecoder.o ./Core/exi/EXIHeaderDecoder.su ./Core/exi/EXIHeaderEncoder.cyclo ./Core/exi/EXIHeaderEncoder.d ./Core/exi/EXIHeaderEncoder.o ./Core/exi/EXIHeaderEncoder.su ./Core/exi/EncoderChannel.cyclo ./Core/exi/EncoderChannel.d ./Core/exi/EncoderChannel.o ./Core/exi/EncoderChannel.su ./Core/exi/MethodsBag.cyclo ./Core/exi/MethodsBag.d ./Core/exi/MethodsBag.o ./Core/exi/MethodsBag.su ./Core/exi/appHandEXIDatatypes.cyclo ./Core/exi/appHandEXIDatatypes.d ./Core/exi/appHandEXIDatatypes.o ./Core/exi/appHandEXIDatatypes.su ./Core/exi/appHandEXIDatatypesDecoder.cyclo ./Core/exi/appHandEXIDatatypesDecoder.d ./Core/exi/appHandEXIDatatypesDecoder.o ./Core/exi/appHandEXIDatatypesDecoder.su ./Core/exi/appHandEXIDatatypesEncoder.cyclo ./Core/exi/appHandEXIDatatypesEncoder.d ./Core/exi/appHandEXIDatatypesEncoder.o ./Core/exi/appHandEXIDatatypesEncoder.su ./Core/exi/dinEXIDatatypes.cyclo ./Core/exi/dinEXIDatatypes.d ./Core/exi/dinEXIDatatypes.o ./Core/exi/dinEXIDatatypes.su ./Core/exi/dinEXIDatatypesDecoder.cyclo ./Core/exi/dinEXIDatatypesDecoder.d ./Core/exi/dinEXIDatatypesDecoder.o ./Core/exi/dinEXIDatatypesDecoder.su ./Core/exi/dinEXIDatatypesEncoder.cyclo ./Core/exi/dinEXIDatatypesEncoder.d ./Core/exi/dinEXIDatatypesEncoder.o ./Core/exi/dinEXIDatatypesEncoder.su ./Core/exi/projectExiConnector.cyclo ./Core/exi/projectExiConnector.d ./Core/exi/projectExiConnector.o ./Core/exi/projectExiConnector.su

.PHONY: clean-Core-2f-exi

