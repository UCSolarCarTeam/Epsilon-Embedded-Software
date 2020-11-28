#include "unity.h"
#include "ReadAuxVoltageTaskTest.h"
#include "ReadAuxVoltageTask.h"
#include "Mockstm32f4xx_hal_spi.h"
#include "Mockstm32f4xx_hal_gpio.h"
#include "Mockcmsis_os2.h"

osMutexId_t auxStatusReadAuxVoltageMutex;
AuxStatus auxStatus;
SPI_HandleTypeDef hspi3;
uint8_t spiRxBuff[2] = {0};

void runReadAuxVoltageTaskTests()
{
    RUN_TEST(test_checkValidVoltageRead);

}

void test_checkValidVoltageRead()
{
    uint32_t prevWakeTime = 0;
    HAL_GPIO_WritePin_Expect(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Receive_DMA_ExpectAndReturn(&hspi3, spiRxBuff, 2, HAL_OK);
    osThreadFlagsWait_ExpectAndReturn(0x1, osFlagsWaitAny, SPI_TIMEOUT, 0x1);
    HAL_GPIO_WritePin_Expect(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);
    osMutexAcquire_ExpectAndReturn(auxStatusReadAuxVoltageMutex, 100, osOK);
    osMutexRelease_ExpectAndReturn(auxStatusReadAuxVoltageMutex, 0);
    osDelayUntil_ExpectAndReturn(prevWakeTime + READ_AUX_VOLTAGE_TASK_FREQ, 0);

    spiRxBuff[1] = 0b00001100;
    spiRxBuff[0] = 0b10111000;

    readAuxVoltage(&prevWakeTime);
    TEST_ASSERT_EQUAL_MESSAGE(12, auxStatus.auxVoltage, "AuxVoltage not 12 volts");

}
