/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Lights.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */
uint8_t lightsInputs; // Initialized to 0
uint8_t batteryErrors[5]; //Initialized to {0,0,0,0,0}
uint8_t driversInputs[4]; // Initialized to 0
uint8_t auxBmsInputs[2];
uint8_t regenInputs[2];
SigLightsHandle sigLightsHandle;

static osThreadId lightsTaskHandle;
static osThreadId lightsCanTaskHandle;
static osThreadId heartbeatHandle;
static osThreadId blinkLightsHandle;

CAN_TxHeaderTypeDef canTxHdr;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
void StartDefaultTask(void const* argument);

/* USER CODE BEGIN PFP */
static void MX_CAN_UserInit(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */


    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_CAN_Init();
    /* USER CODE BEGIN 2 */
    MX_CAN_UserInit();

    // Setup for next CAN Receive Interrupt
    if (HAL_CAN_ActivateNotification(&hcan, (CAN_IT_RX_FIFO0_MSG_PENDING |
                                     CAN_IT_ERROR_WARNING |
                                     CAN_IT_ERROR_PASSIVE |
                                     CAN_IT_BUSOFF |
                                     CAN_IT_LAST_ERROR_CODE |
                                     CAN_IT_ERROR) != HAL_OK))
    {
        /* Reception Error */
        Error_Handler();
    }
    if(HAL_CAN_Start(&hcan) != HAL_OK)
    {
      Error_Handler();
    }

    /* USER CODE END 2 */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    // For concurrency between sendHeartbeat() and reportLightsToCan()
    osMutexId canHandleMutex;
    osMutexDef(canHandleMutex);
    canHandleMutex = osMutexCreate(osMutex(canHandleMutex));

    if (canHandleMutex == NULL)
    {
        Error_Handler();
    }

    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* USER CODE BEGIN RTOS_THREADS */
    // Setup task to update physical lights
    osThreadDef(lightsTask, updateLightsTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    lightsTaskHandle = osThreadCreate(osThread(lightsTask), NULL);
    // Setup task to report lights status to CAN
    osThreadDef(lightsCanTask, reportLightsToCanTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    lightsCanTaskHandle = osThreadCreate(osThread(lightsCanTask), canHandleMutex);
    // Setup task to report heartbeat to CAN
    osThreadDef(heartbeatTask, sendHeartbeatTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    heartbeatHandle = osThreadCreate(osThread(heartbeatTask), canHandleMutex);
    // Setup task to handle blinking left and right signal lights
    osThreadDef(blinkLightsTask, blinkSignalLightsTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    blinkLightsHandle = osThreadCreate(osThread(blinkLightsTask), NULL);

    /* USER CODE END RTOS_THREADS */

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

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL5;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

    /* USER CODE BEGIN CAN_Init 0 */

    /* USER CODE END CAN_Init 0 */

    /* USER CODE BEGIN CAN_Init 1 */

    /* USER CODE END CAN_Init 1 */
    hcan.Instance = CAN1;
    hcan.Init.Prescaler = 2;
    hcan.Init.Mode = CAN_MODE_LOOPBACK;
    hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan.Init.TimeSeg1 = CAN_BS1_5TQ;
    hcan.Init.TimeSeg2 = CAN_BS2_4TQ;
    hcan.Init.TimeTriggeredMode = DISABLE;
    hcan.Init.AutoBusOff = DISABLE;
    hcan.Init.AutoWakeUp = DISABLE;
    hcan.Init.AutoRetransmission = DISABLE;
    hcan.Init.ReceiveFifoLocked = DISABLE;
    hcan.Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(&hcan) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN CAN_Init 2 */

    /* USER CODE END CAN_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, LED_RED_Pin | LED_GREEN_Pin | LED_BLUE_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, RSIGNAL_Pin | LSIGNAL_Pin | BRAKE_Pin | HEAD_Pin
                      | STROBE_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : LED_RED_Pin LED_GREEN_Pin LED_BLUE_Pin */
    GPIO_InitStruct.Pin = LED_RED_Pin | LED_GREEN_Pin | LED_BLUE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : RSIGNAL_Pin LSIGNAL_Pin BRAKE_Pin HEAD_Pin
                             STROBE_Pin */
    GPIO_InitStruct.Pin = RSIGNAL_Pin | LSIGNAL_Pin | BRAKE_Pin | HEAD_Pin
                          | STROBE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
static void MX_CAN_UserInit(void)
{
    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterBank = 0; // Use first filter bank
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST; // Look for specific can messages
    // sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = LIGHTS_INPUT_STDID << 5; // Filter registers need to be shifted left 5 bits
    sFilterConfig.FilterIdLow = 0; // Filter registers need to be shifted left 5 bits
    // sFilterConfig.FilterIdHigh = 0; // Filter registers need to be shifted left 5 bits
    // sFilterConfig.FilterIdLow = 0; // Filter registers need to be shifted left 5 bits
    sFilterConfig.FilterMaskIdHigh = DRIVERS_INPUTS_STDID << 5;
    sFilterConfig.FilterMaskIdLow = 0; // Unused
    sFilterConfig.FilterFIFOAssignment = 0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 0; // Set all filter banks for CAN2

    CAN_FilterTypeDef batteryFilterConfig;
    batteryFilterConfig.FilterBank = 1; // Use secondary filter bank
    batteryFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    batteryFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST; // Look for specific can messages
    batteryFilterConfig.FilterIdHigh = AUXBMS_INPUT_STDID << 5; // Filter registers need to be shifted left 5 bits
    batteryFilterConfig.FilterIdLow = 0; // Filter registers need to be shifted left 5 bits
    batteryFilterConfig.FilterMaskIdHigh = DRIVERS_INPUTS_STDID << 5;
    batteryFilterConfig.FilterMaskIdLow = 0; //unused
    batteryFilterConfig.FilterFIFOAssignment = 0;
    batteryFilterConfig.FilterActivation = ENABLE;
    batteryFilterConfig.SlaveStartFilterBank = 0; // Set all filter banks for CAN2


    if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        Error_Handler();
    }

    if (HAL_CAN_ConfigFilter(&hcan, &batteryFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        Error_Handler();
    }


    /* Configure Transmission process */
    // canTxHdr.StdId = 0x0; // CAN message address, set in Lights.c


    canTxHdr.ExtId = 0x0; // Only used if (canTxHdr.IDE == CAN_ID_EXT)
    canTxHdr.RTR = CAN_RTR_DATA; // Data request, not remote request
    canTxHdr.IDE = CAN_ID_STD; // Standard CAN, not Extended
    canTxHdr.TransmitGlobalTime = DISABLE;
    canTxHdr.DLC = 1;
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const* argument)
{




    /* USER CODE BEGIN 5 */
    /* Infinite loop */
    for (;;)
    {
        osDelay(1);
    }

    /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM2)
    {
        HAL_IncTick();
    }

    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
