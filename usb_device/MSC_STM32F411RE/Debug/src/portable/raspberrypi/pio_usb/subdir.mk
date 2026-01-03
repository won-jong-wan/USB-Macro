################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c \
../Libs/tinyusb/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c 

OBJS += \
./src/portable/raspberrypi/pio_usb/dcd_pio_usb.o \
./src/portable/raspberrypi/pio_usb/hcd_pio_usb.o 

C_DEPS += \
./src/portable/raspberrypi/pio_usb/dcd_pio_usb.d \
./src/portable/raspberrypi/pio_usb/hcd_pio_usb.d 


# Each subdirectory must supply rules for building sources it contributes
src/portable/raspberrypi/pio_usb/dcd_pio_usb.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c src/portable/raspberrypi/pio_usb/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/raspberrypi/pio_usb/hcd_pio_usb.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c src/portable/raspberrypi/pio_usb/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-portable-2f-raspberrypi-2f-pio_usb

clean-src-2f-portable-2f-raspberrypi-2f-pio_usb:
	-$(RM) ./src/portable/raspberrypi/pio_usb/dcd_pio_usb.cyclo ./src/portable/raspberrypi/pio_usb/dcd_pio_usb.d ./src/portable/raspberrypi/pio_usb/dcd_pio_usb.o ./src/portable/raspberrypi/pio_usb/dcd_pio_usb.su ./src/portable/raspberrypi/pio_usb/hcd_pio_usb.cyclo ./src/portable/raspberrypi/pio_usb/hcd_pio_usb.d ./src/portable/raspberrypi/pio_usb/hcd_pio_usb.o ./src/portable/raspberrypi/pio_usb/hcd_pio_usb.su

.PHONY: clean-src-2f-portable-2f-raspberrypi-2f-pio_usb

