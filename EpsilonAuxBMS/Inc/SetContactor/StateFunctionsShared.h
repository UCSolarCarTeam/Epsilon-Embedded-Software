#pragma once
#include "AuxBmsTasks.h"
#include "stm32f4xx_hal_adc.h"
#include "OrionStatus.h"
#include "ContactorsStructs.h"

// Turns off all contactors and adjusts auxstatus if isContactorError is 1
void disconnectContactors(uint8_t isContactorError);

// Checks if stateInfo->hasDischargeBeenSet is set but discharge is disconnected
const uint8_t dischargeDisconnectedDuringContactorSetting(const uint8_t dischargeSense, uint8_t hasDischargeBeenSet);

// Checks if stateInfo->hasChargeBeenSet is set but charge disconnected
const uint8_t chargeDisconnectedDuringContactorSetting(const uint8_t chargeSense, uint8_t hasChargeBeenSet);
