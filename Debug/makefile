################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include commons/commons/collections/subdir.mk
-include commons/commons/subdir.mk
-include commons/Debug/commons/collections/subdir.mk
-include commons/Debug/commons/subdir.mk
-include UMV/Debug/subdir.mk
-include UMV/subdir.mk
-include Sockets/Debug/subdir.mk
-include Sockets/subdir.mk
-include Programa/Debug/subdir.mk
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
all: prueba2

# Tool invocations
prueba2: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "prueba2" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) prueba2
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
