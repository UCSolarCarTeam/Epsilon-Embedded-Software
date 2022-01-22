#include "TransmitSerial.h"

#include "stm32f4xx_hal_uart.h"

/*
* Flashing Red LED: The USART line is has errored sending this message, will re-try
* Solid Red LED: The USART Port has timed out, check if the other end of the serial port is initialized and listening
*/
void transmitMessage(uint8_t* payload, uint16_t bufferSize)
{
    uint16_t attemptCount = 0;

    while (attemptCount < NUMBER_OF_ATTEMPTS)
    {
        HAL_StatusTypeDef transmitState = HAL_UART_Transmit(&huart3, payload, bufferSize, USART_TIMEOUT_PERIOD);

        if (transmitState == HAL_OK)
        {
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, LED_OFF);
            return;
        }
        else if (transmitState == HAL_TIMEOUT)
        {
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, LED_ON);
            attemptCount++;
        }
        else
        {
            HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
            attemptCount++;
        }

    }
}
