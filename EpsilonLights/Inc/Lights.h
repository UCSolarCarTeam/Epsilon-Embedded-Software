#pragma once

#include <stdbool.h>

typedef struct LightsRequests {
    bool headlightsOff;
    bool headlightsLow;
    bool headlightsHigh;
    bool signalRight;
    bool signalLeft;
    bool hazard;
    bool interior;
    bool bmsStrobe;
} LightsRequests;

void updateLights(void const* arg);

void reportLightsToCan(void const* arg);


void setLowBeams(bool set);
bool readLowBeams();

void setHighBeams(bool set);
bool readHighBeams();

void setBrakeLights(bool set);
bool readBrakeLights();

void setLeftSignal(bool set);
bool readLeftSignal();

void setRightSignal(bool set);
bool readRightSignal();

void setBmsStrobe(bool set);
bool readBmsStrobe();
