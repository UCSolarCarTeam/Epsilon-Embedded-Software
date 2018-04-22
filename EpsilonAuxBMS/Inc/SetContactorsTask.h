#pragma once
#include "AuxBmsTasks.h"
#include "stm32f4xx_hal_adc.h"
#include "OrionStatus.h"

extern ADC_HandleTypeDef hadc1; // main.c

// Task for turning on contactors
// arg: osMutexId* auxStatusMutex
void setAuxContactorsTask(void const* arg);
// Function for reading current amplifier value
void readCurrent(void);
// Function for checking if a specific contactor is on
int checkContactor(uint8_t pin, uint8_t port, uint8_t current_multiplier);
