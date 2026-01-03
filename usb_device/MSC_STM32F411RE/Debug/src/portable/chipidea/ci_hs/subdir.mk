################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/src/portable/chipidea/ci_hs/dcd_ci_hs.c \
../Libs/tinyusb/src/portable/chipidea/ci_hs/hcd_ci_hs.c 

OBJS += \
./src/portable/chipidea/ci_hs/dcd_ci_hs.o \
./src/portable/chipidea/ci_hs/hcd_ci_hs.o 

C_DEPS += \
./src/portable/chipidea/ci_hs/dcd_ci_hs.d \
./src/portable/chipidea/ci_hs/hcd_ci_hs.d 


# Each subdirectory must supply rules for building sources it contributes
src/portable/chipidea/ci_hs/dcd_ci_hs.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/chipidea/ci_hs/dcd_ci_hs.c src/portable/chipidea/ci_hs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
src/portable/chipidea/ci_hs/hcd_ci_hs.o: C:/Users/jonwo/STM32CubeIDE/workspace_1.19.0/MSC_STM32F411RE/Libs/tinyusb/src/portable/chipidea/ci_hs/hcd_ci_hs.c src/portable/chipidea/ci_hs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src-2f-portable-2f-chipidea-2f-ci_hs

clean-src-2f-portable-2f-chipidea-2f-ci_hs:
	-$(RM) ./src/portable/chipidea/ci_hs/dcd_ci_hs.cyclo ./src/portable/chipidea/ci_hs/dcd_ci_hs.d ./src/portable/chipidea/ci_hs/dcd_ci_hs.o ./src/portable/chipidea/ci_hs/dcd_ci_hs.su ./src/portable/chipidea/ci_hs/hcd_ci_hs.cyclo ./src/portable/chipidea/ci_hs/hcd_ci_hs.d ./src/portable/chipidea/ci_hs/hcd_ci_hs.o ./src/portable/chipidea/ci_hs/hcd_ci_hs.su

.PHONY: clean-src-2f-portable-2f-chipidea-2f-ci_hs

