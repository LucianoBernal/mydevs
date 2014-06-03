################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TP_SO/Kernel/Kernel.c \
../TP_SO/Kernel/PLP.c \
../TP_SO/Kernel/abrirPXP.c 

OBJS += \
./TP_SO/Kernel/Kernel.o \
./TP_SO/Kernel/PLP.o \
./TP_SO/Kernel/abrirPXP.o 

C_DEPS += \
./TP_SO/Kernel/Kernel.d \
./TP_SO/Kernel/PLP.d \
./TP_SO/Kernel/abrirPXP.d 


# Each subdirectory must supply rules for building sources it contributes
TP_SO/Kernel/%.o: ../TP_SO/Kernel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


