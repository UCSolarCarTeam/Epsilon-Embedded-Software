#pragma once
#include "cmsis_os.h"

typedef struct AuxStatus
{
    unsigned commonContactorState : 1;
    unsigned chargeContactorState : 1;
    unsigned dischargeContactorState : 1;
    unsigned highVoltageEnableState : 1;
    unsigned auxVoltage : 5;
    unsigned strobeBmsLight : 1;
    unsigned allowCharge : 1;
    unsigned contactorError : 1;
    // The following members will not be reported to CAN
    unsigned startUpSequenceDone: 1;
    osMutexId auxStatusMutex;
} AuxStatus;

extern AuxStatus auxStatus;
