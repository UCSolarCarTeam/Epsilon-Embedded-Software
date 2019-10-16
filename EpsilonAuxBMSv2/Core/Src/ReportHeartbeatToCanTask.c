#include "ReportHeartbeatToCanTask.h"

// Refer to communications protocol
static const uint32_t AUX_HEARBEAT_FREQ = 1000; // 1Hz
static const uint32_t AUX_HEARTBEAT_STDID = 0x720U;

void reportHeartbeatToCanTask(void const* arg)
{
    // For concurrency with reportAuxToCanTask()
    osMutexId_t* canHandleMutex = (osMutexId_t*) arg;
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(prevWakeTime + AUX_HEARBEAT_FREQ);

        if (osMutexAcquire(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) // Any available Tx mailboxes?
        {
            uint8_t data[1];
            uint32_t mailbox;
            // Set CAN message address
            canTxHeader.StdId = AUX_HEARTBEAT_STDID;
            canTxHeader.DLC = 1;
            // Always 1
            data[0] = 1;

            // Send CAN message
            if (HAL_CAN_AddTxMessage(&hcan1, &canTxHeader, data, &mailbox) == HAL_OK)
                // Toggle Red LED for every heartbeat sent
            {
                HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
            }
        }

        osMutexRelease(canHandleMutex);
    }
}
