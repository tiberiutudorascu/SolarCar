
#include "main.h"
#include "stm32f1xx_hal.h"
void HAL_MspInit(void)
{


  //Here we will do low level processor specific inits.
 	//1. Set up the priority grouping of the arm cortex mx processor
 	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
 	//2. Enable the required system exceptions of the arm cortex processor
 	SCB->SHCSR |= 0x7 << 16; //usage fault,bus fault and memory fault system exceptions

 	//3. configure the priority of the system exceptions
 	HAL_NVIC_SetPriority(MemoryManagement_IRQn,0,0);
 	HAL_NVIC_SetPriority(BusFault_IRQn,0,0);
 	HAL_NVIC_SetPriority(UsageFault_IRQn,0,0);



}
void MX_GPIO_INIT()
{
	GPIO_InitTypeDef gpioLED = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();
	gpioLED.Pin = GPIO_PIN_13;
	gpioLED.Mode = GPIO_MODE_OUTPUT_PP;
	gpioLED.Pull = GPIO_NOPULL;
	gpioLED.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &gpioLED);


}
void CAN_INIT()
{
	//CAN_HandleTypeDef hcan ={0};
	__HAL_RCC_CAN1_CLK_ENABLE();


	hcan.Instance = CAN1;
	hcan.Init.Mode = CAN_MODE_LOOPBACK;
	hcan.Init.AutoBusOff = DISABLE;
	hcan.Init.AutoRetransmission = ENABLE;
	hcan.Init.AutoWakeUp = DISABLE;
	hcan.Init.ReceiveFifoLocked = DISABLE;
	hcan.Init.TransmitFifoPriority = DISABLE;
	hcan.Init.TimeTriggeredMode = DISABLE;

	hcan.Init.Prescaler = 5;
	hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
	hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
	if(HAL_CAN_Init(&hcan) != HAL_OK)
	{
		Error_Handler();
	}


}
void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_CAN1_CLK_ENABLE();

	    GPIO_InitStruct.Pin  = GPIO_PIN_11;
	    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    // PA12 - CAN_TX (AF push-pull)
	    GPIO_InitStruct.Pin   = GPIO_PIN_12;
	    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 15, 0);
	HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 15, 0);
	HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 15, 0);
	HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 15, 0);

	HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
	HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);


}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart ={0};

	//1. here we will do the low level inits of the usart2 peripheral
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();
	//2. do the pin muxing configurations
	gpio_uart.Pin = GPIO_PIN_9;
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pull= GPIO_NOPULL;
	gpio_uart.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_10;
	gpio_uart.Mode = GPIO_MODE_INPUT;
	gpio_uart.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	//3. enable the irq and set up the priority (NVIC settings
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 15, 0);
}


