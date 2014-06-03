################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UMV/consola.c \
../UMV/funcionesaux.c 

OBJS += \
./UMV/consola.o \
./UMV/funcionesaux.o 

C_DEPS += \
./UMV/consola.d \
./UMV/funcionesaux.d 


# Each subdirectory must supply rules for building sources it contributes
UMV/%.o: ../UMV/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


