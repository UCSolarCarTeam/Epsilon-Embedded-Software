/* AUTOGENERATED FILE. DO NOT EDIT. */
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "cmock.h"
#include "MockCheckContactorSet.h"

static const char* CMockString_current_multiplier = "current_multiplier";
static const char* CMockString_isContactorSet = "isContactorSet";
static const char* CMockString_isSenseStable = "isSenseStable";
static const char* CMockString_pin = "pin";
static const char* CMockString_port = "port";
static const char* CMockString_readCurrentThroughContactors = "readCurrentThroughContactors";
static const char* CMockString_resetSenseArray = "resetSenseArray";
static const char* CMockString_senses = "senses";

typedef struct _CMOCK_readCurrentThroughContactors_CALL_INSTANCE
{
    UNITY_LINE_TYPE LineNumber;
    uint32_t ReturnVal;

} CMOCK_readCurrentThroughContactors_CALL_INSTANCE;

typedef struct _CMOCK_isContactorSet_CALL_INSTANCE
{
    UNITY_LINE_TYPE LineNumber;
    int ReturnVal;
    uint16_t Expected_pin;
    GPIO_TypeDef* Expected_port;
    int Expected_current_multiplier;

} CMOCK_isContactorSet_CALL_INSTANCE;

typedef struct _CMOCK_resetSenseArray_CALL_INSTANCE
{
    UNITY_LINE_TYPE LineNumber;
    uint32_t* Expected_senses;

} CMOCK_resetSenseArray_CALL_INSTANCE;

typedef struct _CMOCK_isSenseStable_CALL_INSTANCE
{
    UNITY_LINE_TYPE LineNumber;
    int ReturnVal;
    uint32_t* Expected_senses;

} CMOCK_isSenseStable_CALL_INSTANCE;

static struct MockCheckContactorSetInstance
{
    CMOCK_MEM_INDEX_TYPE readCurrentThroughContactors_CallInstance;
    CMOCK_MEM_INDEX_TYPE isContactorSet_CallInstance;
    CMOCK_MEM_INDEX_TYPE resetSenseArray_CallInstance;
    CMOCK_MEM_INDEX_TYPE isSenseStable_CallInstance;
} Mock;

extern jmp_buf AbortFrame;

void MockCheckContactorSet_Verify(void)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_MEM_INDEX_TYPE call_instance;
    call_instance = Mock.readCurrentThroughContactors_CallInstance;

    if (CMOCK_GUTS_NONE != call_instance)
    {
        UNITY_SET_DETAIL(CMockString_readCurrentThroughContactors);
        UNITY_TEST_FAIL(cmock_line, CMockStringCalledLess);
    }

    call_instance = Mock.isContactorSet_CallInstance;

    if (CMOCK_GUTS_NONE != call_instance)
    {
        UNITY_SET_DETAIL(CMockString_isContactorSet);
        UNITY_TEST_FAIL(cmock_line, CMockStringCalledLess);
    }

    call_instance = Mock.resetSenseArray_CallInstance;

    if (CMOCK_GUTS_NONE != call_instance)
    {
        UNITY_SET_DETAIL(CMockString_resetSenseArray);
        UNITY_TEST_FAIL(cmock_line, CMockStringCalledLess);
    }

    call_instance = Mock.isSenseStable_CallInstance;

    if (CMOCK_GUTS_NONE != call_instance)
    {
        UNITY_SET_DETAIL(CMockString_isSenseStable);
        UNITY_TEST_FAIL(cmock_line, CMockStringCalledLess);
    }
}

void MockCheckContactorSet_Init(void)
{
    MockCheckContactorSet_Destroy();
}

void MockCheckContactorSet_Destroy(void)
{
    CMock_Guts_MemFreeAll();
    memset(&Mock, 0, sizeof(Mock));
}

uint32_t readCurrentThroughContactors(void)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_readCurrentThroughContactors_CALL_INSTANCE* cmock_call_instance;
    UNITY_SET_DETAIL(CMockString_readCurrentThroughContactors);
    cmock_call_instance = (CMOCK_readCurrentThroughContactors_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.readCurrentThroughContactors_CallInstance);
    Mock.readCurrentThroughContactors_CallInstance = CMock_Guts_MemNext(Mock.readCurrentThroughContactors_CallInstance);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
    cmock_line = cmock_call_instance->LineNumber;
    UNITY_CLR_DETAILS();
    return cmock_call_instance->ReturnVal;
}

void readCurrentThroughContactors_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint32_t cmock_to_return)
{
    CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_readCurrentThroughContactors_CALL_INSTANCE));
    CMOCK_readCurrentThroughContactors_CALL_INSTANCE* cmock_call_instance = (CMOCK_readCurrentThroughContactors_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
    memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
    Mock.readCurrentThroughContactors_CallInstance = CMock_Guts_MemChain(Mock.readCurrentThroughContactors_CallInstance, cmock_guts_index);
    cmock_call_instance->LineNumber = cmock_line;
    cmock_call_instance->ReturnVal = cmock_to_return;
}

