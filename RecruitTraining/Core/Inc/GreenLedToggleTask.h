#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_can.h"
#include "cmsis_os.h"

//TODO: Define an external CAN handle (same variable as in main)
//TODO: Define external CAN_Tx header (same variable as in main)
//TODO: Define external green LED toggle task (same variable as in main)
void greenLedToggleTask(void const* arg);
