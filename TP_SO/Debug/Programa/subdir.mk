################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Programa/bibSockets.c \
../Programa/interprete.c 

OBJS += \
./Programa/bibSockets.o \
./Programa/interprete.o 

C_DEPS += \
./Programa/bibSockets.d \
./Programa/interprete.d 


# Each subdirectory must supply rules for building sources it contributes
Programa/%.o: ../Programa/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


