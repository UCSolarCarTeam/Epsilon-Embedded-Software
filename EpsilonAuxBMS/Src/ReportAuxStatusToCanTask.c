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

        auxStatus.commonContactorState = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);
        auxStatus.chargeContactorState = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
        auxStatus.dischargeContactorState = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        // Set CAN message address
        hcan1.pTxMsg->StdId = AUX_STATUS_STDID;
        hcan1.pTxMsg->DLC = 2;
        // Set Data
        hcan1.pTxMsg->Data[0] =
            auxStatus.commonContactorState |
            auxStatus.chargeContactorState << 1 |
            auxStatus.dischargeContactorState << 2 |
            auxStatus.auxVoltage << 3;

        hcan1.pTxMsg->Data[1] =
            auxStatus.strobeBmsLight |
            auxStatus.allowCharge << 1 |
            auxStatus.contactorError << 2 |
            auxStatus.highVoltageEnableState << 3;

        // Send CAN message
        if (HAL_CAN_Transmit_IT(&hcan1) == HAL_OK)
            // Toggle blue LED for every CAN message sent
        {
            HAL_GPIO_TogglePin(BLU_LED_GPIO_Port, BLU_LED_Pin);
        }

        osMutexRelease(canHandleMutex);
    }
}
