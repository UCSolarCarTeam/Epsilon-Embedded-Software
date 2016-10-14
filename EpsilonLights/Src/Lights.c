#include "cmsis_os.h"

#include "Lights.h"

extern CAN_HandleTypeDef hcan2; // main.c
extern uint8_t lightsInputs;
extern uint8_t batteryStatus;

void updateLights(void const* arg)
{
    unsigned char* inputs = (unsigned char*) arg;
    // One time osDelayUntil intialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, 10);
        // requests->interior = true;
    }
}

void reportLightsToCan(void const* arg)
{
    // For concurrency with sendHeartbeat()
    osMutexId* canHandleMutex = (osMutexId*) arg;
    // One time osDelayUntil intialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_STATUS_FREQ);
        // Toggle blue LED for every CAN message sent
        HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            continue;
        }

        // Set CAN msg address
        hcan2.pTxMsg->StdId = LIGHTS_STATUS_STDID;
        // Initalize to avoid garbage values in [6] anad [7]
        LightsStatus stat = {0};
        // Read lights gpios
        stat.lowBeams = HAL_GPIO_ReadPin(HLOW_GPIO_Port, HLOW_Pin);
        stat.highBeams = HAL_GPIO_ReadPin(HHIGH_GPIO_Port, HHIGH_Pin);
        stat.brakes = HAL_GPIO_ReadPin(BRAKE_GPIO_Port, BRAKE_Pin);
        stat.leftSignal = HAL_GPIO_ReadPin(LSIGNAL_GPIO_Port, LSIGNAL_Pin);
        stat.rightSignal = HAL_GPIO_ReadPin(RSIGNAL_GPIO_Port, RSIGNAL_Pin);
        stat.bmsStrobeLight = HAL_GPIO_ReadPin(ESTROBE_GPIO_Port, ESTROBE_Pin);
        hcan2.pTxMsg->Data[0] = stat.asUint8;
        // Send CAN msg
        HAL_CAN_Transmit_IT(&hcan2);

        if (osMutexRelease(canHandleMutex) != osOK)
        {
            continue;
        }
    }
}

void sendHeartbeat(void const* arg)
{
    // For concurrency with reportLightsToCan()
    osMutexId* canHandleMutex = (osMutexId*) arg;
    // One time osDelayUntil intialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, LIGHTS_HEARTBEAT_FREQ);
        // Toggle green LED for every heartbeat sent
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

        if (osMutexWait(canHandleMutex, 0) != osOK)
        {
            Error_Handler();
        }

        // Set CAN msg address
        hcan2.pTxMsg->StdId = LIGHTS_HEARTBEAT_STDID;
        // Always 1
        hcan2.pTxMsg->Data[0] = 1;
        // Send CAN msg
        HAL_CAN_Transmit_IT(&hcan2);

        if (osMutexRelease(canHandleMutex) != osOK)
        {
            Error_Handler();
        }
    }
}

// Reimplement weak definition in stm32f4xx_hal_can.c
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    CanRxMsgTypeDef* msg = hcan->pRxMsg;

    if (msg->StdId == LIGHTS_INPUT_STDID)
    {
        lightsInputs = msg->Data[0];
    }
    else if (msg->StdId == BATTERY_STAT_STDID)
    {
        batteryStatus = msg->Data[4];
    }

    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FMP0);

    if (HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK)
    {
        Error_Handler();
    }
}
