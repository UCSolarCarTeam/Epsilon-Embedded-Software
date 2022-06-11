#pragma once
#include "main.h"

typedef struct
{
    CAN_RxHeaderTypeDef canRxHeader;
    uint8_t data[8];
} CanRxQueueData;
