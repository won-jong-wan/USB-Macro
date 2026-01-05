################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.c \
../Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.c 

OBJS += \
./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.o \
./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.o 

C_DEPS += \
./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.d \
./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/%.o Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/%.su Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/%.cyclo: ../Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/%.c Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-hw-2f-mcu-2f-sony-2f-cxd56-2f-mkspk

clean-Libs-2f-tinyusb-2f-hw-2f-mcu-2f-sony-2f-cxd56-2f-mkspk:
	-$(RM) ./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.cyclo ./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.d ./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.o ./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/clefia.su ./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.cyclo ./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.d ./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.o ./Libs/tinyusb/hw/mcu/sony/cxd56/mkspk/mkspk.su

.PHONY: clean-Libs-2f-tinyusb-2f-hw-2f-mcu-2f-sony-2f-cxd56-2f-mkspk

