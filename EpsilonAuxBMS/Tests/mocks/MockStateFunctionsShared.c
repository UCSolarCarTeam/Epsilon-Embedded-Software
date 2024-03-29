/* AUTOGENERATED FILE. DO NOT EDIT. */
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "cmock.h"
#include "MockStateFunctionsShared.h"

static const char* CMockString_chargeDisconnectedDuringContactorSetting = "chargeDisconnectedDuringContactorSetting";
static const char* CMockString_chargeSense = "chargeSense";
static const char* CMockString_dischargeDisconnectedDuringContactorSetting = "dischargeDisconnectedDuringContactorSetting";
static const char* CMockString_dischargeSense = "dischargeSense";
static const char* CMockString_disconnectContactors = "disconnectContactors";
static const char* CMockString_hasChargeBeenSet = "hasChargeBeenSet";
static const char* CMockString_hasDischargeBeenSet = "hasDischargeBeenSet";
static const char* CMockString_isContactorError = "isContactorError";

typedef struct _CMOCK_disconnectContactors_CALL_INSTANCE
{
    UNITY_LINE_TYPE LineNumber;
    uint8_t Expected_isContactorError;

} CMOCK_disconnectContactors_CALL_INSTANCE;

typedef struct _CMOCK_dischargeDisconnectedDuringContactorSetting_CALL_INSTANCE
{
    UNITY_LINE_TYPE LineNumber;
    uint8_t ReturnVal;
    uint8_t Expected_dischargeSense;
    uint8_t Expected_hasDischargeBeenSet;

} CMOCK_dischargeDisconnectedDuringContactorSetting_CALL_INSTANCE;

typedef struct _CMOCK_chargeDisconnectedDuringContactorSetting_CALL_INSTANCE
{
    UNITY_LINE_TYPE LineNumber;
    uint8_t ReturnVal;
    uint8_t Expected_chargeSense;
    uint8_t Expected_hasChargeBeenSet;

} CMOCK_chargeDisconnectedDuringContactorSetting_CALL_INSTANCE;

static struct MockStateFunctionsSharedInstance
{
    CMOCK_MEM_INDEX_TYPE disconnectContactors_CallInstance;
    CMOCK_MEM_INDEX_TYPE dischargeDisconnectedDuringContactorSetting_CallInstance;
    CMOCK_MEM_INDEX_TYPE chargeDisconnectedDuringContactorSetting_CallInstance;
} Mock;

extern jmp_buf AbortFrame;

void MockStateFunctionsShared_Verify(void)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_MEM_INDEX_TYPE call_instance;
    call_instance = Mock.disconnectContactors_CallInstance;

    if (CMOCK_GUTS_NONE != call_instance)
    {
        UNITY_SET_DETAIL(CMockString_disconnectContactors);
        UNITY_TEST_FAIL(cmock_line, CMockStringCalledLess);
    }

    call_instance = Mock.dischargeDisconnectedDuringContactorSetting_CallInstance;

    if (CMOCK_GUTS_NONE != call_instance)
    {
        UNITY_SET_DETAIL(CMockString_dischargeDisconnectedDuringContactorSetting);
        UNITY_TEST_FAIL(cmock_line, CMockStringCalledLess);
    }

    call_instance = Mock.chargeDisconnectedDuringContactorSetting_CallInstance;

    if (CMOCK_GUTS_NONE != call_instance)
    {
        UNITY_SET_DETAIL(CMockString_chargeDisconnectedDuringContactorSetting);
        UNITY_TEST_FAIL(cmock_line, CMockStringCalledLess);
    }
}

void MockStateFunctionsShared_Init(void)
{
    MockStateFunctionsShared_Destroy();
}

void MockStateFunctionsShared_Destroy(void)
{
    CMock_Guts_MemFreeAll();
    memset(&Mock, 0, sizeof(Mock));
}

void disconnectContactors(uint8_t isContactorError)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_disconnectContactors_CALL_INSTANCE* cmock_call_instance;
    UNITY_SET_DETAIL(CMockString_disconnectContactors);
    cmock_call_instance = (CMOCK_disconnectContactors_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.disconnectContactors_CallInstance);
    Mock.disconnectContactors_CallInstance = CMock_Guts_MemNext(Mock.disconnectContactors_CallInstance);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
    cmock_line = cmock_call_instance->LineNumber;
    {
        UNITY_SET_DETAILS(CMockString_disconnectContactors, CMockString_isContactorError);
        UNITY_TEST_ASSERT_EQUAL_HEX8(cmock_call_instance->Expected_isContactorError, isContactorError, cmock_line, CMockStringMismatch);
    }
    UNITY_CLR_DETAILS();
}

void CMockExpectParameters_disconnectContactors(CMOCK_disconnectContactors_CALL_INSTANCE* cmock_call_instance, uint8_t isContactorError)
{
    cmock_call_instance->Expected_isContactorError = isContactorError;
}

void disconnectContactors_CMockExpect(UNITY_LINE_TYPE cmock_line, uint8_t isContactorError)
{
    CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_disconnectContactors_CALL_INSTANCE));
    CMOCK_disconnectContactors_CALL_INSTANCE* cmock_call_instance = (CMOCK_disconnectContactors_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
    memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
    Mock.disconnectContactors_CallInstance = CMock_Guts_MemChain(Mock.disconnectContactors_CallInstance, cmock_guts_index);
    cmock_call_instance->LineNumber = cmock_line;
    CMockExpectParameters_disconnectContactors(cmock_call_instance, isContactorError);
}

