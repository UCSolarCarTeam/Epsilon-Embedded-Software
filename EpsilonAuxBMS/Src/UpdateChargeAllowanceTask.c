#include "UpdateChargeAllowanceTask.h"

static const uint32_t CHARGE_ALLOWANCE_UPDATE_FREQ = 50; // Every 100ms
// These values are subject to change based on what regulations say
// Ask Dan if any issues
static const float MAX_CELL_VOLTAGE = 3.8;
static const float MIN_CELL_VOLTAGE = 3.2;

void updateChargeAllowanceTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        osDelayUntil(&prevWakeTime, CHARGE_ALLOWANCE_UPDATE_FREQ);

        /* Orion BMS sends voltages relating to two DIFFERENT cells: the cell with the highest voltage
           and the cell with the lowest voltage. Must check if the voltage of the cell with the highest
           voltage is too high or in range and if the cell with the lowest voltage is too low or in range.
        */

        uint8_t voltagesInRange = 1;
        uint8_t allowCharge = 1;
        uint8_t allowDischarge = 1;

        if ((double)orionStatus.maxCellVoltage > MAX_CELL_VOLTAGE * 0.8) // Will have the cutoff to be 20% below
        {
            voltagesInRange = 0;
            allowCharge = 0;
            // Turn off charge contactor
            HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CHARGE_CONTACTOR_ENABLE, GPIO_PIN_RESET);
        }

        if ((double)orionStatus.minCellVoltage < MIN_CELL_VOLTAGE * 1.2) // Will have the cutoff to be 20% above
        {
            voltagesInRange = 0;
            allowDischarge = 0;
            // Turn off High voltage enable and discharge contactor
            HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(DISCHARGE_CONTACTOR_ENABLE_GPIO_Port, DISCHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
        }

        // Setting the allowance of charge and charge/discharge contactor state for auxStatus
        if (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK)
        {
            continue;
        }

        auxStatus.allowCharge = allowCharge;

        if (!allowCharge)
        {
            auxStatus.chargeContactorState = 0;
        }

        if (!allowDischarge)
        {
            auxStatus.dischargeContactorState = 0;
        }

        osMutexRelease(auxStatus.auxStatusMutex);

        // Setting the battery voltage in range indicator for orionStatus
        if (osMutexWait(orionStatus.orionStatusMutex, 0) != osOK)
        {
            continue;
        }

        orionStatus.batteryVoltagesInRange = voltagesInRange;

        osMutexRelease(orionStatus.orionStatusMutex);
    }
}
