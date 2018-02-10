#pragma once

#include "stm32f4xx.h"

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing
#define BATTERY_PKG_ID 7

#define DISCHARGE_RELAY_ENABLED_MASK 0x01
#define CHARGE_RELAY_ENABLED_MASK 0x02
#define CHARGER_SAFETY_ENABLED_MASK 0x04
#define MALFUNCTION_INDICATOR_ACTIVE_MASK 0x08
#define MULTIPURPOSE_INPUT_SIGNAL_STATUS_MASK 0x10
#define ALWAYS_ON_SIGNAL_STATUS_MASK 0x20
#define IS_READY_SIGNAL_STATUS_MASK 0x40
#define IS_CHARGING_SIGNAL_STATUS_MASK 0x80
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

struct BatteryData
{
    uint32_t bmsLastReceived;
    unsigned char bmsRelayStatus;
    unsigned char populatedCells;
    float input12volt;
    float fanVoltage;
    float packCurrent;
    float packVoltage;
    float packStateofCharge;
    float packAmphours;
    float packDepthofDischarge;
    unsigned char highTemperature;
    unsigned char highThermistorId;
    unsigned char lowTemperature;
    unsigned char lowThermistorId;
    unsigned char averageTemperature;
    unsigned char internalTemperature;
    unsigned char fanSpeed;
    unsigned char requestedFanSpeed;
    unsigned short int lowCellVoltage;
    unsigned char lowCellVoltageId;
    unsigned short int highCellVoltage;
    unsigned char highCellVoltageId;
    unsigned short int averageCellVoltage;
    unsigned char prechargeState;
    unsigned char auxVoltage;
    unsigned char strobeBMSLight;
    unsigned char allowCharge;
    unsigned char contactorError;
    uint32_t auxBmsLastReceived;
};

extern struct BatteryData batteryData;
