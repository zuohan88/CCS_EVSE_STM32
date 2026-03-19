################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usb_device.c \
../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_cdc_acm_if.c \
../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_desc.c 

OBJS += \
./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usb_device.o \
./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_cdc_acm_if.o \
./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_desc.o 

C_DEPS += \
./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usb_device.d \
./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_cdc_acm_if.d \
./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_desc.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/%.o Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/%.su Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/%.cyclo: ../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/%.c Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"C:/Users/zuoha/STM32CubeIDE/workspace_1.14.1/g474re_EVSE4/Core/Third_Party/lwjson" -I../Core/Third_Party/pyplc/canbus/qpc -I../Core/Third_Party/pyplc/canbus/ring -I../Core/Third_Party/pyplc/canbus/ring/et -I../Core/Third_Party/pyplc/canbus -I../Core/Third_Party/pyplc/ring/et -I../Core/Third_Party/pyplc/ring -I../Core/Third_Party/pyplc/qca_spi -I../Core/Third_Party/pyplc/exi -I../Core/Third_Party/pyplc/exi/appHandshake -I../Core/Third_Party/pyplc/exi/codec -I../Core/Third_Party/pyplc/exi/din -I../Core/Third_Party/pyplc/ccs -I../Core/Src/App -I../Core/Third_Party/TM -I../Core/Third_Party/TM/Low -I../Core/Third_Party/AL94_USB_Composite -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/App -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Class -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Core -I../Core/Third_Party/AL94_USB_Composite/COMPOSITE/Target -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Third_Party-2f-AL94_USB_Composite-2f-COMPOSITE-2f-App

clean-Core-2f-Third_Party-2f-AL94_USB_Composite-2f-COMPOSITE-2f-App:
	-$(RM) ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usb_device.cyclo ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usb_device.d ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usb_device.o ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usb_device.su ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_cdc_acm_if.cyclo ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_cdc_acm_if.d ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_cdc_acm_if.o ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_cdc_acm_if.su ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_desc.cyclo ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_desc.d ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_desc.o ./Core/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_desc.su

.PHONY: clean-Core-2f-Third_Party-2f-AL94_USB_Composite-2f-COMPOSITE-2f-App

