/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h> //Includerea bibliotecii string.h
#include <stdint.h> // Includerea bibliotecii stdint
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {
	//HABARNAM INCA CUM SA L TRIMIT
	int PLM;
} LORA_READY_CAN;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */

volatile bool flag = 0; //Initializarea flagului flag ca variabila volatila pentru evidenta timer-ului
volatile bool flag_uart = 0;
CANMSG_T CAN_MESSAGE; //Initializarea variabilei CAN_MESSAGE de tipul CANMSG_T pentru prelucrarea mesajelor primite pe CANBUS
BMS_t BMS = {0};

static CANMSG_T RXQ[16]; //Declararea unui buffer circular cu 16 pozitii pentru receptarea mesajelor de pe CANBUS
static volatile uint16_t RX_HEAD = 0; //Initializarea head-ului pentru coada
static volatile uint16_t RX_TAIL = 0; //Initializarea tail-ului pentru coada

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_TIM2_Init(void);
static void MX_UART4_Init(void);
/* USER CODE BEGIN PFP */

void CAN_TX(void); //Prototipul functiei CAN_TX() de tipul void
void CAN_FILTER_CONFIG(void); //Prototipul functiei CAN_FILTER_CONFIG() de tipul void

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// --- Functii Helper Ring Buffer ---
static inline uint16_t rb_next(uint16_t i) { //Functie care verifica urmatoarea pozitie din coada
	return (uint16_t) ((i + 1u) & (16 - 1u)); //daca ajunge la pozitia 16, revine la 0 cu ajutorul modulo si returneaza pozitia
}

static inline bool rb_empty(void) {	//Funcite care verifica daca coada circulara este goala si returneaza TRUE/FALSE
	return RX_HEAD == RX_TAIL;
}
static inline bool rb_full(void) { //Functie care verifica daca coada circulara este plina si returneaza TRUE/FALSE
	return rb_next(RX_HEAD) == RX_TAIL;
}
static inline void rb_add(const CANMSG_T *CAN_MESSAGE) { //Functie care adauga o variabila de tipul CANMSG_T in coada circulara
	if (rb_full()) {
		RX_TAIL = rb_next(RX_TAIL); //Daca coada este plina deplasam cu o pozitie tail, eliminand un element din coada
	}
	memcpy(&RXQ[RX_HEAD], CAN_MESSAGE, sizeof(CANMSG_T));

	RX_HEAD = rb_next(RX_HEAD); //Deplasarea cu o pozitie a head-ului
}
static inline bool rb_pop(CANMSG_T *CAN_MESSAGEread) { //Functie care elimina un element din coada
	if (rb_empty()) { //Daca este goala returneaza 0
		return 0;
	}
	memcpy(CAN_MESSAGEread, &RXQ[RX_TAIL], sizeof(CANMSG_T)); //Se copiaza un mesaj din coada in buffer pentru procesare
	RX_TAIL = rb_next(RX_TAIL); 		//Se deplaseaza head-ul
	return 1; //Returnare 1 daca functia a reusit eliminarea
}

// --- Functii CAN  ---

void CAN_FILTER_CONFIG(void) {
	CAN_FilterTypeDef canfilter = { 0 };

	canfilter.FilterActivation = ENABLE;
	canfilter.FilterBank = 0;
	canfilter.FilterFIFOAssignment = CAN_RX_FIFO0;
	canfilter.FilterIdHigh = 0x0000;
	canfilter.FilterIdLow = 0x0000;
	canfilter.FilterMaskIdHigh = 0x0000;
	canfilter.FilterMaskIdLow = 0x0000;
	canfilter.FilterMode = CAN_FILTERMODE_IDMASK;
	canfilter.FilterScale = CAN_FILTERSCALE_32BIT;

// Folosim hcan1
	if (HAL_CAN_ConfigFilter(&hcan1, &canfilter) != HAL_OK) {
		Error_Handler();
	}
}

