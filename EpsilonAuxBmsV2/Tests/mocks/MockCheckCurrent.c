/* AUTOGENERATED FILE. DO NOT EDIT. */
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "cmock.h"
#include "MockCheckCurrent.h"

static const char* CMockString_isCurrentLow = "isCurrentLow";
static const char* CMockString_numContactorsClosed = "numContactorsClosed";

typedef struct _CMOCK_isCurrentLow_CALL_INSTANCE
{
    UNITY_LINE_TYPE LineNumber;
    uint8_t ReturnVal;
    uint8_t Expected_numContactorsClosed;

} CMOCK_isCurrentLow_CALL_INSTANCE;

static struct MockCheckCurrentInstance
{
    CMOCK_MEM_INDEX_TYPE isCurrentLow_CallInstance;
} Mock;

extern jmp_buf AbortFrame;

void MockCheckCurrent_Verify(void)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_MEM_INDEX_TYPE call_instance;
    call_instance = Mock.isCurrentLow_CallInstance;

    if (CMOCK_GUTS_NONE != call_instance)
    {
        UNITY_SET_DETAIL(CMockString_isCurrentLow);
        UNITY_TEST_FAIL(cmock_line, CMockStringCalledLess);
    }
}

void MockCheckCurrent_Init(void)
{
    MockCheckCurrent_Destroy();
}

void MockCheckCurrent_Destroy(void)
{
    CMock_Guts_MemFreeAll();
    memset(&Mock, 0, sizeof(Mock));
}

uint8_t isCurrentLow(uint8_t numContactorsClosed)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_isCurrentLow_CALL_INSTANCE* cmock_call_instance;
    UNITY_SET_DETAIL(CMockString_isCurrentLow);
    cmock_call_instance = (CMOCK_isCurrentLow_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.isCurrentLow_CallInstance);
    Mock.isCurrentLow_CallInstance = CMock_Guts_MemNext(Mock.isCurrentLow_CallInstance);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
    cmock_line = cmock_call_instance->LineNumber;
    {
        UNITY_SET_DETAILS(CMockString_isCurrentLow, CMockString_numContactorsClosed);
        UNITY_TEST_ASSERT_EQUAL_HEX8(cmock_call_instance->Expected_numContactorsClosed, numContactorsClosed, cmock_line, CMockStringMismatch);
    }
    UNITY_CLR_DETAILS();
    return cmock_call_instance->ReturnVal;
}

void CMockExpectParameters_isCurrentLow(CMOCK_isCurrentLow_CALL_INSTANCE* cmock_call_instance, uint8_t numContactorsClosed)
{
    cmock_call_instance->Expected_numContactorsClosed = numContactorsClosed;
}

void isCurrentLow_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t numContactorsClosed, uint8_t cmock_to_return)
{
    CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_isCurrentLow_CALL_INSTANCE));
    CMOCK_isCurrentLow_CALL_INSTANCE* cmock_call_instance = (CMOCK_isCurrentLow_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
    memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
    Mock.isCurrentLow_CallInstance = CMock_Guts_MemChain(Mock.isCurrentLow_CallInstance, cmock_guts_index);
    cmock_call_instance->LineNumber = cmock_line;
    CMockExpectParameters_isCurrentLow(cmock_call_instance, numContactorsClosed);
    cmock_call_instance->ReturnVal = cmock_to_return;
}

