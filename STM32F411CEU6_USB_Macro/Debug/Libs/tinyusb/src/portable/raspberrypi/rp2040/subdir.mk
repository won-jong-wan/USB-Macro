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
./Libs/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.o \
./Libs/tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.o \
./Libs/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.o 

C_DEPS += \
./Libs/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.d \
./Libs/tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.d \
./Libs/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/src/portable/raspberrypi/rp2040/%.o Libs/tinyusb/src/portable/raspberrypi/rp2040/%.su Libs/tinyusb/src/portable/raspberrypi/rp2040/%.cyclo: ../Libs/tinyusb/src/portable/raspberrypi/rp2040/%.c Libs/tinyusb/src/portable/raspberrypi/rp2040/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kccistc/Desktop/USB-Macro/STM32F411CEU6_USB_Macro/Libs/tinyusb/hw" -I"C:/Users/kccistc/Desktop/USB-Macro/STM32F411CEU6_USB_Macro/Libs/tinyusb/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-src-2f-portable-2f-raspberrypi-2f-rp2040

clean-Libs-2f-tinyusb-2f-src-2f-portable-2f-raspberrypi-2f-rp2040:
	-$(RM) ./Libs/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.cyclo ./Libs/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.d ./Libs/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.o ./Libs/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.su ./Libs/tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.cyclo ./Libs/tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.d ./Libs/tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.o ./Libs/tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.su ./Libs/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.cyclo ./Libs/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.d ./Libs/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.o ./Libs/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.su

.PHONY: clean-Libs-2f-tinyusb-2f-src-2f-portable-2f-raspberrypi-2f-rp2040

