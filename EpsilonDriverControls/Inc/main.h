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
#include "stm32f4xx_hal.h"

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
#define OTG_FS_PowerSwitchOn_Pin GPIO_PIN_0
#define OTG_FS_PowerSwitchOn_GPIO_Port GPIOC
#define B1_Pin GPIO_PIN_0
#define B1_GPIO_Port GPIOA
#define REGEN_Pin GPIO_PIN_1
#define REGEN_GPIO_Port GPIOA
#define ACCELERATION_Pin GPIO_PIN_2
#define ACCELERATION_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_5
#define LED_RED_GPIO_Port GPIOA
#define LED_GREEN_Pin GPIO_PIN_6
#define LED_GREEN_GPIO_Port GPIOA
#define LED_BLUE_Pin GPIO_PIN_7
#define LED_BLUE_GPIO_Port GPIOA
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define RSIGNAL_Pin GPIO_PIN_10
#define RSIGNAL_GPIO_Port GPIOE
#define LSIGNAL_Pin GPIO_PIN_11
#define LSIGNAL_GPIO_Port GPIOE
#define VOLUME_DOWN_Pin GPIO_PIN_7
#define VOLUME_DOWN_GPIO_Port GPIOC
#define VOLUME_UP_Pin GPIO_PIN_8
#define VOLUME_UP_GPIO_Port GPIOC
#define NEXT_SONG_Pin GPIO_PIN_9
#define NEXT_SONG_GPIO_Port GPIOC
#define LAST_SONG_Pin GPIO_PIN_8
#define LAST_SONG_GPIO_Port GPIOA
#define HEADLIGHTS_HIGH_Pin GPIO_PIN_11
#define HEADLIGHTS_HIGH_GPIO_Port GPIOA
#define HEADLIGHTS_LOW_Pin GPIO_PIN_12
#define HEADLIGHTS_LOW_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define HEADLIGHTS_OFF_Pin GPIO_PIN_10
#define HEADLIGHTS_OFF_GPIO_Port GPIOC
#define AUX_Pin GPIO_PIN_11
#define AUX_GPIO_Port GPIOC
#define HAZARDS_Pin GPIO_PIN_12
#define HAZARDS_GPIO_Port GPIOC
#define FORWARD_Pin GPIO_PIN_0
#define FORWARD_GPIO_Port GPIOD
#define INTERIOR_Pin GPIO_PIN_1
#define INTERIOR_GPIO_Port GPIOD
#define REVERSE_Pin GPIO_PIN_2
#define REVERSE_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define PUSH_TO_TALK_Pin GPIO_PIN_7
#define PUSH_TO_TALK_GPIO_Port GPIOB
#define HORN_Pin GPIO_PIN_8
#define HORN_GPIO_Port GPIOB
#define RESET_Pin GPIO_PIN_9
#define RESET_GPIO_Port GPIOB
#define CONTEXT_Pin GPIO_PIN_0
#define CONTEXT_GPIO_Port GPIOE
#define BRAKES_Pin GPIO_PIN_1
#define BRAKES_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
#define CANInterruptPriority 5 //obtained from stm32f4xx_hal_msp.c file
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */