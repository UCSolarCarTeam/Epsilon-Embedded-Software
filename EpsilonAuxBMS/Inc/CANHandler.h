#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_can.h"
#include "cmsis_os.h"
#include "AuxStatus.h"

#define AUX_HEARBEAT_FREQ 1000 // 1Hz
#define AUX_HEARTBEAT_STDID 0x720U

#define AUX_STATUS_FREQ 100 // 10Hz
#define AUX_STATUS_STDID 0x721U

#define MAX_MIN_VOLTAGES_STDID 0x30A

// Will go in hal_can.h when code is finished
#define CAN_FLAG_FMP0              0x12000003U

extern CAN_HandleTypeDef hcan1; // main.c
extern uint16_t orionBmsInputs[2];
extern AuxStatus auxStatus;

// Task for sending heartbeat via CAN
// arg: osMutexId* canHandleMutex
void sendHeartbeatTask(void const* arg);
// Task for sending aux status via CAN
// arg: osMutexId* canHandleMutex
void reportAuxToCanTask(void const* arg);
