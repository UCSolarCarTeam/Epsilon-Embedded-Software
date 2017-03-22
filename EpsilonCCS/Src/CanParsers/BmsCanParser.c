#include "cmsis_os.h"

#include "BmsCanParser.h"

#include "BatteryData.h"
#include "BatteryFaultsData.h"

#define BMS_HEARTBEAT_ID (0x300)
#define STARTUP_INFO_ID (0x301)
#define PACK_INFO_ID (0x302)
#define ERRORS_ID (0x303)
#define TEMPINFO_ID (0x304)
#define CELL_VOLTAGES_ID (0x305)

void parseBmsCanMessage(uint32_t stdId, uint8_t* data)
{
    switch (stdId)
    {
        case BMS_HEARTBEAT_ID:
            parseBmsHeartbeat();
            break;

        case STARTUP_INFO_ID:
            parseStartupInfo(data);
            break;

        case PACK_INFO_ID:
            parsePackInfo(data);
            break;

        case ERRORS_ID:
            parseErrors(data);
            break;

        case TEMPINFO_ID:
            parseTempInfo(data);
            break;

        case CELL_VOLTAGES_ID:
            parseCellVoltages(data);
            break;
    }
}

void parseBmsHeartbeat()
{
    batteryData.bmsLastReceived = osKernelSysTick();
}

void parseStartupInfo(uint8_t* data)
{

    batteryData.bmsRelayStatus = data[0];

    batteryData.populatedCells = data[1];

    uint16_t input12voltInt = // Units 0.1 V
        (data[2] << 0) |
        (data[3] << 8);
    batteryData.input12volt = (float)input12voltInt / 10.0f;

    uint16_t fanVoltageInt = // Units 0.01 V
        (data[4] << 0) |
        (data[5] << 8);
    batteryData.fanVoltage = (float)fanVoltageInt / 100.0f;
}

void parsePackInfo(uint8_t* data)
{
    uint16_t packCurrentInt =  // Units 0.1 A
        (data[0] << 0) |
        (data[1] << 8);
    batteryData.packCurrent = (float)packCurrentInt / 10.0f;

    uint16_t packVoltageInt =  // Units 0.1 V
        (data[2] << 0) |
        (data[3] << 8);
    batteryData.packVoltage = (float)packVoltageInt / 10.0f;

    batteryData.packStateofCharge = (float)data[4] / 2.0f; // Units 0.5%

    uint16_t packAmphoursCan =  // Units 0.1 Ah
        (data[5] << 0) |
        (data[6] << 8);
    batteryData.packAmphours = (float)packAmphoursCan / 10.0f;

    batteryData.packDepthofDischarge = data[7] / 2.0f; // Units 0.5%
}

