#include "ChargeEnableCheck.h"

void chargeEnableCheck(SetContactorStateInfo* stateInfo)
{
    uint8_t contactorError = 0;

    const uint8_t commonSense = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);
    const uint8_t dischargeSense = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);
    const uint8_t chargeSense = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);

    if (!commonSense || dischargeDisconnectedDuringContactorSetting(dischargeSense, stateInfo->hasDischargeBeenSet))
    {
        disconnectContactors(1);
        stateInfo->state = CONTACTOR_DISCONNECTED;
        return;
    }

    if (!chargeSense)
    {
        int numContactors = dischargeSense ? 3 : 2;
        stateInfo->hasChargeBeenSet = isContactorSet(CHARGE_SENSE_Pin, CHARGE_SENSE_GPIO_Port, numContactors);

        if (!stateInfo->hasChargeBeenSet) // Turn back off if not set
        {
            HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
        }
    }

    switch (stateInfo->prevState)
    {
        case COMMON_ENABLE_CHECK:
        case IDLE:
            contactorError = !stateInfo->hasChargeBeenSet;
            break;

        case DISCHARGE_ENABLE_CHECK:
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

    if (!stateInfo->hasDischargeBeenSet)
    {
        HAL_GPIO_WritePin(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
        stateInfo->prevState = stateInfo->state;
        stateInfo->state = DISCHARGE_ENABLE_CHECK;
    }
    else if (!stateInfo->hasChargeBeenSet)
    {
        stateInfo->prevState = stateInfo->state;
        stateInfo->state = DISCHARGE_ENABLE_CHECK;
    }
}
