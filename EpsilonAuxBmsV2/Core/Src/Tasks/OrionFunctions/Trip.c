#include "Trip.h"

void updateAuxTrip(OrionCanInfo* message, AuxTrip* auxTripToUpdate)
{
    uint8_t tripDuetoLowCell = 0;
    uint8_t tripDueToHighTempDuringCharge = 0;
    uint8_t tripDueToHighTempDuringDischarge = 0;
    uint8_t tripDuetoPackCurrent = 0;
    uint8_t tripDuetoHighCell = 0;

    const uint8_t chargeSense = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
    uint8_t protectionTrip = (!chargeSense && message->packCurrent < 0) && 
                                    (!(auxBmsContactorState.commonState == CLOSING) && !(auxBmsContactorState.chargeState == CLOSING) && !(auxBmsContactorState.dischargeState == CLOSING)
                                     && !(auxBmsContactorState.commonState == CLOSED && auxBmsContactorState.chargeState == OPEN && auxBmsContactorState.dischargeState == OPEN));


    if (DEFAULT_VOLTAGE_UNITS * message->highCellVoltage >= ORION_MAX_CELL_VOLTAGE)
    {
        tripDuetoHighCell = 1;
    }

    // It's ok to just have a high temperature,
    // as long as the car is not charging
    if (message->highTemperature >= ORION_MAX_CHARGE_TEMP
            && message->packCurrent < 0)
    {
        tripDueToHighTempDuringCharge = 1;
    }

    // Charge current is negative
    if (message->packCurrent <= ORION_MAX_CHARGE_CURRENT)
    {
        tripDuetoPackCurrent = 1;
    }

    if (DEFAULT_VOLTAGE_UNITS * message->lowCellVoltage <= ORION_MIN_CELL_VOLTAGE)
    {
        tripDuetoLowCell = 1;
    }

    // It's ok to just have a high temperature,
    // as long as the car is not discharging
    if (message->highTemperature >= ORION_MAX_DISCHARGE_TEMP
            && message->packCurrent > 0)
    {
        tripDueToHighTempDuringDischarge = 1;
    }

    // Discharge current is positive
    if (message->packCurrent >= ORION_MAX_DISCHARGE_CURRENT)
    {
        tripDuetoPackCurrent = 1;
    }

    auxTripToUpdate->protectionTrip = protectionTrip;
    auxTripToUpdate->dischargeTripDueToLowCellVoltage = tripDuetoLowCell;
    auxTripToUpdate->dischargeTripDueToHighTemperatureAndCurrent = tripDueToHighTempDuringDischarge;
    auxTripToUpdate->dischargeTripDueToPackCurrent = tripDuetoPackCurrent;
    auxTripToUpdate->chargeTripDueToHighCellVoltage = tripDuetoHighCell;
    auxTripToUpdate->chargeTripDueToHighTemperatureAndCurrent = tripDueToHighTempDuringCharge;
    auxTripToUpdate->chargeTripDueToPackCurrent = tripDuetoPackCurrent;
}

/*
Determines whether Discharge should cause a trip based on Orion CAN messages and current line between contactors
Discharge will cause a trip due to:
  * the min cell voltage being lower than the Orion limit
  * high temperature while discharging
  * discharge pack current being too high
  * current line between contactors not low enough to close discharge
*/
uint8_t checkDischargeTrip(AuxTrip auxTrip)
{
    return auxTrip.dischargeTripDueToLowCellVoltage ||
           auxTrip.dischargeTripDueToHighTemperatureAndCurrent ||
           auxTrip.dischargeTripDueToPackCurrent ||
           auxTrip.dischargeNotClosedDueToHighCurrent; //set in discharge gatekeeper task
}

/*
Determines whether charge should cause a trip based on Orion CAN messages and current line between contactors
Charge will cause a trip due to:
  * the max cell voltage being higher than the Orion limit
  * high temperature while charging
  * discharge pack current being too high (absolute value)
  * current line between contactors not low enough to close charge
*/
uint8_t checkChargeTrip(AuxTrip auxTrip)
{
    return auxTrip.chargeTripDueToHighCellVoltage ||
           auxTrip.chargeTripDueToHighTemperatureAndCurrent ||
           auxTrip.chargeTripDueToPackCurrent ||
           auxTrip.chargeNotClosedDueToHighCurrent; //set in charge gatekeeper task
}
