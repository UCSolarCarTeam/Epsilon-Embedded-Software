#include "CommonContactorGatekeeperTask.h"

void commonContactorGatekeeperTask(void* arg)
{
    for (;;)
    {
        commonContactorGatekeeper();
    }
}

/*
Attempts to close the common contactor.
Enables contactor and then delays.
Checks for sucess by making sure the pre-charge current is low  enough for one contactor being set and ensures that the sense pin is low.
If contactor did not close successfully it will set the enable low, delay and try to turn it on again.
If the contactor close succesfully it will trigger the charge contactor gatekeeper to close the charge contactor.
*/
void closeCommonContactor()
{
    auxBmsContactorState.commonState = CLOSING;
    // Enable contactor, then delay
    uint8_t orionDischargeEnableSense = HAL_GPIO_ReadPin(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin);
    uint8_t orionChargeEnableSense = HAL_GPIO_ReadPin(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin);
    HAL_GPIO_WritePin(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_SET);
    osDelay(COMMON_CHECK_CONTACTOR_DELAY);

    // Check contactor is set by reading sense pin and checking that precharge current is low
    uint8_t commonSense = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);
    uint8_t currentLow = isCurrentLow(1);
    orionDischargeEnableSense = HAL_GPIO_ReadPin(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin);
    orionChargeEnableSense = HAL_GPIO_ReadPin(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin);

    if (commonSense && currentLow) // Common contactor closed successfully, so trigger charge to turn on
    {
        auxBmsContactorState.commonState = CLOSED;
        //osEventFlagsSet(contactorControlEventBits, CHARGE_CLOSED);
    }
    else   // Common contactor not closed successfully, so delay then try again
    {
        auxBmsContactorState.commonState = CONTACTOR_ERROR;
        HAL_GPIO_WritePin(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_RESET);
        osDelay (COMMON_RETRY_CONTACTOR_DELAY);
        osEventFlagsSet(contactorControlEventBits, COMMON_CLOSED);
    }
}

/*
Opens common contactor then sets the priority to the task back to normal and then triggers charge and discharge contactors to turn off.
Note: the priorities of the charge and discharge should've been set to real time prior to this.
The priority is changed to realtime by:
  - OrionInterfaceTask
  - ContactorStatusUpdateTask
*/
void openCommonContactor()
{
    HAL_GPIO_WritePin(COMMON_ENABLE_GPIO_Port, COMMON_ENABLE_Pin, GPIO_PIN_RESET);
    auxBmsContactorState.commonState = OPEN;
    osEventFlagsSet(contactorControlEventBits, DISCHARGE_OPENED | CHARGE_OPENED);
    osThreadSetPriority (commonContactorGatekeeperTaskHandle, osPriorityNormal);
}

/*
Controls the common contactor.
Waits on events to close or open the common contactor.
Note: Opening the contactor has a higher priority than closing the contactor.
*/
void commonContactorGatekeeper()
{
    uint32_t contactorFlags = osEventFlagsWait(contactorControlEventBits, COMMON_CLOSED | COMMON_OPENED, osFlagsWaitAny, osWaitForever);

    if (contactorFlags & COMMON_OPENED)
    {
        openCommonContactor();
    }
    else if ((contactorFlags & COMMON_CLOSED) && !auxBmsContactorState.contactorsDisconnected) // Only close contactor if the contactors have not been disconnected
    {
        closeCommonContactor();
    }
}
