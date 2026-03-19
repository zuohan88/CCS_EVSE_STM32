################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/CDC_frame/CDC_framing.c 

OBJS += \
./Core/CDC_frame/CDC_framing.o 

C_DEPS += \
./Core/CDC_frame/CDC_framing.d 


# Each subdirectory must supply rules for building sources it contributes
Core/CDC_frame/%.o Core/CDC_frame/%.su Core/CDC_frame/%.cyclo: ../Core/CDC_frame/%.c Core/CDC_frame/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/TM -I../Core/TM/Low -I../Core/qca_spi -I../Core/exi -I../Core/ccs -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-CDC_frame

clean-Core-2f-CDC_frame:
	-$(RM) ./Core/CDC_frame/CDC_framing.cyclo ./Core/CDC_frame/CDC_framing.d ./Core/CDC_frame/CDC_framing.o ./Core/CDC_frame/CDC_framing.su

.PHONY: clean-Core-2f-CDC_frame

