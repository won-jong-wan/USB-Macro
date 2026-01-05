################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c \
../Libs/tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.c \
../Libs/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c 

OBJS += \
./src/portable/raspberrypi/rp2040/dcd_rp2040.o \
./src/portable/raspberrypi/rp2040/hcd_rp2040.o \
./src/portable/raspberrypi/rp2040/rp2040_usb.o 

C_DEPS += \
./src/portable/raspberrypi/rp2040/dcd_rp2040.d \
./src/portable/raspberrypi/rp2040/hcd_rp2040.d \
./src/portable/raspberrypi/rp2040/rp2040_usb.d 


# Each subdirectory must supply rules for building sources it contributes
src/portable/raspberrypi/rp2040/dcd_rp2040.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c src/portable/raspberrypi/rp2040/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/raspberrypi/rp2040/hcd_rp2040.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.c src/portable/raspberrypi/rp2040/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/raspberrypi/rp2040/rp2040_usb.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c src/portable/raspberrypi/rp2040/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-portable-2f-raspberrypi-2f-rp2040

clean-src-2f-portable-2f-raspberrypi-2f-rp2040:
	-$(RM) ./src/portable/raspberrypi/rp2040/dcd_rp2040.cyclo ./src/portable/raspberrypi/rp2040/dcd_rp2040.d ./src/portable/raspberrypi/rp2040/dcd_rp2040.o ./src/portable/raspberrypi/rp2040/dcd_rp2040.su ./src/portable/raspberrypi/rp2040/hcd_rp2040.cyclo ./src/portable/raspberrypi/rp2040/hcd_rp2040.d ./src/portable/raspberrypi/rp2040/hcd_rp2040.o ./src/portable/raspberrypi/rp2040/hcd_rp2040.su ./src/portable/raspberrypi/rp2040/rp2040_usb.cyclo ./src/portable/raspberrypi/rp2040/rp2040_usb.d ./src/portable/raspberrypi/rp2040/rp2040_usb.o ./src/portable/raspberrypi/rp2040/rp2040_usb.su

.PHONY: clean-src-2f-portable-2f-raspberrypi-2f-rp2040

