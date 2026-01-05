################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/portable/nuvoton/nuc120/dcd_nuc120.c 

OBJS += \
./src/portable/nuvoton/nuc120/dcd_nuc120.o 

C_DEPS += \
./src/portable/nuvoton/nuc120/dcd_nuc120.d 


# Each subdirectory must supply rules for building sources it contributes
src/portable/nuvoton/nuc120/dcd_nuc120.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/nuvoton/nuc120/dcd_nuc120.c src/portable/nuvoton/nuc120/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-portable-2f-nuvoton-2f-nuc120

clean-src-2f-portable-2f-nuvoton-2f-nuc120:
	-$(RM) ./src/portable/nuvoton/nuc120/dcd_nuc120.cyclo ./src/portable/nuvoton/nuc120/dcd_nuc120.d ./src/portable/nuvoton/nuc120/dcd_nuc120.o ./src/portable/nuvoton/nuc120/dcd_nuc120.su

.PHONY: clean-src-2f-portable-2f-nuvoton-2f-nuc120

