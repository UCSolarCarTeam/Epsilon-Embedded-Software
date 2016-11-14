#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_can.h"

#define HEARTBEAT_CAN_FREQ 1000
#define HEARTBEAT_STDID 0x700U

#define LIGHTS_CAN_FREQ 100
#define LIGHTS_STDID 0x701U

#define MUSIC_CAN_FREQ 200
#define MUSIC_STDID 0x702U

#define DRIVER_CAN_FREQ 50 
#define DRIVER_STDID 0x703U

extern CAN_HandleTypeDef hcan2; // main.c

void sendHeartbeatTask(void const* arg);
void sendLightsTask(void const* arg);
void sendMusicTask(void const* arg);
void sendDriverTask(void const* arg);
