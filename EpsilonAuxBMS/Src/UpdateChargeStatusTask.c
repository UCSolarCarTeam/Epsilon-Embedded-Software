#include "UpdateChargeStatusTask.h"

static const uint32_t CHARGE_STATUS_UPDATE_FREQ = 1; // Every 1ms
// These values are subject to change based on what regulations say
// Ask Dan if any issues
static const float MAX_CELL_VOLTAGE = 3.8;
static const float MIN_CELL_VOLTAGE = 3.2;

void updateChargeStatusTask(void const* arg)
{
    // For concurrency with setContactorsTask()
    osMutexId* auxStatusMutex = (osMutexId*) arg;
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, CHARGE_STATUS_UPDATE_FREQ);

        if (osMutexWait(auxStatusMutex, 0) != osOK)
        {
            continue;
        }

        if ((double)orionStatus.maxCellVoltage > MAX_CELL_VOLTAGE * 0.8) // Will have the cutoff to be 20% below
        {
            orionStatus.batteryVoltagesInRange = 0;
            // Turn off charge contactor
            HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin, GPIO_PIN_RESET);
            auxStatus.chargeContactorState = 0;
            auxStatus.allowCharge = 0;
        }
        else
        {
            orionStatus.batteryVoltagesInRange = 1;
            auxStatus.allowCharge = 1;
        }

        if ((double)orionStatus.minCellVoltage < MIN_CELL_VOLTAGE * 1.2) // Will have the cutoff to be 20% above
        {
            orionStatus.batteryVoltagesInRange = 0;
            // Turn off High voltage enable and discharge contactor
            HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_RESET);
            auxStatus.dischargeContactorState = 0;
        }
        else
        {
            orionStatus.batteryVoltagesInRange = 1;
        }
    }

    osMutexRelease(auxStatusMutex);
}