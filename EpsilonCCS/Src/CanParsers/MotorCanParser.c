#include "cmsis_os.h"

#include "MotorCanParser.h"

#include "KeyMotorData.h"
#include "MotorDetailsData.h"
#include "MotorFaultsData.h"

#define M0_ID (0)
#define M1_ID (1)

#define M0_BASE_CAN_ID (0x400)
#define M0_STATUS_CAN_ID (0x401)
#define M0_BUS_CAN_ID (0x402)
#define M0_VELOCITY_CAN_ID (0x403)
#define M0_PHASE_CURRENT_CAN_ID (0x404)
#define M0_VOLTAGE_VECTOR_CAN_ID (0x405)
#define M0_CURRENT_VECTOR_CAN_ID (0x406)
#define M0_BACKEMF_CAN_ID (0x407)
#define M0_15V_VOLTAGE_RAIL_CAN_ID (0x408)
#define M0_3_3V_1_9V_VOLTAGE_RAIL_CAN_ID (0x409)
#define M0_HEATSINK_TEMPERATURE_CAN_ID (0x40B)
#define M0_DSP_BOARD_TEMPERATURE_CAN_ID (0x40C)
#define M0_ODOMETER_BUS_AMPHOURS_CAN_ID (0x40E)
#define M0_SLIP_SPEED_CAN_ID (0x417)
#define M1_BASE_CAN_ID (0x420)
#define M1_STATUS_CAN_ID (0x421)
#define M1_BUS_CAN_ID (0x422)
#define M1_VELOCITY_CAN_ID (0x423)
#define M1_PHASE_CURRENT_CAN_ID (0x424)
#define M1_VOLTAGE_VECTOR_CAN_ID (0x425)
#define M1_CURRENT_VECTOR_CAN_ID (0x426)
#define M1_BACKEMF_CAN_ID (0x427)
#define M1_15V_VOLTAGE_RAIL_CAN_ID (0x428)
#define M1_3_3V_1_9V_VOLTAGE_RAIL_CAN_ID (0x429)
#define M1_HEATSINK_TEMPERATURE_CAN_ID (0x42B)
#define M1_DSP_BOARD_TEMPERATURE_CAN_ID (0x42C)
#define M1_ODOMETER_BUS_AMPHOURS_CAN_ID (0x42E)
#define M1_SLIP_SPEED_CAN_ID (0x437)

#define MOTOR_COMMANDS_CAN_ID (0x501)

