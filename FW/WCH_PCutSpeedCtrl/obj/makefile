################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/pr_queue/subdir.mk
-include User/hybos_port/subdir.mk
-include User/hybos/subdir.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
WCH_PCutSpeedCtrl.hex \

SECONDARY_LIST += \
WCH_PCutSpeedCtrl.lst \

SECONDARY_SIZE += \
WCH_PCutSpeedCtrl.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: WCH_PCutSpeedCtrl.elf secondary-outputs

# Tool invocations
WCH_PCutSpeedCtrl.elf: $(OBJS) $(USER_OBJS)
	@	riscv-wch-elf-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -T "/mnt/Work/Projects/CATEGORIZED/CNC/Hardwares/PCutSpeedCTRL/V00_01_b/WCH_PCutSpeedCtrl/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"WCH_PCutSpeedCtrl.map" --specs=nano.specs --specs=nosys.specs -o "WCH_PCutSpeedCtrl.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lprintf
WCH_PCutSpeedCtrl.hex: WCH_PCutSpeedCtrl.elf
	@	riscv-wch-elf-objcopy -O ihex "WCH_PCutSpeedCtrl.elf" "WCH_PCutSpeedCtrl.hex"
WCH_PCutSpeedCtrl.lst: WCH_PCutSpeedCtrl.elf
	@	riscv-wch-elf-objdump --all-headers --demangle --disassemble -M xw "WCH_PCutSpeedCtrl.elf" > "WCH_PCutSpeedCtrl.lst"
WCH_PCutSpeedCtrl.siz: WCH_PCutSpeedCtrl.elf
	riscv-wch-elf-size --format=berkeley "WCH_PCutSpeedCtrl.elf"

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) WCH_PCutSpeedCtrl.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets