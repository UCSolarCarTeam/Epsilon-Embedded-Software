#pragma once

#include "stm32f4xx.h"

void parseDriverControlsCanMessage(uint32_t stdId, uint8_t* data);
void parseDriverControlsHearbeat();
void parseDriverControlsLightsInput(uint8_t* data);
void parseDriverControlsMusicInput(uint8_t* data);
void parseDriverControlsDriverInput(uint8_t* data);