################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Main/Src/drive.c \
../Main/Src/init.c \
../Main/Src/motor.c \
../Main/Src/sensor.c 

OBJS += \
./Main/Src/drive.o \
./Main/Src/init.o \
./Main/Src/motor.o \
./Main/Src/sensor.o 

C_DEPS += \
./Main/Src/drive.d \
./Main/Src/init.d \
./Main/Src/motor.d \
./Main/Src/sensor.d 


# Each subdirectory must supply rules for building sources it contributes
Main/Src/%.o Main/Src/%.su Main/Src/%.cyclo: ../Main/Src/%.c Main/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DSTM32F411xE -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/j93es/STM32CubeIDE/workspace_1.12.1/Nucleo-Linetracer/Main/Inc" -I"/Users/j93es/STM32CubeIDE/workspace_1.12.1/Nucleo-Linetracer/External Library/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Main-2f-Src

clean-Main-2f-Src:
	-$(RM) ./Main/Src/drive.cyclo ./Main/Src/drive.d ./Main/Src/drive.o ./Main/Src/drive.su ./Main/Src/init.cyclo ./Main/Src/init.d ./Main/Src/init.o ./Main/Src/init.su ./Main/Src/motor.cyclo ./Main/Src/motor.d ./Main/Src/motor.o ./Main/Src/motor.su ./Main/Src/sensor.cyclo ./Main/Src/sensor.d ./Main/Src/sensor.o ./Main/Src/sensor.su

.PHONY: clean-Main-2f-Src

