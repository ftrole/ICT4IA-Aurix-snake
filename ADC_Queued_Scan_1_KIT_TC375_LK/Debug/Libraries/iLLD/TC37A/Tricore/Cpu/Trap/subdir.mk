################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/iLLD/TC37A/Tricore/Cpu/Trap/IfxCpu_Trap.c 

OBJS += \
./Libraries/iLLD/TC37A/Tricore/Cpu/Trap/IfxCpu_Trap.o 

COMPILED_SRCS += \
./Libraries/iLLD/TC37A/Tricore/Cpu/Trap/IfxCpu_Trap.src 

C_DEPS += \
./Libraries/iLLD/TC37A/Tricore/Cpu/Trap/IfxCpu_Trap.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/iLLD/TC37A/Tricore/Cpu/Trap/%.src: ../Libraries/iLLD/TC37A/Tricore/Cpu/Trap/%.c Libraries/iLLD/TC37A/Tricore/Cpu/Trap/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc37x "-fC:/Users/franc/AURIX-v1.6.0-workspace/ADC_Queued_Scan_1_KIT_TC375_LK/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc37x -o "$@"  "$<"  -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

Libraries/iLLD/TC37A/Tricore/Cpu/Trap/%.o: ./Libraries/iLLD/TC37A/Tricore/Cpu/Trap/%.src Libraries/iLLD/TC37A/Tricore/Cpu/Trap/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


