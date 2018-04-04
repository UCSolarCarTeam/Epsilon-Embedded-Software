#include "AuxBMSTasks.h"

void readOrionInputTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, AUX_READ_ORION_ENABLE_FREQ);

        if (orionBmsInputs[0] > MAX_VOLTAGE)
        {
            batteryVoltagesOK = 0;
            // Turn off charge contactor
            HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin, GPIO_PIN_RESET);
            auxStatus.chargeContactorState = 0;
            auxStatus.allowCharge = 0;
        }
        else
        {
            batteryVoltagesOK = 1;
            auxStatus.allowCharge = 1;
        }

        if (orionBmsInputs[1] < MIN_VOLTAGE)
        {
            batteryVoltagesOK = 0;
            // Turn off High voltage enable and discharge contactor
            HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_RESET);
            auxStatus.dischargeContactorState = 0;
        }
        else
        {
            batteryVoltagesOK = 1;
        }
    }
}
