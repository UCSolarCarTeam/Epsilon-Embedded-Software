#pragma once

void runTripTests();
void test_checkDischargeTripDueToLowCell();
void test_checkDischargeTripDueToHighTempAndCurrent();
void test_checkDischargeTripDueToPackCurrent();
void test_checkDischargeTripForNoTrip();
void test_checkChargeTripDueToHighCell();
void test_checkChargeTripDueToHighTempAndCurrent();
void test_checkChargeTripDueToPackCurrent();
void test_checkChargeTripForNoTrip();
void test_checkProtectionTripForTrip();
void test_checkProtectionTripForNoTrip();
