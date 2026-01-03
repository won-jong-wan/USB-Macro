################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/device/usbd.c \
../Libs/tinyusb/src/device/usbd_control.c 

OBJS += \
./src/device/usbd.o \
./src/device/usbd_control.o 

C_DEPS += \
./src/device/usbd.d \
./src/device/usbd_control.d 


# Each subdirectory must supply rules for building sources it contributes
src/device/usbd.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/device/usbd.c src/device/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/device/usbd_control.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/device/usbd_control.c src/device/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-device

clean-src-2f-device:
	-$(RM) ./src/device/usbd.cyclo ./src/device/usbd.d ./src/device/usbd.o ./src/device/usbd.su ./src/device/usbd_control.cyclo ./src/device/usbd_control.d ./src/device/usbd_control.o ./src/device/usbd_control.su

.PHONY: clean-src-2f-device

