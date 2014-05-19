################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../commons/commons/bitarray.c \
../commons/commons/config.c \
../commons/commons/error.c \
../commons/commons/log.c \
../commons/commons/process.c \
../commons/commons/string.c \
../commons/commons/temporal.c \
../commons/commons/txt.c 

OBJS += \
./commons/commons/bitarray.o \
./commons/commons/config.o \
./commons/commons/error.o \
./commons/commons/log.o \
./commons/commons/process.o \
./commons/commons/string.o \
./commons/commons/temporal.o \
./commons/commons/txt.o 

C_DEPS += \
./commons/commons/bitarray.d \
./commons/commons/config.d \
./commons/commons/error.d \
./commons/commons/log.d \
./commons/commons/process.d \
./commons/commons/string.d \
./commons/commons/temporal.d \
./commons/commons/txt.d 


# Each subdirectory must supply rules for building sources it contributes
commons/commons/%.o: ../commons/commons/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/Bibliotecas_Comunes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


