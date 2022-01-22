#pragma once

#include "stm32f4xx.h"
#include "stm32f4xx_hal_uart.h"

#define LED_ON 1
#define LED_OFF 0
#define NUMBER_OF_ATTEMPTS 3

#define USART_TIMEOUT_PERIOD 5000

extern UART_HandleTypeDef huart3;

void transmitMessage(uint8_t* payload, uint16_t bufferSize);
