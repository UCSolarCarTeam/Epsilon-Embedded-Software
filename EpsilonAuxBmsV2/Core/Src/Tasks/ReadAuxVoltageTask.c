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
    if (HAL_SPI_Receive_DMA(&hspi3, spiRxBuff, AUX_BMS_SPI_BUFFER_SIZE) == HAL_OK)
    {
        //Wait for spi to finish
        uint32_t flags = osThreadFlagsWait(SPI_READY_FLAG, osFlagsWaitAny, SPI_TIMEOUT);

        if (flags == osFlagsErrorTimeout)
        {
            spiVoltage = SPI_ERROR_CODE;
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
        spiVoltage = SPI_ERROR_CODE;
    }

    //Set Chip Select to be high.
    HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);

    if (osMutexAcquire(auxStatusReadAuxVoltageMutex, MUTEX_TIMEOUT) == osOK)
    {
        if (spiVoltage == SPI_ERROR_CODE) // Something went wrong during SPI-ADC read
        {
            auxStatus.auxVoltage = 0x1F; // Set auxVoltage to maximum value (31... this is not expected, as the aux voltage should be ~12, so this indicates an error)
        }
        else
        {
            float relative_voltage = AUX_NOMINAL_VOLTAGE * spiVoltage / AUX_ADC_NOMINAL_OUTPUT;
            auxStatus.auxVoltage = myRound(relative_voltage) & 0x1F; // Round and keep bottom 5 bits
        }

        osMutexRelease(auxStatusReadAuxVoltageMutex);
    }

    *prevWakeTime += READ_AUX_VOLTAGE_TASK_FREQ;
    osDelayUntil(*prevWakeTime);
}

// Doing this because travis can't find <math.h> round
int myRound(float val)
{
    return ((int)(val + 0.5)); // If val fraction >= 0.5, will round up. If val fraction < 0.5, will round down
}
