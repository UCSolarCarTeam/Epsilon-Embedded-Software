#include "ChargeContactorGatekeeperTask.h"

void chargeContactorGatekeeperTask(void* arg)
{
    for (;;)
    {
        chargeContactorGatekeeper();
    }
}

/*
Attempts to close the charge contactor.
Enables contactor and then delays.
Checks for success by making sure the pre-charge current is low enough for two or three contactor being closed (depending on if discharge has been closed or not)
and ensures that the charge sense pin is low.

If contactor did not close successfully it will set the enable low. If discharge has been closed, it will delay then try again.
Otherwise, it will trigger the discharge contactor gatekeeper to close the discharge contactor.

If the contactor closed succesfully and the discharge contactor is open, it will trigger the discharge contactor gatekeeper to close the discharge contactor.
*/
void closeChargeContactor()
{
    auxBmsContactorState.chargeState = CLOSING;
    // Enable contactor, then delay
    HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
    osDelay(CONTACTOR_DELAY);

    // Check contactor is set by reading sense pin and checking that precharge current is low depending on if discharge is closed/closing
    uint8_t chargeSense = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
    uint8_t isDischargeClosed = auxBmsContactorState.dischargeState == CLOSED || auxBmsContactorState.dischargeState == CLOSING;
    uint8_t currentLow = isDischargeClosed ? isCurrentLow(3) : isCurrentLow(2);
    uint8_t contactorError = !(currentLow && chargeSense);

    if (contactorError) // charge contactor not closed successfully
    {
        auxBmsContactorState.chargeState = CONTACTOR_ERROR;
        HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);

        if (isDischargeClosed) // Discharge closed so delay and try again
        {
            osDelay(CONTACTOR_DELAY);
            osEventFlagsSet(contactorControlEventBits, CHARGE_ON);
        }
    }
    else // charge contactor closed succesfully
    {
        auxBmsContactorState.chargeState = CLOSED;
    }

    if (!isDischargeClosed) // Discharge not closed so trigger it to turn on again
    {
        osEventFlagsSet(contactorControlEventBits, DISCHARGE_ON);
    }
}

/*
Opens charge contactor then sets the priority to the task back to normal.
*/
void openChargeContactor()
{
    HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_RESET);
    auxBmsContactorState.chargeState = OPEN;
    osThreadSetPriority (chargeContactorGatekeeperTaskHandle, osPriorityNormal);
}

/*
Controls the charge contactor.
Waits on events to close or open the charge contactor.
Note: Opening the contactor has a higher priority than closing the contactor.
*/
void chargeContactorGatekeeper()
{
    uint32_t contactorFlags = osEventFlagsWait(contactorControlEventBits, CHARGE_ON | CHARGE_OFF, osFlagsWaitAny, osWaitForever);

    if (contactorFlags & CHARGE_OFF)
    {
        openChargeContactor();
    }
    else if ((contactorFlags & CHARGE_ON) && !auxBmsContactorState.contactorsDisconnected)
    {
        closeChargeContactor();
    }

}
