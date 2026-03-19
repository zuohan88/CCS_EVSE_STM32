################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Third_Party/pyplc/exi/din/dinEXIDatatypes.c \
../Core/Third_Party/pyplc/exi/din/dinEXIDatatypesDecoder.c \
../Core/Third_Party/pyplc/exi/din/dinEXIDatatypesEncoder.c 

OBJS += \
./Core/Third_Party/pyplc/exi/din/dinEXIDatatypes.o \
./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesDecoder.o \
./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesEncoder.o 

C_DEPS += \
./Core/Third_Party/pyplc/exi/din/dinEXIDatatypes.d \
./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesDecoder.d \
./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesEncoder.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Third_Party/pyplc/exi/din/%.o Core/Third_Party/pyplc/exi/din/%.su Core/Third_Party/pyplc/exi/din/%.cyclo: ../Core/Third_Party/pyplc/exi/din/%.c Core/Third_Party/pyplc/exi/din/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"C:/Users/zuoha/STM32CubeIDE/workspace_1.14.1/g474re_EVSE4/Core/Third_Party/lwjson" -I../Core/Third_Party/pyplc/canbus/qpc -I../Core/Third_Party/pyplc/canbus/ring -I../Core/Third_Party/pyplc/canbus/ring/et -I../Core/Third_Party/pyplc/canbus -I../Core/Third_Party/pyplc/ring/et -I../Core/Third_Party/pyplc/ring -I../Core/Third_Party/pyplc/qca_spi -I../Core/Third_Party/pyplc/exi -I../Core/Third_Party/pyplc/exi/appHandshake -I../Core/Third_Party/pyplc/exi/codec -I../Core/Third_Party/pyplc/exi/din -I../Core/Third_Party/pyplc/ccs -I../Core/Src/App -I../Core/Third_Party/TM -I../Core/Third_Party/TM/Low -I../Core/Third_Party/AL94_USB_Composite -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Target -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Third_Party-2f-pyplc-2f-exi-2f-din

clean-Core-2f-Third_Party-2f-pyplc-2f-exi-2f-din:
	-$(RM) ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypes.cyclo ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypes.d ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypes.o ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypes.su ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesDecoder.cyclo ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesDecoder.d ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesDecoder.o ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesDecoder.su ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesEncoder.cyclo ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesEncoder.d ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesEncoder.o ./Core/Third_Party/pyplc/exi/din/dinEXIDatatypesEncoder.su

.PHONY: clean-Core-2f-Third_Party-2f-pyplc-2f-exi-2f-din

