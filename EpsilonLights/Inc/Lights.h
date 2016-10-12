#pragma once

#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"

#define LIGHTS_GPIO_PORT GPIOD

typedef struct LightsRequests
{
    bool headlightsOff;
    bool headlightsLow;
    bool headlightsHigh;
    bool signalRight;
    bool signalLeft;
    bool hazard;
    bool interior;
    bool bmsStrobe;
} LightsRequests;

void updateLights(void const* arg);
void reportLightsToCan(void const* arg);
void setLightsGpio(bool set, uint16_t lightsPin);
bool readLightsGpio(uint16_t GPIO_Pin);

