# pragma once

#include "stm32f4xx.h"

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing

#define AUX_BMS_PKG_ID 11

#define PRECHARGE_STATE_MASK (0x07) // First 3 bits
#define AUX_VOLTAGE_MASK (0xF8) // Last 5 bits
#define STROBE_MASK (0x01) // First bit
#define ALLOW_CHARGE_MASK (0X02) // Second bit
#define HIGH_VOLTAGE_ENABLE_STATE_MASK (0X04) // Third bit
#define ALLOW_DISCHARGE_MASK (0X08) //Fourth bit
#define ORION_CAN_RECEIVED_RECENTLY_MASK (0x10) //Fifth bit

#define CHARGE_CONTACTOR_ERROR_MASK (0x10)
#define DISCHARGE_CONTACTOR_ERROR_MASK (0x20)
#define COMMON_CONTACTOR_ERROR_MASK (0x40)
#define DISCHARGE_SHOULD_TRIP_MASK (0x80)
#define CHARGE_SHOULD_TRIP_MASK (0x01)
#define CHARGE_OPEN_BUT_SHOULD_BE_CLOSED_MASK (0x02)
#define DISCHARGE_OPEN_BUT_SHOULD_BE_CLOSED_MASK (0x04)

#define CHARGE_TRIP_DUE_TO_HIGH_CELL_VOLTAGE_MASK (0x01)
#define CHARGE_TRIP_DUE_TO_HIGH_TEMPERATURE_AND_CURRENT_MASK (0x02)
#define CHARGE_TRIP_DUE_TO_PACK_CURRENT_MASK (0x04)
#define DISCHARGE_TRIP_DUE_TO_LOW_CELL_VOLTAGE_MASK (0x08)
#define DISCHARGE_TRIP_DUE_TO_HIGH_TEMPERATURE_AND_CURRENT_MASK (0x10)
#define DISCHARGE_TRIP_DUE_TO_PACK_CURRENT_MASK (0x20)
#define PROTECTION_TRIP_MASK (0x40)
#define TRIP_DUE_TO_ORION_MESSAGE_TIMEOUT_MASK (0x80)
#define CHARGE_NOT_CLOSED_DUE_TO_HIGH_CURRENT_MASK (0x0100)
#define DISCHARGE_NOT_CLOSED_DUE_TO_HIGH_CURRENT_MASK (0x0200)
#define TRIP_DUE_TO_CONTACTOR_DISCONNECTED_UNEXPECTEDLY_MASK (0x0400)

enum BatteryPrechargeState
{
    OFF = 0,
    COMMON_ENGAGED = 1,
    CHARGE_ENGAGED = 2,
    DISCHARGE_ENGAGED = 3,
    ALL_ENGAGED = 4,
    INVALID_STATE = 5
};

struct AuxTrip
{
    unsigned char chargeTripDueToHighCellVoltage;
    unsigned char chargeTripDueToHighTemperatureAndCurrent;
    unsigned char chargeTripDueToPackCurrent;
    unsigned char dischargeTripDueToLowCellVoltage;
    unsigned char dischargeTripDueToHighTemperatureAndCurrent;
    unsigned char dischargeTripDueToPackCurrent;
    unsigned char protectionTrip;
    unsigned char dischargeNotClosedDueToHighCurrent;
    unsigned char chargeNotClosedDueToHighCurrent;
    unsigned char tripDueToOrionMessageTimeout;
    unsigned char tripDueToContactorDisconnectedUnexpectedly;
};

struct ContactorDebugInfo
{
    unsigned char chargeContactorError;
    unsigned char dischargeContactorError;
    unsigned char commonContactorError;
    unsigned char dischargeShouldTrip;
    unsigned char chargeShouldTrip;
    unsigned char chargeOpenButShouldBeClosed;
    unsigned char dischargeOpenButShouldBeClosed;
};

struct AuxBmsData
{
    unsigned char prechargeState;
    unsigned char auxVoltage;
    unsigned char strobeBmsLight;
    unsigned char allowCharge;
    unsigned char highVoltageEnableState;
    unsigned char allowDischarge;
    unsigned char orionCanReceivedRecently;
    struct ContactorDebugInfo contactorDebugInfo;
    struct AuxTrip auxTrip;
    uint32_t auxBmsLastReceived;
};

extern struct AuxBmsData auxBmsData;