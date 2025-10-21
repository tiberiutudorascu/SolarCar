#include "stm32f1xx_hal.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

void SystemClock_Config();
void MX_GPIO_INIT();
void CAN_INIT();
void CAN_TX();
void CAN_FILTER_CONFIG();
void UART_INIT();
void Error_Handler();

CAN_HandleTypeDef hcan;
UART_HandleTypeDef huart;
int main(void) {
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_INIT();
	UART_INIT();
	CAN_INIT();
	CAN_FILTER_CONFIG();
	if (HAL_CAN_ActivateNotification(&hcan,
	CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF)
			!= HAL_OK) {
		Error_Handler();
	}

	if (HAL_CAN_Start(&hcan) != HAL_OK) {
		Error_Handler();
	}
	CAN_TX();
	while(1)
	{

	}

}

void Error_Handler(void) {
	while (1) {

	}
}

void SystemClock_Config() {
	RCC_OscInitTypeDef osc_init = { 0 };
	RCC_ClkInitTypeDef clk_init = { 0 };
	osc_init.HSEState = RCC_HSE_OFF;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.HSICalibrationValue = 16;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	osc_init.PLL.PLLMUL = RCC_PLL_MUL10;
	if (HAL_RCC_OscConfig(&osc_init) != HAL_OK) {
		Error_Handler();
	}

	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
	RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
	clk_init.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
}

void CAN_TX() {
	CAN_TxHeaderTypeDef txheader;
	uint32_t txmailbox;
	uint8_t test[6] = "test\n\r";

	txheader.DLC = 6;
	txheader.StdId = 0x01;
	txheader.IDE = CAN_ID_STD;
	txheader.RTR = CAN_RTR_DATA;

	if (HAL_CAN_AddTxMessage(&hcan, &txheader, test, &txmailbox) != HAL_OK) {
		Error_Handler();
		while (1) {
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			HAL_Delay(800);
			printf("1\n");
		}
	}

}

void CAN_FILTER_CONFIG() {
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

	if (HAL_CAN_ConfigFilter(&hcan, &canfilter) != HAL_OK) {
		Error_Handler();
	}

}
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
	const char msg[] = "Message sent from mailbox 1\n\r";

	if (HAL_UART_Transmit(&huart, (uint8_t*) msg, sizeof(msg) - 1,
	HAL_MAX_DELAY) != HAL_OK) {
		Error_Handler();
	}
}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
	const char msg[] = "Message sent from mailbox 1\n\r";

	if (HAL_UART_Transmit(&huart, (uint8_t*) msg, sizeof(msg) - 1,
	HAL_MAX_DELAY) != HAL_OK) {
		Error_Handler();
	}
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
	const char msg[] = "Message sent from mailbox 2\n\r";

	if (HAL_UART_Transmit(&huart, (uint8_t*) msg, sizeof(msg) - 1,
	HAL_MAX_DELAY) != HAL_OK) {
		Error_Handler();
	}
}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CAN_RxHeaderTypeDef rxheader = { 0 };
	uint8_t recieved_msg[8];

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxheader, recieved_msg)
			!= HAL_OK) {
		Error_Handler();
	}
	HAL_UART_Transmit(&huart, (uint8_t*) recieved_msg, sizeof(recieved_msg) - 1,
	HAL_MAX_DELAY);

}
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {

}

void UART_INIT() {
	huart.Instance = USART1;
	huart.Init.BaudRate = 115200;
	huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.Mode = UART_MODE_TX_RX;
	if (HAL_UART_Init(&huart) != HAL_OK) {
		Error_Handler();
	}
}

