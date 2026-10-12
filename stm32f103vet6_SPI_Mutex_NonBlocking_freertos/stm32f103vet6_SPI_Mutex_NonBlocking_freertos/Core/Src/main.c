/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SPI_DATA_LEN              4

#define SPI_STATUS_WAIT           0
#define SPI_STATUS_DONE           1
#define SPI_STATUS_ERROR          0xFF

#define SPI_SLAVE_BASE_RESPONSE   0x5A
#define SPI_TRANSACTION_TIMEOUT   pdMS_TO_TICKS(100)


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart2;

osThreadId defaultTaskHandle;
osThreadId SpiTask1Handle;
osThreadId SpiTask2Handle;
/* USER CODE BEGIN PV */
SemaphoreHandle_t spiMutexHandle;
TaskHandle_t spiWaitingTaskHandle = NULL;

volatile uint8_t spi1Status = SPI_STATUS_WAIT;
volatile uint8_t spi2Status = SPI_STATUS_WAIT;

uint8_t slaveTx[SPI_DATA_LEN];
uint8_t slaveRx[SPI_DATA_LEN];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);
void StartSpiTask1(void const * argument);
void StartSpiTask2(void const * argument);

/* USER CODE BEGIN PFP */
HAL_StatusTypeDef SPI1_Transaction(uint8_t *txBuf,
                                   uint8_t *rxBuf,
                                   uint16_t len,
                                   TickType_t timeout);

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
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  spiMutexHandle = xSemaphoreCreateMutex();
  if (spiMutexHandle == NULL)
  {
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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of SpiTask1 */
  osThreadDef(SpiTask1, StartSpiTask1, osPriorityNormal, 0, 256);
  SpiTask1Handle = osThreadCreate(osThread(SpiTask1), NULL);

  /* definition and creation of SpiTask2 */
  osThreadDef(SpiTask2, StartSpiTask2, osPriorityNormal, 0, 256);
  SpiTask2Handle = osThreadCreate(osThread(SpiTask2), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_SLAVE;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_INPUT;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (hspi->Instance == SPI1)
    {
        spi1Status = SPI_STATUS_DONE;

        if (spiWaitingTaskHandle != NULL)
        {
            vTaskNotifyGiveFromISR(spiWaitingTaskHandle, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
    else if (hspi->Instance == SPI2)
    {
        spi2Status = SPI_STATUS_DONE;
    }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (hspi->Instance == SPI1)
    {
        spi1Status = SPI_STATUS_ERROR;

        if (spiWaitingTaskHandle != NULL)
        {
            vTaskNotifyGiveFromISR(spiWaitingTaskHandle, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
    else if (hspi->Instance == SPI2)
    {
        spi2Status = SPI_STATUS_ERROR;
    }
}

HAL_StatusTypeDef SPI1_Transaction(uint8_t *txBuf,
                                   uint8_t *rxBuf,
                                   uint16_t len,
                                   TickType_t timeout)
{
    HAL_StatusTypeDef retSlave;
    HAL_StatusTypeDef retMaster;
    TickType_t startTick;

    if (xSemaphoreTake(spiMutexHandle, timeout) != pdTRUE)
    {
        return HAL_TIMEOUT;
    }

    if (len > SPI_DATA_LEN)
    {
    	xSemaphoreGive(spiMutexHandle);
        return HAL_ERROR;
    }

    if (hspi1.State != HAL_SPI_STATE_READY)
    {
        HAL_SPI_Abort(&hspi1);
    }

    if (hspi2.State != HAL_SPI_STATE_READY)
    {
        HAL_SPI_Abort(&hspi2);
    }

    spi1Status = SPI_STATUS_WAIT;
    spi2Status = SPI_STATUS_WAIT;
    spiWaitingTaskHandle = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTake(pdTRUE, 0);

    for (uint16_t i = 0; i < len; i++)
    {
        slaveTx[i] = SPI_SLAVE_BASE_RESPONSE + i;
        slaveRx[i] = 0x00;
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

    retSlave = HAL_SPI_TransmitReceive_IT(&hspi2, slaveTx, slaveRx, len);
    if (retSlave != HAL_OK)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        spiWaitingTaskHandle = NULL;
        xSemaphoreGive(spiMutexHandle);
        return retSlave;
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    for (volatile uint32_t i = 0; i < 1000U; i++)
    {
        __NOP();
    }

    retMaster = HAL_SPI_TransmitReceive_IT(&hspi1, txBuf, rxBuf, len);
    if (retMaster != HAL_OK)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_SPI_Abort(&hspi1);
        HAL_SPI_Abort(&hspi2);

        spiWaitingTaskHandle = NULL;
        xSemaphoreGive(spiMutexHandle);
        return retMaster;
    }

    if (ulTaskNotifyTake(pdTRUE, timeout) == 0)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_SPI_Abort(&hspi1);
        HAL_SPI_Abort(&hspi2);

        spiWaitingTaskHandle = NULL;
        xSemaphoreGive(spiMutexHandle);
        return HAL_TIMEOUT;
    }

    startTick = xTaskGetTickCount();
    while (spi2Status == SPI_STATUS_WAIT)
    {
        if ((xTaskGetTickCount() - startTick) > timeout)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
            HAL_SPI_Abort(&hspi1);
            HAL_SPI_Abort(&hspi2);

            spiWaitingTaskHandle = NULL;
            xSemaphoreGive(spiMutexHandle);
            return HAL_TIMEOUT;
        }

        osDelay(1);
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

    spiWaitingTaskHandle = NULL;
    xSemaphoreGive(spiMutexHandle);

    if (spi1Status == SPI_STATUS_ERROR || spi2Status == SPI_STATUS_ERROR)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartSpiTask1 */
/**
* @brief Function implementing the SpiTask1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSpiTask1 */
void StartSpiTask1(void const * argument)
{
  /* USER CODE BEGIN StartSpiTask1 */
	uint8_t txBuf[SPI_DATA_LEN];
	uint8_t rxBuf[SPI_DATA_LEN];
	char msg[160];

  /* Infinite loop */
  for(;;)
  {
	  txBuf[0] = 0xA5;
	  txBuf[1] = 0x1F;
	  txBuf[2] = 0xB2;
	  txBuf[3] = 0x3E;

	  memset(rxBuf, 0, sizeof(rxBuf));

      if (SPI1_Transaction(txBuf, rxBuf, SPI_DATA_LEN, SPI_TRANSACTION_TIMEOUT) == HAL_OK)
      {
    	  snprintf(msg, sizeof(msg),
    	           "Task 1 TX=%02X %02X %02X %02X | RX=%02X %02X %02X %02X | DEV=%02X %02X %02X %02X\r\n",
    	           txBuf[0], txBuf[1], txBuf[2], txBuf[3],
    	           rxBuf[0], rxBuf[1], rxBuf[2], rxBuf[3],
    	           slaveRx[0], slaveRx[1], slaveRx[2], slaveRx[3]);
      }
      else
      {
    	  snprintf(msg, sizeof(msg), "Task 1 Error \r\n");
      }

      HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
      osDelay(500);
  }
  /* USER CODE END StartSpiTask1 */
}

/* USER CODE BEGIN Header_StartSpiTask2 */
/**
* @brief Function implementing the SpiTask2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSpiTask2 */
void StartSpiTask2(void const * argument)
{
  /* USER CODE BEGIN StartSpiTask2 */
	uint8_t txBuf[SPI_DATA_LEN];
	uint8_t rxBuf[SPI_DATA_LEN];
	char msg[160];
  /* Infinite loop */
  for(;;)
  {
	  txBuf[0] = 0x8C;
	  txBuf[1] = 0x4D;
	  txBuf[2] = 0x55;
	  txBuf[3] = 0x66;

	  memset(rxBuf, 0, sizeof(rxBuf));

      if (SPI1_Transaction(txBuf, rxBuf, SPI_DATA_LEN, SPI_TRANSACTION_TIMEOUT) == HAL_OK)
      {
    	  snprintf(msg, sizeof(msg),
    	           "Task 2 TX=%02X %02X %02X %02X | RX=%02X %02X %02X %02X | DEV=%02X %02X %02X %02X\r\n",
    	           txBuf[0], txBuf[1], txBuf[2], txBuf[3],
    	           rxBuf[0], rxBuf[1], rxBuf[2], rxBuf[3],
    	           slaveRx[0], slaveRx[1], slaveRx[2], slaveRx[3]);
      }
      else
      {
    	  snprintf(msg, sizeof(msg), "Task 2 Error \r\n");
      }

      HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
      osDelay(1000);
  }
  /* USER CODE END StartSpiTask2 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
