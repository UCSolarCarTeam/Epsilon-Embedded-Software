#pragma once

typedef struct
{
    uint16_t maxCellVoltage;
    uint16_t minCellVoltage;
    unsigned char highTemperature;
    float packCurrent;
} OrionCanInfo;
