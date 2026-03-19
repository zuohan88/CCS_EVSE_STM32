################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Third_Party/pyplc/exi/codec/BitInputStream.c \
../Core/Third_Party/pyplc/exi/codec/BitOutputStream.c \
../Core/Third_Party/pyplc/exi/codec/ByteStream.c \
../Core/Third_Party/pyplc/exi/codec/DecoderChannel.c \
../Core/Third_Party/pyplc/exi/codec/EXIHeaderDecoder.c \
../Core/Third_Party/pyplc/exi/codec/EXIHeaderEncoder.c \
../Core/Third_Party/pyplc/exi/codec/EncoderChannel.c \
../Core/Third_Party/pyplc/exi/codec/MethodsBag.c 

OBJS += \
./Core/Third_Party/pyplc/exi/codec/BitInputStream.o \
./Core/Third_Party/pyplc/exi/codec/BitOutputStream.o \
./Core/Third_Party/pyplc/exi/codec/ByteStream.o \
./Core/Third_Party/pyplc/exi/codec/DecoderChannel.o \
./Core/Third_Party/pyplc/exi/codec/EXIHeaderDecoder.o \
./Core/Third_Party/pyplc/exi/codec/EXIHeaderEncoder.o \
./Core/Third_Party/pyplc/exi/codec/EncoderChannel.o \
./Core/Third_Party/pyplc/exi/codec/MethodsBag.o 

C_DEPS += \
./Core/Third_Party/pyplc/exi/codec/BitInputStream.d \
./Core/Third_Party/pyplc/exi/codec/BitOutputStream.d \
./Core/Third_Party/pyplc/exi/codec/ByteStream.d \
./Core/Third_Party/pyplc/exi/codec/DecoderChannel.d \
./Core/Third_Party/pyplc/exi/codec/EXIHeaderDecoder.d \
./Core/Third_Party/pyplc/exi/codec/EXIHeaderEncoder.d \
./Core/Third_Party/pyplc/exi/codec/EncoderChannel.d \
./Core/Third_Party/pyplc/exi/codec/MethodsBag.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Third_Party/pyplc/exi/codec/%.o Core/Third_Party/pyplc/exi/codec/%.su Core/Third_Party/pyplc/exi/codec/%.cyclo: ../Core/Third_Party/pyplc/exi/codec/%.c Core/Third_Party/pyplc/exi/codec/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"C:/Users/zuoha/STM32CubeIDE/workspace_1.14.1/g474re_EVSE4/Core/Third_Party/lwjson" -I../Core/Third_Party/pyplc/canbus/qpc -I../Core/Third_Party/pyplc/canbus/ring -I../Core/Third_Party/pyplc/canbus/ring/et -I../Core/Third_Party/pyplc/canbus -I../Core/Third_Party/pyplc/ring/et -I../Core/Third_Party/pyplc/ring -I../Core/Third_Party/pyplc/qca_spi -I../Core/Third_Party/pyplc/exi -I../Core/Third_Party/pyplc/exi/appHandshake -I../Core/Third_Party/pyplc/exi/codec -I../Core/Third_Party/pyplc/exi/din -I../Core/Third_Party/pyplc/ccs -I../Core/Src/App -I../Core/Third_Party/TM -I../Core/Third_Party/TM/Low -I../Core/Third_Party/AL94_USB_Composite -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Target -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Third_Party-2f-pyplc-2f-exi-2f-codec

clean-Core-2f-Third_Party-2f-pyplc-2f-exi-2f-codec:
	-$(RM) ./Core/Third_Party/pyplc/exi/codec/BitInputStream.cyclo ./Core/Third_Party/pyplc/exi/codec/BitInputStream.d ./Core/Third_Party/pyplc/exi/codec/BitInputStream.o ./Core/Third_Party/pyplc/exi/codec/BitInputStream.su ./Core/Third_Party/pyplc/exi/codec/BitOutputStream.cyclo ./Core/Third_Party/pyplc/exi/codec/BitOutputStream.d ./Core/Third_Party/pyplc/exi/codec/BitOutputStream.o ./Core/Third_Party/pyplc/exi/codec/BitOutputStream.su ./Core/Third_Party/pyplc/exi/codec/ByteStream.cyclo ./Core/Third_Party/pyplc/exi/codec/ByteStream.d ./Core/Third_Party/pyplc/exi/codec/ByteStream.o ./Core/Third_Party/pyplc/exi/codec/ByteStream.su ./Core/Third_Party/pyplc/exi/codec/DecoderChannel.cyclo ./Core/Third_Party/pyplc/exi/codec/DecoderChannel.d ./Core/Third_Party/pyplc/exi/codec/DecoderChannel.o ./Core/Third_Party/pyplc/exi/codec/DecoderChannel.su ./Core/Third_Party/pyplc/exi/codec/EXIHeaderDecoder.cyclo ./Core/Third_Party/pyplc/exi/codec/EXIHeaderDecoder.d ./Core/Third_Party/pyplc/exi/codec/EXIHeaderDecoder.o ./Core/Third_Party/pyplc/exi/codec/EXIHeaderDecoder.su ./Core/Third_Party/pyplc/exi/codec/EXIHeaderEncoder.cyclo ./Core/Third_Party/pyplc/exi/codec/EXIHeaderEncoder.d ./Core/Third_Party/pyplc/exi/codec/EXIHeaderEncoder.o ./Core/Third_Party/pyplc/exi/codec/EXIHeaderEncoder.su ./Core/Third_Party/pyplc/exi/codec/EncoderChannel.cyclo ./Core/Third_Party/pyplc/exi/codec/EncoderChannel.d ./Core/Third_Party/pyplc/exi/codec/EncoderChannel.o ./Core/Third_Party/pyplc/exi/codec/EncoderChannel.su ./Core/Third_Party/pyplc/exi/codec/MethodsBag.cyclo ./Core/Third_Party/pyplc/exi/codec/MethodsBag.d ./Core/Third_Party/pyplc/exi/codec/MethodsBag.o ./Core/Third_Party/pyplc/exi/codec/MethodsBag.su

.PHONY: clean-Core-2f-Third_Party-2f-pyplc-2f-exi-2f-codec

