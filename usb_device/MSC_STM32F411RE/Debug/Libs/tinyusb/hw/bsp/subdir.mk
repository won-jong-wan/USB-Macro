################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/hw/bsp/board.c 

OBJS += \
./Libs/tinyusb/hw/bsp/board.o 

C_DEPS += \
./Libs/tinyusb/hw/bsp/board.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/hw/bsp/%.o Libs/tinyusb/hw/bsp/%.su Libs/tinyusb/hw/bsp/%.cyclo: ../Libs/tinyusb/hw/bsp/%.c Libs/tinyusb/hw/bsp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-hw-2f-bsp

clean-Libs-2f-tinyusb-2f-hw-2f-bsp:
	-$(RM) ./Libs/tinyusb/hw/bsp/board.cyclo ./Libs/tinyusb/hw/bsp/board.d ./Libs/tinyusb/hw/bsp/board.o ./Libs/tinyusb/hw/bsp/board.su

.PHONY: clean-Libs-2f-tinyusb-2f-hw-2f-bsp

