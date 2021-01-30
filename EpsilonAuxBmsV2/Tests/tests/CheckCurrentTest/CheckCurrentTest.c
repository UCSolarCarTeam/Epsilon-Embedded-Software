#include "CheckCurrentTest.h"

ADC_HandleTypeDef hadc1;


uint32_t calcAdcRetVal(float desiredCurrent)
{
    float voltage = desiredCurrent * CURRENT_SENSE_RESISTOR;
    float boostedVoltage = voltage * GAIN;
    uint32_t adcVal = (uint32_t)(boostedVoltage * 0xFFFU / 3.3);  // ADC is 12 bits so max value is 0xFFF
    return adcVal;
}

void currentLowTest()
{
    float currentToRead = CURRENT_LOWER_THRESHOLD / 2;
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, calcAdcRetVal(currentToRead));
    uint8_t currentLow = isCurrentLow(0);
    TEST_ASSERT_EQUAL_MESSAGE(1, currentLow, "Current not low");
}

void currentHighTest()
{
    float currentToRead = CURRENT_LOWER_THRESHOLD * 5;
    printf("%.2f\n", currentToRead);
    HAL_ADC_PollForConversion_ExpectAndReturn(&hadc1, ADC_POLL_TIMEOUT, HAL_OK);
    HAL_ADC_GetValue_ExpectAndReturn(&hadc1, calcAdcRetVal(currentToRead));
    uint8_t currentLow = isCurrentLow(2);
    TEST_ASSERT_EQUAL_MESSAGE(0, currentLow, "Current not high");
}

int runCheckCurrentTest()
{
    UNITY_BEGIN();

    RUN_TEST(currentLowTest);
    RUN_TEST(currentHighTest);

    return UNITY_END();
}
