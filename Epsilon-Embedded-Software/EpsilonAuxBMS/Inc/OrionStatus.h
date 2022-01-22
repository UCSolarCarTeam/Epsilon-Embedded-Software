#pragma once

#include "cmsis_os.h"
typedef struct OrionStatus
{
    /**
    * Set to 1 maxCellVoltage is below defined maximum
    * and minCellVoltage is above defined minimum, otherwise 0
    * see ReadOrionGpioTask.c
    */
    unsigned batteryVoltagesInRange : 1;
    /**
    * Set to 1 if Orion had overriden the previous contactor gpio value.
    * otherwise 0
    */
    unsigned chargeContactorOverriden : 1;
    unsigned dischargeContactorOverriden : 1;
    unsigned allowCharge : 1;
    unsigned allowDischarge : 1;
    unsigned shutOff : 1;
    unsigned canMsgReceived : 1;
    uint16_t maxCellVoltage;
    uint16_t minCellVoltage;
    unsigned char highTemperature;
    float packCurrent;
    osMutexId orionStatusMutex;
} OrionStatus;

extern OrionStatus orionStatus;
