#pragma once
#include "AuxBmsTasks.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "OrionStatus.h"

extern ADC_HandleTypeDef hadc1; // main.c

// Task for turning on contactors
// arg: osMutexId* auxStatusMutex
void setContactorsTask(void const* arg);
