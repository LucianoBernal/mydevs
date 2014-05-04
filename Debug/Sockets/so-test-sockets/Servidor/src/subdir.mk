################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sockets/so-test-sockets/Servidor/src/Server.c 

OBJS += \
./Sockets/so-test-sockets/Servidor/src/Server.o 

C_DEPS += \
./Sockets/so-test-sockets/Servidor/src/Server.d 


# Each subdirectory must supply rules for building sources it contributes
Sockets/so-test-sockets/Servidor/src/%.o: ../Sockets/so-test-sockets/Servidor/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


