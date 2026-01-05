################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/portable/nxp/lpc17_40/dcd_lpc17_40.c 

OBJS += \
./src/portable/nxp/lpc17_40/dcd_lpc17_40.o 

C_DEPS += \
./src/portable/nxp/lpc17_40/dcd_lpc17_40.d 


# Each subdirectory must supply rules for building sources it contributes
src/portable/nxp/lpc17_40/dcd_lpc17_40.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/nxp/lpc17_40/dcd_lpc17_40.c src/portable/nxp/lpc17_40/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-portable-2f-nxp-2f-lpc17_40

clean-src-2f-portable-2f-nxp-2f-lpc17_40:
	-$(RM) ./src/portable/nxp/lpc17_40/dcd_lpc17_40.cyclo ./src/portable/nxp/lpc17_40/dcd_lpc17_40.d ./src/portable/nxp/lpc17_40/dcd_lpc17_40.o ./src/portable/nxp/lpc17_40/dcd_lpc17_40.su

.PHONY: clean-src-2f-portable-2f-nxp-2f-lpc17_40

