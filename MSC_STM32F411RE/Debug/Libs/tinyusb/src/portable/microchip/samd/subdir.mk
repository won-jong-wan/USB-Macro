################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/portable/microchip/samd/dcd_samd.c \
../Libs/tinyusb/src/portable/microchip/samd/hcd_samd.c 

OBJS += \
./Libs/tinyusb/src/portable/microchip/samd/dcd_samd.o \
./Libs/tinyusb/src/portable/microchip/samd/hcd_samd.o 

C_DEPS += \
./Libs/tinyusb/src/portable/microchip/samd/dcd_samd.d \
./Libs/tinyusb/src/portable/microchip/samd/hcd_samd.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/src/portable/microchip/samd/%.o Libs/tinyusb/src/portable/microchip/samd/%.su Libs/tinyusb/src/portable/microchip/samd/%.cyclo: ../Libs/tinyusb/src/portable/microchip/samd/%.c Libs/tinyusb/src/portable/microchip/samd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/jonwo/Desktop/USB-Macro/MSC_STM32F411RE/Libs/tinyusb/hw" -I"C:/Users/jonwo/Desktop/USB-Macro/MSC_STM32F411RE/Libs/tinyusb/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-src-2f-portable-2f-microchip-2f-samd

clean-Libs-2f-tinyusb-2f-src-2f-portable-2f-microchip-2f-samd:
	-$(RM) ./Libs/tinyusb/src/portable/microchip/samd/dcd_samd.cyclo ./Libs/tinyusb/src/portable/microchip/samd/dcd_samd.d ./Libs/tinyusb/src/portable/microchip/samd/dcd_samd.o ./Libs/tinyusb/src/portable/microchip/samd/dcd_samd.su ./Libs/tinyusb/src/portable/microchip/samd/hcd_samd.cyclo ./Libs/tinyusb/src/portable/microchip/samd/hcd_samd.d ./Libs/tinyusb/src/portable/microchip/samd/hcd_samd.o ./Libs/tinyusb/src/portable/microchip/samd/hcd_samd.su

.PHONY: clean-Libs-2f-tinyusb-2f-src-2f-portable-2f-microchip-2f-samd

