#pragma once

static const unsigned int MUTEX_TIMEOUT = 100; // 100ms
static const unsigned int TASK_QUEUE_PUT_TIMEOUT = 0; // 0ms
static const unsigned int IRQ_QUEUE_PUT_TIMEOUT = 0; // 0ms

#define AUX_BMS_SPI_BUFFER_SIZE 2U // Done so that the arrays don't complain about variable array size
