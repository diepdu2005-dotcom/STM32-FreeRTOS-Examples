################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Project_Embeded/stm32f103vet6_freertos_v1a/stm32f103vet6_freertos_v1a/Core/Src/system_stm32f1xx.c 

OBJS += \
./Drivers/CMSIS/system_stm32f1xx.o 

C_DEPS += \
./Drivers/CMSIS/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/system_stm32f1xx.o: D:/Project_Embeded/stm32f103vet6_freertos_v1a/stm32f103vet6_freertos_v1a/Core/Src/system_stm32f1xx.c Drivers/CMSIS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../../Core/Inc -I../../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../../Drivers/STM32F1xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS

clean-Drivers-2f-CMSIS:
	-$(RM) ./Drivers/CMSIS/system_stm32f1xx.cyclo ./Drivers/CMSIS/system_stm32f1xx.d ./Drivers/CMSIS/system_stm32f1xx.o ./Drivers/CMSIS/system_stm32f1xx.su

.PHONY: clean-Drivers-2f-CMSIS

