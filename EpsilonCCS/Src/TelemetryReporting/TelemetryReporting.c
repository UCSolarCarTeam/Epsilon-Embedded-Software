#include "TelemetryReporting.h"
#include "TelemetryUtils.h"

#include "KeyMotorData.h"

#define KEY_MOTOR_LENGTH (43)

void sendTelemetryTask()
{
    for (;;)
    {

    }
}

void sendKeyMotor()
{
    unsigned int unframedPacketLength = KEY_MOTOR_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = KEY_MOTOR_PKG_ID;

    unsigned char motor0AliveArray[] = {messageIsRecent(keyMotorData.m0lastReceived)};
    writeBoolsIntoArray(packetPayload, 1, motor0AliveArray, 1);
    writeFloatIntoArray(packetPayload, 2, keyMotorData.m0SetCurrent);
    writeFloatIntoArray(packetPayload, 6, keyMotorData.m0SetVelocity);
    writeFloatIntoArray(packetPayload, 10, keyMotorData.m0BusCurrent);
    writeFloatIntoArray(packetPayload, 14, keyMotorData.m0BusVoltage);
    writeFloatIntoArray(packetPayload, 18, keyMotorData.m0VehicleVelocity);
    unsigned char motor1AliveArray[] = {messageIsRecent(keyMotorData.m0lastReceived)};
    writeBoolsIntoArray(packetPayload, 22, motor1AliveArray, 1);
    writeFloatIntoArray(packetPayload, 23, keyMotorData.m1SetCurrent);
    writeFloatIntoArray(packetPayload, 27, keyMotorData.m1SetVelocity);
    writeFloatIntoArray(packetPayload, 31, keyMotorData.m1BusCurrent);
    writeFloatIntoArray(packetPayload, 35, keyMotorData.m1BusVoltage);
    writeFloatIntoArray(packetPayload, 39, keyMotorData.m1VehicleVelocity);

    addChecksum(packetPayload, KEY_MOTOR_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    // TODO: Send over USB UART
}

void sendMotorDetails()
{

}

void sendDriverControls()
{

}

void sendMotorFaults()
{

}

void sendBatteryFaults()
{

}

void sendBattery()
{

}

void sendCmu()
{

}

void sendMppt()
{

}

void sendLights()
{

}
