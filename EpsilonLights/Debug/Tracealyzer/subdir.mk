################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Tracealyzer/trcAssert.c \
../Tracealyzer/trcCounter.c \
../Tracealyzer/trcDiagnostics.c \
../Tracealyzer/trcEntryTable.c \
../Tracealyzer/trcError.c \
../Tracealyzer/trcEvent.c \
../Tracealyzer/trcEventBuffer.c \
../Tracealyzer/trcExtension.c \
../Tracealyzer/trcHardwarePort.c \
../Tracealyzer/trcHeap.c \
../Tracealyzer/trcISR.c \
../Tracealyzer/trcInternalEventBuffer.c \
../Tracealyzer/trcInterval.c \
../Tracealyzer/trcKernelPort.c \
../Tracealyzer/trcMultiCoreEventBuffer.c \
../Tracealyzer/trcObject.c \
../Tracealyzer/trcPrint.c \
../Tracealyzer/trcSnapshotRecorder.c \
../Tracealyzer/trcStackMonitor.c \
../Tracealyzer/trcStateMachine.c \
../Tracealyzer/trcStaticBuffer.c \
../Tracealyzer/trcStreamingRecorder.c \
../Tracealyzer/trcString.c \
../Tracealyzer/trcTask.c \
../Tracealyzer/trcTimestamp.c 

OBJS += \
./Tracealyzer/trcAssert.o \
./Tracealyzer/trcCounter.o \
./Tracealyzer/trcDiagnostics.o \
./Tracealyzer/trcEntryTable.o \
./Tracealyzer/trcError.o \
./Tracealyzer/trcEvent.o \
./Tracealyzer/trcEventBuffer.o \
./Tracealyzer/trcExtension.o \
./Tracealyzer/trcHardwarePort.o \
./Tracealyzer/trcHeap.o \
./Tracealyzer/trcISR.o \
./Tracealyzer/trcInternalEventBuffer.o \
./Tracealyzer/trcInterval.o \
./Tracealyzer/trcKernelPort.o \
./Tracealyzer/trcMultiCoreEventBuffer.o \
./Tracealyzer/trcObject.o \
./Tracealyzer/trcPrint.o \
./Tracealyzer/trcSnapshotRecorder.o \
./Tracealyzer/trcStackMonitor.o \
./Tracealyzer/trcStateMachine.o \
./Tracealyzer/trcStaticBuffer.o \
./Tracealyzer/trcStreamingRecorder.o \
./Tracealyzer/trcString.o \
./Tracealyzer/trcTask.o \
./Tracealyzer/trcTimestamp.o 

C_DEPS += \
./Tracealyzer/trcAssert.d \
./Tracealyzer/trcCounter.d \
./Tracealyzer/trcDiagnostics.d \
./Tracealyzer/trcEntryTable.d \
./Tracealyzer/trcError.d \
./Tracealyzer/trcEvent.d \
./Tracealyzer/trcEventBuffer.d \
./Tracealyzer/trcExtension.d \
./Tracealyzer/trcHardwarePort.d \
./Tracealyzer/trcHeap.d \
./Tracealyzer/trcISR.d \
./Tracealyzer/trcInternalEventBuffer.d \
./Tracealyzer/trcInterval.d \
./Tracealyzer/trcKernelPort.d \
./Tracealyzer/trcMultiCoreEventBuffer.d \
./Tracealyzer/trcObject.d \
./Tracealyzer/trcPrint.d \
./Tracealyzer/trcSnapshotRecorder.d \
./Tracealyzer/trcStackMonitor.d \
./Tracealyzer/trcStateMachine.d \
./Tracealyzer/trcStaticBuffer.d \
./Tracealyzer/trcStreamingRecorder.d \
./Tracealyzer/trcString.d \
./Tracealyzer/trcTask.d \
./Tracealyzer/trcTimestamp.d 


