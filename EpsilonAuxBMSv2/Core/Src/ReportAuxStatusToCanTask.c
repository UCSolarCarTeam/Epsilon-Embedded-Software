#include "ReportAuxStatusToCanTask.h"

// Refer to communication protocol
static const uint32_t AUX_STATUS_FREQ  = 100; // 10Hz
static const uint32_t AUX_STATUS_STDID  = 0x721U;

void reportAuxStatusToCanTask(void const* arg)
{
    // For concurrency with sendHeartbeatTask()
    osMutexId_t* canHandleMutex = (osMutexId_t*) arg;
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(prevWakeTime + AUX_STATUS_FREQ);

        auxStatus.commonContactorState = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);
        auxStatus.chargeContactorState = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
        auxStatus.dischargeContactorState = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);
        auxStatus.highVoltageEnableState = HAL_GPIO_ReadPin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin);

        if (osMutexAcquire(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) // Any available Tx mailboxes?
        {
            uint8_t data[2];
            uint32_t mailbox; // Might find a use for mailbox parameter later
            // (e.g. to implement TxMailbox?CompleteCallback to check a transmission actually completed)
            // Set CAN Tx header info
            canTxHeader.StdId = AUX_STATUS_STDID;
            canTxHeader.DLC = 2;

            // Set Data
            data[0] =
                auxStatus.commonContactorState |
                auxStatus.chargeContactorState << 1 |
                auxStatus.dischargeContactorState << 2 |
                auxStatus.auxVoltage << 3;

            data[1] =
                auxStatus.strobeBmsLight |
                auxStatus.allowCharge << 1 |
                auxStatus.contactorError << 2 |
                auxStatus.highVoltageEnableState << 3;

            // Send CAN message
            if (HAL_CAN_AddTxMessage(&hcan1, &canTxHeader, data, &mailbox) == HAL_OK)
                // Toggle blue LED for every CAN message sent
            {
                HAL_GPIO_TogglePin(BLU_LED_GPIO_Port, BLU_LED_Pin);
            }
        }

        osMutexRelease(canHandleMutex);
    }
}
