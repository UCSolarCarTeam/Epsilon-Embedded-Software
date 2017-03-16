#include "LightsCanParser.h"

#include "LightsData.h"

extern struct LightsData lightsData;

#define LIGHTS_HEARTBEAT_ID (0x710)
#define LIGHTS_STATUS_ID (0x711)

void parseLightsCanMessage(uint32_t stdId, uint8_t* data)
{
	switch (stdId)
	{
		case LIGHTS_HEARTBEAT_ID:
			parseLightsHeartbeat(data);
			break;
		case LIGHTS_STATUS_ID:
			parseLightsStatus(data);
			break;
	}
}

void parseLightsHeartbeat(uint8_t* data)
{
	lightsData.lightsAlive = data[0];
}

void parseLightsStatus(uint8_t* data)
{
	lightsData.lowBeams = data[0] & LOW_BEAMS_MASK;
	lightsData.highBeams = data[0] & HIGH_BEAMS_MASK;
	lightsData.brakes = data[0] & BRAKES_MASK;
	lightsData.leftSignal = data[0] & LEFT_SIGNAL_MASK;
	lightsData.rightSignal = data[0] & RIGHT_SIGNAL_MASK;
	lightsData.bmsStrobeLight = data[0] & BMS_STROBE_LIGHT_MASK;
}
