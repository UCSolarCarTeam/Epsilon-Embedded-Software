#pragma once

#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"

#define HEADLIGHTS_OFF_PIN GPIO_PIN_1
#define HEADLIGHTS_LOW_PIN GPIO_PIN_2
#define HEADLIGHTS_HIGH_PIN GPIO_PIN_3
#define SIGNAL_RIGHT_PIN GPIO_PIN_4
#define SIGNAL_LEFT_PIN GPIO_PIN_5
#define HAZARD_PIN GPIO_PIN_6
#define INTERIOR_PIN GPIO_PIN_7
#define BMS_STROBE_PIN GPIO_PIN_8

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
