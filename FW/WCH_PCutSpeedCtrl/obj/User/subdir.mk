################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/NTC_Table.c \
../User/ch32v00x_it.c \
../User/diff_int.c \
../User/init_wch.c \
../User/main.c \
../User/mavg_int.c \
../User/system_ch32v00x.c 

C_DEPS += \
./User/NTC_Table.d \
./User/ch32v00x_it.d \
./User/diff_int.d \
./User/init_wch.d \
./User/main.d \
./User/mavg_int.d \
./User/system_ch32v00x.d 

OBJS += \
./User/NTC_Table.o \
./User/ch32v00x_it.o \
./User/diff_int.o \
./User/init_wch.o \
./User/main.o \
./User/mavg_int.o \
./User/system_ch32v00x.o 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-wch-elf-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/Debug" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/Core" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/User" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/Peripheral/inc" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/User/hybos" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/User/hybos_port" -I"/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/User/pr_queue" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
