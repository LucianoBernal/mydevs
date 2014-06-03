################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TP_SO/UMV/consola.c \
../TP_SO/UMV/funcionesaux.c 

OBJS += \
./TP_SO/UMV/consola.o \
./TP_SO/UMV/funcionesaux.o 

C_DEPS += \
./TP_SO/UMV/consola.d \
./TP_SO/UMV/funcionesaux.d 


# Each subdirectory must supply rules for building sources it contributes
TP_SO/UMV/%.o: ../TP_SO/UMV/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


