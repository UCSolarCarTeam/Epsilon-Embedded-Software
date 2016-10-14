/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
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
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "Lights.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan2;

osThreadId lightsTaskHandle;
osThreadId lightsCanTaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
LightsRequests lightsRequests =
{
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_CAN2_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{
    /* USER CODE BEGIN 1 */
    /* USER CODE END 1 */
    /* MCU Configuration----------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Configure the system clock */
    SystemClock_Config();
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_CAN2_Init();

    /* USER CODE BEGIN 2 */
    if (HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0) != HAL_OK)
    {
        /* Reception Error */
        Error_Handler();
    }

    /* USER CODE END 2 */
    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */
    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */
    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */
    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    /* USER CODE BEGIN RTOS_THREADS */
    osThreadDef(lightsTask, updateLights, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    lightsTaskHandle = osThreadCreate(osThread(lightsTask), &lightsRequests);
    osThreadDef(lightsCanTask, reportLightsToCan, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    lightsCanTaskHandle = osThreadCreate(osThread(lightsCanTask), &hcan2);
    /* USER CODE END RTOS_THREADS */
    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */
    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */
        /* USER CODE BEGIN 3 */
    }

    /* USER CODE END 3 */
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 320;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* CAN2 init function */
static void MX_CAN2_Init(void)
{
    static CanTxMsgTypeDef txMessage;
    static CanRxMsgTypeDef rxMessage;
    hcan2.Instance = CAN2;
    hcan2.pTxMsg = &txMessage;
    hcan2.pRxMsg = &rxMessage;

    hcan2.Init.Prescaler = 4;
    hcan2.Init.Mode = CAN_MODE_LOOPBACK;
    hcan2.Init.SJW = CAN_SJW_1TQ;
    hcan2.Init.BS1 = CAN_BS1_5TQ;
    hcan2.Init.BS2 = CAN_BS2_4TQ;
    hcan2.Init.TTCM = DISABLE;
    hcan2.Init.ABOM = DISABLE;
    hcan2.Init.AWUM = DISABLE;
    hcan2.Init.NART = DISABLE;
    hcan2.Init.RFLM = DISABLE;
    hcan2.Init.TXFP = DISABLE;

    if (HAL_CAN_Init(&hcan2) != HAL_OK)
    {
        Error_Handler();
    }

    CAN_FilterConfTypeDef sFilterConfig;
    sFilterConfig.FilterNumber = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = 0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.BankNumber = 0x2D;

    if (HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        Error_Handler();
    }

    /*##-3- Configure Transmission process #####################################*/
    hcan2.pTxMsg->StdId = 0x711; // CAN message address
    hcan2.pTxMsg->ExtId = 0x0; // Only used if (hcan2.pTxMsg->IDE == CAN_ID_EXT)
    hcan2.pTxMsg->RTR = CAN_RTR_DATA; // Data request, not remote request
    hcan2.pTxMsg->IDE = CAN_ID_STD; // Standard CAN, not Extended
    hcan2.pTxMsg->DLC = 1; // Data size in bytes
}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_RESET);
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, LED0_Pin | LED1_Pin | LED2_Pin, GPIO_PIN_RESET);
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, RSIGNAL_Pin | LSIGNAL_Pin | BRAKE_Pin | HHIGH_Pin
                      | HLOW_Pin | ESTROBE_Pin, GPIO_PIN_RESET);
    /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
    GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);
    /*Configure GPIO pin : B1_Pin */
    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
    /*Configure GPIO pins : LED0_Pin LED1_Pin LED2_Pin */
    GPIO_InitStruct.Pin = LED0_Pin | LED1_Pin | LED2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    /*Configure GPIO pin : BOOT1_Pin */
    GPIO_InitStruct.Pin = BOOT1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);
    /*Configure GPIO pins : RSIGNAL_Pin LSIGNAL_Pin BRAKE_Pin HHIGH_Pin
                             HLOW_Pin ESTROBE_Pin */
    GPIO_InitStruct.Pin = RSIGNAL_Pin | LSIGNAL_Pin | BRAKE_Pin | HHIGH_Pin
                          | HLOW_Pin | ESTROBE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    /*Configure GPIO pin : MEMS_INT2_Pin */
    GPIO_InitStruct.Pin = MEMS_INT2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
    // HAL_GPIO_TogglePin(GPIOA, LED1_Pin);
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    if (hcan->pRxMsg->StdId == 0x711 && 
        hcan->pRxMsg->IDE == CAN_ID_STD && 
        hcan->pRxMsg->DLC == 1)
    {
        HAL_GPIO_TogglePin(GPIOA, LED1_Pin);
    }
    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FMP0);
    if(HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK)
    {
        /* Reception Error */
        Error_Handler();
    }
}
/* USER CODE END 4 */


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    // HAL_GPIO_WritePin(GPIOA, LED0_Pin, 1);
    while (1)
    {
    }

    /* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
