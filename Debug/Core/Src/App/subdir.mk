################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/App/AppADC.c \
../Core/Src/App/AppCanBus.c \
../Core/Src/App/AppControl.c \
../Core/Src/App/AppGPIO.c \
../Core/Src/App/AppInsulation.c \
../Core/Src/App/AppSerial.c \
../Core/Src/App/app.c \
../Core/Src/App/app_func.c \
../Core/Src/App/evseStateMachine.c 

OBJS += \
./Core/Src/App/AppADC.o \
./Core/Src/App/AppCanBus.o \
./Core/Src/App/AppControl.o \
./Core/Src/App/AppGPIO.o \
./Core/Src/App/AppInsulation.o \
./Core/Src/App/AppSerial.o \
./Core/Src/App/app.o \
./Core/Src/App/app_func.o \
./Core/Src/App/evseStateMachine.o 

C_DEPS += \
./Core/Src/App/AppADC.d \
./Core/Src/App/AppCanBus.d \
./Core/Src/App/AppControl.d \
./Core/Src/App/AppGPIO.d \
./Core/Src/App/AppInsulation.d \
./Core/Src/App/AppSerial.d \
./Core/Src/App/app.d \
./Core/Src/App/app_func.d \
./Core/Src/App/evseStateMachine.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/App/%.o Core/Src/App/%.su Core/Src/App/%.cyclo: ../Core/Src/App/%.c Core/Src/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"C:/Users/zuoha/STM32CubeIDE/workspace_1.14.1/g474re_EVSE4/Core/Third_Party/lwjson" -I../Core/Third_Party/pyplc/canbus/qpc -I../Core/Third_Party/pyplc/canbus/ring -I../Core/Third_Party/pyplc/canbus/ring/et -I../Core/Third_Party/pyplc/canbus -I../Core/Third_Party/pyplc/ring/et -I../Core/Third_Party/pyplc/ring -I../Core/Third_Party/pyplc/qca_spi -I../Core/Third_Party/pyplc/exi -I../Core/Third_Party/pyplc/exi/appHandshake -I../Core/Third_Party/pyplc/exi/codec -I../Core/Third_Party/pyplc/exi/din -I../Core/Third_Party/pyplc/ccs -I../Core/Src/App -I../Core/Third_Party/TM -I../Core/Third_Party/TM/Low -I../Core/Third_Party/AL94_USB_Composite -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Target -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-App

clean-Core-2f-Src-2f-App:
	-$(RM) ./Core/Src/App/AppADC.cyclo ./Core/Src/App/AppADC.d ./Core/Src/App/AppADC.o ./Core/Src/App/AppADC.su ./Core/Src/App/AppCanBus.cyclo ./Core/Src/App/AppCanBus.d ./Core/Src/App/AppCanBus.o ./Core/Src/App/AppCanBus.su ./Core/Src/App/AppControl.cyclo ./Core/Src/App/AppControl.d ./Core/Src/App/AppControl.o ./Core/Src/App/AppControl.su ./Core/Src/App/AppGPIO.cyclo ./Core/Src/App/AppGPIO.d ./Core/Src/App/AppGPIO.o ./Core/Src/App/AppGPIO.su ./Core/Src/App/AppInsulation.cyclo ./Core/Src/App/AppInsulation.d ./Core/Src/App/AppInsulation.o ./Core/Src/App/AppInsulation.su ./Core/Src/App/AppSerial.cyclo ./Core/Src/App/AppSerial.d ./Core/Src/App/AppSerial.o ./Core/Src/App/AppSerial.su ./Core/Src/App/app.cyclo ./Core/Src/App/app.d ./Core/Src/App/app.o ./Core/Src/App/app.su ./Core/Src/App/app_func.cyclo ./Core/Src/App/app_func.d ./Core/Src/App/app_func.o ./Core/Src/App/app_func.su ./Core/Src/App/evseStateMachine.cyclo ./Core/Src/App/evseStateMachine.d ./Core/Src/App/evseStateMachine.o ./Core/Src/App/evseStateMachine.su

.PHONY: clean-Core-2f-Src-2f-App

