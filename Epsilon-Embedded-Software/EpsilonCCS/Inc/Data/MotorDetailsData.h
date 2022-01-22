#pragma once

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing

#define M0_DETAILS_DATA_PKG_ID 2
#define M1_DETAILS_DATA_PKG_ID 3

struct MotorDetailsData
{
    float phaseCCurrent;
    float phaseBCurrent;
    float motorVoltageReal;
    float motorVoltageImaginary;
    float motorCurrentReal;
    float motorCurrentImaginary;
    float backEmfReal;
    float backEmfImaginary;
    float railSupply15v;
    float railSupply3_3v;
    float railSupply1_9v;
    float heatSinkTemperature;
    float motorTemperature;
    float dspBoardTemp;
    float dcBusAmpHours;
    float odometer;
    float slipSpeed;
};

extern struct MotorDetailsData motor0DetailsData;
extern struct MotorDetailsData motor1DetailsData;
