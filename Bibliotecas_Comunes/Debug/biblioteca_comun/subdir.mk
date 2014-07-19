################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../biblioteca_comun/ClienteSerializacion.c \
../biblioteca_comun/Serializacion.c \
../biblioteca_comun/bibliotecaSockets.c 

OBJS += \
./biblioteca_comun/ClienteSerializacion.o \
./biblioteca_comun/Serializacion.o \
./biblioteca_comun/bibliotecaSockets.o 

C_DEPS += \
./biblioteca_comun/ClienteSerializacion.d \
./biblioteca_comun/Serializacion.d \
./biblioteca_comun/bibliotecaSockets.d 


# Each subdirectory must supply rules for building sources it contributes
biblioteca_comun/%.o: ../biblioteca_comun/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


