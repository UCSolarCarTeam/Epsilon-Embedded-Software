#include "FirstCheck.h"

void firstCheck(SetContactorStateInfo* stateInfo)
{
    // Check current is low before beginning to turn on contactors
    // If current is high for some reason, cycle again
    uint8_t contactorError = !isContactorSet(0xFF, NULL, 1); // Don't want to read an actual sense pin

    if (osMutexWait(auxStatus.auxStatusMutex, 0) != osOK)
    {
        return;
    }

    auxStatus.contactorError = contactorError;

    osMutexRelease(auxStatus.auxStatusMutex);

    if (!contactorError)
    {
        // Turn on Common Contactor
        HAL_GPIO_WritePin(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_SET);
        stateInfo->prevState = stateInfo->state;
        stateInfo->state = COMMON_ENABLE_CHECK;
    }

}
