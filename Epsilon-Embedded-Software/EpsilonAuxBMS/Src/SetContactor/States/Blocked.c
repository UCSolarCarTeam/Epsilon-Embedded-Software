#include "Blocked.h"

void blocked(SetContactorStateInfo* stateInfo)
{
    const uint8_t commonSense = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);
    const uint8_t chargeSense = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
    const uint8_t dischargeSense = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);

    // If orion isn't overiding the contactors, but they are supposed to have been set, but aren't
    // , this is an error condition and all contactors should be disconnected.
    if (stateInfo->prevState > COMMON_ENABLE_CHECK &&
            (!commonSense ||
             chargeDisconnectedDuringContactorSetting(chargeSense, stateInfo->hasChargeBeenSet) ||
             dischargeDisconnectedDuringContactorSetting(dischargeSense, stateInfo->hasDischargeBeenSet)))
    {
        disconnectContactors(1);
        stateInfo->state = CONTACTOR_DISCONNECTED;
    }
    else if (orionStatus.batteryVoltagesInRange)
    {
        if (orionStatus.chargeContactorOverriden)
        {
            stateInfo->hasChargeBeenSet = 0;
        }

        if (orionStatus.dischargeContactorOverriden)
        {
            stateInfo->hasDischargeBeenSet = 0;
        }

        if (osMutexWait(orionStatus.orionStatusMutex, 0) != osOK)
        {
            return;
        }

        orionStatus.chargeContactorOverriden = 0;
        orionStatus.dischargeContactorOverriden = 0;

        osMutexRelease(orionStatus.orionStatusMutex);
        stateInfo->state = FIRST_CHECK;
    }
}
