#include "cmsis_os.h"

#include "AuxBmsCanParser.h"

#include "AuxBmsData.h"

#define AUX_BMS_HEARTBEAT_ID (0x720)
#define AUX_BMS_INFO_ID (0x721)
#define AUX_BMS_AUX_TRIP_ID (0x722)

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

        case AUX_BMS_AUX_TRIP_ID:
            parseAuxTrip(data);
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
    auxBmsData.auxVoltage = (data[0] & AUX_VOLTAGE_MASK) >> 3;
    auxBmsData.strobeBmsLight = data[1] & STROBE_MASK;
    auxBmsData.allowCharge = data[1] & ALLOW_CHARGE_MASK;
    auxBmsData.highVoltageEnableState = data[1] & HIGH_VOLTAGE_ENABLE_STATE_MASK;
    auxBmsData.allowDischarge = data[1] & ALLOW_DISCHARGE_MASK;
    auxBmsData.orionCanReceivedRecently = data[1] & ORION_CAN_RECEIVED_RECENTLY_MASK;

    auxBmsData.contactorDebugInfo.chargeContactorError = data[1] & CHARGE_CONTACTOR_ERROR_MASK;
    auxBmsData.contactorDebugInfo.dischargeContactorError = data[1] & DISCHARGE_CONTACTOR_ERROR_MASK;
    auxBmsData.contactorDebugInfo.commonContactorError = data[1] & COMMON_CONTACTOR_ERROR_MASK;
    auxBmsData.contactorDebugInfo.dischargeShouldTrip = data[2] & DISCHARGE_SHOULD_TRIP_MASK;
    auxBmsData.contactorDebugInfo.chargeShouldTrip = data[2] & CHARGE_SHOULD_TRIP_MASK;
    auxBmsData.contactorDebugInfo.chargeOpenButShouldBeClosed = data[2] & CHARGE_OPEN_BUT_SHOULD_BE_CLOSED_MASK;
    auxBmsData.contactorDebugInfo.dischargeOpenButShouldBeClosed = data[2] & DISCHARGE_OPEN_BUT_SHOULD_BE_CLOSED_MASK;
}

void parseAuxTrip(uint8_t* data)
{
    auxBmsData.auxTrip.chargeTripDueToHighCellVoltage = data[0] & CHARGE_TRIP_DUE_TO_HIGH_CELL_VOLTAGE_MASK;
    auxBmsData.auxTrip.chargeTripDueToHighTemperatureAndCurrent = data[0] & CHARGE_TRIP_DUE_TO_HIGH_TEMPERATURE_AND_CURRENT_MASK;
    auxBmsData.auxTrip.chargeTripDueToPackCurrent = data[0] & CHARGE_TRIP_DUE_TO_PACK_CURRENT_MASK;
    auxBmsData.auxTrip.dischargeTripDueToLowCellVoltage = data[0] & DISCHARGE_TRIP_DUE_TO_LOW_CELL_VOLTAGE_MASK;
    auxBmsData.auxTrip.dischargeTripDueToHighTemperatureAndCurrent = data[0] & DISCHARGE_TRIP_DUE_TO_HIGH_TEMPERATURE_AND_CURRENT_MASK;
    auxBmsData.auxTrip.dischargeTripDueToPackCurrent = data[0] & DISCHARGE_TRIP_DUE_TO_PACK_CURRENT_MASK;
    auxBmsData.auxTrip.protectionTrip = data[0] & PROTECTION_TRIP_MASK;
    auxBmsData.auxTrip.tripDueToOrionMessageTimeout = data[0] & TRIP_DUE_TO_ORION_MESSAGE_TIMEOUT_MASK;

    auxBmsData.auxTrip.chargeNotClosedDueToHighCurrent = data[1] & CHARGE_NOT_CLOSED_DUE_TO_HIGH_CURRENT_MASK;
    auxBmsData.auxTrip.dischargeNotClosedDueToHighCurrent = data[1] & DISCHARGE_NOT_CLOSED_DUE_TO_HIGH_CURRENT_MASK;
    auxBmsData.auxTrip.tripDueToContactorDisconnectedUnexpectedly = data[1] & TRIP_DUE_TO_CONTACTOR_DISCONNECTED_UNEXPECTEDLY_MASK;
}
