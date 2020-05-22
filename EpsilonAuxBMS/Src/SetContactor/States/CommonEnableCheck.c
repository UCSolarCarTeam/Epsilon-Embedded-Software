#include "CommonEnableCheck.h"

void commonEnableCheck(SetContactorStateInfo* stateInfo)
{
    uint8_t contactorError = !isContactorSet(COMMON_SENSE_Pin, COMMON_SENSE_GPIO_Port, 1);

    if (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK)
    {
        return;
    }

    auxStatus.contactorError = contactorError;

    osMutexRelease(auxStatus.auxStatusMutex);

    if (!contactorError)
    {
        stateInfo->prevState = stateInfo->state;
        stateInfo->state = CHARGE_ENABLE_CHECK;
        HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    }
}
