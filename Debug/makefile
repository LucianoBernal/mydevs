################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include TP_SO/UMV/Debug/subdir.mk
-include TP_SO/UMV/subdir.mk
-include TP_SO/Sockets/Debug/subdir.mk
-include TP_SO/Sockets/subdir.mk
-include TP_SO/Programa/Debug/subdir.mk
-include TP_SO/Programa/subdir.mk
-include TP_SO/Kernel/subdir.mk
-include TP_SO/Debug/UMV/subdir.mk
-include Bibliotecas_Comunes/parser/subdir.mk
-include Bibliotecas_Comunes/commons/collections/subdir.mk
-include Bibliotecas_Comunes/commons/subdir.mk
-include Bibliotecas_Comunes/Debug/parser/subdir.mk
-include Bibliotecas_Comunes/Debug/commons/collections/subdir.mk
-include Bibliotecas_Comunes/Debug/commons/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: prueba2

dependents:
	-cd /home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes/Debug && $(MAKE) all

# Tool invocations
prueba2: $(OBJS) $(USER_OBJS) /home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes/Debug/libBibliotecas_Comunes.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes/Debug" -o "prueba2" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) prueba2
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/tp-2014-1c-skynet/Bibliotecas_Comunes/Debug/libBibliotecas_Comunes.so:

-include ../makefile.targets
