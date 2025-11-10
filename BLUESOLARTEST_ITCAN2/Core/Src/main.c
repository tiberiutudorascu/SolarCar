#include "stm32f1xx_hal.h" //Inlcuderea bibliotecii pentru Hardware Abstaction Layer (HAL) a STM32F1X
#include "main.h" // Includerea fisierului header main.h
#include <string.h> //Includerea bibliotecii string.h
#include <stdbool.h> // Includerea bibliotecii stdbool
#include <stdint.h> // Includerea bibliotecii stdint

void SystemClock_Config(); //Prototipul functiei SystemClock_Config() de tipul void
void MX_GPIO_INIT(); //Prototipul functiei MX_GPIO_INIT() de tipul void
void CAN_INIT(); //Prototipul functiei CAN_INIT() de tipul void
void CAN_TX(); //Prototipul functiei CAN_TX() de tipul void
void CAN_FILTER_CONFIG(); //Prototipul functiei CAN_FILTER_CONFIG() de tipul void
void UART_INIT(); //Prototipul functiei UART_INIT() de tipul void
void Error_Handler(); //Prototipul functiei Error_Handler() de tipul void
void CANTIM2_INIT(void); //Prototipul functiei CANTIM2_INIT() de tipul void

CAN_HandleTypeDef hcan; //Initializarea variabilei hcan de tipul CAN_HandleTypeDef
UART_HandleTypeDef huart; //Initializarea variabilei huart de tipul UART_HandleTypeDef
TIM_HandleTypeDef htim2; //Initializarea variabilei htim2 de tipul TIM_HandleTypeDef
volatile bool flag = 0; //Initializarea flagului flag ca variabila volatila pentru evidenta timer-ului

CANMSG_T CAN_MESSAGE; //Initializarea variabilei CAN_MESSAGE de tipul CANMSG_T pentru prelucrarea mesajelor primite pe CANBUS

typedef struct {
	//HABARNAM INCA NUM SA L TRIMIT
	int PL;
} LORA_READY_CAN;

static CANMSG_T RXQ[16]; //Declararea unui buffer circular cu 16 pozitii pentru receptarea mesajelor de pe CANBUS
static volatile uint16_t RX_HEAD = 0; //Initializarea head-ului pentru coada
static volatile uint16_t RX_TAIL = 0; //Initializarea tail-ului pentru coada

