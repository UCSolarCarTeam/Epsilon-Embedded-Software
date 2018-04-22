#pragma once

#include "AuxBmsTasks.h"
#include "stm32f4xx_hal_spi.h"
#include <math.h>

extern SPI_HandleTypeDef hspi3; // main.c

// Task for reading aux voltage
// arg: NULL
void readAuxVoltageTask(void const* arg);
