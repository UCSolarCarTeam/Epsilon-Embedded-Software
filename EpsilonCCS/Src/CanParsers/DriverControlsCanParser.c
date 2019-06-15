#include "cmsis_os.h"

#include "DriverControlsCanParser.h"

#include "DriverControlData.h"

#define DRIVER_CONTROLS_HEARTBEAT_ID (0x700)
#define DRIVER_CONTROLS_LIGHTS_ID (0x701)
#define DRIVER_CONTROLS_MUSIC_ID (0x702)
#define DRIVER_CONTROLS_DRIVER_ID (0x703)

void parseDriverControlsCanMessage(uint32_t stdId, uint8_t* data)
{
    switch (stdId)
    {
        case DRIVER_CONTROLS_HEARTBEAT_ID:
            parseDriverControlsHearbeat();
            break;

        case DRIVER_CONTROLS_LIGHTS_ID:
            parseDriverControlsLightsInput(data);
            break;

        case DRIVER_CONTROLS_MUSIC_ID:
            parseDriverControlsMusicInput(data);
            break;

        case DRIVER_CONTROLS_DRIVER_ID :
            parseDriverControlsDriverInput(data);
            break;
    }
}

void parseDriverControlsHearbeat()
{
    driverControlData.lastReceived = osKernelSysTick();
}

void parseDriverControlsLightsInput(uint8_t* data)
{
    driverControlData.lightsInputs.headlightsOff = data[0] & HEADLIGHTS_OFF_MASK;
    driverControlData.lightsInputs.headlightsLow = data[0] & HEADLIGHTS_LOW_MASK;
    driverControlData.lightsInputs.headlightsHigh = data[0] & HEADLIGHTS_HIGH_MASK;
    driverControlData.lightsInputs.signalRight = data[0] & SIGNAL_RIGHT_MASK;
    driverControlData.lightsInputs.signalLeft = data[0] & SIGNAL_LEFT_MASK;
    driverControlData.lightsInputs.hazard = data[0] & HAZARD_MASK;
    driverControlData.lightsInputs.interior = data[0] & INTERIOR_MASK;
}

void parseDriverControlsMusicInput(uint8_t* data)
{
    driverControlData.musicInputs.volumeUp = data[0] & VOLUME_UP_MASK;
    driverControlData.musicInputs.volumeDown = data[0] & VOLUME_DOWN_MASK;
    driverControlData.musicInputs.nextSong = data[0] & NEXT_SONG_MASK;
    driverControlData.musicInputs.prevSong = data[0] & PREV_SONG_MASK;
}

void parseDriverControlsDriverInput(uint8_t* data)
{
    driverControlData.acceleration = // bits 0-11
        ((data[0] & 0xFF) << 0) | // bits 0-7
        ((data[1] & 0x0F) << 8);  // bits 8-11
    driverControlData.regenBraking = // bits 12-23
        ((data[1] & 0xF0) >> 4) | // bits 12-19
        ((data[2] & 0xFF) << 4);  // bits 20-23
    // bits 24-29
    driverControlData.driverInputs.brakes = data[3] & BRAKES_MASK;
    driverControlData.driverInputs.forward = data[3] & FORWARD_MASK;
    driverControlData.driverInputs.reverse = data[3] & REVERSE_MASK;
    driverControlData.driverInputs.pushToTalk = data[3] & PUSH_TO_TALK_MASK;
    driverControlData.driverInputs.horn = data[3] & HORN_MASK;
    driverControlData.driverInputs.reset = data[3] & RESET_MASK;
    driverControlData.driverInputs.aux = data[3] & AUX_MASK;
    driverControlData.driverInputs.lap = data[3] & LAP_MASK;
}
