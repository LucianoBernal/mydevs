################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bibliotecas_Comunes/commons/collections/dictionary.c \
../Bibliotecas_Comunes/commons/collections/list.c \
../Bibliotecas_Comunes/commons/collections/queue.c 

OBJS += \
./Bibliotecas_Comunes/commons/collections/dictionary.o \
./Bibliotecas_Comunes/commons/collections/list.o \
./Bibliotecas_Comunes/commons/collections/queue.o 

C_DEPS += \
./Bibliotecas_Comunes/commons/collections/dictionary.d \
./Bibliotecas_Comunes/commons/collections/list.d \
./Bibliotecas_Comunes/commons/collections/queue.d 


# Each subdirectory must supply rules for building sources it contributes
Bibliotecas_Comunes/commons/collections/%.o: ../Bibliotecas_Comunes/commons/collections/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


