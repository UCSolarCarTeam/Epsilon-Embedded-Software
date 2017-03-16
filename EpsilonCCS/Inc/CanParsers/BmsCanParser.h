#pragma once

#include "stm32f4xx.h"

void parseBmsCanMessage(uint32_t stdId, uint8_t* data);
void parseBmsHeartbeat(uint8_t* data);
void parseStartupInfo(uint8_t* data);
void parsePackInfo(uint8_t* data);
void parseErrors(uint8_t* data);
void parseTempInfo(uint8_t* data);
void parseCellVoltages(uint8_t* data);
