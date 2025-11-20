#include "main.h"
#include "stm32f1xx_hal.h"

void BMS0x100(const CANMSG_T CAN_MESSAGE) {

	if (CAN_MESSAGE.dlc < 7) {
		Error_Handler();
	}

	// Citire valori brute din buffer (big-endian)
	const uint16_t v_raw = be16u(&CAN_MESSAGE.data[0]); // primii 2 octeti = Voltage
	const uint16_t i_raw = be16u(&CAN_MESSAGE.data[2]); // urmatorii 2 octeti = Current
	const uint8_t soc_raw = CAN_MESSAGE.data[4];        // octetul 5 = SOC
	const uint8_t soh_raw = CAN_MESSAGE.data[5];        // octetul 6 = SOH
	const uint8_t pack_amp_hours_raw = CAN_MESSAGE.data[6]; //octetul 7 = pack amphours
	//IMPLEMENTARE CRC CHECKSUM
	float BMSpackVoltage = v_raw * VOLT_CURR_SCALE; // [V]
	float BMSpackCurrent = i_raw * VOLT_CURR_SCALE; // [A]
	float BMSsoc = soc_raw * SOC_SCALE;        // [%]
	float BMSsoh = soh_raw * SOH_TEMP_SCALE;   // [%]
	float BMSamphours = pack_amp_hours_raw * VOLT_CURR_SCALE;

	static char buf[96];
	int n = snprintf(buf, sizeof(buf),
	                 "BMS id=0x%03lX V=%.2fV I=%.2fA SOC=%.1f%% SOH=%.1f%%\r\n",
	                 (unsigned long)CAN_MESSAGE.id,
	                 BMSpackVoltage, BMSpackCurrent, BMSsoc, BMSsoh,BMSamphours);
	if (n < 0)
		return;

	if (HAL_UART_Transmit_IT(&huart, (uint8_t*) buf, (uint16_t) n) != HAL_OK) {

		Error_Handler();
	}

}
