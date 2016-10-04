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
    const uint32_t DELAY_MS = 200;
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, DELAY_MS);
        // TODO report CAN
        prevWakeTime = osKernelSysTick();
    }
}

void setLightsGpio(bool set, uint16_t lightsPin)
{
    HAL_GPIO_WritePin(LIGHTS_GPIO_PERIPHERAL, lightsPin, set);
}

bool readLightsGpio(uint16_t lightsPin)
{
    return HAL_GPIO_ReadPin(LIGHTS_GPIO_PERIPHERAL, lightsPin);;
}

