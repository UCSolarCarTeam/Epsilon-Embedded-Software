#include "UpdateChargeAllowanceTask.h"

static const uint32_t CHARGE_ALLOWANCE_UPDATE_FREQ = 100; // Every 100ms
// These values are subject to change based on what regulations say
// Ask Dan if any issues
static const float MAX_CELL_VOLTAGE = 3.8;
static const float MIN_CELL_VOLTAGE = 3.2;
static const float DEFAULT_VOLTAGE_UNITS = 0.0001; // From: https://www.orionbms.com/manuals/utility/

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
        if (orionStatus.shutOff)
        {
            continue;
        }

        uint8_t voltagesInRange = 1;
        uint8_t allowCharge = 1;
        uint8_t allowDischarge = 1;
        uint8_t contactorOverride = 0;
        uint8_t commonContactorOff = 0;
        uint8_t shutOff = 0;

        if (auxStatus.startUpSequenceDone)
        {
            if (driversInput.forwardReverse) // In forward or reverse
            {
                if (!HAL_GPIO_ReadPin(CHARGE_ENABLE_SENSE_GPIO_Port, CHARGE_ENABLE_SENSE_Pin) ||
                        !HAL_GPIO_ReadPin(DISCHARGE_ENABLE_SENSE_GPIO_Port, DISCHARGE_ENABLE_SENSE_Pin))
                {
                    contactorOverride = 1;
                    commonContactorOff = 1;
                    allowCharge = 0;
                    allowDischarge = 0;
                    shutOff = 1;
                    // Turn all contactors and high voltage enable off
                    HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(COMMON_CONTACTOR_ENABLE_GPIO_Port, COMMON_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(CHARGE_CONTACTOR_ENABLE_GPIO_Port, CHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(DISCHARGE_CONTACTOR_ENABLE_GPIO_Port, DISCHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
                }
            }
            else // Aux mode
            {
                if (!HAL_GPIO_ReadPin(CHARGE_ENABLE_SENSE_GPIO_Port, CHARGE_ENABLE_SENSE_Pin))
                {
                    contactorOverride = 1;
                    allowCharge = 0;
                    // Turn off charge contactor
                    HAL_GPIO_WritePin(CHARGE_CONTACTOR_ENABLE_GPIO_Port, CHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);

                    if (!HAL_GPIO_ReadPin(DISCHARGE_ENABLE_SENSE_GPIO_Port, DISCHARGE_ENABLE_SENSE_Pin))
                    {
                        allowDischarge = 0;
                        commonContactorOff = 1;
                        shutOff = 1;
                        // Turn off common and discharge contactor, and high voltage enable
                        HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
                        HAL_GPIO_WritePin(COMMON_CONTACTOR_ENABLE_GPIO_Port, COMMON_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
                        HAL_GPIO_WritePin(DISCHARGE_CONTACTOR_ENABLE_GPIO_Port, DISCHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
                    }
                }
                else
                {
                    if (!HAL_GPIO_ReadPin(DISCHARGE_ENABLE_SENSE_GPIO_Port, DISCHARGE_ENABLE_SENSE_Pin))
                    {
                        contactorOverride = 1;
                        allowDischarge = 0;
                        // Turn off discharge contactor and high voltage enable
                        HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
                        HAL_GPIO_WritePin(DISCHARGE_CONTACTOR_ENABLE_GPIO_Port, DISCHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
                    }
                }
            }
        }


        if (DEFAULT_VOLTAGE_UNITS * orionStatus.maxCellVoltage > MAX_CELL_VOLTAGE)
        {
            voltagesInRange = 0;
            contactorOverride = 1;

            if (allowCharge) // To avoid wasting time writing to the pin again
            {
                allowCharge = 0;
                // Turn off charge contactor
                HAL_GPIO_WritePin(CHARGE_CONTACTOR_ENABLE_GPIO_Port, CHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
            }
        }

        if (DEFAULT_VOLTAGE_UNITS * orionStatus.minCellVoltage < MIN_CELL_VOLTAGE)
        {
            voltagesInRange = 0;
            contactorOverride = 1;

            if (allowDischarge) // To avoid wasting time writing to the pin again
            {
                allowDischarge = 0;
                // Turn off High voltage enable and discharge contactor
                HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(DISCHARGE_CONTACTOR_ENABLE_GPIO_Port, DISCHARGE_CONTACTOR_ENABLE_Pin, GPIO_PIN_RESET);
            }
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
            auxStatus.highVoltageEnableState = 0;
        }

        if (commonContactorOff)
        {
            auxStatus.commonContactorState = 0;
            auxStatus.strobeBmsLight = 1;
        }

        osMutexRelease(auxStatus.auxStatusMutex);

        // Setting the battery voltage in range indicator for orionStatus
        if (osMutexWait(orionStatus.orionStatusMutex, 0) != osOK)
        {
            continue;
        }

        orionStatus.batteryVoltagesInRange = voltagesInRange;
        orionStatus.allowCharge = allowCharge;
        orionStatus.allowDischarge = allowDischarge;
        orionStatus.shutOff = shutOff;

        if (contactorOverride)
        {
            orionStatus.contactorOverriden = 1;
        }

        osMutexRelease(orionStatus.orionStatusMutex);
    }
}
