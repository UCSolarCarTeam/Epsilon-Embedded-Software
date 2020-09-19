#include "OrionInterfaceTask.h"

void orionInterfaceTask(void* arg)
{
    OrionCanInfo message = (OrionCanInfo)
    {
        0
    };

    for (;;)
    {
        orionInterface(&message);
    }
}

void orionInterface(OrionCanInfo* message)
{
    osStatus_t status = osMessageQueueGet(orionInterfaceQueue, message, NULL, 400);
    AuxStatus localAuxStatus = (AuxStatus)
    {
        0
    };
    AuxTrip localAuxTrip = (AuxTrip)
    {
        0
    };
    uint8_t shouldDisconnectContactors = 0;
    // Read Orion charge and discharge enable GPIO
    uint8_t orionDischargeEnableSense = HAL_GPIO_ReadPin(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin);
    uint8_t orionChargeEnableSense = HAL_GPIO_ReadPin(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin);


    if (status == osErrorTimeout)
    {
        //Set orion can message recieved recently to 0
        localAuxStatus.orionCanReceivedRecently = 0;
    }
    else
    {
        // Determine trip conditions and contactor settings based on Orion CAN
        localAuxStatus.orionCanReceivedRecently = 1;
        checkCellVoltage(message, &localAuxStatus);
        localAuxStatus.dischargeShouldTrip = checkDischargeTrip(message, &localAuxTrip);
        localAuxStatus.chargeShouldTrip = checkChargeTrip(message, &localAuxTrip);
        shouldDisconnectContactors = localAuxStatus.dischargeShouldTrip
                                     || localAuxStatus.chargeShouldTrip
                                     || checkProtectionTrip(message, &localAuxTrip);
    }

    // Determine trip conditions and contactor settings based on Orion GPIO
    if (!orionDischargeEnableSense && !orionChargeEnableSense)
    {
        shouldDisconnectContactors = 1;
        localAuxStatus.allowCharge = 0;
        localAuxStatus.allowDischarge = 0;
        localAuxStatus.strobeBmsLight = 1;
    }
    else if (!orionDischargeEnableSense)
    {
        localAuxStatus.allowDischarge = 0;
    }
    else if (!orionChargeEnableSense)
    {
        localAuxStatus.allowCharge = 0;
    }

    // Set Aux Status and Aux Trip
    if (osMutexAcquire(auxTripMutex, 100) == osOK)
    {
        auxTrip = localAuxTrip;
        osMutexRelease(auxTripMutex);
    }

    if (osMutexAcquire(auxStatusOrionInterfaceMutex, 100) == osOK)
    {
        updateAuxStatus(&localAuxStatus);
        osMutexRelease(auxStatusOrionInterfaceMutex);
    }

    // Trigger contactor control
    if (shouldDisconnectContactors)
    {
        disconnectContactors();
    }
    else
    {
        uint32_t contactorControlEventFlags = 0;

        if (!localAuxStatus.allowCharge)
        {
            contactorControlEventFlags |= CHARGE_OFF;
            osThreadSetPriority (chargeContactorGatekeeperTaskHandle, osPriorityRealtime);
        }
        else if (auxBmsContactorState.startupDone)
        {
            if (auxBmsContactorState.chargeState == OPEN)
            {
                contactorControlEventFlags |= CHARGE_ON;
            }
        }

        if (!localAuxStatus.allowDischarge)
        {
            contactorControlEventFlags |= DISCHARGE_OFF;
            osThreadSetPriority (dischargeContactorGatekeeperTaskHandle, osPriorityRealtime);
        }
        else if (auxBmsContactorState.startupDone)
        {
            if ((auxBmsContactorState.dischargeState == OPEN) && (auxBmsContactorState.chargeState == CLOSED))
            {
                contactorControlEventFlags |= DISCHARGE_ON;
            }
        }

        osEventFlagsSet(contactorControlEventBits, contactorControlEventFlags);
    }
}

void updateAuxStatus(AuxStatus* auxStatusToRead)
{
    auxStatus.strobeBmsLight = auxStatusToRead->strobeBmsLight;
    auxStatus.allowCharge = auxStatusToRead->allowCharge;
    auxStatus.allowDischarge = auxStatusToRead->allowDischarge;
    auxStatus.orionCanReceivedRecently = auxStatusToRead->orionCanReceivedRecently;
    auxStatus.dischargeShouldTrip = auxStatusToRead->dischargeShouldTrip;
    auxStatus.chargeShouldTrip = auxStatusToRead->chargeShouldTrip;
}

void checkCellVoltage(OrionCanInfo* message, AuxStatus* auxStatusToUpdate)
{
    if (DEFAULT_VOLTAGE_UNITS * message->maxCellVoltage > AUX_BMS_MAX_CELL_VOLTAGE)
    {
        auxStatusToUpdate->allowCharge = 0;
    }

    if (DEFAULT_VOLTAGE_UNITS * message->minCellVoltage < AUX_BMS_MIN_CELL_VOLTAGE)
    {
        auxStatusToUpdate->allowDischarge = 0;
    }
}
