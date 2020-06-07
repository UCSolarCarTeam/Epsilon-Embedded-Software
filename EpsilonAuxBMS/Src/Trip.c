#include "Trip.h"
#include "OrionStatus.h"
#include "AuxBmsTasks.h"

static const float ORION_MAX_CELL_VOLTAGE = 4.20; // Much match orion bms value
static const float ORION_MIN_CELL_VOLTAGE = 2.55;

static const float DEFAULT_VOLTAGE_UNITS = 0.0001; // From: https://www.orionbms.com/manuals/utility/

static const int ORION_MAX_CHARGE_TEMP = 44;
static const short ORION_MAX_CHARGE_CURRENT = -47;

static const int ORION_MAX_DISCHARGE_TEMP = 59;
static const short ORION_MAX_DISCHARGE_CURRENT = 229;

// When orion tells aux bms to open the charge contactor,
// read the state of the car to see if we should trip
int chargeShouldTrip()
{
    int chargeShouldTrip = 0;

    if (orionStatus.canMsgReceived)
    {
        int tripDuetoHighCell = 0;

        if (DEFAULT_VOLTAGE_UNITS * orionStatus.maxCellVoltage >= ORION_MAX_CELL_VOLTAGE)
        {
            tripDuetoHighCell = 1;
        }

        // It's ok to just have a high temperature,
        // as long as the car is not charging
        int tripDuetoHighTempAndCurrent = 0;

        if (orionStatus.highTemperature >= ORION_MAX_CHARGE_TEMP
                && orionStatus.packCurrent < 0)
        {
            tripDuetoHighTempAndCurrent = 1;
        }

        int tripDuetoPackCurrent = 0;

        // Charge current is negative
        if (orionStatus.packCurrent <= ORION_MAX_CHARGE_CURRENT)
        {
            tripDuetoPackCurrent = 1;
        }

        chargeShouldTrip = tripDuetoHighCell ||
                           tripDuetoHighTempAndCurrent ||
                           tripDuetoPackCurrent;
    }

    return chargeShouldTrip;
}

// When orion tells aux bms to open the discharge contactor,
// read the state of the car to see if we should trip

int dischargeShouldTrip()
{
    int dischargeShouldTrip = 0;

    if (orionStatus.canMsgReceived)
    {
        int tripDuetoLowCell = 0;

        if (DEFAULT_VOLTAGE_UNITS * orionStatus.minCellVoltage <= ORION_MIN_CELL_VOLTAGE)
        {
            tripDuetoLowCell = 1;
        }

        // It's ok to just have a high temperature,
        // as long as the car is not discharging
        int tripDuetoHighTempAndCurrent = 0;

        if (orionStatus.highTemperature >= ORION_MAX_DISCHARGE_TEMP
                && orionStatus.packCurrent > 0)
        {
            tripDuetoHighTempAndCurrent = 1;
        }

        int tripDuetoPackCurrent = 0;

        if (orionStatus.packCurrent >= ORION_MAX_DISCHARGE_CURRENT)
        {
            tripDuetoPackCurrent = 1;
        }

        dischargeShouldTrip = tripDuetoLowCell ||
                              tripDuetoHighTempAndCurrent ||
                              tripDuetoPackCurrent;
    }

    return dischargeShouldTrip;
}

int protectionTripping()
{
    // If the charge contactor is off, but we are still charging, trip
    const uint8_t chargeSense = !HAL_GPIO_ReadPin(CHARGE_SENSE_GPIO_Port, CHARGE_SENSE_Pin);

    if (!chargeSense && orionStatus.packCurrent < 0)
    {
        return 1;
    }

    return 0;
}

int carShouldTrip()
{
    return (chargeShouldTrip() || dischargeShouldTrip() || protectionTripping());
}