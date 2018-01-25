#include "cmsis_os.h"
#include "stm32f4xx.h"

#include "TelemetryReporting.h"
#include "TelemetryUtils.h"
#include "TransmitSerial.h"

#include "KeyMotorData.h"
#include "MotorDetailsData.h"
#include "DriverControlData.h"
#include "MotorFaultsData.h"
#include "BatteryFaultsData.h"
#include "BatteryData.h"
#include "MpptData.h"
#include "LightsData.h"

#define KEY_MOTOR_LENGTH (43)
#define MOTOR_DETAILS_LENGTH (69)
#define DRIVER_CONTROLS_LENGTH (9)
#define MOTOR_FAULTS_LENGTH (9)
#define BATTERY_FAULTS_LENGTH (6)
#define BATTERY_DETAILS_LENGTH (51)
#define MPPT_DETAILS_LENGTH (10)
#define LIGHTS_DETAILS_LENGTH (3)

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

    transmitMessage(packetPayload, packetLength);
}

void sendMotorDetails(int n)
{
    unsigned int unframedPacketLength = MOTOR_DETAILS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    switch (n)
    {
        case 0:
            packetPayload[0] = M0_DETAILS_DATA_PKG_ID;
            writeFloatIntoArray(packetPayload, 1, motor0DetailsData.phaseCCurrent);
            writeFloatIntoArray(packetPayload, 5, motor0DetailsData.phaseBCurrent);
            writeFloatIntoArray(packetPayload, 9, motor0DetailsData.motorVoltageReal);
            writeFloatIntoArray(packetPayload, 13, motor0DetailsData.motorVoltageImaginary);
            writeFloatIntoArray(packetPayload, 17, motor0DetailsData.motorCurrentReal);
            writeFloatIntoArray(packetPayload, 21, motor0DetailsData.motorCurrentImaginary);
            writeFloatIntoArray(packetPayload, 25, motor0DetailsData.backEmfReal);
            writeFloatIntoArray(packetPayload, 29, motor0DetailsData.backEmfImaginary);
            writeFloatIntoArray(packetPayload, 33, motor0DetailsData.railSupply15v);
            writeFloatIntoArray(packetPayload, 37, motor0DetailsData.railSupply3_3v);
            writeFloatIntoArray(packetPayload, 41, motor0DetailsData.railSupply1_9v);
            writeFloatIntoArray(packetPayload, 45, motor0DetailsData.heatSinkTemperature);
            writeFloatIntoArray(packetPayload, 49, motor0DetailsData.motorTemperature);
            writeFloatIntoArray(packetPayload, 53, motor0DetailsData.dspBoardTemp);
            writeFloatIntoArray(packetPayload, 57, motor0DetailsData.dcBusAmpHours);
            writeFloatIntoArray(packetPayload, 61, motor0DetailsData.odometer);
            writeFloatIntoArray(packetPayload, 65, motor0DetailsData.slipSpeed);
            break;

        case 1:
            packetPayload[0] = M1_DETAILS_DATA_PKG_ID;
            writeFloatIntoArray(packetPayload, 1, motor1DetailsData.phaseCCurrent);
            writeFloatIntoArray(packetPayload, 5, motor1DetailsData.phaseBCurrent);
            writeFloatIntoArray(packetPayload, 9, motor1DetailsData.motorVoltageReal);
            writeFloatIntoArray(packetPayload, 13, motor1DetailsData.motorVoltageImaginary);
            writeFloatIntoArray(packetPayload, 17, motor1DetailsData.motorCurrentReal);
            writeFloatIntoArray(packetPayload, 21, motor1DetailsData.motorCurrentImaginary);
            writeFloatIntoArray(packetPayload, 25, motor1DetailsData.backEmfReal);
            writeFloatIntoArray(packetPayload, 29, motor1DetailsData.railSupply15v);
            writeFloatIntoArray(packetPayload, 33, motor1DetailsData.railSupply3_3v);
            writeFloatIntoArray(packetPayload, 37, motor1DetailsData.railSupply1_9v);
            writeFloatIntoArray(packetPayload, 41, motor1DetailsData.heatSinkTemperature);
            writeFloatIntoArray(packetPayload, 45, motor1DetailsData.motorTemperature);
            writeFloatIntoArray(packetPayload, 49, motor1DetailsData.dspBoardTemp);
            writeFloatIntoArray(packetPayload, 53, motor1DetailsData.dcBusAmpHours);
            writeFloatIntoArray(packetPayload, 57, motor1DetailsData.odometer);
            writeFloatIntoArray(packetPayload, 61, motor1DetailsData.slipSpeed);
            break;
    }

    addChecksum(packetPayload, MOTOR_DETAILS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packetPayload, packetLength);
}

void sendDriverControls()
{
    unsigned int unframedPacketLength = DRIVER_CONTROLS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = DRIVER_CONTROL_PKG_ID;
    unsigned char driverControlsBoardAliveArray[] = {messageIsRecent(driverControlData.lastReceived)};
    writeBoolsIntoArray(packetPayload, 1, driverControlsBoardAliveArray, 1);
    writeBoolsIntoArray(packetPayload, 2, &driverControlData.lightsInputs, 7);
    writeBoolsIntoArray(packetPayload, 3, &driverControlData.musicInputs, 4);
    writeUShortIntoArray(packetPayload, 4, driverControlData.acceleration);
    writeUShortIntoArray(packetPayload, 6, driverControlData.regenBraking);
    writeBoolsIntoArray(packetPayload, 8, &driverControlData.driverInputs, 7);

    addChecksum(packetPayload, DRIVER_CONTROLS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packetPayload, packetLength);
}

