#pragma once
#include "stm32f4xx_hal_can.h"

typedef struct
{
    CAN_TxHeaderTypeDef canTxHeader;
    uint8_t data[8];
} CanTxGatekeeperQueueData;
