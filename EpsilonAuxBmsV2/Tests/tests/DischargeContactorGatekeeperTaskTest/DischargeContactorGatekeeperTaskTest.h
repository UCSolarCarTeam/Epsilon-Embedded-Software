#pragma once
#include "Mockcmsis_os2.h"
#include "ContactorState.h"
#include "MockCheckCurrent.h"
#include "Mockstm32f4xx_hal_gpio.h"
#include "ContactorEventFlags.h"
#include "DischargeContactorGatekeeperTask.h"
#include <string.h>
#include "unity.h"

int runDischargeContactorGatekeeperTaskTest();
