/*
 * pedal_handler.h
 *
 *  Created on: Dec 9, 2025
 *      Author: tamis
 */

#ifndef INC_PEDAL_HANDLER_H_
#define INC_PEDAL_HANDLER_H_

#include "main.h"

//ENUM - starea de sanatate a pedalei
typedef enum{
	PEDAL_OK,					// totul functioneaza OK
	ERR_POT1_DISCONNECTED,	//fir rupt la senzor 1 , conectat la GND
	ERR_POT2_DISCONNECTED, 	//fir rupt la senzor 2 , conectat la GND
	ERR_POT1_SHORT,		 // scurtcircuit la vcc pt senzor 1
	ERR_POT2_SHORT,  // scurtcircuit la vcc pt senzor 2
	ERR_IMPLAUSIBLE  // eraore prea mare intre senzori (eroare mecanica sau pedala rupta :) )
}PedalStatus_t;


typedef struct {

	uint16_t raw_adc1; // valoarea de la adc nefiltrata de la potentiometrul 1   (raw 0- 4095)
	uint16_t raw_adc2;	// valoarea de la adc nefiltrata de la potentiometrul 2

	uint16_t filt_adc1; // valoarea filtrata de la potentiometrul 1  ( dupa moving average )
	uint16_t filt_adc2; // valoarea filtrata de la potentiometrul 2  ( dupa moving average )

	uint8_t throttle_pct; // pozitie pedala 0-100%
	PedalStatus_t status; // verifica daca statusul este OK (starea curenta )

	bool safe_state_locked;

}PedalHandler_t;


extern PedalHandler_t gPedal; // variabila globala accesibila din main.c


// functii publice
void Pedal_Init(void);
void Pedal_Process(void);


#endif /* INC_PEDAL_HANDLER_H_ */