static inline uint16_t rb_next(uint16_t i) { //Functie care verifica urmatoarea pozitie din coada
	return (uint16_t) ((i + 1u) & (16 - 1u));//daca ajunge la pozitia 16, revine la 0 cu ajutorul modulo si returneaza pozitia
	}

	static inline bool rb_empty(void) {	//Funcite care verifica daca coada circulara este goala si returneaza TRUE/FALSE
		return RX_HEAD == RX_TAIL;
	}
	static inline bool rb_full(void) { //Functie care verifica daca coada circulara este plina si returneaza TRUE/FALSE
		return rb_next(RX_HEAD) == RX_TAIL;
	}
	static inline void rb_add(const CANMSG_T *canmsg) { //Functie care adauga o variabila de tipul CANMSG_T in coada circulara
		if (rb_full()) {
			RX_TAIL = rb_next(RX_TAIL); //Daca coada este plina deplasam cu o pozitie tail, eliminand un element din coada
		}
		memcpy(&RXQ[RX_HEAD], canmsg, sizeof(CANMSG_T));

		RX_HEAD = rb_next(RX_HEAD); //Deplasarea cu o pozitie a head-ului
	}
	static inline bool rb_pop(CANMSG_T *canmsgread) { //Functie care elimina un element din coada
		if (rb_empty()) { //Daca este goala returneaza 0
			return 0;
		}
		memcpy(canmsgread, &RXQ[RX_TAIL], sizeof(CANMSG_T)); //Se copiaza un mesaj din coada in buffer pentru procesare
		RX_TAIL = rb_next(RX_TAIL); 		//Se deplaseaza head-ul
		return 1; //Returnare 1 daca functia a reusit eliminarea
	}

	int main(void) {
		HAL_Init(); //Apelarea functiei HAL_INIT() pentru intializarea hardware abstraction layer
		SystemClock_Config(); //Aplearea functiei de configurare a ceasului si a oscilatorului
		MX_GPIO_INIT(); //Apelarea functiei pentru configurarea unui GPIO
		UART_INIT(); //Apelarea functiei pentru initalizarea portului UART
		CAN_INIT(); //Apelarea functiei pentru intializarea comunicarii CAN si configurarea pinilor
		CAN_FILTER_CONFIG(); //Apelarea functiei CAN_FILTER_CONFIG() pentru configurarea filtrelor pentru CANBUS
		CANTIM2_INIT(); //Apelarea functiei CANTIM2_INIT pentru configurarea timerului 2

		if (HAL_CAN_ActivateNotification(&hcan,
		CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF) //Activarea notificarilor atunci cand mailbox-ul este gol
				!= HAL_OK) { 												// Este un mesaj primit in FIFO0 sau este o eroare pe bus
			Error_Handler();												//Daca nu e returnat HAL_OK, intram in error handler
		}

		if (HAL_CAN_Start(&hcan) != HAL_OK) {								//Pornirea comunicarii CAN
			Error_Handler();	//Daca nu e returnat HAL_OK, intram in error handler
		}
		HAL_TIM_Base_Start_IT(&htim2);	//Pornirea timerului

		while (1) { //Bucla infinita pentru rularea programului

			if (flag == 1) {	//Daca flag este 1
				CAN_TX();		//un mesaj e trimis pe bus
				flag = 0;		//flag se reseteaza la 0

			}
			while (rb_pop(&CAN_MESSAGE)) {		//Cat timp se citesc mesaje din coada
				switch (CAN_MESSAGE.id) {		//Se verifica ID ul si se proceseaza functia in functie de acesta
				case 0x100: {
					BMS0x100(CAN_MESSAGE);		//Apelarea functiei BMS0x100
					break;
				}
				default:
					break;

				}
			}
		}

	}

	void Error_Handler(void) {			//Functia Error Handler (Bucla infinita momentan)
		while (1) {

		}
	}

	void SystemClock_Config() {				//Functie pentru configurarea ceasului si a oscilatorului
		RCC_OscInitTypeDef osc_init = { 0 };
		RCC_ClkInitTypeDef clk_init = { 0 };
		osc_init.HSEState = RCC_HSE_OFF; //Setam high speed external clock off
		osc_init.HSIState = RCC_HSI_ON; //Setam high speed internal clock on
		osc_init.HSICalibrationValue = 16; //Il calibram cu valoarea standard 16
		osc_init.PLL.PLLState = RCC_PLL_ON; //Activam phase locked loop pentru a multiplica valoarea de la HSI
		osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2; // Divizam inputul cu 2
		osc_init.PLL.PLLMUL = RCC_PLL_MUL10; //Si il multiplicam cu 10
		if (HAL_RCC_OscConfig(&osc_init) != HAL_OK) { //Trimitem configurarea oscilatorului si speram sa obtinem HAL_OK
			Error_Handler(); //Daca nu, error handler
		}

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
		RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2; //Initializam sysclock, hclk, pclk1, pclk2, adica system clock, ahb si apb1 si apb2
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; //Sursa ceasului este phase locked loop clock pe care l am configurat sus
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1; //Pentru AHB divizam cu 1
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2; //Pentru APB1 cu 2
		clk_init.APB2CLKDivider = RCC_HCLK_DIV1; //Pentru APB2 cu 1
		if (HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_1) != HAL_OK) {
			Error_Handler(); //Trimitem configurarea cu flash latency ul din reference manual in functie de frecventa
		}
	}

	void CAN_TX() {		//Functie pentru trimiterea unui mesaj pe bus
		CAN_TxHeaderTypeDef txheader; //Variabila txheader de tipul CAN_TxHeaderTypeDef
		uint32_t txmailbox; //Mailboxul in care punem mesajul
		uint8_t test[6] = "test\r\n";  //Mesaj de test

		txheader.DLC = 6; 	//Dimensiunea frame ului trimis
		txheader.StdId = 0x01; //ID ul standard
		txheader.IDE = CAN_ID_STD; //Id standard
		txheader.RTR = CAN_RTR_DATA; //Tipul de date trimis, trimitem data nu remote frame

		if (HAL_CAN_AddTxMessage(&hcan, &txheader, test, &txmailbox) //Apelam functia si trimitem parametrii, speram ca primim HAL_OK
				!= HAL_OK) {
			Error_Handler();
			while (1) {
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
				HAL_Delay(800); //Asta aici era de test pentru loopback dar nu l am mai scos
				printf("1\n");
			}
		}

	}

	void CAN_FILTER_CONFIG() { //Functie de configurare Recieve ALL mastile oprite filtre pe id oprite etc
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
	void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {		//CallBack pentru cand un mesaj este trimis cu succes, printeaza mesaj in functie de mailbox
		const char msg[] = "Message sent from mailbox 1\n\r";

		if (HAL_UART_Transmit(&huart, (uint8_t*) msg, sizeof(msg) - 1,
		HAL_MAX_DELAY) != HAL_OK) {
			Error_Handler();
		}

	}
	/*
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
	 } */
	void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) { //Callback pentru cand un mesaj este primit pe FIFO0
		CAN_RxHeaderTypeDef rxheader = { 0 }; //RX header
		uint8_t recieved_msg[8]; //Bufferul pentru mesaj
		CANMSG_T recievedCAN = { 0 }; //Mesajul de adaugat in coada circulara

		if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxheader, recieved_msg)
				!= HAL_OK) {
			Error_Handler();  //Daca se primeste mesaj si returneaza HAL_OK bine, daca nu, Error_handler()
		}
		uint8_t len = (rxheader.DLC > 8) ? 8 : rxheader.DLC;   //Calculam dimensiunea pachetului
		recievedCAN.id = rxheader.StdId; //Copiem IDUL
		recievedCAN.dlc = len; //DLC primeste dimensiunea pachetului
		memcpy(recievedCAN.data, recieved_msg, len); //Copiem payloadul in mesajul nostru pentru procesare
		recievedCAN.flags = rxheader.RTR; //Verificam eventuale flaguri

		rb_add(&recievedCAN); //Adaugam mesajul in coada
		//HAL_UART_Transmit(&huart, (uint8_t*) recieved_msg, sizeof(recieved_msg) - 1,
		//HAL_MAX_DELAY);

	}
	void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) { //Callback pentru eroare pe canbus

	}
	void CANTIM2_INIT() {  //Initializarea timerului 2
		htim2.Instance = TIM2;
		htim2.Init.Prescaler = 0x18F; //Prescaler si perioada se calculeaza folosind excelul de pe drive
		htim2.Init.Period = 0xC34F; //Timer creat pentru 500ms

		if (HAL_TIM_Base_Init(&htim2) != HAL_OK) { //Apelam functia hal si speram la hal_ok
			Error_Handler();
		}

	}

	void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) { //Callback pentru timer2 atunci cand trec 500ms
		if (htim->Instance == TIM2)
			flag = 1; //Modificarea flagului in true
	}

	void UART_INIT() { //Functie pentru config/init portului UART
		huart.Instance = USART1; //Folosim usart1
		huart.Init.BaudRate = 115200; //Baudrate 115200
		huart.Init.WordLength = UART_WORDLENGTH_8B; //Word length de 8 Bytes
		huart.Init.StopBits = UART_STOPBITS_1; //Un singur stop bit
		huart.Init.Parity = UART_PARITY_NONE; //Fara bit de paritate
		huart.Init.HwFlowCtl = UART_HWCONTROL_NONE; //Nu avem request to send si clear to send
		huart.Init.Mode = UART_MODE_TX_RX; //Transmit si recieve pe uart
		if (HAL_UART_Init(&huart) != HAL_OK) { //Speram la HAL_OK
			Error_Handler(); //Error_handler in caz ca nu
		}
	}

