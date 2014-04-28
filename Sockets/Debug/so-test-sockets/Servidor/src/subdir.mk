################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../so-test-sockets/Servidor/src/Server.c 

OBJS += \
./so-test-sockets/Servidor/src/Server.o 

C_DEPS += \
./so-test-sockets/Servidor/src/Server.d 


# Each subdirectory must supply rules for building sources it contributes
so-test-sockets/Servidor/src/%.o: ../so-test-sockets/Servidor/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


