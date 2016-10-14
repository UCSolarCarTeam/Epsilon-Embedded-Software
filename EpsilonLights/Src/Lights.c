#include "cmsis_os.h"

#include "Lights.h"

extern CAN_HandleTypeDef hcan2; // main.c

void updateLights(void const* arg)
{
    unsigned char* inputs = (unsigned char*) arg;
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, 10);
        // requests->interior = true;
    }
}

void reportLightsToCan(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_STATUS_FREQ);
    
        if (osMutexWait(canHandleMutex, 0) != osOK) {
            Error_Handler();
        }
        
        hcan2.pTxMsg->StdId = LIGHTS_STATUS_STDID;
        hcan2.pTxMsg->Data[0] = 0xAD;
        HAL_CAN_Transmit_IT(&hcan2);
    
        if(osMutexRelease(canHandleMutex) != osOK)
        {
            Error_Handler();
        }
    }
}

void sendHeartbeat(void const* arg)
{
    osMutexId* canHandleMutex = (osMutexId*) arg;
    uint32_t prevWakeTime = osKernelSysTick();

    for(;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_HEARTBEAT_FREQ);
    
        if (osMutexWait(canHandleMutex, 0) != osOK) {
            Error_Handler();
        }
        
        hcan2.pTxMsg->StdId = LIGHTS_HEARTBEAT_STDID;
        hcan2.pTxMsg->Data[0] = 1;
        HAL_CAN_Transmit_IT(&hcan2);   
    
        if(osMutexRelease(canHandleMutex) != osOK)
        {
            Error_Handler();
        }
    }
}
