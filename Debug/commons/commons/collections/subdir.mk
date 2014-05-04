################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../commons/commons/collections/dictionary.c \
../commons/commons/collections/list.c \
../commons/commons/collections/queue.c 

OBJS += \
./commons/commons/collections/dictionary.o \
./commons/commons/collections/list.o \
./commons/commons/collections/queue.o 

C_DEPS += \
./commons/commons/collections/dictionary.d \
./commons/commons/collections/list.d \
./commons/commons/collections/queue.d 


# Each subdirectory must supply rules for building sources it contributes
commons/commons/collections/%.o: ../commons/commons/collections/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -pthread -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


