################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/kits/common/bsp/bsp_trace.c 

OBJS += \
./BSP/bsp_trace.o 

C_DEPS += \
./BSP/bsp_trace.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/bsp_trace.o: C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/kits/common/bsp/bsp_trace.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb '-DEFM32WG990F256=1' '-DDEBUG=1' -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/kits/EFM32WG_STK3800/examples/touch" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/kits/EFM32WG_STK3800/config" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emdrv/common/inc" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emdrv/config" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emdrv/dmadrv/inc" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emdrv/gpiointerrupt/inc" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emdrv/nvm/inc" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emdrv/rtcdrv/inc" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emdrv/sleep/inc" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emdrv/spidrv/inc" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emdrv/uartdrv/inc" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/emdrv/ustimer/inc" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/kits/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/kits/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v3_3/developer/sdks/efm32/v2/Device/SiliconLabs/EFM32WG/Include" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -std=c99 -MMD -MP -MF"BSP/bsp_trace.d" -MT"BSP/bsp_trace.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


