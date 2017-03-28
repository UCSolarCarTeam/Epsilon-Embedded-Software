#include "cmsis_os.h"
#include "stm32f4xx.h"

#include "TelemetryReporting.h"
#include "TelemetryUtils.h"

#include "KeyMotorData.h"

#define KEY_MOTOR_LENGTH (43)

#define CCS_TELEM_PERIOD_MS (200) // 5Hz == 200ms

void sendTelemetryTask()
{
    uint32_t prevWakeTime = osKernelSysTick();
    // Salvos defined in https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit#gid=782574835
    static unsigned char salvo = 1;

    for (;;)
    {
        osDelayUntil(&prevWakeTime, CCS_TELEM_PERIOD_MS);

        sendKeyMotor();
        sendLights();
        sendMotorFaults();
        sendBatteryFaults();
        sendDriverControls();
        sendBattery();

        switch (salvo)
        {
            case 1:
                sendMotorDetails(0);
                sendMppt(0);
                sendMppt(1);
                salvo = 2;
                break;

            case 2:
                sendMotorDetails(1);
                sendMppt(2);
                salvo = 1;
                break;

            default:
                // Error state
                salvo = 1;
                break;
        }

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

void sendMotorDetails(int n)
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

void sendMppt(int n)
{

}

void sendLights()
{

}
