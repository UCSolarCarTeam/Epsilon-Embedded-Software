#include "cmsis_os.h"

#include "AuxBmsCanParser.h"

#include "BatteryData.h"

#define AUX_BMS_HEARTBEAT_ID (0x720)
#define AUX_BMS_INFO_ID (0x721)

#define PRECHARGE_STATE_MASK (0x07) // First 3 bits
#define AUX_VOLTAGE_MASK (0xF8) // Last 5 bits

void parseAuxBmsCanMessage(uint32_t stdId, uint8_t* data)
{
    switch (stdId)
    {
        case AUX_BMS_HEARTBEAT_ID:
            parseAuxBmsHeartbeat();
            break;

        case AUX_BMS_INFO_ID:
            parseAuxBmsInfo(data);
            break;
    }
}

void parseAuxBmsHeartbeat()
{
    batteryData.auxBmsLastReceived = osKernelSysTick();
}

void parseAuxBmsInfo(uint8_t* data)
{
    batteryData.prechargeState = data[0] & PRECHARGE_STATE_MASK;
    batteryData.auxVoltage = data[0] & AUX_VOLTAGE_MASK;
}
