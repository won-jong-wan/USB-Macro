################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/clock_config.c \
../Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/pin_mux.c 

OBJS += \
./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/clock_config.o \
./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/pin_mux.o 

C_DEPS += \
./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/clock_config.d \
./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/%.o Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/%.su Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/%.cyclo: ../Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/%.c Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Libs/tinyusb/src -I../Libs/tinyusb/hw -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-mcx-2f-boards-2f-frdm_mcxa153-2f-board

clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-mcx-2f-boards-2f-frdm_mcxa153-2f-board:
	-$(RM) ./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/clock_config.cyclo ./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/clock_config.d ./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/clock_config.o ./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/clock_config.su ./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/pin_mux.cyclo ./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/pin_mux.d ./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/pin_mux.o ./Libs/tinyusb/hw/bsp/mcx/boards/frdm_mcxa153/board/pin_mux.su

.PHONY: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-mcx-2f-boards-2f-frdm_mcxa153-2f-board

