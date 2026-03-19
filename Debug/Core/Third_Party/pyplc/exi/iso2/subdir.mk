################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypes.c \
../Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesDecoder.c \
../Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesEncoder.c 

OBJS += \
./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypes.o \
./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesDecoder.o \
./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesEncoder.o 

C_DEPS += \
./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypes.d \
./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesDecoder.d \
./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesEncoder.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Third_Party/pyplc/exi/iso2/%.o Core/Third_Party/pyplc/exi/iso2/%.su Core/Third_Party/pyplc/exi/iso2/%.cyclo: ../Core/Third_Party/pyplc/exi/iso2/%.c Core/Third_Party/pyplc/exi/iso2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"C:/Users/zuoha/STM32CubeIDE/workspace_1.14.1/g474re_EVSE4/Core/Third_Party/lwjson" -I../Core/Third_Party/pyplc/canbus/qpc -I../Core/Third_Party/pyplc/canbus/ring -I../Core/Third_Party/pyplc/canbus/ring/et -I../Core/Third_Party/pyplc/canbus -I../Core/Third_Party/pyplc/ring/et -I../Core/Third_Party/pyplc/ring -I../Core/Third_Party/pyplc/qca_spi -I../Core/Third_Party/pyplc/exi -I../Core/Third_Party/pyplc/exi/appHandshake -I../Core/Third_Party/pyplc/exi/codec -I../Core/Third_Party/pyplc/exi/din -I../Core/Third_Party/pyplc/ccs -I../Core/Src/App -I../Core/Third_Party/TM -I../Core/Third_Party/TM/Low -I../Core/Third_Party/AL94_USB_Composite -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Target -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Third_Party-2f-pyplc-2f-exi-2f-iso2

clean-Core-2f-Third_Party-2f-pyplc-2f-exi-2f-iso2:
	-$(RM) ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypes.cyclo ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypes.d ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypes.o ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypes.su ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesDecoder.cyclo ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesDecoder.d ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesDecoder.o ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesDecoder.su ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesEncoder.cyclo ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesEncoder.d ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesEncoder.o ./Core/Third_Party/pyplc/exi/iso2/iso2EXIDatatypesEncoder.su

.PHONY: clean-Core-2f-Third_Party-2f-pyplc-2f-exi-2f-iso2