void parseErrors(uint8_t* data)
{
    uint32_t limitFlag =
        (data[0] << 0) |
        (data[1] << 8);
    batteryFaultsData.batteryLimitFlags.dclReducedDueToLowSoc =
        limitFlag & DCL_REDUCED_DUE_TO_LOW_SOC_MASK;
    batteryFaultsData.batteryLimitFlags.dclReducedDueToHighCellResistance =
        limitFlag & DCL_REDUCED_DUE_TO_HIGH_CELL_RESISTANCE_MASK;
    batteryFaultsData.batteryLimitFlags.dclReducedDueToTemperature =
        limitFlag & DCL_REDUCED_DUE_TO_TEMPERATURE_MASK;
    batteryFaultsData.batteryLimitFlags.dclReducedDueToLowCellVoltage =
        limitFlag & DCL_REDUCED_DUE_TO_LOW_CELL_VOLTAGE_MASK;
    batteryFaultsData.batteryLimitFlags.dclReducedDueToLowPackVoltage =
        limitFlag & DCL_REDUCED_DUE_TO_LOW_PACK_VOLTAGE_MASK;
    batteryFaultsData.batteryLimitFlags.dclAndCclReducedDueToVoltageFailsafe =
        limitFlag & DCL_AND_CCL_REDUCED_DUE_TO_VOLTAGE_FAILSAFE_MASK;
    batteryFaultsData.batteryLimitFlags.dclAndCclReducedDueToCommunicationFailsafe =
        limitFlag & DCL_AND_CCL_REDUCED_DUE_TO_COMMUNICATION_FAILSAFE_MASK;
    batteryFaultsData.batteryLimitFlags.cclReducedDueToHighSoc =
        limitFlag & CCL_REDUCED_DUE_TO_HIGH_SOC_MASK;
    batteryFaultsData.batteryLimitFlags.cclReducedDueToHighCellResistance =
        limitFlag & CCL_REDUCED_DUE_TO_HIGH_CELL_RESISTANCE_MASK;
    batteryFaultsData.batteryLimitFlags.cclReducedDueToTemperature =
        limitFlag & CCL_REDUCED_DUE_TO_TEMPERATURE_MASK;
    batteryFaultsData.batteryLimitFlags.cclReducedDueToHighCellVoltage =
        limitFlag & CCL_REDUCED_DUE_TO_HIGH_CELL_VOLTAGE_MASK;
    batteryFaultsData.batteryLimitFlags.cclReducedDueToHighPackVoltage =
        limitFlag & CCL_REDUCED_DUE_TO_HIGH_PACK_VOLTAGE_MASK;
    batteryFaultsData.batteryLimitFlags.cclReducedDueToChargerLatch =
        limitFlag & CCL_REDUCED_DUE_TO_CHARGER_LATCH_MASK;
    batteryFaultsData.batteryLimitFlags.cclReducedDueToAlternateCurrentLimit =
        limitFlag & CCL_REDUCED_DUE_TO_ALTERNATE_CURRENT_LIMIT_MASK;

    uint32_t errorFlag =
        (data[2] << 0) |
        (data[3] << 8) |
        (data[4] << 16);
    batteryFaultsData.batteryErrorFlags.internalCommununicationFault =
        errorFlag & INTERNAL_COMMUNUNICATION_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.internalConversionFault =
        errorFlag & INTERNAL_CONVERSION_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.weakCellFault =
        errorFlag & WEAK_CELL_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.lowCellVoltageFault =
        errorFlag & LOW_CELL_VOLTAGE_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.openWiringFault =
        errorFlag & OPEN_WIRING_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.currentSensorFault =
        errorFlag & CURRENT_SENSOR_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.packVoltageSensorFault =
        errorFlag & PACK_VOLTAGE_SENSOR_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.weakPackFault =
        errorFlag & WEAK_PACK_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.voltageRedundancyFault =
        errorFlag & VOLTAGE_REDUNDANCY_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.fanMonitorFault =
        errorFlag & FAN_MONITOR_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.thermistorFault =
        errorFlag & THERMISTOR_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.canBusCommunicationsFault =
        errorFlag & CANBUS_COMMUNICATIONS_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.alwaysOnSupplyFault =
        errorFlag & ALWAYS_ON_SUPPLY_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.highVoltageIsolationFault =
        errorFlag & HIGH_VOLTAGE_ISOLATION_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.power12vSupplyFault =
        errorFlag & POWER_12V_SUPPLY_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.chargeLimitEnforcementFault =
        errorFlag & CHARGE_LIMIT_ENFORCEMENT_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.dischargeLimitEnforcementFault =
        errorFlag & DISCHARGE_LIMIT_ENFORCEMENT_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.chargerSafetyRelayFault =
        errorFlag & CHARGER_SAFETY_RELAY_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.internalMemoryFault =
        errorFlag & INTERNAL_MEMORY_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.internalThermistorFault =
        errorFlag & INTERNAL_THERMISTOR_FAULT_MASK;
    batteryFaultsData.batteryErrorFlags.internalLogicFault =
        errorFlag & INTERNAL_LOGIC_FAULT_MASK;
}

void parseTempInfo(uint8_t* data)
{
    batteryData.highTemperature = data[0];
    batteryData.highThermistorId = data[1];
    batteryData.lowTemperature = data[2];
    batteryData.lowThermistorId = data[3];
    batteryData.averageTemperature = data[4];
    batteryData.internalTemperature = data[5];
    batteryData.fanSpeed = data[6];
    batteryData.requestedFanSpeed = data[7];
}

void parseCellVoltages(uint8_t* data)
{
    batteryData.lowCellVoltage =
        (data[0] << 0) |
        (data[1] << 8);
    batteryData.lowCellVoltageId =
        data[2];
    batteryData.highCellVoltage =
        (data[3] << 0) |
        (data[4] << 8);
    batteryData.highCellVoltageId =
        data[5];
    batteryData.averageCellVoltage =
        (data[6] << 0) |
        (data[7] << 8);
}