int isContactorSet(uint16_t pin, GPIO_TypeDef* port, int current_multiplier)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_isContactorSet_CALL_INSTANCE* cmock_call_instance;
    UNITY_SET_DETAIL(CMockString_isContactorSet);
    cmock_call_instance = (CMOCK_isContactorSet_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.isContactorSet_CallInstance);
    Mock.isContactorSet_CallInstance = CMock_Guts_MemNext(Mock.isContactorSet_CallInstance);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
    cmock_line = cmock_call_instance->LineNumber;
    {
        UNITY_SET_DETAILS(CMockString_isContactorSet, CMockString_pin);
        UNITY_TEST_ASSERT_EQUAL_HEX16(cmock_call_instance->Expected_pin, pin, cmock_line, CMockStringMismatch);
    }
    {
        UNITY_SET_DETAILS(CMockString_isContactorSet, CMockString_port);
        UNITY_TEST_ASSERT_EQUAL_MEMORY((void*)(cmock_call_instance->Expected_port), (void*)(port), sizeof(GPIO_TypeDef), cmock_line, CMockStringMismatch);
    }
    {
        UNITY_SET_DETAILS(CMockString_isContactorSet, CMockString_current_multiplier);
        UNITY_TEST_ASSERT_EQUAL_INT(cmock_call_instance->Expected_current_multiplier, current_multiplier, cmock_line, CMockStringMismatch);
    }
    UNITY_CLR_DETAILS();
    return cmock_call_instance->ReturnVal;
}

void CMockExpectParameters_isContactorSet(CMOCK_isContactorSet_CALL_INSTANCE* cmock_call_instance, uint16_t pin, GPIO_TypeDef* port, int current_multiplier)
{
    cmock_call_instance->Expected_pin = pin;
    cmock_call_instance->Expected_port = port;
    cmock_call_instance->Expected_current_multiplier = current_multiplier;
}

void isContactorSet_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint16_t pin, GPIO_TypeDef* port, int current_multiplier, int cmock_to_return)
{
    CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_isContactorSet_CALL_INSTANCE));
    CMOCK_isContactorSet_CALL_INSTANCE* cmock_call_instance = (CMOCK_isContactorSet_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
    memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
    Mock.isContactorSet_CallInstance = CMock_Guts_MemChain(Mock.isContactorSet_CallInstance, cmock_guts_index);
    cmock_call_instance->LineNumber = cmock_line;
    CMockExpectParameters_isContactorSet(cmock_call_instance, pin, port, current_multiplier);
    cmock_call_instance->ReturnVal = cmock_to_return;
}

void resetSenseArray(uint32_t* senses)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_resetSenseArray_CALL_INSTANCE* cmock_call_instance;
    UNITY_SET_DETAIL(CMockString_resetSenseArray);
    cmock_call_instance = (CMOCK_resetSenseArray_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.resetSenseArray_CallInstance);
    Mock.resetSenseArray_CallInstance = CMock_Guts_MemNext(Mock.resetSenseArray_CallInstance);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
    cmock_line = cmock_call_instance->LineNumber;
    {
        UNITY_SET_DETAILS(CMockString_resetSenseArray, CMockString_senses);

        if (cmock_call_instance->Expected_senses == NULL)
        {
            UNITY_TEST_ASSERT_NULL(senses, cmock_line, CMockStringExpNULL);
        }
        else
        {
            UNITY_TEST_ASSERT_EQUAL_HEX32_ARRAY(cmock_call_instance->Expected_senses, senses, 1, cmock_line, CMockStringMismatch);
        }
    }
    UNITY_CLR_DETAILS();
}

void CMockExpectParameters_resetSenseArray(CMOCK_resetSenseArray_CALL_INSTANCE* cmock_call_instance, uint32_t* senses)
{
    cmock_call_instance->Expected_senses = senses;
}

void resetSenseArray_CMockExpect(UNITY_LINE_TYPE cmock_line, uint32_t* senses)
{
    CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_resetSenseArray_CALL_INSTANCE));
    CMOCK_resetSenseArray_CALL_INSTANCE* cmock_call_instance = (CMOCK_resetSenseArray_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
    memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
    Mock.resetSenseArray_CallInstance = CMock_Guts_MemChain(Mock.resetSenseArray_CallInstance, cmock_guts_index);
    cmock_call_instance->LineNumber = cmock_line;
    CMockExpectParameters_resetSenseArray(cmock_call_instance, senses);
}

int isSenseStable(uint32_t* senses)
{
    UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
    CMOCK_isSenseStable_CALL_INSTANCE* cmock_call_instance;
    UNITY_SET_DETAIL(CMockString_isSenseStable);
    cmock_call_instance = (CMOCK_isSenseStable_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.isSenseStable_CallInstance);
    Mock.isSenseStable_CallInstance = CMock_Guts_MemNext(Mock.isSenseStable_CallInstance);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
    cmock_line = cmock_call_instance->LineNumber;
    {
        UNITY_SET_DETAILS(CMockString_isSenseStable, CMockString_senses);

        if (cmock_call_instance->Expected_senses == NULL)
        {
            UNITY_TEST_ASSERT_NULL(senses, cmock_line, CMockStringExpNULL);
        }
        else
        {
            UNITY_TEST_ASSERT_EQUAL_HEX32_ARRAY(cmock_call_instance->Expected_senses, senses, 1, cmock_line, CMockStringMismatch);
        }
    }
    UNITY_CLR_DETAILS();
    return cmock_call_instance->ReturnVal;
}

void CMockExpectParameters_isSenseStable(CMOCK_isSenseStable_CALL_INSTANCE* cmock_call_instance, uint32_t* senses)
{
    cmock_call_instance->Expected_senses = senses;
}

void isSenseStable_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint32_t* senses, int cmock_to_return)
{
    CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_isSenseStable_CALL_INSTANCE));
    CMOCK_isSenseStable_CALL_INSTANCE* cmock_call_instance = (CMOCK_isSenseStable_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
    UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
    memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
    Mock.isSenseStable_CallInstance = CMock_Guts_MemChain(Mock.isSenseStable_CallInstance, cmock_guts_index);
    cmock_call_instance->LineNumber = cmock_line;
    CMockExpectParameters_isSenseStable(cmock_call_instance, senses);
    cmock_call_instance->ReturnVal = cmock_to_return;
}

