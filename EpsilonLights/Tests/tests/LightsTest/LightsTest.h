#pragma once
//#include "Lights.h"

extern uint8_t lightsInputs;
extern uint8_t auxBmsInputs[2];
extern uint8_t batteryErrors[5];
extern SigLightsHandle sigLightsHandle;
extern uint8_t driversInputs[4];
extern updateLights lightCharacteristics;


void hi();

void runLightsTests();

void test_headLightsOff_brakesLightsOff_hazardsOff_bmsStrobeLightsOff();
void test_headLightsLow_breakLightsOn_hazardsOn_bmsStrobeLightOn();
void test_headLightsHigh_headLights_Low_breakLightsOn_hazardsOn_bmsStrobeLightOn();
void blinkSignalLights_sigLightsLeftOFF_sigLightsRightOFF();
void blinkSignalLights_sigLightsLeftON_sigLightsRightON_prevSigState_Zero();

void blinkSignalLights_prevSigState0_sigLightsLeftON_sigLightsRightON();
void keepBlinkersOFF_resetBlinkerTimerToZero(uint32_t prevWakeTime, uint8_t prevSigstate);
void turnBlinkersON_updateTimer(uint32_t prevWakeTime, uint8_t prevSigState);
void keepBlinkersOFF_updateTimer(uint32_t prevWakeTime, uint8_t prevSigState);

void updateStrobeLights2_ESTROBEON_UpdateblinkerTimer();
void updateStrobeLights2_ESTROBEOFF_ResetBlinkerTimer();
void updateStrobeLights2_ESTROBEOFF_UpdateBlinkerTimer();
void reportLightsToCan_test();
void sendHeartbeat_test();