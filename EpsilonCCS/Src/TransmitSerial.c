#include "TransmitSerial.h"

#include "stm32f4xx_hal_uart.h"
#define LIGHT_ON 1
#define LIGHT_OFF 0

void transmitMessage(uint8_t* payload, uint16_t bufferSize)
{
    while (1) //In case of busy
    {
        HAL_StatusTypeDef transmitState = HAL_UART_Transmit(&huart3, payload, bufferSize, 5000);

        if (transmitState == HAL_TIMEOUT || transmitState == HAL_ERROR)
        {
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, LIGHT_ON);
        }
        else
        {
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, LIGHT_OFF);
        }
    }
}
