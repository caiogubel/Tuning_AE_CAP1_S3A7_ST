################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../synergy/ssp/src/bsp/mcu/all/bsp_common.c \
../synergy/ssp/src/bsp/mcu/all/bsp_common_leds.c \
../synergy/ssp/src/bsp/mcu/all/bsp_delay.c \
../synergy/ssp/src/bsp/mcu/all/bsp_irq.c \
../synergy/ssp/src/bsp/mcu/all/bsp_locking.c \
../synergy/ssp/src/bsp/mcu/all/bsp_register_protection.c \
../synergy/ssp/src/bsp/mcu/all/bsp_sbrk.c 

OBJS += \
./synergy/ssp/src/bsp/mcu/all/bsp_common.o \
./synergy/ssp/src/bsp/mcu/all/bsp_common_leds.o \
./synergy/ssp/src/bsp/mcu/all/bsp_delay.o \
./synergy/ssp/src/bsp/mcu/all/bsp_irq.o \
./synergy/ssp/src/bsp/mcu/all/bsp_locking.o \
./synergy/ssp/src/bsp/mcu/all/bsp_register_protection.o \
./synergy/ssp/src/bsp/mcu/all/bsp_sbrk.o 

C_DEPS += \
./synergy/ssp/src/bsp/mcu/all/bsp_common.d \
./synergy/ssp/src/bsp/mcu/all/bsp_common_leds.d \
./synergy/ssp/src/bsp/mcu/all/bsp_delay.d \
./synergy/ssp/src/bsp/mcu/all/bsp_irq.d \
./synergy/ssp/src/bsp/mcu/all/bsp_locking.d \
./synergy/ssp/src/bsp/mcu/all/bsp_register_protection.d \
./synergy/ssp/src/bsp/mcu/all/bsp_sbrk.d 


# Each subdirectory must supply rules for building sources it contributes
synergy/ssp/src/bsp/mcu/all/%.o: ../synergy/ssp/src/bsp/mcu/all/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	C:\Renesas\Synergy\e2studio_v5.4.0.023_ssp_v1.3.3\eclipse\../Utilities/isdebuild arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -D_RENESAS_SYNERGY_ -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy_cfg\ssp_cfg\bsp" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy_cfg\ssp_cfg\driver" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\bsp" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\driver\api" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\driver\instances" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\src" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\src\synergy_gen" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy_cfg\ssp_cfg\framework\el" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\framework\el" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\src\framework\el\tx" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\src\framework\el\ux" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy_cfg\ssp_cfg\framework" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\framework\api" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\framework\instances" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


