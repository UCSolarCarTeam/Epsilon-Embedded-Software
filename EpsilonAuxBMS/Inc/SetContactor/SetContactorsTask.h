#pragma once
#include "AuxBmsTasks.h"
#include "stm32f4xx_hal_adc.h"
#include "OrionStatus.h"
#include "CheckContactorSet.h"
#include "FirstCheck.h"
#include "CommonEnableCheck.h"
#include "ChargeEnableCheck.h"
#include "DischargeEnableCheck.h"
#include "Idle.h"
#include "Blocked.h"
#include "StateFunctionsShared.h"

// Task for turning on contactors
// arg: osMutexId* auxStatusMutex
void setContactorsTask(void const* arg);

//First check before setting states in case battery voltages are not in range, start up sequence is not done and makes sure the state is not disconnected
const uint8_t safeToSetContactor(ContactorsSettingState state);
