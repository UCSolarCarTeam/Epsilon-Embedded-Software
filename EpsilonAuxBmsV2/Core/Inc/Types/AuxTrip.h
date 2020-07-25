#pragma once

typedef struct
{
    unsigned chargeTripDueToHighCellVoltage : 1;
    unsigned chargeTripDueToHighTemperatureAndCurrent : 1;
    unsigned chargeTripDueToPackCurrent : 1;
    unsigned dischargeTripDueToLowCellVoltage : 1;
    unsigned dischargeTripDueToHighTemperatureAndCurrent : 1;
    unsigned dischargeTripDueToPackCurrent : 1;
    unsigned protectionTrip : 1;
} AuxTrip;
