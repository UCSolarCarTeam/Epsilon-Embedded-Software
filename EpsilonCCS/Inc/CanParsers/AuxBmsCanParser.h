#pragma once

#include "stm32f4xx.h"

void parseAuxBmsCanMessage(uint32_t stdId, uint8_t data[8]);
void parseAuxBmsHeartbeat(uint8_t data[8]);
void parseAuxBmsInfo(uint8_t data[8]);
