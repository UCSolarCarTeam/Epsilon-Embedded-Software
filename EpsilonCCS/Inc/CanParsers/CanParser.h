#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_hal_can.h"

void parseCanMessage(uint32_t stdId, uint8_t *data);
