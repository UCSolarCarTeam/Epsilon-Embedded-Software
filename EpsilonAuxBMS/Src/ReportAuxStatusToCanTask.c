#include "ReportAuxStatusToCanTask.h"

// Refer to communication protocol
static const uint32_t AUX_STATUS_FREQ  = 100; // 10Hz
static const uint32_t AUX_STATUS_STDID  = 0x721U;

void reportAuxStatusToCanTask(void const* arg)
{
    // For concurrency with sendHeartbeatTask()
    osMutexId* canHandleMutex = (osMutexId*) arg;
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_STATUS_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        // Set CAN message address
        hcan1.pTxMsg->StdId = AUX_STATUS_STDID;
        // Set Data
        hcan1.pTxMsg->Data[0] = 0;
        hcan1.pTxMsg->Data[0] += auxStatus.commonContactorState * 0x1;
        hcan1.pTxMsg->Data[0] += auxStatus.chargeContactorState * 0x2;
        hcan1.pTxMsg->Data[0] += auxStatus.dischargeContactorState * 0x4;
        hcan1.pTxMsg->Data[0] |= auxStatus.auxVoltage * 0x8;
        // Initialize to 0 so there's no garbage in [2] to [7]
        hcan1.pTxMsg->Data[1] = 0;
        hcan1.pTxMsg->Data[1] += auxStatus.strobeBmsLight * 0x1;
        hcan1.pTxMsg->Data[1] += auxStatus.allowCharge * 0x2;
        hcan1.pTxMsg->Data[1] += auxStatus.contactorError * 0x4;

        // Send CAN message
        if (HAL_CAN_Transmit_IT(&hcan1) == HAL_OK)
            // Toggle blue LED for every CAN message sent
        {
            HAL_GPIO_TogglePin(BLU_LED_GPIO_Port, BLU_LED_Pin);
        }

        osMutexRelease(canHandleMutex);
    }
}
