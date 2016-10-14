#pragma once

#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_can.h"

#define LIGHTS_HEARTBEAT_FREQ 1000 // 1Hz = 1000ms
#define LIGHTS_HEARTBEAT_STDID 0x710

#define LIGHTS_STATUS_FREQ 200 // 5Hz = 200ms
#define LIGHTS_STATUS_STDID 0x711

void updateLights(void const* arg);
void reportLightsToCan(void const* arg);
void sendHeartbeat(void const* arg);

