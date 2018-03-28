#include "cmsis_os.h"

#include "AuxBmsCanParser.h"

#include "AuxBmsData.h"

#define AUX_BMS_HEARTBEAT_ID (0x720)
#define AUX_BMS_INFO_ID (0x721)

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
    auxBmsData.auxBmsLastReceived = osKernelSysTick();
}

void parseAuxBmsInfo(uint8_t* data)
{
    auxBmsData.prechargeState = data[0] & PRECHARGE_STATE_MASK;
    auxBmsData.auxVoltage = data[0] & AUX_VOLTAGE_MASK;
    auxBmsData.strobeBmsLight = data[1] & STROBE_MASK;
    auxBmsData.allowCharge = data[1] & ALLOW_CHARGE_MASK;
    auxBmsData.contactorError = data[1] & CONTACTOR_ERROR_MASK;
}
