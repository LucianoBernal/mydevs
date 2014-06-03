################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bibliotecas_Comunes/commons/bitarray.c \
../Bibliotecas_Comunes/commons/config.c \
../Bibliotecas_Comunes/commons/error.c \
../Bibliotecas_Comunes/commons/log.c \
../Bibliotecas_Comunes/commons/process.c \
../Bibliotecas_Comunes/commons/string.c \
../Bibliotecas_Comunes/commons/temporal.c \
../Bibliotecas_Comunes/commons/txt.c 

OBJS += \
./Bibliotecas_Comunes/commons/bitarray.o \
./Bibliotecas_Comunes/commons/config.o \
./Bibliotecas_Comunes/commons/error.o \
./Bibliotecas_Comunes/commons/log.o \
./Bibliotecas_Comunes/commons/process.o \
./Bibliotecas_Comunes/commons/string.o \
./Bibliotecas_Comunes/commons/temporal.o \
./Bibliotecas_Comunes/commons/txt.o 

C_DEPS += \
./Bibliotecas_Comunes/commons/bitarray.d \
./Bibliotecas_Comunes/commons/config.d \
./Bibliotecas_Comunes/commons/error.d \
./Bibliotecas_Comunes/commons/log.d \
./Bibliotecas_Comunes/commons/process.d \
./Bibliotecas_Comunes/commons/string.d \
./Bibliotecas_Comunes/commons/temporal.d \
./Bibliotecas_Comunes/commons/txt.d 


# Each subdirectory must supply rules for building sources it contributes
Bibliotecas_Comunes/commons/%.o: ../Bibliotecas_Comunes/commons/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


