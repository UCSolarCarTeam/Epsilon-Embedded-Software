/* AUTOGENERATED FILE. DO NOT EDIT. */
#ifndef _MOCKSTM32F4XX_HAL_CAN_H
#define _MOCKSTM32F4XX_HAL_CAN_H

#include "unity.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"

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

void Mockstm32f4xx_hal_can_Init(void);
void Mockstm32f4xx_hal_can_Destroy(void);
void Mockstm32f4xx_hal_can_Verify(void);




#define HAL_CAN_Init_ExpectAndReturn(hcan, cmock_retval) HAL_CAN_Init_CMockExpectAndReturn(__LINE__, hcan, cmock_retval)
void HAL_CAN_Init_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, HAL_StatusTypeDef cmock_to_return);
#define HAL_CAN_ConfigFilter_ExpectAndReturn(hcan, sFilterConfig, cmock_retval) HAL_CAN_ConfigFilter_CMockExpectAndReturn(__LINE__, hcan, sFilterConfig, cmock_retval)
void HAL_CAN_ConfigFilter_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, CAN_FilterConfTypeDef* sFilterConfig, HAL_StatusTypeDef cmock_to_return);
#define HAL_CAN_DeInit_ExpectAndReturn(hcan, cmock_retval) HAL_CAN_DeInit_CMockExpectAndReturn(__LINE__, hcan, cmock_retval)
void HAL_CAN_DeInit_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, HAL_StatusTypeDef cmock_to_return);
#define HAL_CAN_MspInit_Expect(hcan) HAL_CAN_MspInit_CMockExpect(__LINE__, hcan)
void HAL_CAN_MspInit_CMockExpect(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan);
#define HAL_CAN_MspDeInit_Expect(hcan) HAL_CAN_MspDeInit_CMockExpect(__LINE__, hcan)
void HAL_CAN_MspDeInit_CMockExpect(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan);
#define HAL_CAN_Transmit_ExpectAndReturn(hcan, Timeout, cmock_retval) HAL_CAN_Transmit_CMockExpectAndReturn(__LINE__, hcan, Timeout, cmock_retval)
void HAL_CAN_Transmit_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, uint32_t Timeout, HAL_StatusTypeDef cmock_to_return);
#define HAL_CAN_Transmit_IT_ExpectAndReturn(hcan, cmock_retval) HAL_CAN_Transmit_IT_CMockExpectAndReturn(__LINE__, hcan, cmock_retval)
void HAL_CAN_Transmit_IT_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, HAL_StatusTypeDef cmock_to_return);
#define HAL_CAN_Receive_ExpectAndReturn(hcan, FIFONumber, Timeout, cmock_retval) HAL_CAN_Receive_CMockExpectAndReturn(__LINE__, hcan, FIFONumber, Timeout, cmock_retval)
void HAL_CAN_Receive_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, uint8_t FIFONumber, uint32_t Timeout, HAL_StatusTypeDef cmock_to_return);
#define HAL_CAN_Receive_IT_ExpectAndReturn(hcan, FIFONumber, cmock_retval) HAL_CAN_Receive_IT_CMockExpectAndReturn(__LINE__, hcan, FIFONumber, cmock_retval)
void HAL_CAN_Receive_IT_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, uint8_t FIFONumber, HAL_StatusTypeDef cmock_to_return);
#define HAL_CAN_Sleep_ExpectAndReturn(hcan, cmock_retval) HAL_CAN_Sleep_CMockExpectAndReturn(__LINE__, hcan, cmock_retval)
void HAL_CAN_Sleep_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, HAL_StatusTypeDef cmock_to_return);
#define HAL_CAN_WakeUp_ExpectAndReturn(hcan, cmock_retval) HAL_CAN_WakeUp_CMockExpectAndReturn(__LINE__, hcan, cmock_retval)
void HAL_CAN_WakeUp_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, HAL_StatusTypeDef cmock_to_return);
#define HAL_CAN_IRQHandler_Expect(hcan) HAL_CAN_IRQHandler_CMockExpect(__LINE__, hcan)
void HAL_CAN_IRQHandler_CMockExpect(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan);
#define HAL_CAN_TxCpltCallback_Expect(hcan) HAL_CAN_TxCpltCallback_CMockExpect(__LINE__, hcan)
void HAL_CAN_TxCpltCallback_CMockExpect(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan);
//#define HAL_CAN_RxCpltCallback_Expect(hcan) HAL_CAN_RxCpltCallback_CMockExpect(__LINE__, hcan)
//void HAL_CAN_RxCpltCallback_CMockExpect(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan);
#define HAL_CAN_ErrorCallback_Expect(hcan) HAL_CAN_ErrorCallback_CMockExpect(__LINE__, hcan)
void HAL_CAN_ErrorCallback_CMockExpect(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan);
#define HAL_CAN_GetError_ExpectAndReturn(hcan, cmock_retval) HAL_CAN_GetError_CMockExpectAndReturn(__LINE__, hcan, cmock_retval)
void HAL_CAN_GetError_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, uint32_t cmock_to_return);
#define HAL_CAN_GetState_ExpectAndReturn(hcan, cmock_retval) HAL_CAN_GetState_CMockExpectAndReturn(__LINE__, hcan, cmock_retval)
void HAL_CAN_GetState_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, CAN_HandleTypeDef* hcan, HAL_CAN_StateTypeDef cmock_to_return);

#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))
#pragma GCC diagnostic pop
#endif
#endif

#endif