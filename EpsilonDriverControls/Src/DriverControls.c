#include "cmsis_os.h"

#include "DriverControls.h"

void sendHeartbeatTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, HEARTBEAT_CAN_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        hcan2.pTxMsg->StdId = HEARTBEAT_STDID;
        hcan2.pTxMsg->DLC = 1;
        hcan2.pTxMsg->Data[0] = 1;
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendLightsTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_CAN_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        hcan2.pTxMsg->StdId = LIGHTS_STDID;
        hcan2.pTxMsg->DLC = 1;
        hcan2.pTxMsg->Data[0] = 0;
        hcan2.pTxMsg->Data[0] += 0x01 * !HAL_GPIO_ReadPin(HEADLIGHTS_OFF_GPIO_Port, HEADLIGHTS_OFF_Pin);
        hcan2.pTxMsg->Data[0] += 0x02 * !HAL_GPIO_ReadPin(HEADLIGHTS_LOW_GPIO_Port, HEADLIGHTS_LOW_Pin);
        hcan2.pTxMsg->Data[0] += 0x04 * !HAL_GPIO_ReadPin(HEADLIGHTS_HIGH_GPIO_Port, HEADLIGHTS_HIGH_Pin);
        hcan2.pTxMsg->Data[0] += 0x08 * !HAL_GPIO_ReadPin(RSIGNAL_GPIO_Port, RSIGNAL_Pin);
        hcan2.pTxMsg->Data[0] += 0x10 * !HAL_GPIO_ReadPin(LSIGNAL_GPIO_Port, LSIGNAL_Pin);
        hcan2.pTxMsg->Data[0] += 0x20 * !HAL_GPIO_ReadPin(HAZARDS_GPIO_Port, HAZARDS_Pin);
        hcan2.pTxMsg->Data[0] += 0x40 * !HAL_GPIO_ReadPin(INTERIOR_GPIO_Port, INTERIOR_Pin);
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendMusicTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, MUSIC_CAN_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        hcan2.pTxMsg->StdId = MUSIC_STDID;
        hcan2.pTxMsg->DLC = 1;
        hcan2.pTxMsg->Data[0] = 0;
        hcan2.pTxMsg->Data[0] += 0x01 * !HAL_GPIO_ReadPin(AUX_GPIO_Port, AUX_Pin);
        hcan2.pTxMsg->Data[0] += 0x02 * !HAL_GPIO_ReadPin(VOLUME_UP_GPIO_Port, VOLUME_UP_Pin);
        hcan2.pTxMsg->Data[0] += 0x03 * !HAL_GPIO_ReadPin(VOLUME_DOWN_GPIO_Port, VOLUME_DOWN_Pin);
        hcan2.pTxMsg->Data[0] += 0x04 * !HAL_GPIO_ReadPin(NEXT_SONG_GPIO_Port, NEXT_SONG_Pin);
        hcan2.pTxMsg->Data[0] += 0x10 * !HAL_GPIO_ReadPin(LAST_SONG_GPIO_Port, LAST_SONG_Pin);
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendDriverTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();
    uint32_t regen_ADCValue = 0;
    uint32_t accel_ADCValue = 0;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, DRIVER_CAN_FREQ);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        hcan2.pTxMsg->StdId = DRIVER_STDID;
        hcan2.pTxMsg->DLC = 4;
        hcan2.pTxMsg->Data[0] = 0;
        hcan2.pTxMsg->Data[1] = 0;
        hcan2.pTxMsg->Data[2] = 0;
        hcan2.pTxMsg->Data[3] = 0;

        if (HAL_ADC_PollForConversion(&hadc1, 1000000) == HAL_OK)
        {
            regen_ADCValue = HAL_ADC_GetValue(&hadc1);
        }
        hcan2.pTxMsg->Data[0] += (regen_ADCValue & 0x000000ffUL);
        hcan2.pTxMsg->Data[1] += (regen_ADCValue & 0x00000f00UL) >> 8; // Use first 4 bits

        if (HAL_ADC_PollForConversion(&hadc2, 1000000) == HAL_OK)
        {
            accel_ADCValue = HAL_ADC_GetValue(&hadc2);
        }
        hcan2.pTxMsg->Data[1] += (accel_ADCValue & 0x0000000fUL) << 4; // Use last 4 bits
        hcan2.pTxMsg->Data[2] += (accel_ADCValue & 0x00000ff0UL) >> 4;

        hcan2.pTxMsg->Data[3] += 0x01 * !HAL_GPIO_ReadPin(BRAKES_GPIO_Port, BRAKES_Pin);
        hcan2.pTxMsg->Data[3] += 0x02 * !HAL_GPIO_ReadPin(FORWARD_GPIO_Port, FORWARD_Pin);
        hcan2.pTxMsg->Data[3] += 0x04 * !HAL_GPIO_ReadPin(REVERSE_GPIO_Port, REVERSE_Pin);
        hcan2.pTxMsg->Data[3] += 0x08 * !HAL_GPIO_ReadPin(PUSH_TO_TALK_GPIO_Port, PUSH_TO_TALK_Pin);
        hcan2.pTxMsg->Data[3] += 0x10 * !HAL_GPIO_ReadPin(HORN_GPIO_Port, HORN_Pin);
        hcan2.pTxMsg->Data[3] += 0x20 * !HAL_GPIO_ReadPin(RESET_GPIO_Port, RESET_Pin);
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}
