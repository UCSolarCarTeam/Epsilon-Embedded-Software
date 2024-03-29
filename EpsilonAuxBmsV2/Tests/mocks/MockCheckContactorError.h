/* AUTOGENERATED FILE. DO NOT EDIT. */
#ifndef _MOCKCHECKCONTACTORERROR_H
#define _MOCKCHECKCONTACTORERROR_H

#include "unity.h"
#include "CheckContactorError.h"

/* Ignore the following warnings, since we are copying code */
#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))
#pragma GCC diagnostic push
#endif
#if !defined(__clang__)
#pragma GCC diagnostic ignored "-Wpragmas"
#endif
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wduplicate-decl-specifier"
#endif

void MockCheckContactorError_Init(void);
void MockCheckContactorError_Destroy(void);
void MockCheckContactorError_Verify(void);




#define isContactorError_ExpectAndReturn(contactorSense, contactorState, cmock_retval) isContactorError_CMockExpectAndReturn(__LINE__, contactorSense, contactorState, cmock_retval)
void isContactorError_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t contactorSense, ContactorState contactorState, uint8_t cmock_to_return);

#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))
#pragma GCC diagnostic pop
#endif
#endif

#endif
