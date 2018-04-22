#pragma once

typedef struct OrionStatus
{
    /**
    * Set to 1 if charge, discharge, and safety lines from Orion are all high,
    * otherwise 0
    */
    unsigned gpioOk : 1;
    /**
    * Set to 1 maxCellVoltage is below defined maximum
    * and minCellVoltage is above defined minimum, otherwise 0
    * see ReadOrionGpioTask.h
    */
    unsigned batteryVoltagesInRange : 1;
    uint16_t maxCellVoltage;
    uint16_t minCellVoltage;
} OrionStatus;

extern OrionStatus orionStatus;
