#include "Idle.h"

void idle(SetContactorStateInfo* stateInfo)
{
    const uint8_t commonSense = !HAL_GPIO_ReadPin(COMMON_SENSE_GPIO_Port, COMMON_SENSE_Pin);
    const uint8_t chargeSense = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
    const uint8_t dischargeSense = !HAL_GPIO_ReadPin(DISCHARGE_SENSE_GPIO_Port, DISCHARGE_SENSE_Pin);

    uint8_t isChargeTurningOn = 0;
    uint8_t isDischargeTurningOn = 0;

    if (orionStatus.shutOff)
    {
        stateInfo->state = SHUTDOWN;
        return;
    }
    else if (!commonSense)
    {
        disconnectContactors(1);
        stateInfo->state = CONTACTOR_DISCONNECTED;
    }

    if (canTurnOnChargeContactor(chargeSense))
    {
        if (osMutexWait(orionStatus.orionStatusMutex, 0) != osOK)
        {
            return;
        }

        orionStatus.chargeContactorOverriden = 0;
        osMutexRelease(orionStatus.orionStatusMutex);

        isChargeTurningOn = 1;
        stateInfo->hasChargeBeenSet = 0;
        // Turn on charge Contactor
        HAL_GPIO_WritePin(CHARGE_ENABLE_GPIO_Port, CHARGE_ENABLE_Pin, GPIO_PIN_SET);
        stateInfo->prevState = stateInfo->state;
        stateInfo->state = CHARGE_ENABLE_CHECK;
    }
    else if (canTurnOnDishargeContactor(dischargeSense))
    {
        if (osMutexWait(orionStatus.orionStatusMutex, 0) != osOK)
        {
            return;
        }

        orionStatus.dischargeContactorOverriden = 0;
        osMutexRelease(orionStatus.orionStatusMutex);

        isDischargeTurningOn = 1;
        stateInfo->hasDischargeBeenSet = 0;
        HAL_GPIO_WritePin(DISCHARGE_ENABLE_GPIO_Port, DISCHARGE_ENABLE_Pin, GPIO_PIN_SET);
        stateInfo->prevState = stateInfo->state;
        stateInfo->state = DISCHARGE_ENABLE_CHECK;
    }


    // If charge contactor should be on, but isn't, disconnect
    // or if discharge contactor should be on, but isn't, disconnect
    if (chargeDisconnectedUnexpectedly(chargeSense, isChargeTurningOn) ||
            dischargeDisconnectedUnexpectedly(dischargeSense, isDischargeTurningOn))
    {
        disconnectContactors(1);
        stateInfo->state = CONTACTOR_DISCONNECTED;
    }
}

const uint8_t canTurnOnDishargeContactor(const uint8_t dischargeSense)
{
    return (orionStatus.allowDischarge && orionStatus.dischargeContactorOverriden && !dischargeSense);
}

const uint8_t canTurnOnChargeContactor(const uint8_t chargeSense)
{
    return (orionStatus.allowCharge && orionStatus.chargeContactorOverriden && !chargeSense);
}

const uint8_t dischargeDisconnectedUnexpectedly(const uint8_t dischargeSense, const uint8_t isDischargeTurningOn)
{
    return (orionStatus.allowDischarge && !dischargeSense && !isDischargeTurningOn && !orionStatus.dischargeContactorOverriden);
}

const uint8_t chargeDisconnectedUnexpectedly(const uint8_t chargeSense, const uint8_t isChargeTurningOn)
{
    return (orionStatus.allowCharge && !chargeSense && !isChargeTurningOn && !orionStatus.chargeContactorOverriden);
}
