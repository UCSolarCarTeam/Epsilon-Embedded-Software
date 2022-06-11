#include "ContactorStatusUpdateTask.h"

void contactorStatusUpdateTask(void* arg)
{
    uint32_t prevWakeTime = xTaskGetTickCount();

    for (;;)
    {
        contactorStatusUpdate(&prevWakeTime);
    }
}

/*
Reads the contactor sense pins and the ContactorState variable to determine contactor errors and real contactor state to be set in AuxStatus.
Sets startupDone variable if all contactor states are CLOSED. (Only set once. Never cleared)
If any contactor opens unexpectedly, disconnect all contactors.
Note: It must acquire the auxStatusContactorStatusUpdateMutex before updating the AuxStatus variable
*/
void contactorStatusUpdate(uint32_t* prevWakeTime)
{
    // Read states of contactors and hv enable
    uint8_t commonSense = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);
    uint8_t chargeSense = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
    uint8_t dischargeSense = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);
    uint8_t hvEnableState = HAL_GPIO_ReadPin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin);

    // Update Aux Status after acquiring mutex
    if (osMutexAcquire(auxStatusContactorStatusUpdateMutex, MUTEX_TIMEOUT) == osOK)
    {
        auxStatus.commonContactorState = commonSense;
        auxStatus.chargeContactorState = chargeSense;
        auxStatus.dischargeContactorState = dischargeSense;
        auxStatus.highVoltageEnableState = hvEnableState;
        auxStatus.chargeOpenButShouldBeClosed = auxBmsContactorState.chargeState == CLOSED && !chargeSense;
        auxStatus.dischargeOpenButShouldBeClosed = auxBmsContactorState.dischargeState == CLOSED && !dischargeSense;

        auxStatus.chargeContactorError = isContactorError(chargeSense, auxBmsContactorState.chargeState);
        auxStatus.dischargeContactorError = isContactorError(dischargeSense, auxBmsContactorState.dischargeState);
        auxStatus.commonContactorError = isContactorError(commonSense, auxBmsContactorState.commonState);

        osMutexRelease(auxStatusContactorStatusUpdateMutex);
    }


    if ( !auxBmsContactorState.startupDone
            && auxBmsContactorState.commonState == CLOSED )
            //&& (auxBmsContactorState.chargeState == CLOSED || auxBmsContactorState.dischargeState == CLOSED))
    {
        auxBmsContactorState.startupDone = 1;
    }

    // Any contactors disconnect unexpectedly, disconnect all of them
    if ((auxBmsContactorState.commonState == CLOSED && !commonSense) ||
            (auxBmsContactorState.chargeState == CLOSED && !chargeSense) ||
            (auxBmsContactorState.dischargeState == CLOSED && !dischargeSense)
       )
    {
        disconnectContactors();
    }

    *prevWakeTime += CONTACTOR_STATUS_UPDATE_FREQ;
    osDelayUntil(*prevWakeTime);
}
