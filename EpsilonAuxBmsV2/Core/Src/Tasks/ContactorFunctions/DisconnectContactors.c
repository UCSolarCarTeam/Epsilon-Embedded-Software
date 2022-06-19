#include "DisconnectContactors.h"

/*
Sets contactorsDisconnected state to 1 to prevent contactors turning on again.
Raises the priority of all the gatekeeper tasks, then triggers common to turn off.
This is done so that common turns off first. The common gatekeeper task will in turn trigger
charge and discharge to turn off.
*/
void disconnectContactors()
{
    auxBmsContactorState.contactorsDisconnected = 1;

    if (osMutexAcquire(auxStatusOrionInterfaceMutex, MUTEX_TIMEOUT) == osOK)
    {
        auxStatus.strobeBmsLight = 1;
        osMutexRelease(auxStatusOrionInterfaceMutex);
    }

    osThreadSetPriority (chargeContactorGatekeeperTaskHandle, osPriorityRealtime);
    osThreadSetPriority (dischargeContactorGatekeeperTaskHandle, osPriorityRealtime);
    osThreadSetPriority (commonContactorGatekeeperTaskHandle, osPriorityRealtime);

    osEventFlagsSet(contactorControlEventBits, COMMON_OPENED);
}
