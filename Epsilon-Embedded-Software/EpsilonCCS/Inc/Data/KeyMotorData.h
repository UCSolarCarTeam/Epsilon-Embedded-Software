#pragma once

#include "stm32f4xx.h"

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing
#define KEY_MOTOR_PKG_ID 1

struct KeyMotorData
{
    uint32_t m0lastReceived;
    float m0SetCurrent;
    float m0SetVelocity;
    float m0BusCurrent;
    float m0BusVoltage;
    float m0VehicleVelocity;
    uint32_t m1lastReceived;
    float m1SetCurrent;
    float m1SetVelocity;
    float m1BusCurrent;
    float m1BusVoltage;
    float m1VehicleVelocity;
};

extern struct KeyMotorData keyMotorData;
