/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define CLK_IN_Pin GPIO_PIN_0
#define CLK_IN_GPIO_Port GPIOD
#define CLK_OUT_Pin GPIO_PIN_1
#define CLK_OUT_GPIO_Port GPIOD
#define RED_LED_Pin GPIO_PIN_5
#define RED_LED_GPIO_Port GPIOA
#define GREEN_LED_Pin GPIO_PIN_6
#define GREEN_LED_GPIO_Port GPIOA
#define BLUE_LED_Pin GPIO_PIN_7
#define BLUE_LED_GPIO_Port GPIOA
#define RIGHT_SIG_Pin GPIO_PIN_10
#define RIGHT_SIG_GPIO_Port GPIOB
#define LEFT_SIG_Pin GPIO_PIN_11
#define LEFT_SIG_GPIO_Port GPIOB
#define BRAKE_Pin GPIO_PIN_12
#define BRAKE_GPIO_Port GPIOB
#define HEAD_Pin GPIO_PIN_13
#define HEAD_GPIO_Port GPIOB
#define STROBE_Pin GPIO_PIN_14
#define STROBE_GPIO_Port GPIOB
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define JTDI_Pin GPIO_PIN_15
#define JTDI_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