const uint8_t dischargeDisconnectedDuringContactorSetting(const uint8_t dischargeSense, uint8_t hasDischargeBeenSet)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_dischargeDisconnectedDuringContactorSetting_CALL_INSTANCE* cmock_call_instance;
    UNITY_SET_DETAIL(CMockString_dischargeDisconnectedDuringContactorSetting);
    cmock_call_instance = (CMOCK_dischargeDisconnectedDuringContactorSetting_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.dischargeDisconnectedDuringContactorSetting_CallInstance);
    Mock.dischargeDisconnectedDuringContactorSetting_CallInstance = CMock_Guts_MemNext(Mock.dischargeDisconnectedDuringContactorSetting_CallInstance);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
    cmock_line = cmock_call_instance->LineNumber;
    {
        UNITY_SET_DETAILS(CMockString_dischargeDisconnectedDuringContactorSetting, CMockString_dischargeSense);
        UNITY_TEST_ASSERT_EQUAL_HEX8(cmock_call_instance->Expected_dischargeSense, dischargeSense, cmock_line, CMockStringMismatch);
    }
    {
        UNITY_SET_DETAILS(CMockString_dischargeDisconnectedDuringContactorSetting, CMockString_hasDischargeBeenSet);
        UNITY_TEST_ASSERT_EQUAL_HEX8(cmock_call_instance->Expected_hasDischargeBeenSet, hasDischargeBeenSet, cmock_line, CMockStringMismatch);
    }
    UNITY_CLR_DETAILS();
    return cmock_call_instance->ReturnVal;
}

void CMockExpectParameters_dischargeDisconnectedDuringContactorSetting(CMOCK_dischargeDisconnectedDuringContactorSetting_CALL_INSTANCE* cmock_call_instance, const uint8_t dischargeSense, uint8_t hasDischargeBeenSet)
{
    cmock_call_instance->Expected_dischargeSense = dischargeSense;
    cmock_call_instance->Expected_hasDischargeBeenSet = hasDischargeBeenSet;
}

void dischargeDisconnectedDuringContactorSetting_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, const uint8_t dischargeSense, uint8_t hasDischargeBeenSet, uint8_t cmock_to_return)
{
    CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_dischargeDisconnectedDuringContactorSetting_CALL_INSTANCE));
    CMOCK_dischargeDisconnectedDuringContactorSetting_CALL_INSTANCE* cmock_call_instance = (CMOCK_dischargeDisconnectedDuringContactorSetting_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
    memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
    Mock.dischargeDisconnectedDuringContactorSetting_CallInstance = CMock_Guts_MemChain(Mock.dischargeDisconnectedDuringContactorSetting_CallInstance, cmock_guts_index);
    cmock_call_instance->LineNumber = cmock_line;
    CMockExpectParameters_dischargeDisconnectedDuringContactorSetting(cmock_call_instance, dischargeSense, hasDischargeBeenSet);
    cmock_call_instance->ReturnVal = cmock_to_return;
}

const uint8_t chargeDisconnectedDuringContactorSetting(const uint8_t chargeSense, uint8_t hasChargeBeenSet)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_chargeDisconnectedDuringContactorSetting_CALL_INSTANCE* cmock_call_instance;
    UNITY_SET_DETAIL(CMockString_chargeDisconnectedDuringContactorSetting);
    cmock_call_instance = (CMOCK_chargeDisconnectedDuringContactorSetting_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.chargeDisconnectedDuringContactorSetting_CallInstance);
    Mock.chargeDisconnectedDuringContactorSetting_CallInstance = CMock_Guts_MemNext(Mock.chargeDisconnectedDuringContactorSetting_CallInstance);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
    cmock_line = cmock_call_instance->LineNumber;
    {
        UNITY_SET_DETAILS(CMockString_chargeDisconnectedDuringContactorSetting, CMockString_chargeSense);
        UNITY_TEST_ASSERT_EQUAL_HEX8(cmock_call_instance->Expected_chargeSense, chargeSense, cmock_line, CMockStringMismatch);
    }
    {
        UNITY_SET_DETAILS(CMockString_chargeDisconnectedDuringContactorSetting, CMockString_hasChargeBeenSet);
        UNITY_TEST_ASSERT_EQUAL_HEX8(cmock_call_instance->Expected_hasChargeBeenSet, hasChargeBeenSet, cmock_line, CMockStringMismatch);
    }
    UNITY_CLR_DETAILS();
    return cmock_call_instance->ReturnVal;
}

void CMockExpectParameters_chargeDisconnectedDuringContactorSetting(CMOCK_chargeDisconnectedDuringContactorSetting_CALL_INSTANCE* cmock_call_instance, const uint8_t chargeSense, uint8_t hasChargeBeenSet)
{
    cmock_call_instance->Expected_chargeSense = chargeSense;
    cmock_call_instance->Expected_hasChargeBeenSet = hasChargeBeenSet;
}

void chargeDisconnectedDuringContactorSetting_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, const uint8_t chargeSense, uint8_t hasChargeBeenSet, uint8_t cmock_to_return)
{
    CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_chargeDisconnectedDuringContactorSetting_CALL_INSTANCE));
    CMOCK_chargeDisconnectedDuringContactorSetting_CALL_INSTANCE* cmock_call_instance = (CMOCK_chargeDisconnectedDuringContactorSetting_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
    memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
    Mock.chargeDisconnectedDuringContactorSetting_CallInstance = CMock_Guts_MemChain(Mock.chargeDisconnectedDuringContactorSetting_CallInstance, cmock_guts_index);
    cmock_call_instance->LineNumber = cmock_line;
    CMockExpectParameters_chargeDisconnectedDuringContactorSetting(cmock_call_instance, chargeSense, hasChargeBeenSet);
    cmock_call_instance->ReturnVal = cmock_to_return;
}

