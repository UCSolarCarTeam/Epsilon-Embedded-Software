#include "cmsis_os.h"
#include "AuxBMS.h"

void sendHeartbeatTask(void const* arg)
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

        // Toggle green LED for every heartbeat sent
        HAL_GPIO_TogglePin(GRN_LED_GPIO_Port, GRN_LED_Pin);
        // Set CAN message address
        hcan1.pTxMsg->StdId = AUX_HEARTBEAT_STDID;
        // Always 1
        hcan1.pTxMsg->Data[0] = 1;
        // Send CAN message
        HAL_CAN_Transmit_IT(&hcan1);
        osMutexRelease(canHandleMutex);
    }
}

void reportAuxToCanTask(void const* arg)
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

        // Toggle blue LED for every CAN message sent
        HAL_GPIO_TogglePin(BLU_LED_GPIO_Port, BLU_LED_Pin);

        // Set CAN message address
        hcan1.pTxMsg->StdId = AUX_STATUS_STDID;
        // Set Data
        hcan1.pTxMsg->Data[0] = 0;
        hcan1.pTxMsg->Data[0] += auxStatus.commonContactorState * 0x1;
        hcan1.pTxMsg->Data[0] += auxStatus.chargeContactorState * 0x2;
        hcan1.pTxMsg->Data[0] += auxStatus.dischargeContactorState * 0x4;
        hcan1.pTxMsg->Data[0] |= (auxStatus.auxVoltage & 0x1F) * 0x8; // Use bottom 5 bits
        // Initialize to 0 so there's no garbage in [2] to [7]
        hcan1.pTxMsg->Data[1] = 0;
        hcan1.pTxMsg->Data[1] += auxStatus.strobeBmsLight * 0x1;
        hcan1.pTxMsg->Data[1] += auxStatus.allowCharge * 0x2;
        hcan1.pTxMsg->Data[1] += auxStatus.contactorError * 0x4;
        // Send CAN message
        HAL_CAN_Transmit_IT(&hcan1);
        osMutexRelease(canHandleMutex);
    }
}

// Reimplement weak definition in stm32f4xx_hal_can.c
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
  CanRxMsgTypeDef* msg = hcan->pRxMsg;

  // Toggle Green LED for every message received
  HAL_GPIO_TogglePin(GRN_LED_GPIO_Port, GRN_LED_Pin);

  if (msg->StdId == MAX_MIN_VOLTAGES_STDID)
  {
      orionBmsInputs[0] = (uint16_t)msg->Data[6]; // Max Cell voltage
      orionBmsInputs[1] = (uint16_t)msg->Data[4]; // Min Cell Voltage
  }

  __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FMP0);
  HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
}
