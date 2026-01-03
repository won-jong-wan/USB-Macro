################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/portable/wch/dcd_ch32_usbfs.c \
../Libs/tinyusb/src/portable/wch/dcd_ch32_usbhs.c \
../Libs/tinyusb/src/portable/wch/hcd_ch32_usbfs.c 

OBJS += \
./src/portable/wch/dcd_ch32_usbfs.o \
./src/portable/wch/dcd_ch32_usbhs.o \
./src/portable/wch/hcd_ch32_usbfs.o 

C_DEPS += \
./src/portable/wch/dcd_ch32_usbfs.d \
./src/portable/wch/dcd_ch32_usbhs.d \
./src/portable/wch/hcd_ch32_usbfs.d 


# Each subdirectory must supply rules for building sources it contributes
src/portable/wch/dcd_ch32_usbfs.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/wch/dcd_ch32_usbfs.c src/portable/wch/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/wch/dcd_ch32_usbhs.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/wch/dcd_ch32_usbhs.c src/portable/wch/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/wch/hcd_ch32_usbfs.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/wch/hcd_ch32_usbfs.c src/portable/wch/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-portable-2f-wch

clean-src-2f-portable-2f-wch:
	-$(RM) ./src/portable/wch/dcd_ch32_usbfs.cyclo ./src/portable/wch/dcd_ch32_usbfs.d ./src/portable/wch/dcd_ch32_usbfs.o ./src/portable/wch/dcd_ch32_usbfs.su ./src/portable/wch/dcd_ch32_usbhs.cyclo ./src/portable/wch/dcd_ch32_usbhs.d ./src/portable/wch/dcd_ch32_usbhs.o ./src/portable/wch/dcd_ch32_usbhs.su ./src/portable/wch/hcd_ch32_usbfs.cyclo ./src/portable/wch/hcd_ch32_usbfs.d ./src/portable/wch/hcd_ch32_usbfs.o ./src/portable/wch/hcd_ch32_usbfs.su

.PHONY: clean-src-2f-portable-2f-wch

