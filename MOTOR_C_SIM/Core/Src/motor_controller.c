/*
 * motor_controller.c
 */

#include "motor_controller.h"

#define RPM_CONSTANT  50
#define CURRENT_CONSTANT  3
#define GEAR_RATIO  50 // presupunem ca    RPM/real_speed = 50
#define MC_VOLTAGE  50 // presupunem teniunea de la motor controller 50V

/* Helper: Curata bufferul inainte de scriere */
static void Clear_Buffer(uint8_t *buffer) {
    for(int i=0; i<8; i++) buffer[i] = 0;
}

/* --- 1    Status Messages (0x600) --- */
void Sim_Status_0x600(uint8_t *buffer) {
    Clear_Buffer(buffer);

    // Byte 0: LYNX ID (Arbitrar 10)
    buffer[0] = 10;

    // Byte 1: LYNX Mode ( CRITIC!)
    // 0x03  = Operational (running) ; daca trimitem 0, VCU va crede ca motorul nu functioneaza
    buffer[1] = 0x03;

    // Byte 4-5: Driver Limit (0xFFFF = Fara limita)
    buffer[4] = 0xFF;
    buffer[5] = 0xFF;
}

/* --- 2    Motor Status (0x610) --- */
void Sim_MotorStatus_0x610(uint8_t *buffer, PedalHandler_t *pedal) {
    Clear_Buffer(buffer);



    //     SIMULARE FIZICA :
    // daca pedala e apasata, crestem RPM-ul si curentul simulat



    int16_t sim_rpm = 0;
    int16_t sim_current = 0;

    if (pedal->status == PEDAL_OK && !pedal->safe_state_locked) {
        sim_rpm = pedal->throttle_pct * RPM_CONSTANT;    // 100% = 5000 RPM
        sim_current = pedal->throttle_pct * CURRENT_CONSTANT; // 100% = 300 Amperi
    }

    // Byte 0-1: Motor Current [A]
    buffer[0] = sim_current & 0xFF;
    buffer[1] = (sim_current >> 8) & 0xFF;


    // Byte 2-3: Motor Speed [RPM]
    buffer[2] = sim_rpm & 0xFF;
    buffer[3] = (sim_rpm >> 8) & 0xFF;


    // Byte 4-5: Vehicle Speed [km/h] (Estimare bruta: RPM / 50)
    int16_t sim_kph = sim_rpm / GEAR_RATIO;

    buffer[4] = sim_kph & 0xFF;
    buffer[5] = (sim_kph >> 8) & 0xFF;


    // Byte 6-7: Power [ W ] (Estimare: Current * 50V)
    int16_t sim_power = sim_current * MC_VOLTAGE;

    buffer[6] = sim_power & 0xFF;
    buffer[7] = (sim_power >> 8) & 0xFF;
}

/* --- 3. Battery Status (0x618) --- */
void Sim_Battery_0x618(uint8_t *buffer) {
    Clear_Buffer(buffer);

    // Byte 0: Status Bits (Bit 2 = BMS Connected)
    buffer[0] = 0x04;

    // Byte 2: SOC [%]
    buffer[2] = 88; // random value (baterie incarcata la 88%)

    // Byte 4-5: Battery Voltage [0.01 V]
    // 48V = 4800 unitati
    uint16_t volts = 4800;
    buffer[4] = volts & 0xFF;
    buffer[5] = (volts >> 8) & 0xFF;
}

/* --- 4. Odometer Data (0x620) --- */
void Sim_Odometer_0x620(uint8_t *buffer) {
    Clear_Buffer(buffer);

    // Byte 0-3: TRIP (UINT32) - Lasam fix momentan
    buffer[0] = 0x50; // 0x50 = 80 unitati (0.8km)
}

/* --- 5. Relative Values (0x626) -----   */
void Sim_Relative_0x626(uint8_t *buffer, PedalHandler_t *pedal) {
	Clear_Buffer(buffer);

	//  	simulam puterea relativa  pentru  Display 	 (-32767 ... +32767)
	int16_t rel_val = pedal->throttle_pct * 300;
	buffer[0] = rel_val & 0xFF;
	buffer[1] = (rel_val >> 8) & 0xFF;
}

/* --- 6. Temperature Data (0x628) --- */
void Sim_Temp_0x628(uint8_t *buffer) {
    Clear_Buffer(buffer);

    // Byte 0-1: Motor Temp [0.1 C] -> 50.0 C = 500
    uint16_t m_temp = 500;
    buffer[0] = m_temp & 0xFF;
    buffer[1] = (m_temp >> 8) & 0xFF;

    // Byte 4: Driver Temp [1 C] -> 35 C
    buffer[4] = 35;
}
