################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/class/cdc/cdc_device.c \
../Libs/tinyusb/src/class/cdc/cdc_host.c \
../Libs/tinyusb/src/class/cdc/cdc_rndis_host.c 

OBJS += \
./src/class/cdc/cdc_device.o \
./src/class/cdc/cdc_host.o \
./src/class/cdc/cdc_rndis_host.o 

C_DEPS += \
./src/class/cdc/cdc_device.d \
./src/class/cdc/cdc_host.d \
./src/class/cdc/cdc_rndis_host.d 


# Each subdirectory must supply rules for building sources it contributes
src/class/cdc/cdc_device.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/class/cdc/cdc_device.c src/class/cdc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/class/cdc/cdc_host.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/class/cdc/cdc_host.c src/class/cdc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/class/cdc/cdc_rndis_host.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/class/cdc/cdc_rndis_host.c src/class/cdc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-class-2f-cdc

clean-src-2f-class-2f-cdc:
	-$(RM) ./src/class/cdc/cdc_device.cyclo ./src/class/cdc/cdc_device.d ./src/class/cdc/cdc_device.o ./src/class/cdc/cdc_device.su ./src/class/cdc/cdc_host.cyclo ./src/class/cdc/cdc_host.d ./src/class/cdc/cdc_host.o ./src/class/cdc/cdc_host.su ./src/class/cdc/cdc_rndis_host.cyclo ./src/class/cdc/cdc_rndis_host.d ./src/class/cdc/cdc_rndis_host.o ./src/class/cdc/cdc_rndis_host.su

.PHONY: clean-src-2f-class-2f-cdc

