################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/portable/renesas/rusb2/dcd_rusb2.c \
../Libs/tinyusb/src/portable/renesas/rusb2/hcd_rusb2.c \
../Libs/tinyusb/src/portable/renesas/rusb2/rusb2_common.c 

OBJS += \
./src/portable/renesas/rusb2/dcd_rusb2.o \
./src/portable/renesas/rusb2/hcd_rusb2.o \
./src/portable/renesas/rusb2/rusb2_common.o 

C_DEPS += \
./src/portable/renesas/rusb2/dcd_rusb2.d \
./src/portable/renesas/rusb2/hcd_rusb2.d \
./src/portable/renesas/rusb2/rusb2_common.d 


# Each subdirectory must supply rules for building sources it contributes
src/portable/renesas/rusb2/dcd_rusb2.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/renesas/rusb2/dcd_rusb2.c src/portable/renesas/rusb2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/renesas/rusb2/hcd_rusb2.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/renesas/rusb2/hcd_rusb2.c src/portable/renesas/rusb2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/renesas/rusb2/rusb2_common.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/renesas/rusb2/rusb2_common.c src/portable/renesas/rusb2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-portable-2f-renesas-2f-rusb2

clean-src-2f-portable-2f-renesas-2f-rusb2:
	-$(RM) ./src/portable/renesas/rusb2/dcd_rusb2.cyclo ./src/portable/renesas/rusb2/dcd_rusb2.d ./src/portable/renesas/rusb2/dcd_rusb2.o ./src/portable/renesas/rusb2/dcd_rusb2.su ./src/portable/renesas/rusb2/hcd_rusb2.cyclo ./src/portable/renesas/rusb2/hcd_rusb2.d ./src/portable/renesas/rusb2/hcd_rusb2.o ./src/portable/renesas/rusb2/hcd_rusb2.su ./src/portable/renesas/rusb2/rusb2_common.cyclo ./src/portable/renesas/rusb2/rusb2_common.d ./src/portable/renesas/rusb2/rusb2_common.o ./src/portable/renesas/rusb2/rusb2_common.su

.PHONY: clean-src-2f-portable-2f-renesas-2f-rusb2