void parseMotorCanMessage(uint32_t stdId, uint8_t* data)
{
    float setVelocity; 
    float setCurrent;
    switch (stdId)
    {
        case MOTOR_COMMANDS_CAN_ID:
            
            setVelocity = arrayToFloat(&data[0]); 
            setCurrent = arrayToFloat(&data[4]);

            keyMotorData.m0SetVelocity = setVelocity;
            keyMotorData.m0SetCurrent = setCurrent;

            keyMotorData.m1SetVelocity = setVelocity;
            keyMotorData.m1SetCurrent = setCurrent;

            break;

        case M0_BASE_CAN_ID:
            keyMotorData.m0lastReceived = osKernelSysTick();
            break;

        case M0_STATUS_CAN_ID:
            parseMotorFaultMessage(M0_ID, data);
            break;

        case M0_BUS_CAN_ID:
            keyMotorData.m0BusVoltage = arrayToFloat(&data[0]);
            keyMotorData.m0BusCurrent = arrayToFloat(&data[4]);
            break;

        case M0_VELOCITY_CAN_ID:
            // motorVelocity = arrayToFloat(&data[0]);
            keyMotorData.m0VehicleVelocity = arrayToFloat(&data[4]);
            break;

        case M0_PHASE_CURRENT_CAN_ID:
            motor0DetailsData.phaseBCurrent = arrayToFloat(&data[0]);
            motor0DetailsData.phaseCCurrent = arrayToFloat(&data[4]);
            break;

        case M0_VOLTAGE_VECTOR_CAN_ID:
            motor0DetailsData.motorVoltageImaginary = arrayToFloat(&data[0]);
            motor0DetailsData.motorVoltageReal = arrayToFloat(&data[4]);
            break;

        case M0_CURRENT_VECTOR_CAN_ID:
            motor0DetailsData.motorCurrentImaginary = arrayToFloat(&data[0]);
            motor0DetailsData.motorCurrentReal = arrayToFloat(&data[4]);
            break;

        case M0_BACKEMF_CAN_ID:
            motor0DetailsData.backEmfImaginary = arrayToFloat(&data[0]);
            motor0DetailsData.backEmfReal = arrayToFloat(&data[4]);
            break;

        case M0_15V_VOLTAGE_RAIL_CAN_ID:
            motor0DetailsData.railSupply15v = arrayToFloat(&data[4]);
            break;

        case M0_3_3V_1_9V_VOLTAGE_RAIL_CAN_ID:
            motor0DetailsData.railSupply1_9v = arrayToFloat(&data[0]);
            motor0DetailsData.railSupply3_3v = arrayToFloat(&data[4]);
            break;

        case M0_HEATSINK_TEMPERATURE_CAN_ID:
            motor0DetailsData.motorTemperature = arrayToFloat(&data[0]);
            motor0DetailsData.heatSinkTemperature = arrayToFloat(&data[4]);
            break;

        case M0_DSP_BOARD_TEMPERATURE_CAN_ID:
            motor0DetailsData.dspBoardTemp = arrayToFloat(&data[0]);
            break;

        case M0_ODOMETER_BUS_AMPHOURS_CAN_ID:
            motor0DetailsData.odometer = arrayToFloat(&data[0]);
            motor0DetailsData.dcBusAmpHours = arrayToFloat(&data[4]);
            break;

        case M0_SLIP_SPEED_CAN_ID:
            motor0DetailsData.slipSpeed = arrayToFloat(&data[4]);
            break;

        case M1_BASE_CAN_ID:
            keyMotorData.m1lastReceived = osKernelSysTick();
            break;

        case M1_STATUS_CAN_ID:
            parseMotorFaultMessage(M1_ID, data);
            break;

        case M1_BUS_CAN_ID:
            keyMotorData.m1BusCurrent = arrayToFloat(&data[0]);
            keyMotorData.m1BusVoltage = arrayToFloat(&data[4]);
            break;

        case M1_VELOCITY_CAN_ID:
            // motorVelocity = arrayToFloat(&data[0]);
            keyMotorData.m1VehicleVelocity = arrayToFloat(&data[4]);
            break;

        case M1_PHASE_CURRENT_CAN_ID:
            motor1DetailsData.phaseBCurrent = arrayToFloat(&data[0]);
            motor1DetailsData.phaseCCurrent = arrayToFloat(&data[4]);
            break;

        case M1_VOLTAGE_VECTOR_CAN_ID:
            motor1DetailsData.motorVoltageImaginary = arrayToFloat(&data[0]);
            motor1DetailsData.motorVoltageReal = arrayToFloat(&data[4]);
            break;

        case M1_CURRENT_VECTOR_CAN_ID:
            motor1DetailsData.motorCurrentImaginary = arrayToFloat(&data[0]);
            motor1DetailsData.motorCurrentReal = arrayToFloat(&data[4]);
            break;

        case M1_BACKEMF_CAN_ID:
            motor1DetailsData.backEmfImaginary = arrayToFloat(&data[0]);
            motor1DetailsData.backEmfReal = arrayToFloat(&data[4]);
            break;

        case M1_15V_VOLTAGE_RAIL_CAN_ID:
            motor1DetailsData.railSupply15v = arrayToFloat(&data[4]);
            break;

        case M1_3_3V_1_9V_VOLTAGE_RAIL_CAN_ID:
            motor1DetailsData.railSupply1_9v = arrayToFloat(&data[0]);
            motor1DetailsData.railSupply3_3v = arrayToFloat(&data[4]);
            break;

        case M1_HEATSINK_TEMPERATURE_CAN_ID:
            motor1DetailsData.motorTemperature = arrayToFloat(&data[0]);
            motor1DetailsData.heatSinkTemperature = arrayToFloat(&data[4]);
            break;

        case M1_DSP_BOARD_TEMPERATURE_CAN_ID:
            motor1DetailsData.dspBoardTemp = arrayToFloat(&data[0]);
            break;

        case M1_ODOMETER_BUS_AMPHOURS_CAN_ID:
            motor1DetailsData.odometer = arrayToFloat(&data[0]);
            motor1DetailsData.dcBusAmpHours = arrayToFloat(&data[4]);
            break;

        case M1_SLIP_SPEED_CAN_ID:
            motor1DetailsData.slipSpeed = arrayToFloat(&data[4]);
            break;
    }
}

