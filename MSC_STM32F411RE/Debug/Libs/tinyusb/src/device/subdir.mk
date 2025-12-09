################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/device/usbd.c \
../Libs/tinyusb/src/device/usbd_control.c 

OBJS += \
./Libs/tinyusb/src/device/usbd.o \
./Libs/tinyusb/src/device/usbd_control.o 

C_DEPS += \
./Libs/tinyusb/src/device/usbd.d \
./Libs/tinyusb/src/device/usbd_control.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/src/device/%.o Libs/tinyusb/src/device/%.su Libs/tinyusb/src/device/%.cyclo: ../Libs/tinyusb/src/device/%.c Libs/tinyusb/src/device/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kccistc/Desktop/USB-Macro/MSC_STM32F411RE/Libs/tinyusb/hw" -I"C:/Users/kccistc/Desktop/USB-Macro/MSC_STM32F411RE/Libs/tinyusb/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-src-2f-device

clean-Libs-2f-tinyusb-2f-src-2f-device:
	-$(RM) ./Libs/tinyusb/src/device/usbd.cyclo ./Libs/tinyusb/src/device/usbd.d ./Libs/tinyusb/src/device/usbd.o ./Libs/tinyusb/src/device/usbd.su ./Libs/tinyusb/src/device/usbd_control.cyclo ./Libs/tinyusb/src/device/usbd_control.d ./Libs/tinyusb/src/device/usbd_control.o ./Libs/tinyusb/src/device/usbd_control.su

.PHONY: clean-Libs-2f-tinyusb-2f-src-2f-device

