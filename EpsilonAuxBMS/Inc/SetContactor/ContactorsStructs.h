#pragma once

//structs
typedef enum ContactorsSettingState
{
    FIRST_CHECK,
    COMMON_ENABLE_CHECK,
    CHARGE_ENABLE_CHECK,
    DISCHARGE_ENABLE_CHECK,
    IDLE,
    BLOCKED,
    CONTACTOR_DISCONNECTED,
    SHUTDOWN
} ContactorsSettingState;

typedef struct SetContactorStateInfo
{
    ContactorsSettingState state;
    ContactorsSettingState prevState;
    uint8_t hasChargeBeenSet;
    uint8_t hasDischargeBeenSet;
} SetContactorStateInfo;
