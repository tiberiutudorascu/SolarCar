/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for MOTOR_C_SIM
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "pedal_handler.h"
#include "motor_controller.h" // aici sunt functiile de populare a datelor (Sim_X_X ...)
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
ADC_HandleTypeDef hadc1;
CAN_HandleTypeDef hcan;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
volatile bool timer_20ms_flag = 0; // Flag setat de Timer la fiecare 20ms
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*  Helper Function: CAN_TX
 */
void CAN_TX(uint32_t StdId, uint32_t DLC, const uint8_t *Data) {
	CAN_TxHeaderTypeDef txheader;
	uint32_t txmailbox;

	txheader.DLC = DLC;
	txheader.StdId = StdId;
	txheader.IDE = CAN_ID_STD;
	txheader.RTR = CAN_RTR_DATA;

	if (HAL_CAN_AddTxMessage(&hcan, &txheader, Data, &txmailbox) != HAL_OK) {
		Error_Handler();
	}
}

/* * Callback Timer
 * Se apeleaza automat la fiecare 20ms
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		timer_20ms_flag = 1; // Ridicam steagul
	}
}
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
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config(); // 40MHz pentru CAN 500kbps

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_CAN_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */

  // 1  Pornire CAN
  if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_BUSOFF) != HAL_OK) {
	  Error_Handler();
  }
  if (HAL_CAN_Start(&hcan) != HAL_OK) {
	  Error_Handler();
  }

  // 2   Pornire Timer 20ms
  HAL_TIM_Base_Start_IT(&htim2);

  // 3   Pornire ADC si Calibrare
  HAL_ADCEx_Calibration_Start(&hadc1);

  // 4    Initializare Logica Pedala
  Pedal_Init();

  /* Variabile pentru temporizare (Counters) */
  uint8_t tick_100ms = 0;
  uint8_t tick_250ms = 0; // de fapt este de 240ms, dar eroarea este neglijabila,
  	  	  	  	  	  	  // pt ca e pt date afisate pe ecran unde nu e sesizabil (si pastram simplitatea avand doar TIM2)
  uint8_t tick_800ms = 0;
  uint8_t tick_1000ms = 0;

  /* Buffer temporar pentru date */
  uint8_t TxData[8];

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  /* Asteptam steagul de la Timer (20ms) */
	  if (timer_20ms_flag == 1) {

		  // --- 1. Procesare Pedala (Citire intrari fizice) ---
		  Pedal_Process();

		  // --- 2. Incrementare Contoare Timp ---
		  tick_100ms++;
		  tick_250ms++;
		  tick_800ms++;
		  tick_1000ms++;

		  // --- 3. Trimitere Mesaje Periodice (Simulare Motor) ---

		  /* GRUP 100ms: Status, Motor Status, Battery */
		  if (tick_100ms >= 5) { // 5 * 20ms = 100ms


			  // A.   Status Message (0x600)
			  Sim_Status_0x600(TxData); // Functia din motor_controller.c populeaza datele
			  CAN_TX(0x600, 8, TxData);


			  // B.  Motor Status (0x610) - RPM si Curent (Simulate din Pedala)
			  Sim_MotorStatus_0x610(TxData, &gPedal);
			  CAN_TX(0x610, 8, TxData);


			  // C.  Battery Status (0x618)
			  Sim_Battery_0x618(TxData);
			  CAN_TX(0x618, 8, TxData);



			  tick_100ms = 0; 	// reset counter

			  // Blink LED la fiecare 100ms pentru vizualizare
			  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

		  }

		  /* GRUP 250ms: Relative Values */
		  if (tick_250ms >= 12) { // 12 * 20ms = 240ms (aprox 250)

			  //  D.	 Relative Values (0x626)
			  Sim_Relative_0x626(TxData, &gPedal);
			  CAN_TX(0x626, 8, TxData);

			  tick_250ms = 0;
		  }

		  /* GRUP 800ms: Temperature */
		  if (tick_800ms >= 40) { // 40 * 20ms = 800ms

			  //   E. 	Temperature Data (0x628)
			  Sim_Temp_0x628(TxData);
			  CAN_TX(0x628, 8, TxData);

			  tick_800ms = 0;
		  }

		  /* GRUP 1000ms: Odometer */
		  if (tick_1000ms >= 50) { // 50 * 20ms = 1s

			  //   F. 	Odometer Data (0x620)
			  Sim_Odometer_0x620(TxData);
			  CAN_TX(0x620, 8, TxData);

			  tick_1000ms = 0;
		  }

		  // Resetam flag-ul principal si asteptam urmatoarea intrerupere
		  timer_20ms_flag = 0;
	  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  // Initializes the RCC Oscillators according to the specified parameters
  // in the RCC_OscInitTypeDef structure.

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL10; // 40MHz pentru CAN
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

 // Initializes the CPU, AHB and APB buses clocks

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK) Error_Handler();

  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) Error_Handler();
}

static void MX_CAN_Init(void)
{
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 4;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_15TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK) Error_Handler();
}

static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 399;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK) Error_Handler();

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) Error_Handler();

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) Error_Handler();
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}
