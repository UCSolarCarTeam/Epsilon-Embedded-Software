#pragma once
#include "stm32f4xx.h"
#include "stm32f4xx_hal_can.h"

#define MPPT_HEARTBEAT_CAN_FREQ 200
#define MPPT_CHANNEL_ZERO_STDID 0x600U
#define MPPT_CHANNEL_ONE_STDID 0x601U
#define MPPT_CHANNEL_TWO_STDID 0x602U
#define MPPT_DLC 8
#define MPPT_RTR 1

extern CAN_HandleTypeDef hcan1;
extern osPoolId mpptCanTxPool;
extern osMessageQId mpptCanTxQueue;

typedef struct // Datasheet is here: https://drive.google.com/open?id=0B4Qf5FVsKl-SVDRFY3RoWUt0RVU
{
    uint32_t stdId;
    uint32_t rtr;
    uint32_t dlc;
} MpptCanMsg;

void mpptRtrTask(void const* arg);
void sendMpptRtrCanTask(void const* arg);
