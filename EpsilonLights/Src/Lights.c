#include <stdbool.h>

#include "cmsis_os.h"

#include "Lights.h"

void updateLights(void const* arg)
{
    LightsRequests* requests = (LightsRequests*) arg;

    for (;;)
    {
        requests->interior = true;
    }
}

void reportLightsToCan(void const* arg)
{
    CAN_HandleTypeDef* hcan2 = (CAN_HandleTypeDef*) arg;
    const uint32_t DELAY_MS = 1000;
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, DELAY_MS);
        hcan2->pTxMsg->Data[0] = 0xAD;
        hcan2->pTxMsg->Data[1] = 0xAD;
        HAL_CAN_Transmit_IT(hcan2);
    }
}

void setLightsGpio(bool set, uint16_t lightsPin)
{
    HAL_GPIO_WritePin(LIGHTS_GPIO_PORT, lightsPin, set);
}

bool readLightsGpio(uint16_t lightsPin)
{
    return HAL_GPIO_ReadPin(LIGHTS_GPIO_PORT, lightsPin);;
}

