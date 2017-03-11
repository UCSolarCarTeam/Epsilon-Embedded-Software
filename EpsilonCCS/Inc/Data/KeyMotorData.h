#pragma once

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing

#define KEY_MOTOR_PKG_ID 1

struct KeyMotorData
{
    unsigned char packageId;
    unsigned char m0Alive;
    float m0SetCurrent;
    float m0SetVelocity;
    float m0BusCurrent;
    float m0BusVoltage;
    float m0VehicleVelocity;
    unsigned char m1Alive;
    float m1SetCurrent;
    float m1SetVelocity;
    float m1BusCurrent;
    float m1BusVoltage;
    float m1VehicleVelocity;
}