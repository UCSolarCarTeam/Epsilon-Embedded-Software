#include "Trip.h"

uint8_t checkDischargeTrip(OrionCanInfo* message, AuxTrip* auxTripToUpdate)
{
    uint8_t tripDuetoLowCell = 0;

    if (DEFAULT_VOLTAGE_UNITS * message->minCellVoltage <= ORION_MIN_CELL_VOLTAGE)
    {
        tripDuetoLowCell = 1;
    }

    // It's ok to just have a high temperature,
    // as long as the car is not discharging
    uint8_t tripDuetoHighTempAndCurrent = 0;

    if (message->highTemperature >= ORION_MAX_DISCHARGE_TEMP
            && message->packCurrent > 0)
    {
        tripDuetoHighTempAndCurrent = 1;
    }

    uint8_t tripDuetoPackCurrent = 0;

    if (message->packCurrent >= ORION_MAX_DISCHARGE_CURRENT)
    {
        tripDuetoPackCurrent = 1;
    }

    auxTripToUpdate->dischargeTripDueToLowCellVoltage = tripDuetoLowCell;
    auxTripToUpdate->dischargeTripDueToHighTemperatureAndCurrent = tripDuetoHighTempAndCurrent;
    auxTripToUpdate->dischargeTripDueToPackCurrent = tripDuetoPackCurrent;

    return tripDuetoLowCell ||
           tripDuetoHighTempAndCurrent ||
           tripDuetoPackCurrent;
}

uint8_t checkChargeTrip(OrionCanInfo* message, AuxTrip* auxTripToUpdate)
{

    uint8_t tripDuetoHighCell = 0;

    if (DEFAULT_VOLTAGE_UNITS * message->maxCellVoltage >= ORION_MAX_CELL_VOLTAGE)
    {
        tripDuetoHighCell = 1;
    }

    // It's ok to just have a high temperature,
    // as long as the car is not charging
    uint8_t tripDuetoHighTempAndCurrent = 0;

    if (message->highTemperature >= ORION_MAX_CHARGE_TEMP
            && message->packCurrent < 0)
    {
        tripDuetoHighTempAndCurrent = 1;
    }

    uint8_t tripDuetoPackCurrent = 0;

    // Charge current is negative
    if (message->packCurrent <= ORION_MAX_CHARGE_CURRENT)
    {
        tripDuetoPackCurrent = 1;
    }

    auxTripToUpdate->chargeTripDueToHighCellVoltage = tripDuetoHighCell;
    auxTripToUpdate->chargeTripDueToHighTemperatureAndCurrent = tripDuetoHighTempAndCurrent;
    auxTripToUpdate->chargeTripDueToPackCurrent = tripDuetoPackCurrent;

    return tripDuetoHighCell ||
           tripDuetoHighTempAndCurrent ||
           tripDuetoPackCurrent;

}

uint8_t checkProtectionTrip(OrionCanInfo* message, AuxTrip* auxTripToUpdate)
{
    // If the charge contactor is off, but we are still charging, trip
    const uint8_t chargeSense = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);
    uint8_t protectionTrip = (!chargeSense && message->packCurrent < 0);
    auxTripToUpdate->protectionTrip = protectionTrip;
    return protectionTrip;
}
