#pragma once

void runLightsTests();

void test_updateLights_allLightsOff();
void test_updateLights_case2();
void test_updateLights_case3();

void test_blinkSignalLights_signalLightsInputsDisabled();
void test_blinkSignalLights_enableSignalLightsAfterPreviousStateWasDisabled();
void test_blinkSignalLights_EnableSignalLightsAfterPreviousStateWasEnabled();

void test_blinkSignalLights_enabledToEnabled_keepBlinkersOffAndResetBlinkerTimerToZero();
void test_blinkSignalLights_enabledToEnabled_turnBlinkersOnAndUpdateTimer();
void test_blinkSignalLights_enabledToEnabled_keepBlinkersOffAndUpdateTimer();

void test_updateStrobeLight_strobeLightOnAndUpdateBlinkerTimer();
void test_updateStrobeLight_strobeLightOffAndResetBlinkerTimer();
void test_updateStrobeLight_strobeLightOffAndUpdateBlinkerTimer();

void test_reportLightsToCan();

void test_sendHeartbeat();

void test_HAL_CAN_RxCpltCallback_lightsInput();
void test_HAL_CAN_RxCpltCallback_batteryStatErrors();
void test_HAL_CAN_RxCpltCallback_driversInputs();
void test_HAL_CAN_RxCpltCallback_auxBmsInput();
void test_HAL_CAN_RxCpltCallback_batteryStatErrorsANDToggleRedPin();
