################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/hybos_port/hybos_single.c 

C_DEPS += \
./User/hybos_port/hybos_single.d 

OBJS += \
./User/hybos_port/hybos_single.o 



# Each subdirectory must supply rules for building sources it contributes
User/hybos_port/%.o: ../User/hybos_port/%.c
	@	riscv-wch-elf-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/Debug" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/Core" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/User" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/Peripheral/inc" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/User/hybos" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/User/hybos_port" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/User/pr_queue" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
