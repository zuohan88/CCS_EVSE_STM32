################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/canbus/ring/ring_buf.c \
../Core/canbus/ring/ring_buf2.c \
../Core/canbus/ring/ring_buf_timer.c \
../Core/canbus/ring/timer_arm.c 

OBJS += \
./Core/canbus/ring/ring_buf.o \
./Core/canbus/ring/ring_buf2.o \
./Core/canbus/ring/ring_buf_timer.o \
./Core/canbus/ring/timer_arm.o 

C_DEPS += \
./Core/canbus/ring/ring_buf.d \
./Core/canbus/ring/ring_buf2.d \
./Core/canbus/ring/ring_buf_timer.d \
./Core/canbus/ring/timer_arm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/canbus/ring/%.o Core/canbus/ring/%.su Core/canbus/ring/%.cyclo: ../Core/canbus/ring/%.c Core/canbus/ring/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/canbus/qpc -I../Core/canbus/ring -I../Core/canbus/ring/et -I../Core/canbus -I../Core/ring/et -I../Core/ring -I../Core/TM -I../Core/TM/Low -I../Core/qca_spi -I../Core/exi -I../Core/ccs -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-canbus-2f-ring

clean-Core-2f-canbus-2f-ring:
	-$(RM) ./Core/canbus/ring/ring_buf.cyclo ./Core/canbus/ring/ring_buf.d ./Core/canbus/ring/ring_buf.o ./Core/canbus/ring/ring_buf.su ./Core/canbus/ring/ring_buf2.cyclo ./Core/canbus/ring/ring_buf2.d ./Core/canbus/ring/ring_buf2.o ./Core/canbus/ring/ring_buf2.su ./Core/canbus/ring/ring_buf_timer.cyclo ./Core/canbus/ring/ring_buf_timer.d ./Core/canbus/ring/ring_buf_timer.o ./Core/canbus/ring/ring_buf_timer.su ./Core/canbus/ring/timer_arm.cyclo ./Core/canbus/ring/timer_arm.d ./Core/canbus/ring/timer_arm.o ./Core/canbus/ring/timer_arm.su

.PHONY: clean-Core-2f-canbus-2f-ring

