#pragma once

extern float regenValuesQueue[];
extern float accelValuesQueue[];

void setAccelValuesQueue(uint8_t number);
void setRegenValuesQueue(uint8_t number);

void runDriverControlTests();

void test_sendHeartbeat();
void test_sendLights();
void test_sendMusic();
void test_sendDriver();
void test_sendCan_EqualToHalOk();
void test_sendCan_NotEqualToHalOk();
void test_sendDriveCommands_wasPreviouslyResetAndWantToResetAgain();
void test_sendDriveCommands_wasNotPreviouslyResetAndWantToReset();
void test_sendDriveCommands_noResetsAtAll();
void test_sendDriveCommands_forwardAndReverseIsAnErrorState();
void test_sendDriveCommands_forwardAndMotorVehicleVelocityIsNotSafe();
void test_sendDriveCommands_reverseAndMotorVehicleVelocityIsNotSafe();
void test_sendDriveCommands_regenPercentageGreaterThanNonZeroThresholdAndAllowingCharge();
void test_sendDriveCommands_regenPercentageGreaterThanNonZeroThresholdAndNotAllowingCharge();
void test_sendDriveCommands_brake();
void test_sendDriveCommands_forwardAndAccelPercentageGreaterThanNonZeroThreshold();
void test_sendDriveCommands_reverseAndAccelPercentageGreaterThanNonZeroThreshold();
void test_sendDriveCommands_regenBrakeForwardAndReverseOff();
