#include "StateFunctionsShared.h"

void disconnectContactors(uint8_t isContactorError)
{
    // Turn all contactors and high voltage enable off
    HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_RESET);

    if (isContactorError)
    {
        while (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK) // Not sure if this is the best idea
            // but will guarantee that we return to this spot
        {
            osDelay(10);
        }

        auxStatus.contactorError = 1;
        auxStatus.strobeBmsLight = 1;
        auxStatus.allowCharge = 0;

        osMutexRelease(auxStatus.auxStatusMutex);
    }
}

const uint8_t dischargeDisconnectedDuringContactorSetting(const uint8_t dischargeSense, uint8_t hasDischargeBeenSet)
{
    return (!orionStatus.dischargeContactorOverriden && hasDischargeBeenSet && !dischargeSense);
}

const uint8_t chargeDisconnectedDuringContactorSetting(const uint8_t chargeSense, uint8_t hasChargeBeenSet)
{
    return (!orionStatus.chargeContactorOverriden && hasChargeBeenSet && !chargeSense);
}
