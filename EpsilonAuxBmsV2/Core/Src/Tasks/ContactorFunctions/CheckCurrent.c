#include "CheckCurrent.h"

/*
Reads the current and determines if it's low or not based on the number of contactors closed.
*/
uint8_t isCurrentLow(uint8_t numContactorsClosed)
{
    uint32_t adcVal = readCurrentThroughContactors();
    float currentThreshold = (numContactorsClosed == 0) ? CURRENT_LOWER_THRESHOLD : CURRENT_LOWER_THRESHOLD * numContactorsClosed;


    float pwrVoltage = adcVal * (3.3 / 0xFFF) / GAIN; // change ADC value into the voltage read
    float pwrCurrent = pwrVoltage / CURRENT_SENSE_RESISTOR; // convert voltage to current
    return (pwrCurrent <= currentThreshold);
}

uint32_t readCurrentThroughContactors(void)
{
    uint32_t stableCurrentValue = 0xDEADBEEF;
    int count = 0;

    while (count < MAX_CURRENT_READ_ATTEMPTS)
    {
        uint32_t senses[NUM_SENSES];

        osDelay(SENSE_SETUP_TIME);
        int currentStable = isCurrentStable(senses);

        if (currentStable == 1)
        {
            stableCurrentValue = senses[0];
            break;
        }
        else if (currentStable == -1)
        {
            break;
        }

        count++;
    }

    return stableCurrentValue;
}

int isCurrentStable(uint32_t senses[])
{

    for (int i = 0; i < NUM_SENSES; i++)
    {
        if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
        {
            senses[i] = HAL_ADC_GetValue(&hadc1);
        }
        else
        {
            return -1;
        }
    }

    uint32_t max = senses[0];
    uint32_t min = senses[0];

    for (int i = 1; i < NUM_SENSES; i++)
    {
        max = (senses[i] > max) ? senses[i] : max;
        min = (senses[i] < min) ? senses[i] : min;
    }

    if ((max - min) < MAX_SENSE_DIFF)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
