#pragma once
#include "stm32f4xx.h"
#include "stm32f4xx_hal_can.h"

#define MPPT_HEARTBEAT_CAN_FREQ 200
#define MPPT_STDID 0x600U
#define MPPT_DLC 8

extern CAN_HandleTypeDef hcan1;
extern osPoolId canTxPool;
extern osMessageQId canTxQueue;

typedef struct // Datasheet is here: https://drive.google.com/open?id=0B4Qf5FVsKl-SVDRFY3RoWUt0RVU
{
    uint32_t stdId;
    uint32_t channel; //not sure of the data type, but it is a binary encoded number from 0-3.
} MpptCanMsg;

void mpptRtrTask(void const* arg);
void sendCanTask(void const* arg);
