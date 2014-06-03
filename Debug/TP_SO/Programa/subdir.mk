################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TP_SO/Programa/bibSockets.c \
../TP_SO/Programa/interprete.c 

OBJS += \
./TP_SO/Programa/bibSockets.o \
./TP_SO/Programa/interprete.o 

C_DEPS += \
./TP_SO/Programa/bibSockets.d \
./TP_SO/Programa/interprete.d 


# Each subdirectory must supply rules for building sources it contributes
TP_SO/Programa/%.o: ../TP_SO/Programa/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


