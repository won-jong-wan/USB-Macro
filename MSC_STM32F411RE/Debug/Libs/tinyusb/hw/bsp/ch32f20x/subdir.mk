################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Libs/tinyusb/hw/bsp/ch32f20x/startup_gcc_ch32f20x_d8c.s 

C_SRCS += \
../Libs/tinyusb/hw/bsp/ch32f20x/ch32f20x_it.c \
../Libs/tinyusb/hw/bsp/ch32f20x/debug_uart.c \
../Libs/tinyusb/hw/bsp/ch32f20x/family.c \
../Libs/tinyusb/hw/bsp/ch32f20x/system_ch32f20x.c 

OBJS += \
./Libs/tinyusb/hw/bsp/ch32f20x/ch32f20x_it.o \
./Libs/tinyusb/hw/bsp/ch32f20x/debug_uart.o \
./Libs/tinyusb/hw/bsp/ch32f20x/family.o \
./Libs/tinyusb/hw/bsp/ch32f20x/startup_gcc_ch32f20x_d8c.o \
./Libs/tinyusb/hw/bsp/ch32f20x/system_ch32f20x.o 

S_DEPS += \
./Libs/tinyusb/hw/bsp/ch32f20x/startup_gcc_ch32f20x_d8c.d 

C_DEPS += \
./Libs/tinyusb/hw/bsp/ch32f20x/ch32f20x_it.d \
./Libs/tinyusb/hw/bsp/ch32f20x/debug_uart.d \
./Libs/tinyusb/hw/bsp/ch32f20x/family.d \
./Libs/tinyusb/hw/bsp/ch32f20x/system_ch32f20x.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/hw/bsp/ch32f20x/%.o Libs/tinyusb/hw/bsp/ch32f20x/%.su Libs/tinyusb/hw/bsp/ch32f20x/%.cyclo: ../Libs/tinyusb/hw/bsp/ch32f20x/%.c Libs/tinyusb/hw/bsp/ch32f20x/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Libs/tinyusb/hw/bsp/ch32f20x/%.o: ../Libs/tinyusb/hw/bsp/ch32f20x/%.s Libs/tinyusb/hw/bsp/ch32f20x/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-ch32f20x

clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-ch32f20x:
	-$(RM) ./Libs/tinyusb/hw/bsp/ch32f20x/ch32f20x_it.cyclo ./Libs/tinyusb/hw/bsp/ch32f20x/ch32f20x_it.d ./Libs/tinyusb/hw/bsp/ch32f20x/ch32f20x_it.o ./Libs/tinyusb/hw/bsp/ch32f20x/ch32f20x_it.su ./Libs/tinyusb/hw/bsp/ch32f20x/debug_uart.cyclo ./Libs/tinyusb/hw/bsp/ch32f20x/debug_uart.d ./Libs/tinyusb/hw/bsp/ch32f20x/debug_uart.o ./Libs/tinyusb/hw/bsp/ch32f20x/debug_uart.su ./Libs/tinyusb/hw/bsp/ch32f20x/family.cyclo ./Libs/tinyusb/hw/bsp/ch32f20x/family.d ./Libs/tinyusb/hw/bsp/ch32f20x/family.o ./Libs/tinyusb/hw/bsp/ch32f20x/family.su ./Libs/tinyusb/hw/bsp/ch32f20x/startup_gcc_ch32f20x_d8c.d ./Libs/tinyusb/hw/bsp/ch32f20x/startup_gcc_ch32f20x_d8c.o ./Libs/tinyusb/hw/bsp/ch32f20x/system_ch32f20x.cyclo ./Libs/tinyusb/hw/bsp/ch32f20x/system_ch32f20x.d ./Libs/tinyusb/hw/bsp/ch32f20x/system_ch32f20x.o ./Libs/tinyusb/hw/bsp/ch32f20x/system_ch32f20x.su

.PHONY: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-ch32f20x

