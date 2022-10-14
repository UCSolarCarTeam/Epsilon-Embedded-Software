/**
  ******************************************************************************
  * File Name          : mxconstants.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * COPYRIGHT(c) 2022 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MXCONSTANT_H
#define __MXCONSTANT_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

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

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MXCONSTANT_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
