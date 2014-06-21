################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ClienteSerializacion.c \
../Serializacion.c \
../Server.c \
../ServerSerializacion.c \
../bibSockets.c \
../bibSockets2.c \
../comenzandoConSelect.c \
../echoClient.c 

OBJS += \
./ClienteSerializacion.o \
./Serializacion.o \
./Server.o \
./ServerSerializacion.o \
./bibSockets.o \
./bibSockets2.o \
./comenzandoConSelect.o \
./echoClient.o 

C_DEPS += \
./ClienteSerializacion.d \
./Serializacion.d \
./Server.d \
./ServerSerializacion.d \
./bibSockets.d \
./bibSockets2.d \
./comenzandoConSelect.d \
./echoClient.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


