################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/App/backup2/AppADC.c \
../Core/Src/App/backup2/AppCanBus.c \
../Core/Src/App/backup2/AppControl.c \
../Core/Src/App/backup2/AppGPIO.c \
../Core/Src/App/backup2/AppInsulation.c \
../Core/Src/App/backup2/AppSerial.c \
../Core/Src/App/backup2/app_func.c \
../Core/Src/App/backup2/evseStateMachine.c 

OBJS += \
./Core/Src/App/backup2/AppADC.o \
./Core/Src/App/backup2/AppCanBus.o \
./Core/Src/App/backup2/AppControl.o \
./Core/Src/App/backup2/AppGPIO.o \
./Core/Src/App/backup2/AppInsulation.o \
./Core/Src/App/backup2/AppSerial.o \
./Core/Src/App/backup2/app_func.o \
./Core/Src/App/backup2/evseStateMachine.o 

C_DEPS += \
./Core/Src/App/backup2/AppADC.d \
./Core/Src/App/backup2/AppCanBus.d \
./Core/Src/App/backup2/AppControl.d \
./Core/Src/App/backup2/AppGPIO.d \
./Core/Src/App/backup2/AppInsulation.d \
./Core/Src/App/backup2/AppSerial.d \
./Core/Src/App/backup2/app_func.d \
./Core/Src/App/backup2/evseStateMachine.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/App/backup2/%.o Core/Src/App/backup2/%.su Core/Src/App/backup2/%.cyclo: ../Core/Src/App/backup2/%.c Core/Src/App/backup2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/Third_Party/pyplc/canbus/qpc -I../Core/Third_Party/pyplc/canbus/ring -I../Core/Third_Party/pyplc/canbus/ring/et -I../Core/Third_Party/pyplc/canbus -I../Core/Third_Party/pyplc/ring/et -I../Core/Third_Party/pyplc/ring -I../Core/Third_Party/pyplc/qca_spi -I../Core/Third_Party/pyplc/exi -I../Core/Third_Party/pyplc/exi/appHandshake -I../Core/Third_Party/pyplc/exi/codec -I../Core/Third_Party/pyplc/exi/din -I../Core/Third_Party/pyplc/ccs -I../Core/Src/App -I../Core/Third_Party/TM -I../Core/Third_Party/TM/Low -I../Core/Third_Party/AL94_USB_Composite -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Target -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-App-2f-backup2

clean-Core-2f-Src-2f-App-2f-backup2:
	-$(RM) ./Core/Src/App/backup2/AppADC.cyclo ./Core/Src/App/backup2/AppADC.d ./Core/Src/App/backup2/AppADC.o ./Core/Src/App/backup2/AppADC.su ./Core/Src/App/backup2/AppCanBus.cyclo ./Core/Src/App/backup2/AppCanBus.d ./Core/Src/App/backup2/AppCanBus.o ./Core/Src/App/backup2/AppCanBus.su ./Core/Src/App/backup2/AppControl.cyclo ./Core/Src/App/backup2/AppControl.d ./Core/Src/App/backup2/AppControl.o ./Core/Src/App/backup2/AppControl.su ./Core/Src/App/backup2/AppGPIO.cyclo ./Core/Src/App/backup2/AppGPIO.d ./Core/Src/App/backup2/AppGPIO.o ./Core/Src/App/backup2/AppGPIO.su ./Core/Src/App/backup2/AppInsulation.cyclo ./Core/Src/App/backup2/AppInsulation.d ./Core/Src/App/backup2/AppInsulation.o ./Core/Src/App/backup2/AppInsulation.su ./Core/Src/App/backup2/AppSerial.cyclo ./Core/Src/App/backup2/AppSerial.d ./Core/Src/App/backup2/AppSerial.o ./Core/Src/App/backup2/AppSerial.su ./Core/Src/App/backup2/app_func.cyclo ./Core/Src/App/backup2/app_func.d ./Core/Src/App/backup2/app_func.o ./Core/Src/App/backup2/app_func.su ./Core/Src/App/backup2/evseStateMachine.cyclo ./Core/Src/App/backup2/evseStateMachine.d ./Core/Src/App/backup2/evseStateMachine.o ./Core/Src/App/backup2/evseStateMachine.su

.PHONY: clean-Core-2f-Src-2f-App-2f-backup2

