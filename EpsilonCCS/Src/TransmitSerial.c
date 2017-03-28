#include "TransmitSerial.h"

#include "stm32f4xx_hal_uart.h"

static unsigned char UartReady = 1;

void transmitMessage(uint8_t* payload, uint16_t bufferSize)
{
	HAL_UART_Transmit(&huart3, payload, bufferSize, 5000);
}
