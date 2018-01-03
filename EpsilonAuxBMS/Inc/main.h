/**
  ******************************************************************************
  * File Name          : main.hpp
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define CONTACTOR_ENABLE2_Pin GPIO_PIN_2
#define CONTACTOR_ENABLE2_GPIO_Port GPIOE
#define CONTACTOR_ENABLE4_Pin GPIO_PIN_3
#define CONTACTOR_ENABLE4_GPIO_Port GPIOE
#define CONTACTOR_ENABLE3_Pin GPIO_PIN_4
#define CONTACTOR_ENABLE3_GPIO_Port GPIOE
#define CONTACTOR_ENABLE1_Pin GPIO_PIN_5
#define CONTACTOR_ENABLE1_GPIO_Port GPIOE
#define SOFT_RESET_Pin GPIO_PIN_6
#define SOFT_RESET_GPIO_Port GPIOE
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
#define CHARGE_ENABLE_SENSE_Pin GPIO_PIN_6
#define CHARGE_ENABLE_SENSE_GPIO_Port GPIOC
#define DISCHARGE_ENABLE_SENSE_Pin GPIO_PIN_7
#define DISCHARGE_ENABLE_SENSE_GPIO_Port GPIOC
#define CHARGE_SAFETY_SENSE_Pin GPIO_PIN_8
#define CHARGE_SAFETY_SENSE_GPIO_Port GPIOC
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

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
