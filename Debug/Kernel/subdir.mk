################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Kernel/abrirPXP.c 

OBJS += \
./Kernel/abrirPXP.o 

C_DEPS += \
./Kernel/abrirPXP.d 


# Each subdirectory must supply rules for building sources it contributes
Kernel/%.o: ../Kernel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

