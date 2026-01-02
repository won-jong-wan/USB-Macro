################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/hw/bsp/stm32n6/family.c 

OBJS += \
./Libs/tinyusb/hw/bsp/stm32n6/family.o 

C_DEPS += \
./Libs/tinyusb/hw/bsp/stm32n6/family.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/hw/bsp/stm32n6/%.o Libs/tinyusb/hw/bsp/stm32n6/%.su Libs/tinyusb/hw/bsp/stm32n6/%.cyclo: ../Libs/tinyusb/hw/bsp/stm32n6/%.c Libs/tinyusb/hw/bsp/stm32n6/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-stm32n6

clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-stm32n6:
	-$(RM) ./Libs/tinyusb/hw/bsp/stm32n6/family.cyclo ./Libs/tinyusb/hw/bsp/stm32n6/family.d ./Libs/tinyusb/hw/bsp/stm32n6/family.o ./Libs/tinyusb/hw/bsp/stm32n6/family.su

.PHONY: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-stm32n6

