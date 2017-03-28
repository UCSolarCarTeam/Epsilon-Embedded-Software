#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_hal_uart.h"

extern UART_HandleTypeDef huart3;

void transmitMessage(uint8_t* payload, uint16_t bufferSize);
