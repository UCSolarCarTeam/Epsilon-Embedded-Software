#include "AuxBMSTasks.h"

void updateAuxVoltageTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();
    // Store voltage value
    uint16_t voltage = 0;
    // Size of data to be received
    uint32_t size = 2;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_UPDATE_AUX_VOLTAGE_FREQ);

        // Data buffer
        uint8_t rxBuff[2] = {0};
        // Set Chip Select to be low
        HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET);

        // Start SCLK and receive input
        if (HAL_SPI_Receive(&hspi3, rxBuff, size, SPI_TIMEOUT) == HAL_OK)
        {
            // ADC sends 4 zeros before sending data,
            //so can ignore top 4 bits of rxBuff[0] and bottom 2 bits of rxBuff[1]
            uint16_t upperBits = (uint16_t)(rxBuff[1] << 6);
            uint16_t lowerBits = (uint16_t)(rxBuff[0] & 0xF6);
            voltage =  0x03FF & ((upperBits) | (lowerBits >> 2)); // The top 6 bits should be zeros, but just in case
        }
        else
        {
            voltage = 0;
        }

        // Set Chip Select to be high
        HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);

        float relative_voltage = AUX_NOMINAL_VOLTAGE * voltage / AUX_ADC_NOMINAL_OUTPUT;

        // Rounding
        float diff = relative_voltage - (int)relative_voltage;

        // If the decimal place is less than 0.5, truncate and round down, else round up
        // and keep bottom 5 bits
        if (diff < 0.5)
        {
            auxStatus.auxVoltage = ((int)relative_voltage) & 0x1F;
        }
        else
        {
            auxStatus.auxVoltage = ((int)relative_voltage + 1) & 0x1F;
        }
    }
}
