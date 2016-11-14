#include "cmsis_os.h"

#include "DriverControls.h"

void sendHeartbeatTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();
    for(;;)
    {
        osDelayUntil(&prevWakeTime, HEARTBEAT_CAN_FREQ);
        if(osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }
        hcan2.pTxMsg->StdId = HEARTBEAT_STDID;
        hcan2.pTxMsg->Data[0] = 1;
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendLightsTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();
    for(;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_CAN_FREQ);
        if(osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }
        hcan2.pTxMsg->StdId = LIGHTS_STDID;
        hcan2.pTxMsg->Data[0] = 1;
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendMusicTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();
    for(;;)
    {
        osDelayUntil(&prevWakeTime, MUSIC_CAN_FREQ);
        if(osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }
        hcan2.pTxMsg->StdId = MUSIC_STDID;
        hcan2.pTxMsg->Data[0] = 1;
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}

void sendDriverTask(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();
    for(;;)
    {
        osDelayUntil(&prevWakeTime, DRIVER_CAN_FREQ);
        if(osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }
        hcan2.pTxMsg->StdId = DRIVER_STDID;
        hcan2.pTxMsg->Data[0] = 1;
        HAL_CAN_Transmit_IT(&hcan2);
        osMutexRelease(canHandleMutex);
    }
}
