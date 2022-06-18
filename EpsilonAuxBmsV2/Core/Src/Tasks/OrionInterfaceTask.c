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

/*
Essentially reads inputs from Orion and gets the Aux BMS to respond accordingly.
Determines:
 * When car should trip (all contactors open)
 * When charge/discharge should be open or closed based on GPIO inputs coming from Orion and based on Max/Min cell voltages

It will get info from the CAN Rx Interrupt parser periodically, and each time it does it will set the orionCanMessageReceivedRecently member of AuxStatus to 1 and keep track of the last time it received a message
If the waiting on the queue times out, that means that no message from Orion has been received recently, so set the orionCanMessageReceivedRecently member of AuxStatus to 0 and trip the car.

Before it finishes or triggers events to control the contactors, it updates the AuxTrip variable, and the AuxStatus variable.
To update the AuxStatus variable, it must first acquire the auxStatusOrionInterfaceMutex
To update the AuxTrip variable, it must first acquire the auxTripMutex

It will check the ContactorState variable before it requests a contactor to open or close (no point closing if already closed)
The reason the gatekeeper tasks don’t have to be periodic is because all additional control will come from here.
If a contactor is open, but it is allowed to be closed, the startupDone member of ContactorState indicates the startup sequence has finished, and Orion was the reason the contactor is open, Orion Interface will request that the contactor is closed.
Will only ever be charge or discharge. If common is open after startup, the car would have tripped.
When opening a contactor, it temporarily raises the gatekeeper task’s priority to RealTime (highest priority)
*/
void orionInterface(OrionCanInfo* message)
{
    if (auxBmsContactorState.orionHappyForStartup) {
        osStatus_t status = osMessageQueueGet(orionInterfaceQueue, message, NULL, ORION_QUEUE_TIMEOUT);

        uint8_t shouldDisconnectContactors = 0;
        // Read Orion charge and discharge enable GPIO
        uint8_t orionDischargeEnableSense = HAL_GPIO_ReadPin(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin);
        uint8_t orionChargeEnableSense = HAL_GPIO_ReadPin(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin);
        // Set aux status and aux trip to default values
        localAuxStatus = (AuxStatus)
        {
            0
        };
        localAuxStatus.allowCharge = 1;
        localAuxStatus.allowDischarge = 1;
        localAuxTrip = (AuxTrip)
        {
            0
        };

        if (status == osErrorTimeout) // If waiting on the queue times out
        {
            //Set orion can message recieved recently to 0
            shouldDisconnectContactors = 1;
            localAuxTrip.tripDueToOrionMessageTimeout = 1;
            localAuxStatus.orionCanReceivedRecently = 0;
        }
        else
        {
            // Determine trip conditions and contactor settings based on Orion CAN
            localAuxStatus.orionCanReceivedRecently = 1;
            updateAllowChargeAndAllowDischarge(message, &localAuxStatus);
            updateAuxTrip(message, &localAuxTrip, manualChargeTrip);
            localAuxStatus.dischargeShouldTrip = checkDischargeTrip(localAuxTrip);
            localAuxStatus.chargeShouldTrip = checkChargeTrip(localAuxTrip);
            shouldDisconnectContactors = localAuxTrip.protectionTrip
                                        || localAuxStatus.dischargeShouldTrip
                                        || localAuxStatus.chargeShouldTrip;
        }

        // Determine trip conditions and contactor settings based on Orion GPIO
        shouldDisconnectContactors |= (!orionDischargeEnableSense && !orionChargeEnableSense);

        // Set auxStatus if a trip is to occur (due to GPIO or CAN messages)
        if (shouldDisconnectContactors)
        {
            localAuxStatus.allowCharge = 0;
            localAuxStatus.allowDischarge = 0;

            if(auxBmsContactorState.startupDone) {
                localAuxStatus.strobeBmsLight = 1;
            }
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
        // Not returning if mutex not acuired so that we can still control the contactors
        if (osMutexAcquire(auxTripMutex, MUTEX_TIMEOUT) == osOK)
        {
            updateGlobalAuxTrip(&localAuxTrip);
            osMutexRelease(auxTripMutex);
        }

        if (osMutexAcquire(auxStatusOrionInterfaceMutex, MUTEX_TIMEOUT) == osOK)
        {
            updateAuxStatus(&localAuxStatus);
            osMutexRelease(auxStatusOrionInterfaceMutex);
        }

        // Trigger contactor control
        if (shouldDisconnectContactors)
        {
            if(auxBmsContactorState.startupDone) {
                disconnectContactors();
            }
        }
        else
        {
            uint32_t contactorControlEventFlags = 0;

            if (!localAuxStatus.allowCharge || manualChargeTrip)
            //if (!localAuxStatus.allowCharge || manualChargeTrip)
            {
                contactorControlEventFlags |= CHARGE_OPENED;
                osThreadSetPriority (chargeContactorGatekeeperTaskHandle, osPriorityRealtime);
            }
            else if (auxBmsContactorState.startupDone)
            {
                if (auxBmsContactorState.chargeState == OPEN && !((auxBmsContactorState.dischargeState == CLOSING)) && !manualChargeTrip)
                //if (auxBmsContactorState.chargeState == OPEN && !manualChargeTrip)
                {
                    contactorControlEventFlags |= CHARGE_CLOSED;

                    auxBmsContactorState.chargeState = CLOSING;
                }
            }

            if (!localAuxStatus.allowDischarge)
            {
                contactorControlEventFlags |= DISCHARGE_OPENED;
                osThreadSetPriority (dischargeContactorGatekeeperTaskHandle, osPriorityRealtime);
            }
            else if (auxBmsContactorState.startupDone)
            {
                // Only close discharge if discharge is currently open, and charge is currently closed
                // The reason for this is so we avoid any race conditions in both contactors closing at the same time
                // Charge will have a higher priority in closing (and it should trigger discharge to close anyways if discharge is open)
                if ((auxBmsContactorState.dischargeState == OPEN) && !((auxBmsContactorState.chargeState == CLOSING)))
                //if ((auxBmsContactorState.dischargeState == OPEN) && ((auxBmsContactorState.chargeState == CLOSED) || (manualChargeTrip && auxBmsContactorState.chargeState == OPEN)))
                {
                    contactorControlEventFlags |= DISCHARGE_CLOSED;
                    auxBmsContactorState.dischargeState = CLOSING;
                }
            }

            osEventFlagsSet(contactorControlEventBits, contactorControlEventFlags);
        }
    }
}

/*
Update AuxTrip in such a way to make common trips permanent.
*/
void updateGlobalAuxTrip(AuxTrip* auxTripToRead) {
    auxTrip.chargeTripDueToHighCellVoltage |= auxTripToRead->chargeTripDueToHighCellVoltage;
    auxTrip.chargeTripDueToHighTemperatureAndCurrent |= auxTripToRead->chargeTripDueToHighTemperatureAndCurrent;
    auxTrip.chargeTripDueToPackCurrent |= auxTripToRead->chargeTripDueToPackCurrent;
    auxTrip.dischargeTripDueToLowCellVoltage = auxTripToRead->dischargeTripDueToLowCellVoltage;
    auxTrip.dischargeTripDueToHighTemperatureAndCurrent |= auxTripToRead->dischargeTripDueToHighTemperatureAndCurrent;
    auxTrip.dischargeTripDueToPackCurrent |= auxTripToRead->dischargeTripDueToPackCurrent;
    auxTrip.protectionTrip |= auxTripToRead->protectionTrip;
    auxTrip.dischargeNotClosedDueToHighCurrent |= auxTripToRead->dischargeNotClosedDueToHighCurrent;
    auxTrip.chargeNotClosedDueToHighCurrent |= auxTripToRead->chargeNotClosedDueToHighCurrent;
    auxTrip.tripDueToOrionMessageTimeout |= auxTripToRead->tripDueToOrionMessageTimeout;
}

/*
Updates the desired fields of aux status
*/
void updateAuxStatus(AuxStatus* auxStatusToRead)
{
    auxStatus.strobeBmsLight |= auxStatusToRead->strobeBmsLight;
    auxStatus.allowCharge = auxStatusToRead->allowCharge;
    auxStatus.allowDischarge = auxStatusToRead->allowDischarge;
    auxStatus.orionCanReceivedRecently = auxStatusToRead->orionCanReceivedRecently;
    auxStatus.dischargeShouldTrip |= auxStatusToRead->dischargeShouldTrip;
    auxStatus.chargeShouldTrip |= auxStatusToRead->chargeShouldTrip;
}

/*
Updates allow charge and allow discharge based on the max and min cell voltages reported from Orion.
If the max cell voltage is greater than the Aux BMS internal limit, then disallow charge.
If the min cell voltage is lower than the Aux BMS internal limit, then disallow discharge.
*/
void updateAllowChargeAndAllowDischarge(OrionCanInfo* message, AuxStatus* auxStatusToUpdate)
{
    if (DEFAULT_VOLTAGE_UNITS * message->highCellVoltage > AUX_BMS_MAX_CELL_VOLTAGE)
    {
        auxStatusToUpdate->allowCharge = 0;
    }

    if (DEFAULT_VOLTAGE_UNITS * message->lowCellVoltage < AUX_BMS_MIN_CELL_VOLTAGE)
    {
        auxStatusToUpdate->allowDischarge = 0;
    }
}

uint8_t checkIfOrionGood(OrionCanInfo* message, uint32_t* startUpCounter) {
    osStatus_t status = osMessageQueueGet(orionInterfaceQueue, message, NULL, ORION_QUEUE_TIMEOUT);

    uint8_t orionHappy = 0;
    // Read Orion charge and discharge enable GPIO
    uint8_t orionDischargeEnableSense = HAL_GPIO_ReadPin(ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port, ORION_DISCHARGE_ENABLE_SENSE_Pin);
    uint8_t orionChargeEnableSense = HAL_GPIO_ReadPin(ORION_CHARGE_ENABLE_SENSE_GPIO_Port, ORION_CHARGE_ENABLE_SENSE_Pin);
    // Set aux status and aux trip to default values
    localAuxStatus = (AuxStatus)
    {
        0
    };
    localAuxStatus.allowCharge = 1;
    localAuxStatus.allowDischarge = 1;
    localAuxTrip = (AuxTrip)
    {
        0
    };

    if (status == osErrorTimeout) // If waiting on the queue times out
    {
        orionHappy = 0;
    }
    else
    {
        // Determine trip conditions and contactor settings based on Orion CAN
        localAuxStatus.orionCanReceivedRecently = 1;
        updateAllowChargeAndAllowDischarge(message, &localAuxStatus);
        updateAuxTrip(message, &localAuxTrip, manualChargeTrip);
        localAuxStatus.dischargeShouldTrip = checkDischargeTrip(localAuxTrip);
        localAuxStatus.chargeShouldTrip = checkChargeTrip(localAuxTrip);
        orionHappy = localAuxTrip.protectionTrip
                                     || localAuxStatus.dischargeShouldTrip
                                     || localAuxStatus.chargeShouldTrip;
    }

    orionHappy |= (!orionDischargeEnableSense && !orionChargeEnableSense);

    startUpCounter += orionHappy;

    if(orionHappy && *startUpCounter > 30) {
        localAuxStatus.strobeBmsLight = 1;
    }

    if (osMutexAcquire(auxTripMutex, MUTEX_TIMEOUT) == osOK)
        {
            auxTrip = localAuxTrip;
            osMutexRelease(auxTripMutex);
        }

        if (osMutexAcquire(auxStatusOrionInterfaceMutex, MUTEX_TIMEOUT) == osOK)
        {
            updateAuxStatus(&localAuxStatus);
            osMutexRelease(auxStatusOrionInterfaceMutex);
        }

    return orionHappy; //if this is 1 it means that orion is NOT happy
}