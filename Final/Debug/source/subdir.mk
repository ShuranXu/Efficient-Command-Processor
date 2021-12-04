################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/cbfifo.c \
../source/cbfifo_test.c \
../source/cmd_proc.c \
../source/handlers.c \
../source/huffman.c \
../source/main.c \
../source/mtb.c \
../source/semihost_hardfault.c \
../source/sysclock.c \
../source/uart.c 

OBJS += \
./source/cbfifo.o \
./source/cbfifo_test.o \
./source/cmd_proc.o \
./source/handlers.o \
./source/huffman.o \
./source/main.o \
./source/mtb.o \
./source/semihost_hardfault.o \
./source/sysclock.o \
./source/uart.o 

C_DEPS += \
./source/cbfifo.d \
./source/cbfifo_test.d \
./source/cmd_proc.d \
./source/handlers.d \
./source/huffman.d \
./source/main.d \
./source/mtb.d \
./source/semihost_hardfault.d \
./source/sysclock.d \
./source/uart.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__REDLIB__ -I"/home/shuran/CUBoulder/PES/PES-Final-Project/Final/source" -I"/home/shuran/CUBoulder/PES/PES-Final-Project/Final" -I"/home/shuran/CUBoulder/PES/PES-Final-Project/Final/startup" -I"/home/shuran/CUBoulder/PES/PES-Final-Project/Final/CMSIS" -O0 -fno-common -g3 -Wall $(GEN_OPTS__FLAG) -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


