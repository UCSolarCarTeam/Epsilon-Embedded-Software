#pragma once

#include "stm32f4xx.h"

void parseLightsCanMessage(uint32_t stdId, uint8_t* data);
void parseLightsHeartbeat(uint8_t* data);
void parseLightsStatus(uint8_t* data);
