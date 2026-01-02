################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/class/msc/msc_device.c \
../Libs/tinyusb/src/class/msc/msc_host.c 

OBJS += \
./Libs/tinyusb/src/class/msc/msc_device.o \
./Libs/tinyusb/src/class/msc/msc_host.o 

C_DEPS += \
./Libs/tinyusb/src/class/msc/msc_device.d \
./Libs/tinyusb/src/class/msc/msc_host.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/src/class/msc/%.o Libs/tinyusb/src/class/msc/%.su Libs/tinyusb/src/class/msc/%.cyclo: ../Libs/tinyusb/src/class/msc/%.c Libs/tinyusb/src/class/msc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/kccistc/Desktop/USB-Macro/STM32F411CEU6_USB_Macro/Libs/tinyusb/hw" -I"C:/Users/kccistc/Desktop/USB-Macro/STM32F411CEU6_USB_Macro/Libs/tinyusb/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-src-2f-class-2f-msc

clean-Libs-2f-tinyusb-2f-src-2f-class-2f-msc:
	-$(RM) ./Libs/tinyusb/src/class/msc/msc_device.cyclo ./Libs/tinyusb/src/class/msc/msc_device.d ./Libs/tinyusb/src/class/msc/msc_device.o ./Libs/tinyusb/src/class/msc/msc_device.su ./Libs/tinyusb/src/class/msc/msc_host.cyclo ./Libs/tinyusb/src/class/msc/msc_host.d ./Libs/tinyusb/src/class/msc/msc_host.o ./Libs/tinyusb/src/class/msc/msc_host.su

.PHONY: clean-Libs-2f-tinyusb-2f-src-2f-class-2f-msc

