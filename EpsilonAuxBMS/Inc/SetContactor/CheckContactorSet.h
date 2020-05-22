#pragma once
#include "AuxBmsTasks.h"
#include "stm32f4xx_hal_adc.h"

extern ADC_HandleTypeDef hadc1; // main.c

// Function for reading current through the contactors.
// The current is read from the current amplifier
uint32_t readCurrentThroughContactors(void);
// Function for checking if a specific contactor has been set
int isContactorSet(uint16_t pin, GPIO_TypeDef* port, int current_multiplier);
// Resets sense array to all 0s
void resetSenseArray(uint32_t senses[]);
// Function for checking if CURRENT_SENSE is at a stable value
int isSenseStable(uint32_t senses[]);
