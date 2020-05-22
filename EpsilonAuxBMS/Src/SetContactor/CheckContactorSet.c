#include "CheckContactorSet.h"

static const float CURRENT_SENSE_RESISTOR  = 0.001; //actually 1 mOhm, but 1 Ohm for testing
static const int GAIN  = 250;
static const uint32_t SENSE_SETUP_TIME  = 50; // Setup time for current_sense
static const float CURRENT_LOWER_THRESHOLD  = 0.25; // Lower current threshold
static const uint32_t ADC_POLL_TIMEOUT  = 10;
static const int NUM_SENSES = 3; // The number of senses to take at a time when reading current amplifier
static const int MAX_SENSE_DIFF = 10; // The maximum difference between senses read from current amplifier
static const int MAX_CURRENT_READ_ATTEMPTS = 50; // The maximum number of times to try obtaining a settled CURRENT_SENSE


int isContactorSet(uint16_t pin, GPIO_TypeDef* port, int current_multiplier)
{
    float pwrVoltage = 0.0;
    float pwrCurrent = 0.0;
    uint8_t sense_pin;
    // Get 12bit current analog input
    uint32_t current_sense = readCurrentThroughContactors();

    if (current_sense == 0xDEADBEEF)
    {
        return 0;
    }

    pwrVoltage = 3.3 * current_sense / 0xFFF / GAIN; // change ADC value into the voltage read
    pwrCurrent = pwrVoltage / CURRENT_SENSE_RESISTOR; // convert voltage to current

    if (pin == 0xFF && port == NULL)
    {
        sense_pin = 1;
    }
    else
    {
        sense_pin = !HAL_GPIO_ReadPin(port, pin);
    }

    // If current is high for some reason or sense pin is high something is wrong
    if (pwrCurrent > CURRENT_LOWER_THRESHOLD * current_multiplier || !sense_pin)
    {
        return 0;
    }

    return 1;
}

/*
  This function takes multiple reads because the current amplifier has a bit of a setup time
  to make sure that erroneous values aren't read, multiple reads are taken till we have a
  relatively steady value.
*/
uint32_t readCurrentThroughContactors(void)
{
    // Enable current amplifier. Allow time to settle and take multiple valid concurrent readings
    // to make sure it isn't changing
    HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_SET);
    uint32_t senses[NUM_SENSES];
    uint32_t returnValue = 0xDEADBEEF;
    int count = 0;

    // Want to limit it so we don't get an infinite loop
    while (count < MAX_CURRENT_READ_ATTEMPTS)
    {
        resetSenseArray(senses);
        osDelay(SENSE_SETUP_TIME);
        int senseStable = isSenseStable(senses);

        if (senseStable == 1)
        {
            HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_RESET);
            returnValue =  senses[0];
            break;
        }
        else if (senseStable == -1)
        {
            HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_RESET);
            break;
        }

        count++;
    }

    HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_RESET);
    return returnValue;
}

/*
  Multiple reads are taken and buffered into an array. The maximum and minimum value read are
  found. Since these values will have the greatest difference, their difference must be less than
  or equal to the allowable difference. If it is, the CURRENT_SENSE is stable, otherwise, it is deemed
  not stable
*/
int isSenseStable(uint32_t senses[])
{
    for (int counter = 0; counter < NUM_SENSES; counter++)
    {
        for (int i = NUM_SENSES - 1; i > 0; i--)
        {
            senses[i] = senses[i - 1];
        }

        if (HAL_ADC_PollForConversion(&hadc1, ADC_POLL_TIMEOUT) == HAL_OK)
        {
            senses[0] = HAL_ADC_GetValue(&hadc1);
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
        if (senses[i] > max)
        {
            max = senses[i];
        }

        if (senses[i] < min)
        {
            min = senses[i];
        }
    }

    if (max - min > MAX_SENSE_DIFF)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void resetSenseArray(uint32_t senses[])
{
    for (int i = 0; i < NUM_SENSES; i++)
    {
        senses[i] = 0;
    }
}
