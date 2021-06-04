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
// Type includes
#include "AuxStatus.h"
#include "AuxTrip.h"
#include "CanRxQueueData.h"
#include "CanTxGatekeeperQueueData.h"
#include "ContactorState.h"
#include "ErrorTypes.h"
#include "OrionCanInfo.h"
// Task Includes
#include "CanRxInterruptParserTask.h"
#include "CanTxGatekeeperTask.h"
#include "ChargeContactorGatekeeperTask.h"
#include "CommonContactorGatekeeperTask.h"
#include "ContactorStatusUpdateTask.h"
#include "DischargeContactorGatekeeperTask.h"
#include "OrionInterfaceTask.h"
#include "ReadAuxVoltageTask.h"
#include "SendAuxStatusTask.h"
#include "SendAuxTripTask.h"
#include "SendHeartbeatTask.h"
#include "StartupTask.h"
#include <string.h>

#ifdef MEMORY_DEBUG
#include "MemoryDebugTask.h"
#endif

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CAN_RX_PARSER_QUEUE_COUNT 4 // Anticipating 3 messages being received. +1 for extra headroom
#define ORION_INTERFACE_QUEUE_COUNT 4 // Same reason as above
#define CAN_TX_GATEKEEPER_QUEUE_COUNT 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan1;

SPI_HandleTypeDef hspi3;
DMA_HandleTypeDef hdma_spi3_rx;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes =
{
    .name = "defaultTask",
    .priority = (osPriority_t) osPriorityBelowNormal,
    .stack_size = 128 * 4
};
/* USER CODE BEGIN PV */
// SPI Rx Buffer
uint8_t spiRxBuff[AUX_BMS_SPI_BUFFER_SIZE] = {0};

// CAN Tx Header
const CAN_TxHeaderTypeDef BASE_CAN_TX_HDR = {.ExtId = 0,
                                             .RTR = CAN_RTR_DATA,
                                             .IDE = CAN_ID_STD,
                                             .TransmitGlobalTime = DISABLE
                                            };

// Queues
osMessageQueueId_t canRxParserQueue;
osMessageQueueId_t orionInterfaceQueue;
osMessageQueueId_t canTxGatekeeperQueue;

// Event Bits
osEventFlagsId_t contactorControlEventBits;

// Mutexes
osMutexId_t auxStatusOrionInterfaceMutex;
osMutexId_t auxStatusReadAuxVoltageMutex;
osMutexId_t auxStatusContactorStatusUpdateMutex;
osMutexId_t auxTripMutex;

// Global variables
ErrorCodes errorCode;
AuxStatus auxStatus;
AuxTrip auxTrip;
AuxBmsContactorState auxBmsContactorState;

// Task Handles
osThreadId_t startupTaskHandle;
osThreadId_t canRxInterruptParserTaskHandle;
osThreadId_t orionInterfaceTaskHandle;
osThreadId_t canTxGatekeeperTaskHandle;
osThreadId_t sendAuxStatusTaskHandle;
osThreadId_t sendAuxTripTaskHandle;
osThreadId_t sendHeartbeatTaskHandle;
osThreadId_t commonContactorGatekeeperTaskHandle;
osThreadId_t chargeContactorGatekeeperTaskHandle;
osThreadId_t dischargeContactorGatekeeperTaskHandle;
osThreadId_t contactorStatusUpdateTaskHandle;
osThreadId_t readAuxVoltageTaskHandle;

#ifdef MEMORY_DEBUG
osThreadId_t memoryDebugTaskHandle;
#endif

// Thread Attributes
const osThreadAttr_t startupTask_attributes =
{
    .name = "startupTask",
    .priority = (osPriority_t) osPriorityHigh2,
    .stack_size = 128 * 4
};
const osThreadAttr_t  canRxInterruptParserTask_attributes =
{
    .name = "canRxInterruptParserTask",
    .priority = (osPriority_t) osPriorityHigh1,
    .stack_size = 128 * 4
};

