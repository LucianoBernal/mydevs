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
../dictionary.c \
../list.c \
../queue.c 

OBJS += \
./ClienteSerializacion.o \
./Serializacion.o \
./Server.o \
./ServerSerializacion.o \
./bibSockets.o \
./dictionary.o \
./list.o \
./queue.o 

C_DEPS += \
./ClienteSerializacion.d \
./Serializacion.d \
./Server.d \
./ServerSerializacion.d \
./bibSockets.d \
./dictionary.d \
./list.d \
./queue.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


