#pragma once
// These values are subject to change based on what regulations say
// Ask electrical if any issues
static const float ORION_MAX_CELL_VOLTAGE = 4.20; // Much match orion bms value
static const float ORION_MIN_CELL_VOLTAGE = 2.55;
static const float AUX_BMS_MAX_CELL_VOLTAGE = 4.15;
static const float AUX_BMS_MIN_CELL_VOLTAGE = 2.60;

static const float DEFAULT_VOLTAGE_UNITS = 0.0001; // From: https://www.orionbms.com/manuals/utility/

static const int ORION_MAX_CHARGE_TEMP = 44;
static const short ORION_MAX_CHARGE_CURRENT = -47;

static const int ORION_MAX_DISCHARGE_TEMP = 59;
static const short ORION_MAX_DISCHARGE_CURRENT = 229;