void sendMotorFaults()
{
    unsigned int unframedPacketLength = MOTOR_FAULTS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = MOTOR_FAULTS_PKG_ID;
    writeBoolsIntoArray(packetPayload, 1, &motorFaultsData.m0ErrorFlags, 8);
    writeBoolsIntoArray(packetPayload, 2, &motorFaultsData.m1ErrorFlags, 8);
    writeBoolsIntoArray(packetPayload, 3, &motorFaultsData.m0LimitFlags, 7);
    writeBoolsIntoArray(packetPayload, 4, &motorFaultsData.m1LimitFlags, 7);
    packetPayload[5] = motorFaultsData.m0CanRxErrorCount;
    packetPayload[6] = motorFaultsData.m0CanTxErrorCount;
    packetPayload[7] = motorFaultsData.m1CanRxErrorCount;
    packetPayload[8] = motorFaultsData.m1CanTxErrorCount;

    addChecksum(packetPayload, MOTOR_FAULTS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packetPayload, packetLength);
}

void sendBatteryFaults()
{
    unsigned int unframedPacketLength = BATTERY_FAULTS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = BATTERY_FAULTS_PKG_ID;
    writeBoolsIntoArray(packetPayload, 1, &batteryFaultsData.batteryErrorFlags, 21);
    writeBoolsIntoArray(packetPayload, 4, &batteryFaultsData.batteryLimitFlags, 16);
    addChecksum(packetPayload, BATTERY_FAULTS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packetPayload, packetLength);
}

void sendBattery()
{
    unsigned int unframedPacketLength = BATTERY_DETAILS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = BATTERY_PKG_ID;
    unsigned char BmuAliveArray[] = {messageIsRecent(batteryData.bmsLastReceived)};
    writeBoolsIntoArray(packetPayload, 1, BmuAliveArray, 1);
    writeBoolsIntoArray(packetPayload, 2, &batteryData.bmsRelayStatus, 8);
    packetPayload[3] = batteryData.populatedCells;
    writeFloatIntoArray(packetPayload, 4, batteryData.input12volt);
    writeFloatIntoArray(packetPayload, 8, batteryData.fanVoltage);
    writeFloatIntoArray(packetPayload, 12, batteryData.packCurrent);
    writeFloatIntoArray(packetPayload, 16, batteryData.packVoltage);
    writeFloatIntoArray(packetPayload, 20, batteryData.packStateofCharge);
    writeFloatIntoArray(packetPayload, 24, batteryData.packAmphours);
    writeFloatIntoArray(packetPayload, 28, batteryData.packDepthofDischarge);
    packetPayload[32] = batteryData.highTemperature;
    packetPayload[33] = batteryData.highThermistorId;
    packetPayload[34] = batteryData.lowTemperature;
    packetPayload[35] = batteryData.lowThermistorId;
    packetPayload[36] = batteryData.averageTemperature;
    packetPayload[37] = batteryData.internalTemperature;
    packetPayload[38] = batteryData.fanSpeed;
    packetPayload[39] = batteryData.requestedFanSpeed;
    writeUShortIntoArray(packetPayload, 40, batteryData.lowCellVoltage);
    packetPayload[42] = batteryData.lowCellVoltageId;
    writeUShortIntoArray(packetPayload, 43, batteryData.highCellVoltage);
    packetPayload[45] = batteryData.highCellVoltageId;
    writeUShortIntoArray(packetPayload, 46, batteryData.averageCellVoltage);
    packetPayload[48] = batteryData.prechargeState;
    packetPayload[49] = batteryData.auxVoltage;
    unsigned char auxBmsAliveArray[] = {messageIsRecent(batteryData.auxBmsLastReceived)};
    writeBoolsIntoArray(packetPayload, 50, auxBmsAliveArray, 1);

    addChecksum(packetPayload, BATTERY_DETAILS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packetPayload, packetLength);
}

void sendMppt(int n)
{
    unsigned int unframedPacketLength = MPPT_DETAILS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = MPPT_PKG_ID;
    unsigned char numberAndAlive = (unsigned char)n & 0x03;

    if (messageIsRecent(mpptData[n].lastReceived))
    {
        numberAndAlive |= 0x80;
    }

    packetPayload[1] = numberAndAlive;

    writeUShortIntoArray(packetPayload, 2, mpptData[n].arrayVoltage);
    writeUShortIntoArray(packetPayload, 4, mpptData[n].arrayCurrent);
    writeUShortIntoArray(packetPayload, 6, mpptData[n].batteryVoltage);
    writeUShortIntoArray(packetPayload, 8, mpptData[n].temperature);

    addChecksum(packetPayload, MPPT_DETAILS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packetPayload, packetLength);
}

void sendLights()
{
    unsigned int unframedPacketLength = LIGHTS_DETAILS_LENGTH + CHECKSUM_LENGTH;
    unsigned char packetPayload[unframedPacketLength];

    packetPayload[0] = LIGHT_PKG_ID;
    unsigned char lightsAliveArray[] = {messageIsRecent(lightsData.lastReceived)};
    writeBoolsIntoArray(packetPayload, 1, lightsAliveArray, 1);
    writeBoolsIntoArray(packetPayload, 2, &lightsData.lowBeams, 6);

    addChecksum(packetPayload, LIGHTS_DETAILS_LENGTH);
    unsigned char packet[unframedPacketLength + FRAMING_LENGTH_INCREASE];
    unsigned int packetLength = frameData(packetPayload, unframedPacketLength, packet);

    transmitMessage(packetPayload, packetLength);
}
