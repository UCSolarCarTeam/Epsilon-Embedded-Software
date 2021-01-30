//Include all files required for your functions to run
#include <stdio.h>
#include "unity.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "OrionInterfaceTest.h"

extern OrionCanInfo message;
extern AuxStatus auxStatus;
extern AuxTrip auxTrip;
extern AuxBmsContactorState auxBmsContactorState;
extern osEventFlagsId_t contactorControlEventBits;

void setUp(void)
{
    message.lowCellVoltage = AUX_BMS_MIN_CELL_VOLTAGE / DEFAULT_VOLTAGE_UNITS;
    message.highCellVoltage = AUX_BMS_MAX_CELL_VOLTAGE / DEFAULT_VOLTAGE_UNITS;
    setNominalAuxStatus();
    auxTrip = (AuxTrip)
    {
        0
    };
    auxBmsContactorState = (AuxBmsContactorState)
    {
        0
    };
    contactorControlEventBits = 0;
}

void tearDown(void)
{
}

int main (void)
{
    UNITY_BEGIN();

    runOrionInterfaceTests();

    return UNITY_END();
}