const osThreadAttr_t  orionInterfaceTask_attributes =
{
    .name = "orionInterfaceTask",
    .priority = (osPriority_t) osPriorityHigh,
    .stack_size = 128 * 4
};
const osThreadAttr_t  canTxGatekeeperTask_attributes =
{
    .name = "canTxGatekeeperTask",
    .priority = (osPriority_t) osPriorityAboveNormal1,
    .stack_size = 128 * 4
};
const osThreadAttr_t  sendAuxStatusTask_attributes =
{
    .name = "sendAuxStatusTask",
    .priority = (osPriority_t) osPriorityAboveNormal,
    .stack_size = 128 * 4
};
const osThreadAttr_t  sendAuxTripTask_attributes =
{
    .name = "sendAuxTripTask",
    .priority = (osPriority_t) osPriorityAboveNormal,
    .stack_size = 128 * 4
};
const osThreadAttr_t  sendHeartbeatTask_attributes =
{
    .name = "sendHeartbeatTask",
    .priority = (osPriority_t) osPriorityAboveNormal,
    .stack_size = 128 * 4
};
const osThreadAttr_t commonContactorGatekeeperTask_attributes =
{
    .name = "commonContactorGatekeeperTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
};
const osThreadAttr_t chargeContactorGatekeeperTask_attributes =
{
    .name = "chargeContactorGatekeeperTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
};
const osThreadAttr_t dischargeContactorGatekeeperTask_attributes =
{
    .name = "dischargeContactorGatekeeperTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
};
const osThreadAttr_t  contactorStatusUpdateTask_attributes =
{
    .name = "contactorStatusUpdateTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
};
const osThreadAttr_t  readAuxVoltageTask_attributes =
{
    .name = "readAuxVoltageTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
};
#ifdef MEMORY_DEBUG
const osThreadAttr_t memoryDebugTask_attributes =
{
    .name = "memoryDebugTask",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 128 * 4
};
#endif

// Mutex Attributes
const osMutexAttr_t auxStatusOrionInterfaceMutex_attributes =
{
    .name = "auxStatusOrionInterfaceMutex",
    .attr_bits = osMutexPrioInherit
};
const osMutexAttr_t auxStatusReadAuxVoltageMutex_attributes =
{
    .name = "auxStatusReadAuxVoltageMutex",
    .attr_bits = osMutexPrioInherit
};
const osMutexAttr_t auxStatusContactorStatusUpdateMutex_attributes =
{
    .name = "auxStatusContactorStatusUpdateMutex",
    .attr_bits = osMutexPrioInherit
};
const osMutexAttr_t auxTripMutex_attributes =
{
    .name = "auxTripMutex"
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN1_Init(void);
static void MX_SPI3_Init(void);
void StartDefaultTask(void* argument);

/* USER CODE BEGIN PFP */
void MX_CAN1_User_Init(void);

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
    errorCode = SETUP;
    auxStatus = (AuxStatus)
    {
        0
    };
    auxTrip = (AuxTrip)
    {
        0
    };
    auxBmsContactorState = (AuxBmsContactorState)
    {
        .commonState = OPEN,
         .chargeState = OPEN,
          .dischargeState = OPEN,
           .startupDone = 0,
            .contactorsDisconnected = 0
    };

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
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_CAN1_Init();
    MX_SPI3_Init();
    /* USER CODE BEGIN 2 */

    // Additional initialization for CAN
    MX_CAN1_User_Init();

    //Activate Can Recieve Interrupts
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING |
                                     CAN_IT_ERROR_WARNING |
                                     CAN_IT_ERROR_PASSIVE |
                                     CAN_IT_BUSOFF |
                                     CAN_IT_LAST_ERROR_CODE |
                                     CAN_IT_ERROR) != HAL_OK)
    {
        /* Reception error */
        Error_Handler();
    }

    // Start CAN Module
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        Error_Handler();
    }

    // Start ADC
    HAL_ADC_Start(&hadc1);

    /* USER CODE END 2 */

    /* Init scheduler */
    osKernelInitialize();

    /* USER CODE BEGIN RTOS_MUTEX */
    auxStatusOrionInterfaceMutex = osMutexNew(&auxStatusOrionInterfaceMutex_attributes);
    auxStatusReadAuxVoltageMutex = osMutexNew(&auxStatusReadAuxVoltageMutex_attributes);
    auxStatusContactorStatusUpdateMutex = osMutexNew(&auxStatusContactorStatusUpdateMutex_attributes);
    auxTripMutex = osMutexNew(&auxTripMutex_attributes);
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    canRxParserQueue = osMessageQueueNew(CAN_RX_PARSER_QUEUE_COUNT, sizeof(CanRxQueueData), NULL);
    orionInterfaceQueue = osMessageQueueNew(ORION_INTERFACE_QUEUE_COUNT, sizeof(OrionCanInfo), NULL);
    canTxGatekeeperQueue = osMessageQueueNew(CAN_TX_GATEKEEPER_QUEUE_COUNT, sizeof(CanTxGatekeeperQueueData), NULL);
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    startupTaskHandle = osThreadNew(startupTask, NULL, &startupTask_attributes);
    canRxInterruptParserTaskHandle = osThreadNew(canRxInterruptParserTask, NULL, &canRxInterruptParserTask_attributes);
    orionInterfaceTaskHandle = osThreadNew(orionInterfaceTask, NULL, &orionInterfaceTask_attributes);
    canTxGatekeeperTaskHandle = osThreadNew(canTxGatekeeperTask, NULL, &canTxGatekeeperTask_attributes);
    sendAuxStatusTaskHandle = osThreadNew(sendAuxStatusTask, NULL, &sendAuxStatusTask_attributes);
    sendAuxTripTaskHandle = osThreadNew(sendAuxTripTask, NULL, &sendAuxTripTask_attributes);
    sendHeartbeatTaskHandle = osThreadNew(sendHeartbeatTask, NULL, &sendHeartbeatTask_attributes);
    commonContactorGatekeeperTaskHandle = osThreadNew(commonContactorGatekeeperTask, NULL, &commonContactorGatekeeperTask_attributes);
    chargeContactorGatekeeperTaskHandle = osThreadNew(chargeContactorGatekeeperTask, NULL, &chargeContactorGatekeeperTask_attributes);
    dischargeContactorGatekeeperTaskHandle = osThreadNew(dischargeContactorGatekeeperTask, NULL, &dischargeContactorGatekeeperTask_attributes);
    contactorStatusUpdateTaskHandle = osThreadNew(contactorStatusUpdateTask, NULL, &contactorStatusUpdateTask_attributes);
    readAuxVoltageTaskHandle = osThreadNew(readAuxVoltageTask, NULL, &readAuxVoltageTask_attributes);
