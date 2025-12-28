/*
 * motor_controller.h
 *
 *  Created on: Dec 22, 2025
 *      Author: tamis
 */
#ifndef INC_MOTOR_CONTROLLER_H_
#define INC_MOTOR_CONTROLLER_H_

#include <stdint.h>
#include "pedal_handler.h"

/* Functia populeaza buffer-ul de 8 bytes cu comanda Silixcon */
void Motor_Prepare_Data(uint8_t *buffer, PedalHandler_t *pedal);

#endif /* INC_MOTOR_CONTROLLER_H_ */
