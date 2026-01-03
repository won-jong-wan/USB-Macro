################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/portable/nxp/khci/dcd_khci.c \
../Libs/tinyusb/src/portable/nxp/khci/hcd_khci.c 

OBJS += \
./src/portable/nxp/khci/dcd_khci.o \
./src/portable/nxp/khci/hcd_khci.o 

C_DEPS += \
./src/portable/nxp/khci/dcd_khci.d \
./src/portable/nxp/khci/hcd_khci.d 


# Each subdirectory must supply rules for building sources it contributes
src/portable/nxp/khci/dcd_khci.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/nxp/khci/dcd_khci.c src/portable/nxp/khci/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/nxp/khci/hcd_khci.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/nxp/khci/hcd_khci.c src/portable/nxp/khci/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-portable-2f-nxp-2f-khci

clean-src-2f-portable-2f-nxp-2f-khci:
	-$(RM) ./src/portable/nxp/khci/dcd_khci.cyclo ./src/portable/nxp/khci/dcd_khci.d ./src/portable/nxp/khci/dcd_khci.o ./src/portable/nxp/khci/dcd_khci.su ./src/portable/nxp/khci/hcd_khci.cyclo ./src/portable/nxp/khci/hcd_khci.d ./src/portable/nxp/khci/hcd_khci.o ./src/portable/nxp/khci/hcd_khci.su

.PHONY: clean-src-2f-portable-2f-nxp-2f-khci

