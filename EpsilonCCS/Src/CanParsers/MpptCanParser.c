#include "cmsis_os.h"

#include "MpptCanParser.h"

#include "MpptData.h"

#define MPPT_NUM_MASK 0x00F

void parseMpptCanMessage(uint32_t stdId, uint8_t* data)
{
    uint8_t mpptId = stdId & MPPT_NUM_MASK;

    if (mpptId >= NUM_OF_MPPTS)
    {
        return;
    }
    else
    {
        mpptData[mpptId].lastReceived = osKernelSysTick();
        mpptData[mpptId].arrayVoltage =
            (data[0] >> 0) |
            (data[1] >> 8);
        mpptData[mpptId].arrayCurrent =
            (data[2] >> 0) |
            (data[3] >> 8);
        mpptData[mpptId].batteryVoltage =
            (data[4] >> 0) |
            (data[5] >> 8);
        mpptData[mpptId].temperature =
            (data[6] >> 0) |
            (data[7] >> 8);
    }
}
