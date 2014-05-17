################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../so-test-sockets/Cliente/src/Cliente.c 

OBJS += \
./so-test-sockets/Cliente/src/Cliente.o 

C_DEPS += \
./so-test-sockets/Cliente/src/Cliente.d 


# Each subdirectory must supply rules for building sources it contributes
so-test-sockets/Cliente/src/%.o: ../so-test-sockets/Cliente/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


