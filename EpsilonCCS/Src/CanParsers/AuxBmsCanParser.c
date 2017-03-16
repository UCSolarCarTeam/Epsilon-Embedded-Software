#include "AuxBmsCanParser.h"

#include "BatteryData.h"

extern struct BatteryData batteryData;

#define AUX_BMS_HEARTBEAT_ID (0x720)
#define AUX_BMS_INFO_ID (0x721)

#define PRECHARGE_STATE_MASK (0x00) // First 3 bits
#define AUX_VOLTAGE_MASK (0xF8) // Last 5 bits

void parseAuxBmsCanMessage(uint32_t stdId, uint8_t data[8])
{
	switch (stdId)
	{
		case AUX_BMS_HEARTBEAT_ID:
			parseAuxBmsHeartbeat(data);
			break;
		case AUX_BMS_INFO_ID:
			parseAuxBmsInfo(data);
			break;
	}
}

void parseAuxBmsHeartbeat(uint8_t data[8])
{
    batteryData.auxBmsAlive = data[0];
}

void parseAuxBmsInfo(uint8_t data[8])
{
	batteryData.prechargeState = data[0] & PRECHARGE_STATE_MASK;
	batteryData.auxVoltage = data[0] & AUX_VOLTAGE_MASK;
}

