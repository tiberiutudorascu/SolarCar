/* USER CODE BEGIN Header */

/**

  **********

  * @file           : main.h

  * ... (restul header-ului) ...

  **********

  */

/* USER CODE END Header */



/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __MAIN_H

#define __MAIN_H



#ifdef __cplusplus

extern "C" {

#endif



/* Includes ------------------------------------------------------------------*/

#include "stm32f1xx_hal.h"



/* Private includes ----------------------------------------------------------*/

/* USER CODE BEGIN Includes */



#include <stdio.h>

#include <stdbool.h>

#include "stm32f1xx_hal.h"

#include "stm32f1xx_hal_tim.h"





/* USER CODE END Includes */



/* Exported types ------------------------------------------------------------*/

/* USER CODE BEGIN ET */



typedef struct {

	uint16_t id;

	uint8_t dlc;

	uint8_t data[8];

	uint8_t flags;

} CANMSG_T;



/* USER CODE END ET */



/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */



extern CANMSG_T CAN_MESSAGE;

extern UART_HandleTypeDef huart;

extern CAN_HandleTypeDef hcan;

extern TIM_HandleTypeDef htim2;

extern ADC_HandleTypeDef hadc1;


/* USER CODE END EC */



/* Exported macro ------------------------------------------------------------*/

/* USER CODE BEGIN EM */



/* USER CODE END EM */



/* Exported functions prototypes ---------------------------------------------*/

void Error_Handler(void);



/* USER CODE BEGIN EFP */



/* USER CODE END EFP */



/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */



/* USER CODE END Private defines */



#ifdef __cplusplus

}

#endif



#endif /* __MAIN_H */
