################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/led_strip_rmt_ws2812_main.c 

OBJS += \
./Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/led_strip_rmt_ws2812_main.o 

C_DEPS += \
./Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/led_strip_rmt_ws2812_main.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/%.o Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/%.su Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/%.cyclo: ../Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/%.c Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-espressif-2f-components-2f-led_strip-2f-examples-2f-led_strip_rmt_ws2812-2f-main

clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-espressif-2f-components-2f-led_strip-2f-examples-2f-led_strip_rmt_ws2812-2f-main:
	-$(RM) ./Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/led_strip_rmt_ws2812_main.cyclo ./Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/led_strip_rmt_ws2812_main.d ./Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/led_strip_rmt_ws2812_main.o ./Libs/tinyusb/hw/bsp/espressif/components/led_strip/examples/led_strip_rmt_ws2812/main/led_strip_rmt_ws2812_main.su

.PHONY: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-espressif-2f-components-2f-led_strip-2f-examples-2f-led_strip_rmt_ws2812-2f-main

