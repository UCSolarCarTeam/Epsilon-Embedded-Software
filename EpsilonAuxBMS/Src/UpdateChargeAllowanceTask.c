#include "UpdateChargeAllowanceTask.h"

static const uint32_t CHARGE_ALLOWANCE_UPDATE_FREQ = 50; // Every 100ms
// These values are subject to change based on what regulations say
// Ask Dan if any issues
static const float MAX_CELL_VOLTAGE = 3.8;
static const float MIN_CELL_VOLTAGE = 3.2;
typedef enum ChargeAllowanceState {VOLTAGE_IN_RANGE,
                                   OUT_OF_RANGE
                                  } ChargeAllowanceState;

void updateChargeAllowanceTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();
    ChargeAllowanceState maxCellVoltageChargeAllowanceState = VOLTAGE_IN_RANGE;
    ChargeAllowanceState minCellVoltageChargeAllowanceState = VOLTAGE_IN_RANGE;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, CHARGE_ALLOWANCE_UPDATE_FREQ);

        /* Orion BMS sends voltages relating to two DIFFERENT cells: the cell with the highest voltage
           and the cell with the lowest voltage. Must check if the voltage of the cell with the highest
           voltage is too high or in range and if the cell with the lowest voltage is too low or in range.
        */
        orionStatus.batteryVoltagesInRange = 1;

        if ((double)orionStatus.maxCellVoltage > MAX_CELL_VOLTAGE * 0.8) // Will have the cutoff to be 20% below
        {
            orionStatus.batteryVoltagesInRange = 0;
            // Turn off charge contactor
            HAL_GPIO_WritePin(CONTACTOR_ENABLE2_GPIO_Port, CONTACTOR_ENABLE2_Pin, GPIO_PIN_RESET);
            maxCellVoltageChargeAllowanceState = OUT_OF_RANGE;
        }
        else
        {
            maxCellVoltageChargeAllowanceState = VOLTAGE_IN_RANGE;
        }

        if ((double)orionStatus.minCellVoltage < MIN_CELL_VOLTAGE * 1.2) // Will have the cutoff to be 20% above
        {
            orionStatus.batteryVoltagesInRange = 0;
            // Turn off High voltage enable and discharge contactor
            HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(CONTACTOR_ENABLE3_GPIO_Port, CONTACTOR_ENABLE3_Pin, GPIO_PIN_RESET);
            minCellVoltageChargeAllowanceState = OUT_OF_RANGE;
        }
        else
        {
            minCellVoltageChargeAllowanceState = VOLTAGE_IN_RANGE;
        }

        // Setting the allowance of charge and charge/discharge contactor state for auxStatus
        if (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK)
        {
            continue;
        }

        if (maxCellVoltageChargeAllowanceState == VOLTAGE_IN_RANGE)
        {
            auxStatus.allowCharge = 1;
        }
        else
        {
            auxStatus.chargeContactorState = 0;
            auxStatus.allowCharge = 0;
        }

        if (minCellVoltageChargeAllowanceState == OUT_OF_RANGE)
        {
            auxStatus.dischargeContactorState = 0;
        }

        osMutexRelease(auxStatus.auxStatusMutex);
    }
}
