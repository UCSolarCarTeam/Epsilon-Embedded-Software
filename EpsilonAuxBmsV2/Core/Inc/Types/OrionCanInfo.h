#pragma once

typedef struct
{
    uint16_t highCellVoltage;
    uint16_t lowCellVoltage;
    unsigned char highTemperature;
    float packCurrent;
} OrionCanInfo;
