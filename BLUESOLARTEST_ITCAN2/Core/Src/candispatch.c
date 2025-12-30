#include "candispatch.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <stdio.h>

bool CRC_CHECKSUM(const CANMSG_T *CAN_MESSAGE) {

	uint32_t crc_check_sum = CAN_MESSAGE->id + 8u + CAN_MESSAGE->data[0]
			+ CAN_MESSAGE->data[1] + CAN_MESSAGE->data[2] + CAN_MESSAGE->data[3]
			+ CAN_MESSAGE->data[4] + CAN_MESSAGE->data[5]
			+ CAN_MESSAGE->data[6];

	uint8_t expected_crc_check_sum = (uint8_t) (crc_check_sum & 0xFF);
	if (expected_crc_check_sum == CAN_MESSAGE->data[7])
		return 1;
	else
		return 0;
}

void BMS0x100(const CANMSG_T CAN_MESSAGE) {

	if (CAN_MESSAGE.dlc < 7) {
		Error_Handler();
	}
	if (CRC_CHECKSUM(&CAN_MESSAGE) != 1) {
		Error_Handler();
	} else {

		const float BMSpackVoltage = be16u(&CAN_MESSAGE.data[0]) * VOLT_CURR_SCALE; // [V]
		const float BMSpackCurrent = be16u(&CAN_MESSAGE.data[2]) * VOLT_CURR_SCALE; // [A]
		const float BMSsoc = CAN_MESSAGE.data[4];        // [%]
		const float BMSsoh = CAN_MESSAGE.data[5] * SOH_TEMP_SCALE;   // [%]
		const float BMSamphours = CAN_MESSAGE.data[6] * VOLT_CURR_SCALE;

		static char buf[96];
		int n =
				snprintf(buf, sizeof(buf),
						"BMS id=0x%03lX V=%.2fV I=%.2fA SOC=%.1f%% SOH=%.1f%% AMPH =%.1f%% \r\n",
						(unsigned long) CAN_MESSAGE.id, BMSpackVoltage,
						BMSpackCurrent, BMSsoc, BMSsoh, BMSamphours);
		if (n < 0)
			return;

		if (HAL_UART_Transmit_IT(&huart, (uint8_t*) buf, (uint16_t) n)
				!= HAL_OK) {

			Error_Handler();
		}
	}

}
void BMS0x101(const CANMSG_T CAN_MESSAGE) {
	if (CAN_MESSAGE.dlc < 7) {
		Error_Handler();
	}

	if (CRC_CHECKSUM(&CAN_MESSAGE) != 1) {
		Error_Handler();
	} else {

		const int8_t BMShighTemperature = CAN_MESSAGE.data[0];  // [C]
		const uint8_t BMShighTermID = CAN_MESSAGE.data[1];      // ID >=1
		const int8_t BMSlowTemperature = CAN_MESSAGE.data[2]; 	// [c]
		const uint8_t BMSlowTermID = CAN_MESSAGE.data[3];			// ID >=1
		const uint8_t BMSaverageTemp = CAN_MESSAGE.data[4];		// [c]
		const uint8_t BMSfanSpeed = CAN_MESSAGE.data[5];				// 0 - 12
		const float BMSfanVoltage = CAN_MESSAGE.data[6] * VOLT_CURR_SCALE;	// [V]

		static char buf[255];
		int n =
				snprintf(buf, sizeof(buf),
						"BMS id=0x%03lX High Temperature=%d C High Termistor=%d Low Temperature=%d C Low Termistor=%d Average Temp =%d C Fan speed=%d Fan Voltage=%0.1f \r\n",
						(unsigned long) CAN_MESSAGE.id, BMShighTemperature,
						BMShighTermID, BMSlowTemperature, BMSlowTermID,
						BMSaverageTemp, BMSfanSpeed, BMSfanVoltage);
		if (n < 0)
			return;

		if (HAL_UART_Transmit_IT(&huart, (uint8_t*) buf, (uint16_t) n)
				!= HAL_OK) {

			Error_Handler();
		}
	}
}

void BMS0x102(const CANMSG_T CAN_MESSAGE) {
	if (CAN_MESSAGE.dlc < 7) {
		Error_Handler();
	}

	if (CRC_CHECKSUM(&CAN_MESSAGE) != 1) {
		Error_Handler();
	} else {
		const uint16_t BMSpackResistance = be16u(&CAN_MESSAGE.data[0]); // [mOhm]
		const float BMSmaxPackVoltage = be16u(&CAN_MESSAGE.data[2])
				* VOLT_CURR_SCALE; // V
		const uint8_t BMStotalPackCycles = CAN_MESSAGE.data[4]; // cycles
		const int8_t BMSinternalTemp = CAN_MESSAGE.data[5]; // C
		const uint8_t BMSreqFanSpeed = CAN_MESSAGE.data[6]; // 0-6

		static char buf[255];
		int n =
				snprintf(buf, sizeof(buf),
						"BMS id=0x%03lX Pack Resistance=%d Max Pack Voltage =%0.1f V mOhm Total Cycles=%d Internal Temperature=%d C Requested Fan Speed=%d \r\n",
						(unsigned long) CAN_MESSAGE.id, BMSpackResistance,
						BMSmaxPackVoltage, BMStotalPackCycles, BMSinternalTemp,
						BMSreqFanSpeed);
		if (n < 0)
			return;

		if (HAL_UART_Transmit_IT(&huart, (uint8_t*) buf, (uint16_t) n)
				!= HAL_OK) {

			Error_Handler();
		}

	}

}

