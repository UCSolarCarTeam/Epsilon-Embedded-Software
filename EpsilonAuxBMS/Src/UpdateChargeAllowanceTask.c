#include "UpdateChargeAllowanceTask.h"
#include "Trip.h"

// These values are subject to change based on what regulations say
// Ask Dan if any issues

static const float AUX_BMS_MAX_CELL_VOLTAGE = 4.15;
static const float AUX_BMS_MIN_CELL_VOLTAGE = 2.60;

static const float DEFAULT_VOLTAGE_UNITS = 0.0001; // From: https://www.orionbms.com/manuals/utility/

// From SetContactorsTask.c
extern void disconnectContactors(uint8_t updateAuxStatus);

void updateChargeAllowance(uint32_t* prevWakeTime)
{
    osDelayUntil(prevWakeTime, CHARGE_ALLOWANCE_UPDATE_FREQ);

    /* Orion BMS sends voltages relating to two DIFFERENT cells: the cell with the highest voltage
           and the cell with the lowest voltage. Must check if the voltage of the cell with the highest
           voltage is too high or in range and if the cell with the lowest voltage is too low or in range.
        */
    if (orionStatus.shutOff)
    {
        return;
    }

    // Indicates if the max and min cell voltages from orion are in the desired range.
    // Defaults to 1, gets reset to 0 if any of the two cell voltages are not in range.
    // Used for communication with SetContactorsTask
    uint8_t voltagesInRange = 1;
    // Indicates if, based on the orion charge enable gpio input and the max cell, the charge contactor
    // should be on. Defaults to 1, gets reset to 0 if the max cell voltage is too high or the charge enable
    // gpio input is read as low. Used for communication with SetContactorsTask
    uint8_t allowCharge = 1;
    // Indicates if, based on the orion discharge enable gpio input and the min cell, the discharge contactor
    // should be on. Defaults to 1, gets reset to 0 if the min cell voltage is too low or the discharge enable
    // gpio input is read as low. Used for communication with SetContactorsTask
    uint8_t allowDischarge = 1;
    // Indicates if this task has overriden the charge contactor value (i.e. reset the enable pin to 0 -> turning the contactor off)
    // Defaults to 0, gets set to 1 if any of the orion charge enable goes low, or the max voltage is too high (these
    // are the two cases where orion will shut off the charge contactor). Used for communication with SetContactorsTask
    uint8_t chargeContactorOverride = 0;
    // Indicates if this task has overriden the discharge contactor value (i.e. reset the enable pin to 0 -> turning the contactor off)
    // Defaults to 0, gets set to 1 if any of the orion discharge enable goes low, or the min voltage is too low (these
    // are the two cases where orion will shut off the discharge contactor). Used for communication with SetContactorsTask
    uint8_t dischargeContactorOverride = 0;
    // Indicates if this task has turned all contactors off and hence the turning on of contactors should "shut down". Used for communication with SetContactorsTask
    uint8_t shutOff = 0;

    if (auxStatus.startUpSequenceDone)
    {
        unsigned orionDischargeEnableSense = HAL_GPIO_ReadPin(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin);
        unsigned orionChargeEnableSense = HAL_GPIO_ReadPin(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin);

        // If the car should trip or orion wants both contactors off
        if (carShouldTrip() || (!orionDischargeEnableSense && !orionChargeEnableSense))
        {
            chargeContactorOverride = 1;
            dischargeContactorOverride = 1;
            allowCharge = 0;
            allowDischarge = 0;
            shutOff = 1;
            // Turn all contactors and high voltage enable off
            disconnectContactors(0);
        }
        else
        {
            // Turn off discharge without tripping
            if (!orionDischargeEnableSense)
            {
                dischargeContactorOverride = 1;
                allowDischarge = 0;
                HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
            }

            // Turn off charge without tripping
            if (!orionChargeEnableSense)
            {
                chargeContactorOverride = 1;
                allowCharge = 0;
                HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
            }
        }
    }

    if (orionStatus.canMsgReceived)
    {
        if (DEFAULT_VOLTAGE_UNITS * orionStatus.maxCellVoltage > AUX_BMS_MAX_CELL_VOLTAGE)
        {
            voltagesInRange = 0;

            if (allowCharge) // To avoid wasting time writing to the pin again
            {
                allowCharge = 0;
                chargeContactorOverride = 1;
                // Turn off charge contactor
                HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
            }
        }

        if (DEFAULT_VOLTAGE_UNITS * orionStatus.minCellVoltage < AUX_BMS_MIN_CELL_VOLTAGE)
        {
            voltagesInRange = 0;

            if (allowDischarge) // To avoid wasting time writing to the pin again
            {
                allowDischarge = 0;
                dischargeContactorOverride = 1;
                // Turn off High voltage enable and discharge contactor
                HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
            }
        }
    }

    // Setting the allowance of charge and charge/discharge contactor state for auxStatus
    if (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK)
    {
        return;
    }

    auxStatus.allowCharge = allowCharge;

    if (shutOff)
    {
        auxStatus.strobeBmsLight = 1;
    }

    osMutexRelease(auxStatus.auxStatusMutex);

    // Setting the battery voltage in range indicator for orionStatus
    if (osMutexWait(orionStatus.orionStatusMutex, 0) != osOK)
    {
        return;
    }

    orionStatus.batteryVoltagesInRange = voltagesInRange;
    orionStatus.allowCharge = allowCharge;
    orionStatus.allowDischarge = allowDischarge;
    orionStatus.shutOff = shutOff;

    if (chargeContactorOverride)
    {
        orionStatus.chargeContactorOverriden = 1;
    }

    if (dischargeContactorOverride)
    {
        orionStatus.dischargeContactorOverriden = 1;
    }

    osMutexRelease(orionStatus.orionStatusMutex);
}

void updateChargeAllowanceTask(void const* arg)
{
    // One time osDelayUntil initialization
    uint32_t prevWakeTime = osKernelSysTick();

    for (;;)
    {
        updateChargeAllowance(&prevWakeTime);
    }
}
