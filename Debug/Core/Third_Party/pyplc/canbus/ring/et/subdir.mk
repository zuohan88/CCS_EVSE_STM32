################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Third_Party/pyplc/canbus/ring/et/et.c \
../Core/Third_Party/pyplc/canbus/ring/et/et_host.c 

OBJS += \
./Core/Third_Party/pyplc/canbus/ring/et/et.o \
./Core/Third_Party/pyplc/canbus/ring/et/et_host.o 

C_DEPS += \
./Core/Third_Party/pyplc/canbus/ring/et/et.d \
./Core/Third_Party/pyplc/canbus/ring/et/et_host.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Third_Party/pyplc/canbus/ring/et/%.o Core/Third_Party/pyplc/canbus/ring/et/%.su Core/Third_Party/pyplc/canbus/ring/et/%.cyclo: ../Core/Third_Party/pyplc/canbus/ring/et/%.c Core/Third_Party/pyplc/canbus/ring/et/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/Third_Party/pyplc/canbus/qpc -I../Core/Third_Party/pyplc/canbus/ring -I../Core/Third_Party/pyplc/canbus/ring/et -I../Core/Third_Party/pyplc/canbus -I../Core/Third_Party/pyplc/ring/et -I../Core/Third_Party/pyplc/ring -I../Core/Third_Party/pyplc/qca_spi -I../Core/Third_Party/pyplc/exi -I../Core/Third_Party/pyplc/ccs -I../Core/Third_Party/TM -I../Core/Third_Party/TM/Low -I../Core/Third_Party/AL94_USB_Composite -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Target -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Third_Party-2f-pyplc-2f-canbus-2f-ring-2f-et

clean-Core-2f-Third_Party-2f-pyplc-2f-canbus-2f-ring-2f-et:
	-$(RM) ./Core/Third_Party/pyplc/canbus/ring/et/et.cyclo ./Core/Third_Party/pyplc/canbus/ring/et/et.d ./Core/Third_Party/pyplc/canbus/ring/et/et.o ./Core/Third_Party/pyplc/canbus/ring/et/et.su ./Core/Third_Party/pyplc/canbus/ring/et/et_host.cyclo ./Core/Third_Party/pyplc/canbus/ring/et/et_host.d ./Core/Third_Party/pyplc/canbus/ring/et/et_host.o ./Core/Third_Party/pyplc/canbus/ring/et/et_host.su

.PHONY: clean-Core-2f-Third_Party-2f-pyplc-2f-canbus-2f-ring-2f-et

