################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/iLLD/TC39B/Tricore/Evadc/Std/IfxEvadc.c 

OBJS += \
./Libraries/iLLD/TC39B/Tricore/Evadc/Std/IfxEvadc.o 

COMPILED_SRCS += \
./Libraries/iLLD/TC39B/Tricore/Evadc/Std/IfxEvadc.src 

C_DEPS += \
./Libraries/iLLD/TC39B/Tricore/Evadc/Std/IfxEvadc.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/iLLD/TC39B/Tricore/Evadc/Std/%.src: ../Libraries/iLLD/TC39B/Tricore/Evadc/Std/%.c Libraries/iLLD/TC39B/Tricore/Evadc/Std/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -D__CPU__=tc39xb "-fC:/Users/franc/Desktop/ICT4IA-Aurix-snake/Snake_Game_KIT_TC397_TFT/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc39xb -o "$@"  "$<"  -cs --dep-file="$(@:.src=.d)" --misrac-version=2012 -N0 -Z0 -Y0 2>&1;
	@echo 'Finished building: $<'
	@echo ' '

Libraries/iLLD/TC39B/Tricore/Evadc/Std/%.o: ./Libraries/iLLD/TC39B/Tricore/Evadc/Std/%.src Libraries/iLLD/TC39B/Tricore/Evadc/Std/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<" --list-format=L1 --optimize=gs
	@echo 'Finished building: $<'
	@echo ' '


