################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/github/ExpressWeb/src/Algorithm.cpp 

OBJS += \
./src/Algorithm.o 

CPP_DEPS += \
./src/Algorithm.d 


# Each subdirectory must supply rules for building sources it contributes
src/Algorithm.o: C:/github/ExpressWeb/src/Algorithm.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"C:\boost_1_67_0\include" -I"C:\github\ExpressWeb\include" -O0 -g3 -Wall -c -fmessage-length=0  -std=gnu++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