# Each subdirectory must supply rules for building sources it contributes
Tracealyzer/%.o Tracealyzer/%.su: ../Tracealyzer/%.c Tracealyzer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Inc -I"C:/Users/marce/Desktop/Epsilon-Embedded-Software/EpsilonLights/Tracealyzer" -I"C:/Users/marce/Desktop/Epsilon-Embedded-Software/EpsilonLights/Tracealyzer/config" -I"C:/Users/marce/Desktop/Epsilon-Embedded-Software/EpsilonLights/Tracealyzer/include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Inc -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Tracealyzer

clean-Tracealyzer:
	-$(RM) ./Tracealyzer/trcAssert.d ./Tracealyzer/trcAssert.o ./Tracealyzer/trcAssert.su ./Tracealyzer/trcCounter.d ./Tracealyzer/trcCounter.o ./Tracealyzer/trcCounter.su ./Tracealyzer/trcDiagnostics.d ./Tracealyzer/trcDiagnostics.o ./Tracealyzer/trcDiagnostics.su ./Tracealyzer/trcEntryTable.d ./Tracealyzer/trcEntryTable.o ./Tracealyzer/trcEntryTable.su ./Tracealyzer/trcError.d ./Tracealyzer/trcError.o ./Tracealyzer/trcError.su ./Tracealyzer/trcEvent.d ./Tracealyzer/trcEvent.o ./Tracealyzer/trcEvent.su ./Tracealyzer/trcEventBuffer.d ./Tracealyzer/trcEventBuffer.o ./Tracealyzer/trcEventBuffer.su ./Tracealyzer/trcExtension.d ./Tracealyzer/trcExtension.o ./Tracealyzer/trcExtension.su ./Tracealyzer/trcHardwarePort.d ./Tracealyzer/trcHardwarePort.o ./Tracealyzer/trcHardwarePort.su ./Tracealyzer/trcHeap.d ./Tracealyzer/trcHeap.o ./Tracealyzer/trcHeap.su ./Tracealyzer/trcISR.d ./Tracealyzer/trcISR.o ./Tracealyzer/trcISR.su ./Tracealyzer/trcInternalEventBuffer.d ./Tracealyzer/trcInternalEventBuffer.o ./Tracealyzer/trcInternalEventBuffer.su ./Tracealyzer/trcInterval.d ./Tracealyzer/trcInterval.o ./Tracealyzer/trcInterval.su ./Tracealyzer/trcKernelPort.d ./Tracealyzer/trcKernelPort.o ./Tracealyzer/trcKernelPort.su ./Tracealyzer/trcMultiCoreEventBuffer.d ./Tracealyzer/trcMultiCoreEventBuffer.o ./Tracealyzer/trcMultiCoreEventBuffer.su ./Tracealyzer/trcObject.d ./Tracealyzer/trcObject.o ./Tracealyzer/trcObject.su ./Tracealyzer/trcPrint.d ./Tracealyzer/trcPrint.o ./Tracealyzer/trcPrint.su ./Tracealyzer/trcSnapshotRecorder.d ./Tracealyzer/trcSnapshotRecorder.o ./Tracealyzer/trcSnapshotRecorder.su ./Tracealyzer/trcStackMonitor.d ./Tracealyzer/trcStackMonitor.o ./Tracealyzer/trcStackMonitor.su ./Tracealyzer/trcStateMachine.d ./Tracealyzer/trcStateMachine.o ./Tracealyzer/trcStateMachine.su ./Tracealyzer/trcStaticBuffer.d ./Tracealyzer/trcStaticBuffer.o ./Tracealyzer/trcStaticBuffer.su ./Tracealyzer/trcStreamingRecorder.d ./Tracealyzer/trcStreamingRecorder.o ./Tracealyzer/trcStreamingRecorder.su ./Tracealyzer/trcString.d ./Tracealyzer/trcString.o ./Tracealyzer/trcString.su ./Tracealyzer/trcTask.d ./Tracealyzer/trcTask.o ./Tracealyzer/trcTask.su ./Tracealyzer/trcTimestamp.d ./Tracealyzer/trcTimestamp.o ./Tracealyzer/trcTimestamp.su

.PHONY: clean-Tracealyzer

