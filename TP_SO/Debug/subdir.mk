################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../funcionesExtra.c \
../prueba.c \
../pruebapunteros.c 

OBJS += \
./funcionesExtra.o \
./prueba.o \
./pruebapunteros.o 

C_DEPS += \
./funcionesExtra.d \
./prueba.d \
./pruebapunteros.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


