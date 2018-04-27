#include "ReadAuxVoltageTask.h"

static const uint32_t AUX_UPDATE_AUX_VOLTAGE_FREQ = 100; // Every 100ms
static const uint32_t SPI_TIMEOUT = 50;
static const float AUX_NOMINAL_VOLTAGE = 12.0;
static const uint16_t AUX_ADC_NOMINAL_OUTPUT = 0x32E; // Pattern = 2.63/3.3 * 0x3FF

void readAuxVoltageTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_UPDATE_AUX_VOLTAGE_FREQ);

        // Store voltage value from SPI
        uint16_t spiVoltage = 0;
        // Data buffer
        uint8_t rxBuff[AUX_VOLTAGE_MSG_SIZE] = {0};
        // Set Chip Select to be low
        HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET);

        // Start SCLK and receive input
        if (HAL_SPI_Receive(&hspi3, rxBuff, AUX_VOLTAGE_MSG_SIZE, SPI_TIMEOUT) == HAL_OK)
        {
            // Set Chip Select to be high. Placed here to keep pin low as short as possible
            HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);
            // ADC sends 10 bits of data with MSB being sent first.
            // ADC sends 4 zeros before sending data, so can ignore top 4 bits of rxBuff[1] (only want bottom 4 bits).
            // Want bit 3 of rxBuff[1] to become bit 9 so left shift
            uint16_t upperBits = (uint16_t)(rxBuff[1] << 6);
            // Bottom 2 bits of rxBuff[0] will be meaningless so ignore as well (only want top 6 bits).
            // Right shift lowerBits to remove bottom 2 bits and line up bit 7 of rxBuff[0] to bit 5
            uint16_t lowerBits = (uint16_t)((rxBuff[0]) >> 2);
            spiVoltage =  0x03FF & (upperBits | lowerBits);
        }
        else
        {
            // Set Chip Select to be high. Placed here to keep pin low as short as possible
            HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);
            spiVoltage = 0xFFFF;
        }


        if (spiVoltage == 0xFFFF) // Something went wrong during SPI-ADC read
        {
            auxStatus.auxVoltage = 0x1F;
        }
        else
        {
            float relative_voltage = AUX_NOMINAL_VOLTAGE * spiVoltage / AUX_ADC_NOMINAL_OUTPUT;
            auxStatus.auxVoltage = ((int)round(relative_voltage)) & 0x1F; // Round and keep bottom 5 bits
        }
    }
}
