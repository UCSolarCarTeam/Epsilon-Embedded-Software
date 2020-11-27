#include "ReadAuxVoltageTask.h"

void readAuxVoltageTask(void* arg)
{
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        readAuxVoltage(&prevWakeTime);
    }
}

/*
Reads the auxiliary battery voltage via SPI.
Triggers the SPI read to occur, then waits for a notification that will occur when the SPI read is done.
It reads the data and transforms it to determine the voltage and updates the voltage field of the AuxStatus variable
If the auxVoltageReadyNotification times out, there was an error so set voltage to an error value
It must acquire the auxStatusReadAuxVoltageMutex before updating the AuxStatus variable
*/
void readAuxVoltage(uint32_t* prevWakeTime)
{
    // Store voltage value from SPI
    uint16_t spiVoltage = 0;

    // Set Chip Select to be low
    HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET);

    // Initiate SPI read
    if (HAL_SPI_Receive_DMA(&hspi3, spiRxBuff, 2) == HAL_OK)
    {
        //Wait for spi to finish
        uint32_t flags = osThreadFlagsWait(0x1, osFlagsWaitAny, SPI_TIMEOUT);

        if (flags == osFlagsErrorTimeout)
        {
            spiVoltage = 0xDEAD;
        }
        else
        {
            // ADC sends 14 bits with 10 bits being useful, however, the SPI can only receive
            // multiples of 8bits or 16bits. For example: 0000 1111 1111 1100
            // rxBuff[1] = 0000 1111
            // rxBuff[0] = 1111 1100
            // data = 11 1111 1111
            spiVoltage =  0x03FF & ((uint16_t)(spiRxBuff[1] << 6) | (uint16_t)((spiRxBuff[0]) >> 2));
        }

    }
    else
    {
        spiVoltage = 0xDEAD;
    }

    //Set Chip Select to be high.
    HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);

    if (osMutexAcquire(auxStatusReadAuxVoltageMutex, 100) == osOK)
    {
        if (spiVoltage == 0xDEAD) // Something went wrong during SPI-ADC read
        {
            auxStatus.auxVoltage = 0x1F;
        }
        else
        {
            float relative_voltage = AUX_NOMINAL_VOLTAGE * spiVoltage / AUX_ADC_NOMINAL_OUTPUT;
            auxStatus.auxVoltage = ((int)round(relative_voltage)) & 0x1F; // Round and keep bottom 5 bits
        }

        osMutexRelease(auxStatusReadAuxVoltageMutex);
    }

    *prevWakeTime += READ_AUX_VOLTAGE_TASK_FREQ;
    osDelayUntil(*prevWakeTime);
}
