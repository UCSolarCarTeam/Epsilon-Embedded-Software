#pragma once

#include "stm32f4xx.h"

void parseBmsCanMessage(uint32_t stdId, uint8_t data[8]);
void parseBmsHeartbeat(uint8_t data[8]);
void parseStartupInfo(uint8_t data[8]);
void parsePackInfo(uint8_t data[8]);
void parseErrors(uint8_t data[8]);
void parseTempInfo(uint8_t data[8]);
void parseCellVoltages(uint8_t data[8]);
