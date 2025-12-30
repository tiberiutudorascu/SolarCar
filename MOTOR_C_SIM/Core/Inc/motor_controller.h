/*
 * motor_controller.h
 */

#ifndef INC_MOTOR_CONTROLLER_H_
#define INC_MOTOR_CONTROLLER_H_

#include <stdint.h>
#include "pedal_handler.h" // Avem nevoie de pedala pentru a simula RPM-ul



//-------- Functii de Simulare (Motor -> VCU) --------------



	// 0x600: Status Message (stare, erori)
void Sim_Status_0x600(uint8_t *buffer);

	// 0x610: Motor Status (RPM, curent, putere) ---
void Sim_MotorStatus_0x610(uint8_t *buffer, PedalHandler_t *pedal);

	// 0x618: Battery Status (tensiune, SOC)
void Sim_Battery_0x618(uint8_t *buffer);

	// 0x620: Odometer (kilometraj)
void Sim_Odometer_0x620(uint8_t *buffer);

	// 0x626: Relative Values (grafice)
void Sim_Relative_0x626(uint8_t *buffer, PedalHandler_t *pedal);


	// 0x628: Temperature (Motor, Driver)
void Sim_Temp_0x628(uint8_t *buffer);





#endif
/* INC_MOTOR_CONTROLLER_H_ */
