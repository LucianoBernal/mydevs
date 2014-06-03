################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bibliotecas_Comunes/parser/metadata_program.c \
../Bibliotecas_Comunes/parser/parser.c 

OBJS += \
./Bibliotecas_Comunes/parser/metadata_program.o \
./Bibliotecas_Comunes/parser/parser.o 

C_DEPS += \
./Bibliotecas_Comunes/parser/metadata_program.d \
./Bibliotecas_Comunes/parser/parser.d 


# Each subdirectory must supply rules for building sources it contributes
Bibliotecas_Comunes/parser/%.o: ../Bibliotecas_Comunes/parser/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


