################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sockets/ClienteSerializacion.c \
../sockets/Serializacion.c \
../sockets/Server.c \
../sockets/ServerSerializacion.c \
../sockets/bibSockets.c \
../sockets/bibSockets2.c \
../sockets/comenzandoConSelect.c \
../sockets/echoClient.c 

OBJS += \
./sockets/ClienteSerializacion.o \
./sockets/Serializacion.o \
./sockets/Server.o \
./sockets/ServerSerializacion.o \
./sockets/bibSockets.o \
./sockets/bibSockets2.o \
./sockets/comenzandoConSelect.o \
./sockets/echoClient.o 

C_DEPS += \
./sockets/ClienteSerializacion.d \
./sockets/Serializacion.d \
./sockets/Server.d \
./sockets/ServerSerializacion.d \
./sockets/bibSockets.d \
./sockets/bibSockets2.d \
./sockets/comenzandoConSelect.d \
./sockets/echoClient.d 


# Each subdirectory must supply rules for building sources it contributes
sockets/%.o: ../sockets/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


