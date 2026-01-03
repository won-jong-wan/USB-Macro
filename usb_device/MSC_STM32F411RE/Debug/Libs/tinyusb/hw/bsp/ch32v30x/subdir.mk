################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/hw/bsp/ch32v30x/ch32v30x_it.c \
../Libs/tinyusb/hw/bsp/ch32v30x/debug_uart.c \
../Libs/tinyusb/hw/bsp/ch32v30x/family.c \
../Libs/tinyusb/hw/bsp/ch32v30x/system_ch32v30x.c 

OBJS += \
./Libs/tinyusb/hw/bsp/ch32v30x/ch32v30x_it.o \
./Libs/tinyusb/hw/bsp/ch32v30x/debug_uart.o \
./Libs/tinyusb/hw/bsp/ch32v30x/family.o \
./Libs/tinyusb/hw/bsp/ch32v30x/system_ch32v30x.o 

C_DEPS += \
./Libs/tinyusb/hw/bsp/ch32v30x/ch32v30x_it.d \
./Libs/tinyusb/hw/bsp/ch32v30x/debug_uart.d \
./Libs/tinyusb/hw/bsp/ch32v30x/family.d \
./Libs/tinyusb/hw/bsp/ch32v30x/system_ch32v30x.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/hw/bsp/ch32v30x/%.o Libs/tinyusb/hw/bsp/ch32v30x/%.su Libs/tinyusb/hw/bsp/ch32v30x/%.cyclo: ../Libs/tinyusb/hw/bsp/ch32v30x/%.c Libs/tinyusb/hw/bsp/ch32v30x/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-ch32v30x

clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-ch32v30x:
	-$(RM) ./Libs/tinyusb/hw/bsp/ch32v30x/ch32v30x_it.cyclo ./Libs/tinyusb/hw/bsp/ch32v30x/ch32v30x_it.d ./Libs/tinyusb/hw/bsp/ch32v30x/ch32v30x_it.o ./Libs/tinyusb/hw/bsp/ch32v30x/ch32v30x_it.su ./Libs/tinyusb/hw/bsp/ch32v30x/debug_uart.cyclo ./Libs/tinyusb/hw/bsp/ch32v30x/debug_uart.d ./Libs/tinyusb/hw/bsp/ch32v30x/debug_uart.o ./Libs/tinyusb/hw/bsp/ch32v30x/debug_uart.su ./Libs/tinyusb/hw/bsp/ch32v30x/family.cyclo ./Libs/tinyusb/hw/bsp/ch32v30x/family.d ./Libs/tinyusb/hw/bsp/ch32v30x/family.o ./Libs/tinyusb/hw/bsp/ch32v30x/family.su ./Libs/tinyusb/hw/bsp/ch32v30x/system_ch32v30x.cyclo ./Libs/tinyusb/hw/bsp/ch32v30x/system_ch32v30x.d ./Libs/tinyusb/hw/bsp/ch32v30x/system_ch32v30x.o ./Libs/tinyusb/hw/bsp/ch32v30x/system_ch32v30x.su

.PHONY: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-ch32v30x

