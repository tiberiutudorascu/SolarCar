/*
 * pedal_handler.c
 *
 *  Created on: Dec 9, 2025
 *      Author: tamis
 */


#include "stm32f1xx_hal.h"

#include "pedal_handler.h"
#include <stdlib.h> // pt functia de modul


 extern ADC_HandleTypeDef hadc1; // referinta catre adc-ul din main.c definit acolo

 PedalHandler_t gPedal; // --- Instanta globala a structurii pedalei ---


 //COSNTANTE DE CALIBRARE , adc pe 12 biti => 0-4095 (0-3V3)

#define ADC_MIN_VALID 100 // sub 0.08V inseamna ca e firul rupt  (pulldown ul intern trage la GND)

#define ADC_MAX_VALID 4000	// peste 3.2V inseamna ca este scurtcircuit la 3.3V (VCC)

#define MAX_DEVIATION 400   // eroare max acceptata intre senzori de 10%

 	 	 // --- buffer filtrare ---
#define FILTER_SIZE 10
 static uint16_t buf1[FILTER_SIZE] = {0};
 static uint16_t buf2[FILTER_SIZE] = {0};
 static uint8_t idx = 0;


 //-------------------------------------------------------------------
 //		-------___ FUNCTII PRIVATE ___-------

 // --- filtru moving average (reduce zgomotul electric proportional cu FILTER_SIZE ) ---
 static uint16_t Apply_Filter(uint16_t new_val, uint16_t *buffer){

	 buffer[idx] = new_val;
	 uint32_t sum =0;				// buffer-ul este circular de dimensiunea FILTER_SIZE (alocat static) ,
	 	 	 	 	 	 	 	   //  iar functia rescrie valorile noi peste peste cele existente

	 for(int i=0; i< FILTER_SIZE; i++){
		 sum+= buffer[i];
	 }

	 return (uint16_t)(sum / FILTER_SIZE);

 }


 // --- citire canal ADC individual (polling ) ---
 static uint16_t ADC_Read(uint32_t Channel){
	 ADC_ChannelConfTypeDef sConfig = {0};
	 uint16_t val =0 ;

	 //reconfiguram ADCul pt canalul dorit
	 sConfig.Channel = Channel;
	 sConfig.Rank= ADC_REGULAR_RANK_1;
	 sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5 ; // timp mai lung => mai stabil


	 if(HAL_ADC_ConfigChannel(&hadc1, &sConfig )!= HAL_OK ){
		 return 0 ;
	 }


	 // start -> asteapta -> citeste -> stop
	 HAL_ADC_Start(&hadc1);
	 if(HAL_ADC_PollForConversion(&hadc1, 10 ) == HAL_OK){
		 val = HAL_ADC_GetValue(&hadc1);
	 }

	 HAL_ADC_Stop(&hadc1);


	 return val;
 }

 //--- SAFE STATE (orpeste masina in caz de eroare ) ---

 static void Stop_Throttle(void) {
	 gPedal.throttle_pct =0; // facem pozitia acceleratiei 0 (taiem acceleratia)

	 //aprindem Led-ul de eroare (pc13)
	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // se poate stinge safe state daca senzorul primeste input ulterior. facem un flag care trebuie verificat
 }


 //--------------------------------------------------------
 //		-------___ FUNCTII PUBLICE ___-------


 void Pedal_Init(void){
	 gPedal.status = PEDAL_OK;
	 gPedal.throttle_pct = 0;

	 // stingem led-ul de eroare
	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // SET este stins , iar RESET este aprins la blue pill
 }


 void Pedal_Process(void){

	// citire raw pe PA0 si PA1
	     gPedal.raw_adc1 = ADC_Read(ADC_CHANNEL_0);
	     gPedal.raw_adc2 = ADC_Read(ADC_CHANNEL_1);

	   // filtrare a semnalului
	     gPedal.filt_adc1 = Apply_Filter(gPedal.raw_adc1, buf1);
	     gPedal.filt_adc2 = Apply_Filter(gPedal.raw_adc2, buf2);



	     idx = (idx + 1) % FILTER_SIZE;	// avansam indexul bufferului circular pentru urmatoarea tura


	  //----------------- VERIFICARE  ERORI (safety checks )  ---------------------------------


	  PedalStatus_t current_error = PEDAL_OK;

	  if(gPedal.filt_adc1 < ADC_MIN_VALID){
		  current_error = ERR_POT1_DISCONNECTED;}

	  else if(gPedal.filt_adc2 < ADC_MIN_VALID){
		  current_error = ERR_POT2_DISCONNECTED;}

	  else if(gPedal.filt_adc1 > ADC_MAX_VALID){
		  current_error = ERR_POT1_SHORT;}

	  else if(gPedal.filt_adc2 > ADC_MAX_VALID){
			  current_error = ERR_POT2_SHORT;}

	  // verificam plauzibilitate
	  else if(abs((int)gPedal.filt_adc1-(int)gPedal.filt_adc2) > MAX_DEVIATION){
		  current_error = ERR_IMPLAUSIBLE;
	  }


	  // logica pt safe state lock (blocat)

	  if(current_error != PEDAL_OK){

		  gPedal.safe_state_locked = true;
		  gPedal.status = current_error;

	  }


	  //------- gestionam iesire: ACCELERATIE/SAFE STATE --------------

	  if(gPedal.safe_state_locked){
		  //suntem in safe state ==> taiem acceleratia

		  Stop_Throttle();

		  // conditia de reset este : - nu mai exista eroarea
		  //					      - eliberam pedala, adica valoarea de la adc este mica ( cu max 100 peste minim )

		  uint32_t avg_val = (gPedal.filt_adc1 + gPedal.filt_adc1)/2 ;

		  if(current_error == PEDAL_OK && avg_val < (ADC_MIN_VALID + 100)){
			  gPedal.safe_state_locked = false;
			  gPedal.status = PEDAL_OK;

			  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET ); // stingem led-ul de eroare

		  }
	  }
	  else {
		  // sistemul e OK, calculam acceleratia normal
		  gPedal.status = PEDAL_OK;
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET ); // LED off, adica totul este ok

		  // mapare liniara simpla: 0-4095 -> 0-100%
		 	     // folosim media celor 2 senzori pentru stabilitate maxima

		 	     uint32_t avg = (gPedal.filt_adc1 + gPedal.filt_adc2) / 2;

		 	     // limitam la  100% (sa nu depaseasca uint8 )
		 	     if (avg > 4095) avg = 4095;

		 	     gPedal.throttle_pct = (uint8_t)((avg * 100) / 4095); // exprimam in procente ( cat la suta este apasata acceleratia )
	  }


 }