#ifdef MEMORY_DEBUG
    memoryDebugTaskHandle = osThreadNew(memoryDebugTask, NULL, &memoryDebugTask_attributes);
#endif
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

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 320;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
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
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */
    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
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
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_11;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN ADC1_Init 2 */

    /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

    /* USER CODE BEGIN CAN1_Init 0 */

    /* USER CODE END CAN1_Init 0 */

    /* USER CODE BEGIN CAN1_Init 1 */

    /* USER CODE END CAN1_Init 1 */
    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 4;
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_5TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = DISABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = DISABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN CAN1_Init 2 */

    /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

    /* USER CODE BEGIN SPI3_Init 0 */

    /* USER CODE END SPI3_Init 0 */

    /* USER CODE BEGIN SPI3_Init 1 */

    /* USER CODE END SPI3_Init 1 */
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
        Error_Handler();
    }

    /* USER CODE BEGIN SPI3_Init 2 */

    /* USER CODE END SPI3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

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
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, CONTACTOR_ENABLE2_Pin | CONTACTOR_ENABLE4_Pin | CONTACTOR_ENABLE3_Pin | CONTACTOR_ENABLE1_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(HV_ENABLE_GPIO_Port, HV_ENABLE_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, RED_LED_Pin | GRN_LED_Pin | BLU_LED_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(CAN1_STBY_GPIO_Port, CAN1_STBY_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(CURRENT_SENSE_ENABLE_GPIO_Port, CURRENT_SENSE_ENABLE_Pin, GPIO_PIN_SET);

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

    /*Configure GPIO pins : ORION_CHARGE_ENABLE_SENSE_Pin ORION_DISCHARGE_ENABLE_SENSE_Pin */
    GPIO_InitStruct.Pin = ORION_CHARGE_ENABLE_SENSE_Pin | ORION_DISCHARGE_ENABLE_SENSE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
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
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : SENSE2_Pin SENSE1_Pin */
    GPIO_InitStruct.Pin = SENSE2_Pin | SENSE1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void MX_CAN1_User_Init(void)
{
    CAN_FilterTypeDef orionVoltageTempFilterConfig;
    orionVoltageTempFilterConfig.FilterBank = 0; // Use first filter bank
    orionVoltageTempFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST; // Look for specific can messages
    orionVoltageTempFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    orionVoltageTempFilterConfig.FilterIdHigh = ORION_HIGH_LOW_VOLTAGES_STDID << 5; // Filter registers need to be shifted left 5 bits
    orionVoltageTempFilterConfig.FilterIdLow = 0;
    orionVoltageTempFilterConfig.FilterMaskIdHigh = ORION_TEMP_INFO_STDID << 5;
    orionVoltageTempFilterConfig.FilterMaskIdLow = 0; // Unused
    orionVoltageTempFilterConfig.FilterFIFOAssignment = 0;
    orionVoltageTempFilterConfig.FilterActivation = ENABLE;
    orionVoltageTempFilterConfig.SlaveStartFilterBank = 0; // Set all filter banks for CAN1

    if (HAL_CAN_ConfigFilter(&hcan1, &orionVoltageTempFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        Error_Handler();
    }

    CAN_FilterTypeDef orionPackInfoFilterConfig;
    orionPackInfoFilterConfig.FilterBank = 1; // Use secondary filter bank
    orionPackInfoFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    orionPackInfoFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST; // Look for specific can messages
    orionPackInfoFilterConfig.FilterIdHigh = ORION_PACK_INFO_STDID << 5; // Filter registers need to be shifted left 5 bits
    orionPackInfoFilterConfig.FilterIdLow = 0; // Filter registers need to be shifted left 5 bits
    orionPackInfoFilterConfig.FilterMaskIdHigh = 0;
    orionPackInfoFilterConfig.FilterMaskIdLow = 0; // Unused
    orionPackInfoFilterConfig.FilterFIFOAssignment = 0;
    orionPackInfoFilterConfig.FilterActivation = ENABLE;
    orionPackInfoFilterConfig.SlaveStartFilterBank = 0; // Set all filter banks for CAN2

    if (HAL_CAN_ConfigFilter(&hcan1, &orionPackInfoFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        Error_Handler();
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    CAN_RxHeaderTypeDef hdr;
    uint8_t             data[8] = {0};

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hdr, data) != HAL_OK)
    {
        return;
    }

    CanRxQueueData message;
    message.canRxHeader = hdr;
    memcpy(message.data, data, sizeof(uint8_t) * 8);
    osMessageQueuePut(canRxParserQueue, &message, 0, IRQ_QUEUE_PUT_TIMEOUT);
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi)
{
    osThreadFlagsSet(readAuxVoltageTaskHandle, SPI_READY_FLAG);
}

void vApplicationMallocFailedHook( void )
{
    errorCode = MALLOC_FAILED;
    Error_Handler();
}

void vApplicationStackOverflowHook( TaskHandle_t* pxTask, signed char* pcTaskName )
{

    if (*pxTask == startupTaskHandle)
    {
        errorCode = STACK_OVERFLOW_STARTUP_TASK;
    }
    else if (*pxTask == canRxInterruptParserTaskHandle)
    {
        errorCode = STACK_OVERFLOW_CAN_RX_PARSER_TASK;
    }
    else if (*pxTask == orionInterfaceTaskHandle)
    {
        errorCode = STACK_OVERFLOW_ORION_INTERFACE_TASK;
    }
    else if (*pxTask == canTxGatekeeperTaskHandle)
    {
        errorCode = STACK_OVERFLOW_CAN_TX_GTKPR_TASK;
    }
    else if (*pxTask == sendAuxStatusTaskHandle)
    {
        errorCode = STACK_OVERFLOW_SEND_AUX_STATUS_TASK;
    }
    else if (*pxTask == sendHeartbeatTaskHandle)
    {
        errorCode = STACK_OVERFLOW_SEND_HEARTBEAT_TASK;
    }
    else if (*pxTask == commonContactorGatekeeperTaskHandle)
    {
        errorCode = STACK_OVERFLOW_COMMON_CONTACTOR_GATEKEEPER_TASK;
    }
    else if (*pxTask == chargeContactorGatekeeperTaskHandle)
    {
        errorCode = STACK_OVERFLOW_CHARGE_CONTACTOR_GATEKEEPER_TASK;
    }
    else if (*pxTask == dischargeContactorGatekeeperTaskHandle)
    {
        errorCode = STACK_OVERFLOW_DISCHARGE_CONTACTOR_GATEKEEPER_TASK;
    }
    else if (*pxTask == contactorStatusUpdateTaskHandle)
    {
        errorCode = STACK_OVERFLOW_CONTACTOR_STATUS_UPDATE_TASK;
    }
    else if (*pxTask == readAuxVoltageTaskHandle)
    {
        errorCode = STACK_OVERFLOW_READ_AUX_VOLTAGE_TASK;
    }
    else
    {
        errorCode = STACK_OVERFLOW;
    }

    Error_Handler();
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void* argument)
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
    vTaskSuspendAll();// Suspend Scheduler
    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
    uint32_t mailbox;
    uint8_t data[1] = {errorCode};
    CAN_TxHeaderTypeDef canTxHeader = BASE_CAN_TX_HDR;
    canTxHeader.DLC = 1;
    //TODO set the STDID
    HAL_CAN_AddTxMessage(&hcan1, &(canTxHeader), data, &mailbox);

    for (;;)
    {

    }

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
