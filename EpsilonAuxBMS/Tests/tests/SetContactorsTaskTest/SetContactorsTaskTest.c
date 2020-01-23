#include "SetContactorsTaskTest.h"

#include "unity.h"

#include "Mockstm32f4xx_hal_gpio.h"
#include "Mockcmsis_os.h"
#include "SetContactorsTask.h"


void runSetContactorsTests()
{
    RUN_TEST(test_disconnectContactors);
}

void test_disconnectContactors()
{
    HAL_GPIO_WritePin_Expect(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin_Expect(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_RESET);
    osMutexWait_ExpectAndReturn(auxStatus.auxStatusMutex, 0, osOK);
    osMutexRelease_ExpectAndReturn(auxStatus.auxStatusMutex, osOK);
    disconnectContactors(1);
}