################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../atencionInterna.c \
../consola.c \
../funcionesaux.c \
../principal.c \
../pruebasUMV.c \
../umv.c 

OBJS += \
./atencionInterna.o \
./consola.o \
./funcionesaux.o \
./principal.o \
./pruebasUMV.o \
./umv.o 

C_DEPS += \
./atencionInterna.d \
./consola.d \
./funcionesaux.d \
./principal.d \
./pruebasUMV.d \
./umv.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


