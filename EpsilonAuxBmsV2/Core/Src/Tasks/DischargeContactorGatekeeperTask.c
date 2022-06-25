#include "DischargeContactorGatekeeperTask.h"

void dischargeContactorGatekeeperTask(void* arg)
{
    for (;;)
    {
        dischargeContactorGatekeeper();
    }
}

/*
Attempts to close the discharge contactor.
Enables contactor and then delays.
Checks for success by making sure the pre-charge current is low enough for two or three contactor being closed (depending on if charge has been closed or not)
and ensures that the discharge sense pin is low.

If contactor did not close successfully it will set the enable low. If charge has been closed, it will delay then try again.
Otherwise, it will trigger the charge contactor gatekeeper to close the charge contactor.

If the contactor closed succesfully and the charge contactor is open, it will trigger the charge contactor gatekeeper to close the charge contactor.
*/
void closeDischargeContactor()
{
    while (auxBmsContactorState.chargeState == CLOSING) {}

    auxBmsContactorState.dischargeState = CLOSING;
    // Enable contactor, then delay

    HAL_GPIO_WritePin(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay(NON_COMMON_CONTACTOR_CHECK_DELAY);

    // Check contactor is set by reading sense pin and checking that precharge current is low
    uint8_t dischargeSense = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);
    uint8_t isChargeClosed = auxBmsContactorState.chargeState == CLOSED || auxBmsContactorState.chargeState == CLOSING;
    uint8_t currentLow = isChargeClosed ? isCurrentLow(3) : isCurrentLow(2);
    uint8_t contactorError = !(currentLow && dischargeSense);


    if (contactorError) // discharge contactor not closed successfully
    {
        auxBmsContactorState.dischargeState = CONTACTOR_ERROR;
        HAL_GPIO_WritePin(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);

        if (!currentLow) { //something closed that isn't supposed to be 
            if (osMutexAcquire(auxTripMutex, MUTEX_TIMEOUT) == osOK)
            {
                auxTrip.dischargeNotClosedDueToHighCurrent |= 1;
                osMutexRelease(auxTripMutex);
            }
        }

        if (isChargeClosed) // charge contactor closed so delay then try again
        {
            osDelay(NON_COMMON_RETRY_CONTACTOR_DELAY);
            osEventFlagsSet(contactorControlEventBits, DISCHARGE_CLOSED);
        }
    }
    else  // discharge contactor closed successfully, so turn on HV enable
    {
        auxBmsContactorState.dischargeState = CLOSED;
        HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_SET);
    }

    if (!isChargeClosed) // Charge not closed so trigger charge contactor gatekeeper task to close charge
    {
        //osEventFlagsSet(contactorControlEventBits, CHARGE_CLOSED);
    }
}

/*
Opens dicharge contactor and turns off high voltage enable then sets the priority to the task back to normal.
The priority is changed to realtime by:
  - OrionInterfaceTask
  - ContactorStatusUpdateTask
*/
void openDischargeContactor()
{
    HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    auxBmsContactorState.dischargeState = OPEN;
    osThreadSetPriority(dischargeContactorGatekeeperTaskHandle, osPriorityNormal);
}

/*
Controls the dicharge contactor.
Waits on events to close or open the dicharge contactor.
Note: Opening the contactor has a higher priority than closing the contactor.
*/
void dischargeContactorGatekeeper()
{
    uint32_t contactorFlags = osEventFlagsWait(contactorControlEventBits, DISCHARGE_CLOSED | DISCHARGE_OPENED, osFlagsWaitAny, osWaitForever);

    if (contactorFlags & DISCHARGE_OPENED)
    {
        openDischargeContactor();
    }
    else if ((contactorFlags & DISCHARGE_CLOSED) && !auxBmsContactorState.contactorsDisconnected)
    {
        closeDischargeContactor();
    }

}