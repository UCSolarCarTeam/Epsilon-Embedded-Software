#pragma once

#include "stm32f4xx.h"

void parseMotorCanMessage(uint32_t stdId, uint8_t* data);
void parseMotorFaultMessage(uint8_t motorId, uint8_t* data);
float arrayToFloat(uint8_t* data);
