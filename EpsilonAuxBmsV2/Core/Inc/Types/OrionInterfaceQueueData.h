#pragma once
#include "OrionCanInfo.h"

typedef enum
{
    CAN_RX,
    GPIO_EXTI
} OrionInterfaceQueueDataType;

typedef union
{
    OrionCanInfo orionCanInfo;
    uint16_t gpioNum;
} OrionInterfaceQueueValue;

typedef struct
{
    OrionInterfaceQueueDataType type;
    OrionInterfaceQueueValue value;
} OrionInterfaceQueueData;
