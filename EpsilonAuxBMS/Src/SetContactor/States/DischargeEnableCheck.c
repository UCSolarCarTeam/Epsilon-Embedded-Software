#include "DischargeEnableCheck.h"

void dischargeEnableCheck(SetContactorStateInfo* stateInfo)
{
    uint8_t contactorError = 0;

    const uint8_t commonSense = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);
    const uint8_t chargeSense = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
    const uint8_t dischargeSense = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);

    if (!commonSense || chargeDisconnectedDuringContactorSetting(chargeSense, stateInfo->hasChargeBeenSet))
    {
        disconnectContactors(1);
        stateInfo->state = CONTACTOR_DISCONNECTED;
        return;
    }

    if (!dischargeSense)
    {
        int numContactors = chargeSense ? 3 : 2;
        stateInfo->hasDischargeBeenSet = isContactorSet(DISCHARGE_SENSE_Pin, DISCHARGE_SENSE_GPIO_Port, numContactors);

        if (stateInfo->hasDischargeBeenSet)
        {
            // Enable high voltage
            HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
        }
        else // Turn back off if not set
        {
            HAL_GPIO_WritePin(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
        }
    }

    switch (stateInfo->prevState)
    {
        case IDLE:
            contactorError = !stateInfo->hasDischargeBeenSet;
            break;

        case CHARGE_ENABLE_CHECK:
            contactorError = !(stateInfo->hasChargeBeenSet && stateInfo->hasDischargeBeenSet);
            break;

        default: // Something went really wrong in the logic, stop everything
            disconnectContactors(0);
            stateInfo->state = SHUTDOWN;
            return;
    }

    if (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK)
    {
        return;
    }

    auxStatus.contactorError = contactorError;

    if (stateInfo->hasDischargeBeenSet && stateInfo->hasChargeBeenSet)
    {
        auxStatus.startUpSequenceDone = 1;
        stateInfo->state = IDLE;
    }

    osMutexRelease(auxStatus.auxStatusMutex);

    if (!stateInfo->hasChargeBeenSet)
    {
        stateInfo->prevState = stateInfo->state;
        stateInfo->state = CHARGE_ENABLE_CHECK;
        HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    }
    else if (!stateInfo->hasDischargeBeenSet)
    {
        stateInfo->prevState = stateInfo->state;
        stateInfo->state = CHARGE_ENABLE_CHECK;
    }
}
