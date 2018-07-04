# pragma once

#include "stm32f4xx.h"

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing

#define AUX_BMS_PKG_ID 11

#define PRECHARGE_STATE_MASK (0x07) // First 3 bits
#define AUX_VOLTAGE_MASK (0xF8) // Last 5 bits
#define STROBE_MASK (0x01) // First bit
#define ALLOW_CHARGE_MASK (0X02) // Second bit
#define CONTACTOR_ERROR_MASK (0X04) // Third bit
#define HIGH_VOLTAGE_ENABLE_MASK (0X08) //Fourth bit

enum BatteryPrechargeState
{
    OFF = 0,
    COMMON_ENGAGED = 1,
    CHARGE_ENGAGED = 2,
    DISCHARGE_ENGAGED = 3,
    ALL_ENGAGED = 4,
    INVALID_STATE = 5
};

struct AuxBmsData
{
    unsigned char prechargeState;
    unsigned char auxVoltage;
    unsigned char strobeBmsLight;
    unsigned char allowCharge;
    unsigned char contactorError;
    unsigned char highVoltageEnable;
    uint32_t auxBmsLastReceived;
};

extern struct AuxBmsData auxBmsData;