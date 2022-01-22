#pragma once
#include "AuxBmsTasks.h"
#include "OrionStatus.h"

static const uint32_t CHARGE_ALLOWANCE_UPDATE_FREQ = 100; // Every 100ms

// Task for determining the allowance of charge/discharge based on orion cell voltages
// It will turn off charge contactor if max cell voltage is too high,
// and discharge contactor is min cell voltage is too low
// arg: osMutexId* auxStatusMutex
void updateChargeAllowanceTask(void const* arg);

void updateChargeAllowance(uint32_t* prevWakeTime);