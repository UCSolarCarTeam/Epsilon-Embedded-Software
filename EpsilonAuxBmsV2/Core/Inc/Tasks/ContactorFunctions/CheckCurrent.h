#pragma once
#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

static const float CURRENT_SENSE_RESISTOR  = 0.001; // 1 mOhm
static const int GAIN  = 250;
static const float CURRENT_LOWER_THRESHOLD  = 0.25; // Lower current threshold (A)
static const uint32_t ADC_POLL_TIMEOUT  = 10;

static const int MAX_SENSE_DIFF = 10; // The maximum difference between senses read from current amplifier
static const int MAX_CURRENT_READ_ATTEMPTS = 50; // The maximum number of times to try obtaining a stable CURRENT_SENSE
static const uint32_t SENSE_SETUP_TIME  = 50; // Setup time for current_sense
static const int NUM_SENSES = 3; // The number of senses to take at a time when reading current amplifier


uint8_t isCurrentLow(uint8_t numContactorsClosed);

int isCurrentStable(uint32_t senses[]);
uint32_t readCurrentThroughContactors(void);