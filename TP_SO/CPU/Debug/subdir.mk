################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../EsqueletoCPU.c \
../Serializacion.c \
../bibSockets2.c \
../funcionesParser.c \
../primerIntentoCPU.c 

OBJS += \
./EsqueletoCPU.o \
./Serializacion.o \
./bibSockets2.o \
./funcionesParser.o \
./primerIntentoCPU.o 

C_DEPS += \
./EsqueletoCPU.d \
./Serializacion.d \
./bibSockets2.d \
./funcionesParser.d \
./primerIntentoCPU.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


