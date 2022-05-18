#pragma once

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing

#define MOTOR_FAULTS_PKG_ID 5

// Errors
#define SOFTWARE_OVER_CURRENT_MASK 0x02
#define DC_BUS_OVER_VOLTAGE_MASK 0x04
#define BAD_MOTOR_POSITION_HALL_SEQUENCE_MASK 0x08
#define WATCHDOG_CAUSED_LAST_RESET_MASK 0x10
#define CONFIG_READ_ERROR_MASK 0x20
#define RAIL_15V_UNDER_VOLTAGE_LOCK_OUT_MASK 0x40
#define DESATURATION_FAULT_MASK 0x80
#define MOTOR_OVER_SPEED_MASK 0x100

// Limits
#define OUTPUT_VOLTAGE_PWM_LIMIT_MASK 0x01
#define MOTOR_CURRENT_LIMIT_MASK 0x02
#define VELOCITY_LIMIT_MASK 0x04
#define BUS_CURRENT_LIMIT_MASK 0x08
#define BUS_VOLTAGE_UPPER_LIMIT_MASK 0x10
#define BUS_VOLTAGE_LOWER_LIMIT_MASK 0x20
#define IPM_OR_MOTOR_TEMPERATURE_LIMIT_MASK 0x40

struct MotorErrorFlags
{
    unsigned char softwareOverCurrent;
    unsigned char dcBusOverVoltage;
    unsigned char badMotorPositionHallSequence;
    unsigned char watchdogCausedLastReset;
    unsigned char configReadError;
    unsigned char rail15vUnderVoltageLockOut;
    unsigned char desaturationFault;
    unsigned char motorOverSpeed;
};

struct MotorLimitFlags
{
    unsigned char outputVoltagePwm;
    unsigned char motorCurrent;
    unsigned char velocity;
    unsigned char busCurrent;
    unsigned char busVoltageUpper;
    unsigned char busVoltageLower;
    unsigned char ipmOrMotorTemperature;
};

struct MotorFaultsData
{
    struct MotorErrorFlags m0ErrorFlags;
    struct MotorErrorFlags m1ErrorFlags;
    struct MotorLimitFlags m0LimitFlags;
    struct MotorLimitFlags m1LimitFlags;
    unsigned char m0CanRxErrorCount;
    unsigned char m0CanTxErrorCount;
    unsigned char m1CanRxErrorCount;
    unsigned char m1CanTxErrorCount;
};

extern struct MotorFaultsData motorFaultsData;
