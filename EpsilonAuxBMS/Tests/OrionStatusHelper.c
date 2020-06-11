#include "OrionStatusHelper.h"

void clearOrionStatus()
{
    orionStatus = (OrionStatus)
    {
        0
    };
}

void setNominalOrionStatus()
{
    orionStatus = (OrionStatus)
    {
        .chargeContactorOverriden = 0,
         .dischargeContactorOverriden = 0,
          .allowCharge = 1,
           .allowDischarge = 1,
            .shutOff = 0,
             .canMsgReceived = 0,
              .maxCellVoltage = 30000,
               .minCellVoltage = 30000,
                .highTemperature = 20,
                 .packCurrent = 20
    };
}