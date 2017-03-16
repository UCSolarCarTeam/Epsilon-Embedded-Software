#pragma once

#include "stm32f4xx.h"

void parseAuxBmsCanMessage(uint32_t stdId, uint8_t* data);
void parseAuxBmsHeartbeat();
void parseAuxBmsInfo(uint8_t* data);
