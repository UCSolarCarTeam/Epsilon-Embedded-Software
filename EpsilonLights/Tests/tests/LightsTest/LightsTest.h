#pragma once
//#include "Lights.h"

extern uint8_t lightsInputs;
extern uint8_t auxBmsInputs[2];
extern uint8_t batteryErrors[5];
extern SigLightsHandle sigLightsHandle;
extern uint8_t driversInputs[4];
extern updateLights lightCharacteristics;


void runLightsTests();

void test_headlightsOff_brakesLightsOff_hazardsOff_bmsStrobeLightsOff();
void test_headlightsLow_breakLightsOn_hazardsOn_bmsStrobeLightOn();