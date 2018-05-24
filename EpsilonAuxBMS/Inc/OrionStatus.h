#pragma once

#include "cmsis_os.h"
typedef struct OrionStatus
{
    /**
    * Set to 1 if charge and discharge enables lines are high
    * otherwise 0
    */
    unsigned gpioOk : 1;
    /**
    * Set to 1 maxCellVoltage is below defined maximum
    * and minCellVoltage is above defined minimum, otherwise 0
    * see ReadOrionGpioTask.c
    */
    unsigned batteryVoltagesInRange : 1;
    unsigned contactorOverriden : 1;
    unsigned allowCharge : 1;
    unsigned allowDischarge : 1;
    unsigned shutOff : 1;
    uint16_t maxCellVoltage;
    uint16_t minCellVoltage;
    osMutexId orionStatusMutex;
} OrionStatus;

extern OrionStatus orionStatus;
