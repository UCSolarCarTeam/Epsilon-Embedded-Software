#include "ReportHeartbeatToCanTask.h"

// Refer to communications protocol
static const uint32_t AUX_HEARBEAT_FREQ = 1000; // 1Hz
static const uint32_t AUX_HEARTBEAT_STDID = 0x720U;

void reportHeartbeatToCanTask(void const* arg)
{
    // For concurrency with reportAuxToCanTask()
    osMutexId* canHandleMutex = (osMutexId*) arg;
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_HEARBEAT_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        hcan1.pTxMsg->DLC = 1;
        // Set CAN message address
        hcan1.pTxMsg->StdId = AUX_HEARTBEAT_STDID;
        hcan1.pTxMsg->DLC = 1;
        // Always 1
        hcan1.pTxMsg->Data[0] = 1;

        // Send CAN message
        if (HAL_CAN_Transmit_IT(&hcan1) == HAL_OK)
            // Toggle Red LED for every heartbeat sent
        {
            HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
        }

        osMutexRelease(canHandleMutex);
    }
}
