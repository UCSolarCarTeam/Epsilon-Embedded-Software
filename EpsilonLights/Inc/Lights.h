#pragma once

#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_can.h"

// Refer to https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing
#define LIGHTS_UPDATE_FREQ 10 // Every 10ms

#define LIGHTS_HEARTBEAT_FREQ 1000 // 1Hz = 1000ms
#define LIGHTS_HEARTBEAT_STDID 0x710U

#define LIGHTS_STATUS_FREQ 200 // 5Hz = 200ms
#define LIGHTS_STATUS_STDID 0x711U

#define LIGHTS_INPUT_STDID 0x701U
#define HOFF_INPUT_INDEX 0
#define HLOW_INPUT_INDEX 1
#define HHIGH_INPUT_INDEX 2
#define RSIGNAL_INPUT_INDEX 3
#define LSIGNAL_INPUT_INDEX 4
#define HAZARDS_INPUT_INDEX 5

#define BATTERY_STAT_STDID 0x6FBU
// Mask for to check faults defined for Tritium BMS in TRI67.010 ver 2
// Check for "cell over voltage" 0x01, "cell under voltage" 0x02, and "cell over temperature" 0x04
#define BATTERY_CRIT_FAULT_MASK 0x07

typedef union LightsStatus
{
    struct
    {
        unsigned int lowBeams : 1;
        unsigned int highBeams : 1;
        unsigned int brakes : 1;
        unsigned int leftSignal : 1;
        unsigned int rightSignal : 1;
        unsigned int bmsStrobeLight : 1;
    };

    uint8_t asUint8;
} LightsStatus;

// Task for updating GPIOs
// arg : NULL
void updateLights(void const* arg);
// Task for sending lights statuses via CAN
// arg : osMutexId* canHandleMutex
void reportLightsToCan(void const* arg);
// Task for sending heartbeat via CAN
// arg : osMutexId* canHandleMutex
void sendHeartbeat(void const* arg);
