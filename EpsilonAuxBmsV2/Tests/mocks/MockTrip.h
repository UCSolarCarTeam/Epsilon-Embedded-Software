/* AUTOGENERATED FILE. DO NOT EDIT. */
#ifndef _MOCKTRIP_H
#define _MOCKTRIP_H

#include "unity.h"
#include "Trip.h"

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

void MockTrip_Init(void);
void MockTrip_Destroy(void);
void MockTrip_Verify(void);




#define updateAuxTrip_Expect(message, auxTripToUpdate) updateAuxTrip_CMockExpect(__LINE__, message, auxTripToUpdate)
void updateAuxTrip_CMockExpect(UNITY_LINE_TYPE cmock_line, OrionCanInfo* message, AuxTrip* auxTripToUpdate);
#define checkDischargeTrip_ExpectAndReturn(auxTrip, cmock_retval) checkDischargeTrip_CMockExpectAndReturn(__LINE__, auxTrip, cmock_retval)
void checkDischargeTrip_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, AuxTrip auxTrip, uint8_t cmock_to_return);
#define checkChargeTrip_ExpectAndReturn(auxTrip, cmock_retval) checkChargeTrip_CMockExpectAndReturn(__LINE__, auxTrip, cmock_retval)
void checkChargeTrip_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, AuxTrip auxTrip, uint8_t cmock_to_return);

#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))
#pragma GCC diagnostic pop
#endif
#endif

#endif
