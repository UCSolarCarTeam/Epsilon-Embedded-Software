#pragma once
#include "stm32f4xx.h"
#include "stm32f4xx_hal_can.h"

#define MPPT_HEARTBEAT_CAN_FREQ 200
#define MPPT_STDID 0x600U
#define MPPT_DLC 8

extern CAN_HandleTypeDef hcan1;
extern osPoolId canTxPool;
extern osMessageQId canTxQueue;

typedef struct // Datasheet is here: http://calgarysolarcarteamhub.com/attachments/download/112/Photon%20Datasheet%20rev1_2.pdf
{
    uint32_t StdId;
    uint32_t Channel; //not sure of the data type, but it is a binary encoded number from 0-3.
} MpptCanMsg;

void mpptRtrTask(void const* arg);
void sendCanTask(void const* arg);