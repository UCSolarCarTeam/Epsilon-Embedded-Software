#pragma once
//#include "Lights.h"

// extern uint8_t lightsInputs;
// extern uint8_t auxBmsInputs[2];
// extern uint8_t batteryErrors[5];
// extern SigLightsHandle sigLightsHandle;
// extern uint8_t driversInputs[4];
// extern updateLights lightCharacteristics;

void runLightsTests();

void test_updateLights_headLightsOffBrakesLightsOffHazardsOffBmsStrobeLightsOff();
void test_updateLights_headLightsLowBreakLightsOnHazardsOnBmsStrobeLightOn();
void test_updateLights_headLightsHighHeadLightsLowBreakLightsOnHazardsOnBmsStrobeLightOn();
void test_blinkSignalLights_sigLightsLeftOFFSigLightsRightOFF();
void test_blinkSignalLights_sigLightsLeftONSigLightsRightONPrevSigStateZero();

void test_blinkSignalLights_prevSigState0SigLightsLeftONSigLightsRightON();
void keepBlinkersOFF_resetBlinkerTimerToZero(uint32_t prevWakeTime, uint8_t prevSigstate);
void turnBlinkersON_updateTimer(uint32_t prevWakeTime, uint8_t prevSigState);
void keepBlinkersOFF_updateTimer(uint32_t prevWakeTime, uint8_t prevSigState);

void test_updateStrobeLights1_ESTROBEONUpdateBlinkerTimer();
void test_updateStrobeLights1_ESTROBEOFFResetBlinkerTimer();
void test_updateStrobeLights1_ESTROBEOFFUpdateBlinkerTimer();
void test_reportLightsToCan();
void test_sendHeartbeat();

void test_HAL_CAN_RxCpltCall_backLightsInputStdId();
void test_HAL_CAN_RxCpltCallback_batteryStatErrorsStdId();
void test_HAL_CAN_RxCpltCallback_driversInputsStdId();
void test_HAL_CAN_RxCpltCallback_auxBmsInputStdId();
void test_HAL_CAN_RxCpltCallback_batteryStatErrorsStdIdANDToggleRedPin();