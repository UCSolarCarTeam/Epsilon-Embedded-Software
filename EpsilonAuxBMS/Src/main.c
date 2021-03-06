/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "UpdateChargeAllowanceTask.h"
#include "SetContactorsTask.h"
#include "ReadAuxVoltageTask.h"
#include "ReportAuxStatusToCanTask.h"
#include "ReportHeartbeatToCanTask.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan1;

SPI_HandleTypeDef hspi3;

UART_HandleTypeDef huart3;


/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
OrionStatus orionStatus;
AuxStatus auxStatus;

static osThreadId updateChargeAllowanceTaskHandle;
static osThreadId setContactorsTaskHandle;
static osThreadId readAuxVoltageTaskHandle;
static osThreadId reportAuxStatusToCanTaskHandle;
static osThreadId reportHeartbeatToCanTaskHandle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI3_Init(void);

/* USER CODE BEGIN PFP */
static void MX_CAN1_UserInit(void);
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
static const uint32_t ORION_MAX_MIN_VOLTAGES_STDID  = 0x305;
static const uint32_t ORION_TEMP_INFO_STDID = 0x304;
static const uint32_t ORION_PACK_INFO_STDID = 0x302;

/* USER CODE END 0 */

int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration----------------------------------------------------------*/

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
    MX_CAN1_Init();
    MX_USART3_UART_Init();
    MX_ADC1_Init();
    MX_SPI3_Init();

    /* USER CODE BEGIN 2 */
    MX_CAN1_UserInit();
    HAL_ADC_Start(&hadc1);

    // Start with not allowing charge
    auxStatus.allowCharge = 0;

    // Start with orionBatteryVoltagesOk set to 1 to allow contactor setting
    orionStatus.batteryVoltagesInRange = 1;

    // Start with canMsgReceived set to 0 to prevent minCellVoltage causing
    // contactor setting problems
    orionStatus.canMsgReceived = 0;

    // Setup for next CAN Receive Interrupt
    if (HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0) != HAL_OK)
    {
        /* Reception Error */
        _Error_Handler(__FILE__, __LINE__);
    }

    /* USER CODE END 2 */

    /* USER CODE BEGIN RTOS_MUTEX */
    // For concurrency between sendHeartbeat() and reportAuxToCan()
    osMutexId canHandleMutex;
    osMutexDef(canHandleMutex);
    canHandleMutex = osMutexCreate(osMutex(canHandleMutex));

    if (canHandleMutex == NULL)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    osMutexDef(auxStatusMutex);
    auxStatus.auxStatusMutex = osMutexCreate(osMutex(auxStatusMutex));

    if (auxStatus.auxStatusMutex == NULL)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    osMutexDef(orionStatusMutex);
    orionStatus.orionStatusMutex = osMutexCreate(osMutex(orionStatusMutex));

    if (orionStatus.orionStatusMutex == NULL)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

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
    // Setup task to determine allowance of charge/discharge based on Orion voltage inputs
    osThreadDef(chargeAllowanceTask, updateChargeAllowanceTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    updateChargeAllowanceTaskHandle = osThreadCreate(osThread(chargeAllowanceTask), NULL);
    // Setup task to turn on contactors
    osThreadDef(contactorsTask, setContactorsTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    setContactorsTaskHandle = osThreadCreate(osThread(contactorsTask), NULL);
    // Setup task to read aux voltage
    osThreadDef(auxVoltageTask, readAuxVoltageTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    readAuxVoltageTaskHandle = osThreadCreate(osThread(auxVoltageTask), NULL);
    // Setup task to report aux status to CAN
    osThreadDef(auxStatusTask, reportAuxStatusToCanTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    reportAuxStatusToCanTaskHandle = osThreadCreate(osThread(auxStatusTask), canHandleMutex);
    // Setup task to report hearbeat to CAN
    osThreadDef(heartbeatTask, reportHeartbeatToCanTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
    reportHeartbeatToCanTaskHandle = osThreadCreate(osThread(heartbeatTask), canHandleMutex);
    /* add threads, ... */
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

    /**Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks
    */
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
        _Error_Handler(__FILE__, __LINE__);
    }

    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    /**Configure the Systick interrupt time
    */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

    /**Configure the Systick
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

    ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_11;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}

/* CAN1 init function */
static void MX_CAN1_Init(void)
{

    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 4;
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.SJW = CAN_SJW_1TQ;
    hcan1.Init.BS1 = CAN_BS1_5TQ;
    hcan1.Init.BS2 = CAN_BS2_4TQ;
    hcan1.Init.TTCM = DISABLE;
    hcan1.Init.ABOM = DISABLE;
    hcan1.Init.AWUM = DISABLE;
    hcan1.Init.NART = DISABLE;
    hcan1.Init.RFLM = DISABLE;
    hcan1.Init.TXFP = DISABLE;

    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}

/* SPI3 init function */
static void MX_SPI3_Init(void)
{

    /* SPI3 parameter configuration*/
    hspi3.Instance = SPI3;
    hspi3.Init.Mode = SPI_MODE_MASTER;
    hspi3.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
    hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
    hspi3.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi3.Init.NSS = SPI_NSS_SOFT;
    hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi3.Init.CRCPolynomial = 10;

    if (HAL_SPI_Init(&hspi3) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart3) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

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
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, CONTACTOR_ENABLE2_Pin | CONTACTOR_ENABLE4_Pin | CONTACTOR_ENABLE3_Pin | CONTACTOR_ENABLE1_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, RED_LED_Pin | GRN_LED_Pin | BLU_LED_Pin | CAN1_STBY_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(ADC_nCS_GPIO_Port, ADC_nCS_Pin, GPIO_PIN_SET);

    /*Configure GPIO pins : CONTACTOR_ENABLE2_Pin CONTACTOR_ENABLE4_Pin CONTACTOR_ENABLE3_Pin CONTACTOR_ENABLE1_Pin */
    GPIO_InitStruct.Pin = CONTACTOR_ENABLE2_Pin | CONTACTOR_ENABLE4_Pin | CONTACTOR_ENABLE3_Pin | CONTACTOR_ENABLE1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pin : HV_ENABLE_Pin */
    GPIO_InitStruct.Pin = HV_ENABLE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(HV_ENABLE_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : RED_LED_Pin GRN_LED_Pin BLU_LED_Pin CAN1_STBY_Pin */
    GPIO_InitStruct.Pin = RED_LED_Pin | GRN_LED_Pin | BLU_LED_Pin | CAN1_STBY_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : CHARGE_ENABLE_SENSE_Pin DISCHARGE_ENABLE_SENSE_Pin */
    GPIO_InitStruct.Pin = CHARGE_ENABLE_SENSE_Pin | DISCHARGE_ENABLE_SENSE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin : CURRENT_SENSE_ENABLE_Pin */
    GPIO_InitStruct.Pin = CURRENT_SENSE_ENABLE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(CURRENT_SENSE_ENABLE_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : ADC_nCS_Pin */
    GPIO_InitStruct.Pin = ADC_nCS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ADC_nCS_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : SENSE4_Pin SENSE3_Pin */
    GPIO_InitStruct.Pin = SENSE4_Pin | SENSE3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : SENSE2_Pin SENSE1_Pin */
    GPIO_InitStruct.Pin = SENSE2_Pin | SENSE1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

// Reimplement weak definition in stm32f4xx_hal_can.c
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    CanRxMsgTypeDef* msg = hcan->pRxMsg;

    if (msg->StdId == ORION_MAX_MIN_VOLTAGES_STDID && msg->DLC == 8)
    {
        // Voltages are 2 bytes each, and memory is stored in little endian format
        orionStatus.minCellVoltage = (uint16_t)msg->Data[0] | msg->Data[1] << 8; // Min Cell voltage
        orionStatus.maxCellVoltage = (uint16_t)msg->Data[3] | msg->Data[4] << 8; // Max Cell Voltage

        orionStatus.canMsgReceived = 1;
    }
    else if (msg->StdId == ORION_TEMP_INFO_STDID && msg->DLC == 8)
    {
        orionStatus.highTemperature = msg->Data[0];
        orionStatus.canMsgReceived = 1;

    }
    else if (msg->StdId == ORION_PACK_INFO_STDID && msg->DLC == 8)
    {
        short packCurrentInt =  // Units 0.1 A
            (msg->Data[0] << 0) |
            (msg->Data[1] << 8);
        orionStatus.packCurrent = (float)packCurrentInt / 10.0f;
        orionStatus.canMsgReceived = 1;

    }

    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FMP0);

    if (HAL_CAN_Receive_IT(hcan, CAN_FIFO0) == HAL_OK)
        // Toggle green LED for every CAN message received
    {
        HAL_GPIO_TogglePin(GRN_LED_GPIO_Port, GRN_LED_Pin);
    }
}

static void MX_CAN1_UserInit(void)
{
    HAL_GPIO_WritePin(CAN1_STBY_GPIO_Port, CAN1_STBY_Pin, GPIO_PIN_RESET);

    CAN_FilterConfTypeDef orionVoltageTempFilterConfig;
    orionVoltageTempFilterConfig.FilterNumber = 0; // Use first filter bank
    orionVoltageTempFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST; // Look for specific can messages
    orionVoltageTempFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    orionVoltageTempFilterConfig.FilterIdHigh = ORION_MAX_MIN_VOLTAGES_STDID << 5; // Filter registers need to be shifted left 5 bits
    orionVoltageTempFilterConfig.FilterIdLow = 0;
    orionVoltageTempFilterConfig.FilterMaskIdHigh = ORION_TEMP_INFO_STDID << 5;
    orionVoltageTempFilterConfig.FilterMaskIdLow = 0; // Unused
    orionVoltageTempFilterConfig.FilterFIFOAssignment = 0;
    orionVoltageTempFilterConfig.FilterActivation = ENABLE;
    orionVoltageTempFilterConfig.BankNumber = 0; // Set all filter banks for CAN1

    if (HAL_CAN_ConfigFilter(&hcan1, &orionVoltageTempFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        _Error_Handler(__FILE__, __LINE__);
    }

    CAN_FilterConfTypeDef orionPackInfoFilterConfig;
    orionPackInfoFilterConfig.FilterNumber = 1; // Use secondary filter bank
    orionPackInfoFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    orionPackInfoFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST; // Look for specific can messages
    orionPackInfoFilterConfig.FilterIdHigh = ORION_PACK_INFO_STDID << 5; // Filter registers need to be shifted left 5 bits
    orionPackInfoFilterConfig.FilterIdLow = 0; // Filter registers need to be shifted left 5 bits
    orionPackInfoFilterConfig.FilterMaskIdHigh = 0;
    orionPackInfoFilterConfig.FilterMaskIdLow = 0; //unused
    orionPackInfoFilterConfig.FilterFIFOAssignment = 0;
    orionPackInfoFilterConfig.FilterActivation = ENABLE;
    orionPackInfoFilterConfig.BankNumber = 0; // Set all filter banks for CAN2

    if (HAL_CAN_ConfigFilter(&hcan1, &orionPackInfoFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        _Error_Handler(__FILE__, __LINE__);
    }

    /* Configure Transmission process */
    static CanTxMsgTypeDef txMessage;
    static CanRxMsgTypeDef rxMessage;
    hcan1.pTxMsg = &txMessage;
    hcan1.pRxMsg = &rxMessage;

    hcan1.pTxMsg->ExtId = 0x0; // Only used if (hcan2.pTxMsg->IDE == CAN_ID_EXT)
    hcan1.pTxMsg->RTR = CAN_RTR_DATA; // Data request, not remote request
    hcan1.pTxMsg->IDE = CAN_ID_STD; // Standard CAN, not Extended
    hcan1.pTxMsg->DLC = 2; // Data size in bytes
}

/* USER CODE END 4 */


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
  * @param  None
  * @retval None
  */
void _Error_Handler(char* file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while (1)
    {
    }

    /* USER CODE END Error_Handler_Debug */
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
