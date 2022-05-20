#pragma once
#include "Mockcmsis_os2.h"
#include "Mockstm32f4xx_hal_gpio.h"
#include "MockTrip.h"
#include "OrionInterfaceTask.h"

void runOrionInterfaceTests();
void test_onlyUpdateAllowCharge();
void test_onlyUpdateAllowDischarge();
void test_osMessageQueueGetTimeout();
void test_shouldDisconnectContactors();
void test_shouldNotDisconnectContactorsChargeStateOpen();
void test_shouldNotDisconnectContactorsChargeStateClosed();
