#pragma once
#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;

static const float CURRENT_SENSE_RESISTOR  = 0.001; // 1 mOhm
static const int GAIN  = 250;
static const float CURRENT_LOWER_THRESHOLD  = 0.25; // Lower current threshold (A)
static const uint32_t ADC_POLL_TIMEOUT  = 10;

uint8_t isCurrentLow(uint8_t numContactorsClosed);
