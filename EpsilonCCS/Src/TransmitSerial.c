#include "TransmitSerial.h"

#include "stm32f4xx_hal_uart.h"

static unsigned char UartReady = 1;

void transmitMessage(uint8_t* payload, uint16_t bufferSize)
{
    while (!UartReady)
    {
    }

    UartReady = 0; // False
    HAL_UART_Transmit_DMA(&huart3, payload, bufferSize);
}

// Reimplement from weak implementation at stm32f4xx_hal_uart.c
// Should get called sometime after transmitMessage() once the transmit is complete
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* UartHandle)
{
    /* Set transmission flag: transfer complete */
    UartReady = 1; // True
}
