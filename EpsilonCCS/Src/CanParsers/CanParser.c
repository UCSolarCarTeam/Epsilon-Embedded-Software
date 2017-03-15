#include "CanParser.h"

#include "AuxBmsCanParser.h"
#include "BmsCanParser.h"
#include "DriverControlsCanParser.h"
#include "LightsCanParser.h"
#include "MotorCanParser.h"
#include "MpptCanParser.h"

// 0x720 >= AUX_BMS >= 0x723
#define AUX_BMS_CAN_ID 0x720
#define AUX_BMS_CAN_MASK 0xFFC
// 0x300 >= BMS >= 0x307
#define BMS_CAN_ID 0x300
#define BMS_CAN_MASK 0xFF8
// 0x700 >= DRIVER_CONTROL >= 0x703
#define DRIVER_CONTROL_CAN_ID 0x700
#define DRIVER_CONTROL_CAN_MASK 0xFFC
// 0x710 >= LIGHTS 0x713
#define LIGHTS_CAN_ID 0x710
#define LIGHTS_CAN_MASK 0xFFC
// 0x400 >= MOTOR >= 0x43F
#define MOTOR_CAN_ID 0x400
#define MOTOR_CAN_MASK 0xFC0
// 0x600 >= MPPT >= 0x600
#define MPPT_CAN_ID 0x600
#define MPPT_CAN_MASK 0xFFF

void parseCanMessage(uint32_t stdId, uint8_t data[8])
{
    if (stdId & BMS_CAN_MASK == BMS_CAN_ID)
    {
        parseBmuCanMessage(stdId, data);
    }
}
