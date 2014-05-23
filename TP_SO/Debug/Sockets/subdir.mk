################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sockets/Server.c \
../Sockets/bibSockets.c 

OBJS += \
./Sockets/Server.o \
./Sockets/bibSockets.o 

C_DEPS += \
./Sockets/Server.d \
./Sockets/bibSockets.d 


# Each subdirectory must supply rules for building sources it contributes
Sockets/%.o: ../Sockets/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


