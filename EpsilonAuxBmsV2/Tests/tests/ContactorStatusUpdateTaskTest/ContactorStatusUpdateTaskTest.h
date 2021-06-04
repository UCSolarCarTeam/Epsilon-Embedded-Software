#pragma once
#include "Mockcmsis_os2.h"
#include "MockCheckContactorError.h"
#include "ContactorState.h"
#include "Mockstm32f4xx_hal_gpio.h"
#include "ContactorStatusUpdateTask.h"
#include "AuxStatusHelper.h"
#include <string.h>
#include "unity.h"

int runContactorStatusUpdateTaskTest();
