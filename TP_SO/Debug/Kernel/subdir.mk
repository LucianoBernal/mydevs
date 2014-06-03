################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Kernel/Kernel.c \
../Kernel/PLP.c \
../Kernel/abrirPXP.c 

OBJS += \
./Kernel/Kernel.o \
./Kernel/PLP.o \
./Kernel/abrirPXP.o 

C_DEPS += \
./Kernel/Kernel.d \
./Kernel/PLP.d \
./Kernel/abrirPXP.d 


# Each subdirectory must supply rules for building sources it contributes
Kernel/%.o: ../Kernel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


