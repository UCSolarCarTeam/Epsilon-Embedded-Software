#pragma once

#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_can.h"

#define LIGHTS_HEARTBEAT_FREQ 1000 // 1Hz = 1000ms
#define LIGHTS_HEARTBEAT_STDID 0x710U

#define LIGHTS_STATUS_FREQ 200 // 5Hz = 200ms
#define LIGHTS_STATUS_STDID 0x711U

#define LIGHTS_INPUT_STDID 0x701U
#define BATTERY_STAT_STDID 0x6FBU

typedef union LightsStatus
{
    struct
    {
        unsigned int lowBeams : 1;
        unsigned int highBeams : 1;
        unsigned int Brakes : 1;
        unsigned int leftSignal : 1;
        unsigned int rightSignal : 1;
        unsigned int bmsStrobeLight : 1;
    };

    uint8_t asUint8;
} LightsStatus;

void updateLights(void const* arg);
void reportLightsToCan(void const* arg);
void sendHeartbeat(void const* arg);