void CAN_TX(void) {
	CAN_TxHeaderTypeDef txheader;
	uint32_t txmailbox;
	uint8_t test[7] = { 0x01, 0xF4,  // Voltage (500 -> 50.0V)
			0x00, 0x64,  // Current (100 -> 10.0A)
			0x50,  // SOC (80%)
			0x63,  // SOH (99%)
			0xFA,  // AmpHours (250 -> 25.0Ah)
			};
	txheader.DLC = 7;
	txheader.StdId = 0x100;
	txheader.IDE = CAN_ID_STD;
	txheader.RTR = CAN_RTR_DATA;

	if (HAL_CAN_AddTxMessage(&hcan1, &txheader, test, &txmailbox) != HAL_OK) {
		// Error_Handler(); // Comentat pentru a evita blocarea daca nu e conectat bus-ul
	}
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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
	MX_CAN1_Init();
	MX_TIM2_Init();
	MX_UART4_Init();
	/* USER CODE BEGIN 2 */

	CAN_FILTER_CONFIG(); // Configuram filtrele

// Activare notificari pe hcan1
	if (HAL_CAN_ActivateNotification(&hcan1,
	CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF)
			!= HAL_OK) {
		Error_Handler();
	}

// Start CAN pe hcan1
	if (HAL_CAN_Start(&hcan1) != HAL_OK) {
		Error_Handler();
	}

// Start Timer
	HAL_TIM_Base_Start_IT(&htim2);


	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		if (flag_uart == 1) {
			flag_uart = 0; // Resetam steagul

			if (HAL_UART_GetState(&huart4) == HAL_UART_STATE_READY) {
				HAL_UART_Transmit_IT(&huart4, (uint8_t*) &BMS, sizeof(BMS_t)); // Trimite mesaj
			}
		}

		while (rb_pop(&CAN_MESSAGE)) { // Proceseaza coada
			switch (CAN_MESSAGE.id) {
			case 0x100: {
				BMS = BMS0x100(CAN_MESSAGE);
				break;
			}
			case 0x101: {
				BMS = BMS0x101(CAN_MESSAGE);
				break;
			}
			case 0x102: {
				BMS = BMS0x102(CAN_MESSAGE);
				break;
			}
				/*  case 0x010: {
				 BMS = BMS0x010(CAN_MESSAGE);
				 } */
			default:
				break;
			}
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 360;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief CAN1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_CAN1_Init(void) {

	/* USER CODE BEGIN CAN1_Init 0 */

	/* USER CODE END CAN1_Init 0 */

	/* USER CODE BEGIN CAN1_Init 1 */

	/* USER CODE END CAN1_Init 1 */
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 5;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_15TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = DISABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN CAN1_Init 2 */

	/* USER CODE END CAN1_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 399;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 49999;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief UART4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART4_Init(void) {

	/* USER CODE BEGIN UART4_Init 0 */

	/* USER CODE END UART4_Init 0 */

	/* USER CODE BEGIN UART4_Init 1 */

	/* USER CODE END UART4_Init 1 */
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart4) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN UART4_Init 2 */

	/* USER CODE END UART4_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) { //Callback pentru timer2
	if (htim->Instance == TIM2)
		flag_uart = 1; //Modificarea flagului in true
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
// Verificam daca e CAN1 (hcan1)
	if (hcan->Instance == CAN1) {
		CAN_RxHeaderTypeDef rxheader = { 0 };
		uint8_t recieved_msg[8];
		CANMSG_T recievedCAN = { 0 };

		if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxheader, recieved_msg)
				!= HAL_OK) {
			Error_Handler();
		}
		uint8_t len = (rxheader.DLC > 8) ? 8 : rxheader.DLC;
		recievedCAN.id = rxheader.StdId;
		recievedCAN.dlc = len;
		memcpy(recievedCAN.data, recieved_msg, len);
		recievedCAN.flags = rxheader.RTR;

		rb_add(&recievedCAN);
	}
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
// Verificam daca e CAN1 (hcan1)
	if (hcan->Instance == CAN1) {
		CAN_RxHeaderTypeDef rxheader = { 0 };
		uint8_t recieved_msg[8];
		CANMSG_T recievedCAN = { 0 };

		if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &rxheader, recieved_msg)
				!= HAL_OK) {
			Error_Handler();
		}
		uint8_t len = (rxheader.DLC > 8) ? 8 : rxheader.DLC;
		recievedCAN.id = rxheader.StdId;
		recievedCAN.dlc = len;
		memcpy(recievedCAN.data, recieved_msg, len);
		recievedCAN.flags = rxheader.RTR;

		rb_add(&recievedCAN);
	}
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
	/*	const char msg[] = "Message sent from mailbox 0\n\r";
	 // Folosim huart1
	 if (HAL_UART_Transmit(&huart4, (uint8_t*) msg, sizeof(msg) - 1,
	 HAL_MAX_DELAY) != HAL_OK) {
	 // Error_Handler();
	 } */
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
