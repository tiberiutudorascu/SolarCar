/*
 * motor_controller.c
 *
 *  Created on: Dec 22, 2025
 *      Author: tamis
 */


#include "motor_controller.h"



void Motor_Prepare_Data(uint8_t *buffer, PedalHandler_t *pedal) {


    // Resetam bufferul
    for(int i=0; i<8; i++) buffer[i] = 0;

    int16_t target_torque = 0;

    // logica de cuplu
    if (pedal->status == PEDAL_OK && !pedal->safe_state_locked){
        // Mapare: 100% = 1000 unitati cuplu
        target_torque = (pedal->throttle_pct * 10);
    }

    else{
        target_torque = 0;
    }

    // Byte 0-1: Torque (Little Endian)
    buffer[0] = target_torque & 0xFF;
    buffer[1] = (target_torque >> 8) & 0xFF;

    // Byte 2-3: Target Speed (Optional, punem 0 sau Max)
    buffer[2] = 0x00;
    buffer[3] = 0x00;

    // Byte 4: Control Flags (Bit 0 = Enable Inverter)
    if ( pedal->status == PEDAL_OK  && !pedal->safe_state_locked  ){
        buffer[4] = 0x01; // Enable
    } else {
        buffer[4] = 0x00; // Disable / Safe Mode
    }

    // Bytes 5,6,7 raman 0
}
