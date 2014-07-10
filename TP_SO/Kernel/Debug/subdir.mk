################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Kernel.c \
../PCP.c \
../PLP.c \
../multiplexorCPUs.c \
../multiplexorScripts.c 

OBJS += \
./Kernel.o \
./PCP.o \
./PLP.o \
./multiplexorCPUs.o \
./multiplexorScripts.o 

C_DEPS += \
./Kernel.d \
./PCP.d \
./PLP.d \
./multiplexorCPUs.d \
./multiplexorScripts.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes" -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


