#include "CheckCurrent.h"

/*
Reads the current and determines if it's low or not based on the number of contactors closed.
*/
uint8_t isCurrentLow(uint8_t numContactorsClosed)
{
    uint32_t adcVal;
    float currentThreshold = (numContactorsClosed == 0) ? CURRENT_LOWER_THRESHOLD : CURRENT_LOWER_THRESHOLD * numContactorsClosed;

    if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
    {
        adcVal = HAL_ADC_GetValue(&hadc1);
    }
    else
    {
        return 0;
    }

    float pwrVoltage = adcVal * (3.3 / 0xFFF) / GAIN; // change ADC value into the voltage read
    float pwrCurrent = pwrVoltage / CURRENT_SENSE_RESISTOR; // convert voltage to current
    return (pwrCurrent <= currentThreshold);
}
