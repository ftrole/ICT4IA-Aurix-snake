################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/iLLD/TC39B/Tricore/Stm/Timer/IfxStm_Timer.c 

OBJS += \
./Libraries/iLLD/TC39B/Tricore/Stm/Timer/IfxStm_Timer.o 

COMPILED_SRCS += \
./Libraries/iLLD/TC39B/Tricore/Stm/Timer/IfxStm_Timer.src 

C_DEPS += \
./Libraries/iLLD/TC39B/Tricore/Stm/Timer/IfxStm_Timer.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/iLLD/TC39B/Tricore/Stm/Timer/%.src: ../Libraries/iLLD/TC39B/Tricore/Stm/Timer/%.c Libraries/iLLD/TC39B/Tricore/Stm/Timer/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc39xb "-fC:/Users/franc/AURIX-v1.6.0-workspace/SPI_CPU_1_KIT_TC397_TFT/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc39xb -o "$@"  "$<"  -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

Libraries/iLLD/TC39B/Tricore/Stm/Timer/%.o: ./Libraries/iLLD/TC39B/Tricore/Stm/Timer/%.src Libraries/iLLD/TC39B/Tricore/Stm/Timer/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


