/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AuxBmsConstants.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CONTACTOR_ENABLE2_Pin GPIO_PIN_2
#define CONTACTOR_ENABLE2_GPIO_Port GPIOE
#define CONTACTOR_ENABLE4_Pin GPIO_PIN_3
#define CONTACTOR_ENABLE4_GPIO_Port GPIOE
#define CONTACTOR_ENABLE3_Pin GPIO_PIN_4
#define CONTACTOR_ENABLE3_GPIO_Port GPIOE
#define CONTACTOR_ENABLE1_Pin GPIO_PIN_5
#define CONTACTOR_ENABLE1_GPIO_Port GPIOE
#define PH0_OSC_IN_Pin GPIO_PIN_0
#define PH0_OSC_IN_GPIO_Port GPIOH
#define PH1_OSC_OUT_Pin GPIO_PIN_1
#define PH1_OSC_OUT_GPIO_Port GPIOH
#define HV_ENABLE_Pin GPIO_PIN_0
#define HV_ENABLE_GPIO_Port GPIOC
#define PWR_CURRENT_SENSE_Pin GPIO_PIN_1
#define PWR_CURRENT_SENSE_GPIO_Port GPIOC
#define RED_LED_Pin GPIO_PIN_0
#define RED_LED_GPIO_Port GPIOA
#define GRN_LED_Pin GPIO_PIN_1
#define GRN_LED_GPIO_Port GPIOA
#define BLU_LED_Pin GPIO_PIN_2
#define BLU_LED_GPIO_Port GPIOA
#define ORION_CHARGE_ENABLE_SENSE_Pin GPIO_PIN_6
#define ORION_CHARGE_ENABLE_SENSE_GPIO_Port GPIOC
#define ORION_DISCHARGE_ENABLE_SENSE_Pin GPIO_PIN_7
#define ORION_DISCHARGE_ENABLE_SENSE_GPIO_Port GPIOC
#define CAN1_STBY_Pin GPIO_PIN_8
#define CAN1_STBY_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWDCLK_Pin GPIO_PIN_14
#define SWDCLK_GPIO_Port GPIOA
#define JTDI_Pin GPIO_PIN_15
#define JTDI_GPIO_Port GPIOA
#define ADC_SPI_SCK_Pin GPIO_PIN_10
#define ADC_SPI_SCK_GPIO_Port GPIOC
#define CURRENT_SENSE_ENABLE_Pin GPIO_PIN_0
#define CURRENT_SENSE_ENABLE_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define ADC_SPI_MISO_Pin GPIO_PIN_4
#define ADC_SPI_MISO_GPIO_Port GPIOB
#define ADC_nCS_Pin GPIO_PIN_5
#define ADC_nCS_GPIO_Port GPIOB
#define SENSE4_Pin GPIO_PIN_8
#define SENSE4_GPIO_Port GPIOB
#define SENSE3_Pin GPIO_PIN_9
#define SENSE3_GPIO_Port GPIOB
#define SENSE2_Pin GPIO_PIN_0
#define SENSE2_GPIO_Port GPIOE
#define SENSE1_Pin GPIO_PIN_1
#define SENSE1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
#define COMMON_SENSE_Pin SENSE1_Pin
#define COMMON_SENSE_GPIO_Port SENSE1_GPIO_Port
#define COMMON_ENABLE_Pin CONTACTOR_ENABLE1_Pin
#define COMMON_ENABLE_GPIO_Port CONTACTOR_ENABLE1_GPIO_Port

#define CHARGE_SENSE_Pin SENSE2_Pin
#define CHARGE_SENSE_GPIO_Port SENSE2_GPIO_Port
#define CHARGE_ENABLE_Pin CONTACTOR_ENABLE2_Pin
#define CHARGE_ENABLE_GPIO_Port CONTACTOR_ENABLE2_GPIO_Port

#define DISCHARGE_SENSE_Pin SENSE3_Pin
#define DISCHARGE_SENSE_GPIO_Port SENSE3_GPIO_Port
#define DISCHARGE_ENABLE_Pin CONTACTOR_ENABLE3_Pin
#define DISCHARGE_ENABLE_GPIO_Port CONTACTOR_ENABLE3_GPIO_Port

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
