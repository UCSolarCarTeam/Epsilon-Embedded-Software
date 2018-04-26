#pragma once
#include "AuxBmsTasks.h"
#include "stm32f4xx_hal_adc.h"
#include "OrionStatus.h"

extern ADC_HandleTypeDef hadc1; // main.c

// Task for turning on contactors
// arg: osMutexId* auxStatusMutex
void setContactorsTask(void const* arg);
// Function for reading current amplifier value
uint32_t readCurrent(void);
// Function for checking if a specific contactor is on
int checkContactor(uint16_t pin, GPIO_TypeDef* port, int current_multiplier);
