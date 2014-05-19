################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include commons/commons/collections/subdir.mk
-include commons/commons/subdir.mk
-include UMV/subdir.mk
-include Sockets/subdir.mk
-include Programa/subdir.mk
-include Kernel/subdir.mk
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
all: SO

dependents:
	-cd /home/utnso/workspace/Bibliotecas_Comunes/Debug && $(MAKE) all

# Tool invocations
SO: $(OBJS) $(USER_OBJS) /home/utnso/workspace/Bibliotecas_Comunes/Debug/libBibliotecas_Comunes.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/Bibliotecas_Comunes/Debug" -o "SO" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) SO
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/workspace/Bibliotecas_Comunes/Debug/libBibliotecas_Comunes.so:

-include ../makefile.targets
