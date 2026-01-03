################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/portable/synopsys/dwc2/dcd_dwc2.c \
../Libs/tinyusb/src/portable/synopsys/dwc2/dwc2_common.c \
../Libs/tinyusb/src/portable/synopsys/dwc2/hcd_dwc2.c 

OBJS += \
./src/portable/synopsys/dwc2/dcd_dwc2.o \
./src/portable/synopsys/dwc2/dwc2_common.o \
./src/portable/synopsys/dwc2/hcd_dwc2.o 

C_DEPS += \
./src/portable/synopsys/dwc2/dcd_dwc2.d \
./src/portable/synopsys/dwc2/dwc2_common.d \
./src/portable/synopsys/dwc2/hcd_dwc2.d 


# Each subdirectory must supply rules for building sources it contributes
src/portable/synopsys/dwc2/dcd_dwc2.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/synopsys/dwc2/dcd_dwc2.c src/portable/synopsys/dwc2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/synopsys/dwc2/dwc2_common.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/synopsys/dwc2/dwc2_common.c src/portable/synopsys/dwc2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/synopsys/dwc2/hcd_dwc2.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/synopsys/dwc2/hcd_dwc2.c src/portable/synopsys/dwc2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-portable-2f-synopsys-2f-dwc2

clean-src-2f-portable-2f-synopsys-2f-dwc2:
	-$(RM) ./src/portable/synopsys/dwc2/dcd_dwc2.cyclo ./src/portable/synopsys/dwc2/dcd_dwc2.d ./src/portable/synopsys/dwc2/dcd_dwc2.o ./src/portable/synopsys/dwc2/dcd_dwc2.su ./src/portable/synopsys/dwc2/dwc2_common.cyclo ./src/portable/synopsys/dwc2/dwc2_common.d ./src/portable/synopsys/dwc2/dwc2_common.o ./src/portable/synopsys/dwc2/dwc2_common.su ./src/portable/synopsys/dwc2/hcd_dwc2.cyclo ./src/portable/synopsys/dwc2/hcd_dwc2.d ./src/portable/synopsys/dwc2/hcd_dwc2.o ./src/portable/synopsys/dwc2/hcd_dwc2.su

.PHONY: clean-src-2f-portable-2f-synopsys-2f-dwc2

