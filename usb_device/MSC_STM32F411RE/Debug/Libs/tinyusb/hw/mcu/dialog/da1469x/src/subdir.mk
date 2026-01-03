################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.c \
../Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.c \
../Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system.c \
../Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.c \
../Libs/tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.c 

OBJS += \
./Libs/tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.o \
./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.o \
./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system.o \
./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.o \
./Libs/tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.o 

C_DEPS += \
./Libs/tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.d \
./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.d \
./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system.d \
./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.d \
./Libs/tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/hw/mcu/dialog/da1469x/src/%.o Libs/tinyusb/hw/mcu/dialog/da1469x/src/%.su Libs/tinyusb/hw/mcu/dialog/da1469x/src/%.cyclo: ../Libs/tinyusb/hw/mcu/dialog/da1469x/src/%.c Libs/tinyusb/hw/mcu/dialog/da1469x/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-hw-2f-mcu-2f-dialog-2f-da1469x-2f-src

clean-Libs-2f-tinyusb-2f-hw-2f-mcu-2f-dialog-2f-da1469x-2f-src:
	-$(RM) ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.cyclo ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.d ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.o ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/da1469x_clock.su ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.cyclo ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.d ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.o ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_gpio.su ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system.cyclo ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system.d ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system.o ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system.su ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.cyclo ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.d ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.o ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/hal_system_start.su ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.cyclo ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.d ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.o ./Libs/tinyusb/hw/mcu/dialog/da1469x/src/system_da1469x.su

.PHONY: clean-Libs-2f-tinyusb-2f-hw-2f-mcu-2f-dialog-2f-da1469x-2f-src

