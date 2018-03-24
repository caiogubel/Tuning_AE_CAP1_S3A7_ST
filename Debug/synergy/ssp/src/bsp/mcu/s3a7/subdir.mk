################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../synergy/ssp/src/bsp/mcu/s3a7/bsp_cache.c \
../synergy/ssp/src/bsp/mcu/s3a7/bsp_clocks.c \
../synergy/ssp/src/bsp/mcu/s3a7/bsp_feature.c \
../synergy/ssp/src/bsp/mcu/s3a7/bsp_fmi_R7FS3A77C3A01CFB.c \
../synergy/ssp/src/bsp/mcu/s3a7/bsp_group_irq.c \
../synergy/ssp/src/bsp/mcu/s3a7/bsp_hw_locks.c \
../synergy/ssp/src/bsp/mcu/s3a7/bsp_module_stop.c \
../synergy/ssp/src/bsp/mcu/s3a7/bsp_rom_registers.c 

OBJS += \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_cache.o \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_clocks.o \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_feature.o \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_fmi_R7FS3A77C3A01CFB.o \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_group_irq.o \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_hw_locks.o \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_module_stop.o \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_rom_registers.o 

C_DEPS += \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_cache.d \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_clocks.d \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_feature.d \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_fmi_R7FS3A77C3A01CFB.d \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_group_irq.d \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_hw_locks.d \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_module_stop.d \
./synergy/ssp/src/bsp/mcu/s3a7/bsp_rom_registers.d 


# Each subdirectory must supply rules for building sources it contributes
synergy/ssp/src/bsp/mcu/s3a7/%.o: ../synergy/ssp/src/bsp/mcu/s3a7/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	C:\Renesas\Synergy\e2studio_v5.4.0.023_ssp_v1.3.3\eclipse\../Utilities/isdebuild arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -D_RENESAS_SYNERGY_ -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy_cfg\ssp_cfg\bsp" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy_cfg\ssp_cfg\driver" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\bsp" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\driver\api" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\driver\instances" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\src" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\src\synergy_gen" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy_cfg\ssp_cfg\framework\el" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\framework\el" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\src\framework\el\tx" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\src\framework\el\ux" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy_cfg\ssp_cfg\framework" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\framework\api" -I"C:\Projects\Tuning_AE_CAP1_S3A7_ST\synergy\ssp\inc\framework\instances" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


