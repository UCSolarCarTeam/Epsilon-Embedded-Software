#pragma once

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing

#define BATTERY_FAULTS_PKG_ID 6

// Error Flags
#define INTERNAL_COMMUNUNICATION_FAULT_MASK 0X0000_0001
#define INTERNAL_CONVERSION_FAULT_MASK 0X0000_0002
#define WEAK_CELL_FAULT_MASK 0X0000_0004
#define LOW_CELL_VOLTAGE_FAULT_MASK 0X0000_0008
#define OPEN_WIRING_FAULT_MASK 0X0000_0010
#define CURRENT_SENSOR_FAULT_MASK 0X0000_0020
#define PACK_VOLTAGE_SENSOR_FAULT_MASK 0X0000_0040
#define WEAK_PACK_FAULT_MASK 0X0000_0080
#define VOLTAGE_REDUNDANCY_FAULT_MASK 0X0000_0100
#define FAN_MONITOR_FAULT_MASK 0X0000_0200
#define THERMISTOR_FAULT_MASK 0X0000_0400
#define CANBUS_COMMUNICATIONS_FAULT_MASK 0X0000_0800
#define ALWAYS_ON_SUPPLY_FAULT_MASK 0x0000_1000
#define HIGH_VOLTAGE_ISOLATION_FAULT_MASK 0X0000_2000
#define POWER_12V_SUPPLY_FAULT_MASK 0X0000_4000
#define CHARGE_LIMIT_ENFORCEMENT_FAULT_MASK 0X0000_8000
#define DISCHARGE_LIMIT_ENFORCEMENT_FAULT_MASK 0X0001_0000
#define CHARGER_SAFETY_RELAY_FAULT_MASK 0X0002_0000
#define INTERNAL_MEMORY_FAULT_MASK 0X0004_0000
#define INTERNAL_THERMISTOR_FAULT_MASK 0X0008_0000
#define INTERNAL_LOGIC_FAULT_MASK 0X0010_0000

// Limit Flags
#define DCL_REDUCED_DUE_TO_LOW_SOC_MASK 0x0001
#define DCL_REDUCED_DUE_TO_HIGH_CELL_RESISTANCE_MASK 0x0002
#define DCL_REDUCED_DUE_TO_TEMPERATURE_MASK 0x0004
#define DCL_REDUCED_DUE_TO_LOW_CELL_VOLTAGE_MASK 0x0008
#define DCL_REDUCED_DUE_TO_LOW_PACK_VOLTAGE_MASK 0x0010
#define DCL_AND_CCL_REDUCED_DUE_TO_VOLTAGE_FAILSAFE_MASK 0x0040
#define DCL_AND_CCL_REDUCED_DUE_TO_COMMUNICATION_FAILSAFE_MASK 0x0080
#define CCL_REDUCED_DUE_TO_HIGH_SOC_MASK 0x0200
#define CCL_REDUCED_DUE_TO_HIGH_CELL_RESISTANCE_MASK 0x0400
#define CCL_REDUCED_DUE_TO_TEMPERATURE_MASK 0x0800
#define CCL_REDUCED_DUE_TO_HIGH_CELL_VOLTAGE_MASK 0x1000
#define CCL_REDUCED_DUE_TO_HIGH_PACK_VOLTAGE_MASK 0x2000
#define CCL_REDUCED_DUE_TO_CHARGER_LATCH_MASK 0x4000
#define CCL_REDUCED_DUE_TO_ALTERNATE_CURRENT_LIMIT_MASK 0x8000

struct BatteryFaultsData {
	unsigned char packageId;
	struct BatteryErrorFlags batteryErrorFlags;
	struct BatteryLimitFlags batteryLimitFlags;
}

struct BatteryErrorFlags {
	unsigned char internalCommununicationFault;
	unsigned char internalConversionFault;
	unsigned char weakCellFault;
	unsigned char lowCellVoltageFault;
	unsigned char openWiringFault;
	unsigned char currentSensorFault;
	unsigned char packVoltageSensorFault;
	unsigned char weakPackFault;
	unsigned char voltageRedundancyFault;
	unsigned char fanMonitorFault;
	unsigned char thermistorFault;
	unsigned char canBusCommunicationsFault;
	unsigned char alwaysOnSupplyFault;
	unsigned char highVoltageIsolationFault;
	unsigned char power12vSupplyFault;
	unsigned char chargeLimitEnforcementFault;
	unsigned char dischargeLimitEnforcementFault;
	unsigned char chargerSafetyRelayFault;
	unsigned char internalMemoryFault;
	unsigned char internalThermistorFault;
	unsigned char internalLogicFault;
}

struct BatteryLimitFlags {
	unsigned char dclReducedDueToLowSoc;
	unsigned char dclReducedDueToHighCellResistance;
	unsigned char dclReducedDueToTemperature;
	unsigned char dclReducedDueToLowCellVoltage;
	unsigned char dclReducedDueToLowPackVoltage;
	unsigned char dclAndCclReducedDueToVoltageFailsafe;
	unsigned char dclAndCclReducedDueToCommunicationFailsafe;
	unsigned char cclReducedDueToHighSoc;
	unsigned char cclReducedDueToHighCellResistance;
	unsigned char cclReducedDueToTemperature;
	unsigned char cclReducedDueToHighCellVoltage;
	unsigned char cclReducedDueToHighPackVoltage;
	unsigned char cclReducedDueToChargerLatch;
	unsigned char cclReducedDueToAlternateCurrentLimit;
}