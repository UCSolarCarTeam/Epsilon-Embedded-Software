#pragma once
#include "cmsis_os.h"
#include "main.h"
#include "AuxStatus.h"
#include <math.h>

void readAuxVoltageTask(void* arg);
void readAuxVoltage(uint32_t* prevWakeTime);



static const uint32_t READ_AUX_VOLTAGE_TASK_FREQ = 100; // 100ms (10Hz)
static const uint32_t SPI_TIMEOUT = 50;
static const float AUX_NOMINAL_VOLTAGE = 12.0;
static const uint16_t AUX_ADC_NOMINAL_OUTPUT = 0x32E; // Pattern = 2.63/3.3 * 0x3FF
static const uint16_t SPI_ERROR_CODE = 0xDEAD;

static const uint32_t SPI_READY_FLAG = 0x1; // Flag that is triggered by SPI IRQ when the SPI read is complete

extern osMutexId_t auxStatusReadAuxVoltageMutex;
extern AuxStatus auxStatus;
extern SPI_HandleTypeDef hspi3;
// Data buffer
extern uint8_t spiRxBuff[AUX_BMS_SPI_BUFFER_SIZE];
