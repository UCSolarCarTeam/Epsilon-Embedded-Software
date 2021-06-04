#include "MemoryDebugTask.h"

void memoryDebugTask(void* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        // Get the minimum
        volatile size_t minimumHeapSize = xPortGetMinimumEverFreeHeapSize();

        // Get Stack sizes
        volatile size_t startupTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(startupTaskHandle);
        volatile size_t canRxInterruptParserTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(canRxInterruptParserTaskHandle);
        volatile size_t orionInterfaceTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(orionInterfaceTaskHandle);
        volatile size_t canTxGatekeeperTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(canTxGatekeeperTaskHandle);
        volatile size_t sendAuxStatusTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(sendAuxStatusTaskHandle);
        volatile size_t sendAuxTripTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(sendAuxTripTaskHandle);
        volatile size_t sendHeartbeatTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(sendHeartbeatTaskHandle);
        volatile size_t commonContactorGatekeeperTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(commonContactorGatekeeperTaskHandle);
        volatile size_t chargeContactorGatekeeperTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(chargeContactorGatekeeperTaskHandle);
        volatile size_t dischargeContactorGatekeeperTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(dischargeContactorGatekeeperTaskHandle);
        volatile size_t contactorStatusUpdateTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(contactorStatusUpdateTaskHandle);
        volatile size_t readAuxVoltageTaskRemainingStackSpace = uxTaskGetStackHighWaterMark2(readAuxVoltageTaskHandle);
        prevWakeTime += MEMORY_DEBUG_TASK_FREQ;
        osDelayUntil(prevWakeTime);
    }
}
