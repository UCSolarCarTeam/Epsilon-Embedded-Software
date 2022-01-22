#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_hal_can.h"

extern osMessageQId canRxQueueId;
extern osPoolId canRxPool;

typedef struct
{
    uint32_t StdId;
    uint8_t Data[8];
} CanMsg;

void parseCanTask(void const* arg);
void parseCanMessage(uint32_t stdId, uint8_t* data);
