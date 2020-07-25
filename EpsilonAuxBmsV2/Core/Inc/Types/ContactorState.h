#pragma once

typedef enum
{
    OPEN,
    CLOSED,
    CONTACTOR_ERROR
} ContactorState;

typedef struct
{
    ContactorState commonState;
    ContactorState chargeState;
    ContactorState dischargeState;
    unsigned startupDone : 1;
} AuxBmsContactorState;