void parseMotorFaultMessage(uint8_t motorId, uint8_t* data)
{
    struct MotorErrorFlags* errorData;
    struct MotorLimitFlags* limitData;
    unsigned char* rxErrorData;
    unsigned char* txErrorData;

    if (motorId == M0_ID)
    {
        errorData = &motorFaultsData.m0ErrorFlags;
        limitData = &motorFaultsData.m0LimitFlags;
        rxErrorData = &motorFaultsData.m0CanRxErrorCount;
        txErrorData = &motorFaultsData.m0CanTxErrorCount;
    }
    else if (motorId == M1_ID)
    {
        errorData = &motorFaultsData.m1ErrorFlags;
        limitData = &motorFaultsData.m1LimitFlags;
        rxErrorData = &motorFaultsData.m1CanRxErrorCount;
        txErrorData = &motorFaultsData.m1CanTxErrorCount;
    }
    else
    {
        return;
    }

    (*limitData).outputVoltagePwm =
        data[0] & OUTPUT_VOLTAGE_PWM_LIMIT_MASK;
    (*limitData).motorCurrent =
        data[0] & MOTOR_CURRENT_LIMIT_MASK;
    (*limitData).velocity =
        data[0] & VELOCITY_LIMIT_MASK;
    (*limitData).busCurrent =
        data[0] & BUS_CURRENT_LIMIT_MASK;
    (*limitData).busVoltageUpper =
        data[0] & BUS_VOLTAGE_UPPER_LIMIT_MASK;
    (*limitData).busVoltageLower =
        data[0] & BUS_VOLTAGE_LOWER_LIMIT_MASK;
    (*limitData).ipmOrMotorTemperature =
        data[0] & IPM_OR_MOTOR_TEMPERATURE_LIMIT_MASK;

    (*errorData).motorOverSpeed =
        data[1] & MOTOR_OVER_SPEED_MASK;
    (*errorData).softwareOverCurrent =
        data[0] & SOFTWARE_OVER_CURRENT_MASK;
    (*errorData).dcBusOverVoltage =
        data[0] & DC_BUS_OVER_VOLTAGE_MASK;
    (*errorData).badMotorPositionHallSequence =
        data[0] & BAD_MOTOR_POSITION_HALL_SEQUENCE_MASK;
    (*errorData).watchdogCausedLastReset =
        data[0] & WATCHDOG_CAUSED_LAST_RESET_MASK;
    (*errorData).configReadError =
        data[0] & CONFIG_READ_ERROR_MASK;
    (*errorData).rail15vUnderVoltageLockOut =
        data[0] & RAIL_15V_UNDER_VOLTAGE_LOCK_OUT_MASK;
    (*errorData).desaturationFault =
        data[0] & DESATURATION_FAULT_MASK;

    *rxErrorData = data[7];
    *txErrorData = data[6];
}

float arrayToFloat(uint8_t* data)
{
    float result = 0;
    ((unsigned char*)&result)[0] = data[0];
    ((unsigned char*)&result)[1] = data[1];
    ((unsigned char*)&result)[2] = data[2];
    ((unsigned char*)&result)[3] = data[3];

    return result;
}
