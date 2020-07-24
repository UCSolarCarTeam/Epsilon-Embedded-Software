#pragma once
#include "StateFunctionsShared.h"

void idle(SetContactorStateInfo* stateInfo);

// Checks if discharge contactor can be turn on
const uint8_t canTurnOnDishargeContactor(const uint8_t dischargeSense);

// Checks if charge contactor can be turn on
const uint8_t canTurnOnChargeContactor(const uint8_t chargeSense);

// Checks if the discharge contactor should be on, but isn't
const uint8_t dischargeDisconnectedUnexpectedly(const uint8_t dischargeSense, const uint8_t isdischargeTurningOn);

// Checks if the charge contactor should be on, but isn't
const uint8_t chargeDisconnectedUnexpectedly(const uint8_t chargeSense, const uint8_t isChargeTurningOn);
