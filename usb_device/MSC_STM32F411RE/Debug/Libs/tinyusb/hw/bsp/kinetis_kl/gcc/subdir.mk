################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../Libs/tinyusb/hw/bsp/kinetis_kl/gcc/startup_MKL25Z4.S 

OBJS += \
./Libs/tinyusb/hw/bsp/kinetis_kl/gcc/startup_MKL25Z4.o 

S_UPPER_DEPS += \
./Libs/tinyusb/hw/bsp/kinetis_kl/gcc/startup_MKL25Z4.d 


# Each subdirectory must supply rules for building sources it contributes
Libs/tinyusb/hw/bsp/kinetis_kl/gcc/%.o: ../Libs/tinyusb/hw/bsp/kinetis_kl/gcc/%.S Libs/tinyusb/hw/bsp/kinetis_kl/gcc/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-kinetis_kl-2f-gcc

clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-kinetis_kl-2f-gcc:
	-$(RM) ./Libs/tinyusb/hw/bsp/kinetis_kl/gcc/startup_MKL25Z4.d ./Libs/tinyusb/hw/bsp/kinetis_kl/gcc/startup_MKL25Z4.o

.PHONY: clean-Libs-2f-tinyusb-2f-hw-2f-bsp-2f-kinetis_kl-2f-gcc

