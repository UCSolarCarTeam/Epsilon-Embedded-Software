#pragma once
#include "OrionStatus.h"

OrionStatus orionStatus;

void clearOrionStatus();

void setNominalOrionStatus();

void assertOrionStatusEqual(OrionStatus expected);