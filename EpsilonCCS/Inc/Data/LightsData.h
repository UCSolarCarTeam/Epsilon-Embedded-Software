#pragma once

#include "stm32f4xx.h"

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing
#define LIGHT_PKG_ID 10

#define LOW_BEAMS_MASK 0x01
#define HIGH_BEAMS_MASK 0x02
#define BRAKE_LIGHTS_MASK 0x04
#define LEFT_SIGNAL_MASK 0x08
#define RIGHT_SIGNAL_MASK 0x10
#define BMS_STROBE_LIGHT_MASK 0x20

struct LightsData
{
    uint32_t lastReceived;
    unsigned char lowBeams;
    unsigned char highBeams;
    unsigned char brakeLights;
    unsigned char leftSignal;
    unsigned char rightSignal;
    unsigned char bmsStrobeLight;
    unsigned char lightsAlive;
};

extern struct LightsData lightsData;
