################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/hw/bsp/nrf/family.c 

OBJS += \
./Libs/tinyusb/hw/bsp/nrf/family.o 

C_DEPS += \
./Libs/tinyusb/hw/bsp/nrf/family.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/hw/bsp/nrf/%.o Libs/tinyusb/hw/bsp/nrf/%.su Libs/tinyusb/hw/bsp/nrf/%.cyclo: ../Libs/tinyusb/hw/bsp/nrf/%.c Libs/tinyusb/hw/bsp/nrf/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-nrf

clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-nrf:
	-$(RM) ./Libs/tinyusb/hw/bsp/nrf/family.cyclo ./Libs/tinyusb/hw/bsp/nrf/family.d ./Libs/tinyusb/hw/bsp/nrf/family.o ./Libs/tinyusb/hw/bsp/nrf/family.su

.PHONY: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-nrf

