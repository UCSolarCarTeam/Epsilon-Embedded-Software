# pragma once

#include "stm32f4xx.h"

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing

#define AUX_BMS_PKG_ID 12

#define PRECHARGE_STATE_MASK (0x07) // First 3 bits
#define AUX_VOLTAGE_MASK (0xF8) // Last 5 bits
#define STROBE_MASK (0x01) // First bit
#define ALLOW_CHARGE_MASK (0X02) // Second bit
#define CONTACTOR_ERROR_MASK (0X04) // Third bit

enum BatteryPrechargeState
{
    IDLE = 0,
    PRECHARGE = 1,
    MEASURE = 2,
    ENABLE_PACK = 3,
    RUN = 4
};

struct AuxBmsData
{
	unsigned char prechargeState;
    unsigned char auxVoltage;
    unsigned char strobeBmsLight;
    unsigned char allowCharge;
    unsigned char contactorError;
    uint32_t auxBmsLastReceived;
};

extern struct AuxBmsData auxBmsData;