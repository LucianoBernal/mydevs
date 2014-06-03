################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TP_SO/Sockets/Serializacion.c \
../TP_SO/Sockets/Server.c \
../TP_SO/Sockets/bibSockets.c 

OBJS += \
./TP_SO/Sockets/Serializacion.o \
./TP_SO/Sockets/Server.o \
./TP_SO/Sockets/bibSockets.o 

C_DEPS += \
./TP_SO/Sockets/Serializacion.d \
./TP_SO/Sockets/Server.d \
./TP_SO/Sockets/bibSockets.d 


# Each subdirectory must supply rules for building sources it contributes
TP_SO/Sockets/%.o: ../TP_SO/Sockets/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


