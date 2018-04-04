#pragma once

typedef struct AuxStatus
{
    unsigned commonContactorState : 1;
    unsigned chargeContactorState : 1;
    unsigned dischargeContactorState : 1;
    unsigned auxVoltage : 5;
    unsigned strobeBmsLight : 1;
    unsigned allowCharge : 1;
    unsigned contactorError : 1;
} AuxStatus;
