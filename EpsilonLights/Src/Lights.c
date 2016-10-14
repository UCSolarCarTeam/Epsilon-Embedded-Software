#include "cmsis_os.h"

#include "Lights.h"

extern CAN_HandleTypeDef hcan2; // main.c
extern uint8_t lightsInputs;
extern uint8_t batteryStatus;

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
        HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);    
    
        if (osMutexWait(canHandleMutex, 0) != osOK) {
            continue;
        }
        
        hcan2.pTxMsg->StdId = LIGHTS_STATUS_STDID;
        LightsStatus stat = {{
            HAL_GPIO_ReadPin(HLOW_GPIO_Port, HLOW_Pin),
            HAL_GPIO_ReadPin(HHIGH_GPIO_Port, HHIGH_Pin),
            HAL_GPIO_ReadPin(BRAKE_GPIO_Port, BRAKE_Pin),
            HAL_GPIO_ReadPin(LSIGNAL_GPIO_Port, LSIGNAL_Pin),
            HAL_GPIO_ReadPin(RSIGNAL_GPIO_Port, RSIGNAL_Pin),
            HAL_GPIO_ReadPin(ESTROBE_GPIO_Port, ESTROBE_Pin)
        }};
        hcan2.pTxMsg->Data[0] = stat.asUint8;
        HAL_CAN_Transmit_IT(&hcan2);
    
        if(osMutexRelease(canHandleMutex) != osOK)
        {
            continue;
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
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);    
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

// Reimplement weak definition in stm32f4xx_hal_can.c
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    CanRxMsgTypeDef* msg = hcan->pRxMsg;

    if (msg->StdId == LIGHTS_INPUT_STDID) {
        lightsInputs = msg->Data[0];
    } else if (msg->StdId == BATTERY_STAT_STDID) {
        batteryStatus = msg->Data[4];   
    }

    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FMP0);

    if (HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK)
    {
        Error_Handler();
    }
}
