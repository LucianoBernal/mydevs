################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sockets/Serializacion.c \
../Sockets/Server.c \
../Sockets/bibSockets.c 

OBJS += \
./Sockets/Serializacion.o \
./Sockets/Server.o \
./Sockets/bibSockets.o 

C_DEPS += \
./Sockets/Serializacion.d \
./Sockets/Server.d \
./Sockets/bibSockets.d 


# Each subdirectory must supply rules for building sources it contributes
Sockets/%.o: ../Sockets/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


