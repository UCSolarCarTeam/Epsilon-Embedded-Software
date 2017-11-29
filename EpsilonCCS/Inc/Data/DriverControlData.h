#pragma once

#include "stm32f4xx.h"

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing
#define DRIVER_CONTROL_PKG_ID 4

// Lights Inputs
#define HEADLIGHTS_OFF_MASK 0x01
#define HEADLIGHTS_LOW_MASK 0x02
#define HEADLIGHTS_HIGH_MASK 0x04
#define SIGNAL_RIGHT_MASK 0x08
#define SIGNAL_LEFT_MASK 0x10
#define HAZARD_MASK 0x20
#define INTERIOR_MASK 0x40

// Music Inputs
#define VOLUME_UP_MASK 0x01
#define VOLUME_DOWN_MASK 0x02
#define NEXT_SONG_MASK 0x04
#define PREV_SONG_MASK 0x8

// Driver Inputs
#define BRAKES_MASK 0x01
#define FORWARD_MASK 0x02
#define REVERSE_MASK 0x04
#define PUSH_TO_TALK_MASK 0x08
#define HORN_MASK 0x10
#define RESET_MASK 0x20
#define AUX_MASK 0x40

struct LightsInputs
{
    unsigned char headlightsOff;
    unsigned char headlightsLow;
    unsigned char headlightsHigh;
    unsigned char signalRight;
    unsigned char signalLeft;
    unsigned char hazard;
    unsigned char interior;
};

struct MusicInputs
{
    unsigned char volumeUp;
    unsigned char volumeDown;
    unsigned char nextSong;
    unsigned char prevSong;
};

struct DriverInputs
{
    unsigned char brakes;
    unsigned char forward;
    unsigned char reverse;
    unsigned char pushToTalk;
    unsigned char horn;
    unsigned char reset;
};

struct DriverControlData
{
    uint32_t lastReceived;
    struct LightsInputs lightsInputs;
    struct MusicInputs musicInputs;
    unsigned short int acceleration;
    unsigned short int regenBraking;
    struct DriverInputs driverInputs;
    unsigned char aux;
};

extern struct DriverControlData driverControlData;
