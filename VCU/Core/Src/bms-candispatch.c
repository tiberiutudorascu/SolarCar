#include <bms-candispatch.h> // Includem headerul nostru corectat
#include "main.h"

SystemMode_t RUN_MODE = DEBUG_MODE; // Modify this line depending on your run mode

LimitAnalysis_t analyze_current_limit_causes(uint16_t raw_status_ccl,
		uint8_t raw_relay_state) {

	LimitAnalysis_t result;
	result.dcl_reasons = raw_status_ccl & MASK_DCL_CAUSES;
	result.is_dcl_limited = (result.dcl_reasons != 0);
	result.ccl_reasons = raw_status_ccl & MASK_CCL_CAUSES;
	result.is_ccl_limited = (result.ccl_reasons != 0);
	result.relay_reasons = raw_relay_state & RELAY_STATE_CAUSES;
	result.relay_state_fault = (result.relay_reasons != 0);

	return result;
}

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

BMS_t BMS0x100(const CANMSG_T CAN_MESSAGE) {
	BMS_t BMS0x100 = { 0 };
	BMS0x100.BMSstart = 0xAA;
	BMS0x100.BMSend = 0xFF;


	if (CAN_MESSAGE.dlc < 7) {
		Error_Handler();
		return BMS0x100;
	}
	if (CRC_CHECKSUM(&CAN_MESSAGE) != 1) {
		Error_Handler();
		return BMS0x100;
	} else {

		BMS0x100.BMSpackVoltage = be16s(&CAN_MESSAGE.data[0]); // * VOLT_CURR_SCALE; // [V]
		BMS0x100.BMSpackCurrent = be16s(&CAN_MESSAGE.data[2]); //* VOLT_CURR_SCALE; // [A]
		BMS0x100.BMSsoc = CAN_MESSAGE.data[4];
		BMS0x100.BMSsoh = CAN_MESSAGE.data[5]; // * SOH_TEMP_SCALE;   // [%]
		BMS0x100.BMSamphours = CAN_MESSAGE.data[6]; // * VOLT_CURR_SCALE;

		/* const float BMSpackVoltage = be16u(&CAN_MESSAGE.data[0])
		 * VOLT_CURR_SCALE; // [V]
		 const float BMSpackCurrent = be16u(&CAN_MESSAGE.data[2])
		 * VOLT_CURR_SCALE; // [A]
		 const float BMSsoc = CAN_MESSAGE.data[4];        // [%]
		 const float BMSsoh = CAN_MESSAGE.data[5] * SOH_TEMP_SCALE;   // [%]
		 const float BMSamphours = CAN_MESSAGE.data[6] * VOLT_CURR_SCALE;

		 static char buf[96];
		 int n = 0;
		 if (RUN_MODE == DEBUG_MODE) {
		 n =
		 snprintf(buf, sizeof(buf),
		 "BMS id=0x%03lX V=%.2fV I=%.2fA SOC=%.1f%% SOH=%.1f%% AMPH =%.1f%% \r\n",
		 (unsigned long) CAN_MESSAGE.id, BMSpackVoltage,
		 BMSpackCurrent, BMSsoc, BMSsoh, BMSamphours);
		 if (RUN_MODE == TEST_MODE_UART_1) {
		 n = snprintf(buf, sizeof(buf),
		 "0x%03lX %.2f %.2f %.1f %.1f %.1f",
		 (unsigned long) CAN_MESSAGE.id, BMSpackVoltage,
		 BMSpackCurrent, BMSsoc, BMSsoh, BMSamphours);
		 }
		 if (RUN_MODE == TEST_MODE_UART_2) {
		 n = snprintf(buf, sizeof(buf),
		 "%03lX %02X %02X %02X %02X %02X %02X %02X %02X %02X ",
		 (unsigned long) CAN_MESSAGE.id,
		 (unsigned int) CAN_MESSAGE.dlc, CAN_MESSAGE.data[0],
		 CAN_MESSAGE.data[1], CAN_MESSAGE.data[2],
		 CAN_MESSAGE.data[3], CAN_MESSAGE.data[4],
		 CAN_MESSAGE.data[5], CAN_MESSAGE.data[6],
		 CAN_MESSAGE.data[7]);
		 }
		 if (n < 0)
		 return;

		 if (HAL_UART_Transmit(&huart4, (uint8_t*) buf, (uint16_t) n,
		 HAL_MAX_DELAY) != HAL_OK) {

		 Error_Handler();
		 }
		 }*/
		return BMS0x100;
	}
}

BMS_t BMS0x101(const CANMSG_T CAN_MESSAGE) {
	BMS_t BMS0x101 = { 0 };
	BMS0x101.BMSstart = 0xAA;
	BMS0x101.BMSend = 0xFF;

	if (CAN_MESSAGE.dlc < 7) {
		Error_Handler();
		return BMS0x101;
	}

	if (CRC_CHECKSUM(&CAN_MESSAGE) != 1) {
		Error_Handler();
		return BMS0x101;
	} else {
		BMS0x101.BMShighTemperature = CAN_MESSAGE.data[0];
		;
		BMS0x101.BMShighTermID = CAN_MESSAGE.data[1];
		BMS0x101.BMSlowTemperature = CAN_MESSAGE.data[2];
		BMS0x101.BMSlowTermID = CAN_MESSAGE.data[3];
		BMS0x101.BMSaverageTemp = CAN_MESSAGE.data[4];
		BMS0x101.BMSfanSpeed = CAN_MESSAGE.data[5];
		BMS0x101.BMSfanVoltage = CAN_MESSAGE.data[6];
		/* const int8_t BMShighTemperature = CAN_MESSAGE.data[0];  	// [C]
		 const uint8_t BMShighTermID = CAN_MESSAGE.data[1];     	// ID >=1
		 const int8_t BMSlowTemperature = CAN_MESSAGE.data[2]; 		// [c]
		 const uint8_t BMSlowTermID = CAN_MESSAGE.data[3];		// ID >=1
		 const uint8_t BMSaverageTemp = CAN_MESSAGE.data[4];			// [c]
		 const uint8_t BMSfanSpeed = CAN_MESSAGE.data[5];		// 0 - 12
		 const float BMSfanVoltage = CAN_MESSAGE.data[6] * VOLT_CURR_SCALE;// [V]

		 static char buf[255];
		 int n = 0;
		 if (RUN_MODE == DEBUG_MODE) {
		 n =
		 snprintf(buf, sizeof(buf),
		 "BMS id=0x%03lX High Temperature=%d C High Termistor=%d Low Temperature=%d C Low Termistor=%d Average Temp =%d C Fan speed=%d Fan Voltage=%0.1f \r\n",
		 (unsigned long) CAN_MESSAGE.id, BMShighTemperature,
		 BMShighTermID, BMSlowTemperature, BMSlowTermID,
		 BMSaverageTemp, BMSfanSpeed, BMSfanVoltage);
		 }
		 if (RUN_MODE == TEST_MODE_UART_1) {
		 n = snprintf(buf, sizeof(buf),
		 "0x%03lX %d %d %d %d %d %d %d %0.1f ",
		 (unsigned long) CAN_MESSAGE.id, (uint8_t) CAN_MESSAGE.dlc,
		 BMShighTemperature, BMShighTermID, BMSlowTemperature,
		 BMSlowTermID, BMSaverageTemp, BMSfanSpeed, BMSfanVoltage);
		 }
		 if (RUN_MODE == TEST_MODE_UART_2) {
		 n = snprintf(buf, sizeof(buf),
		 "%03lX %02X %02X %02X %02X %02X %02X %02X %02X %02X ",
		 (unsigned long) CAN_MESSAGE.id,
		 (unsigned int) CAN_MESSAGE.dlc, CAN_MESSAGE.data[0],
		 CAN_MESSAGE.data[1], CAN_MESSAGE.data[2],
		 CAN_MESSAGE.data[3], CAN_MESSAGE.data[4],
		 CAN_MESSAGE.data[5], CAN_MESSAGE.data[6],
		 CAN_MESSAGE.data[7]);
		 }
		 if (n < 0)
		 return;

		 if (HAL_UART_Transmit(&huart4, (uint8_t*) buf, (uint16_t) n,
		 HAL_MAX_DELAY) != HAL_OK) {

		 Error_Handler();
		 } */
		return BMS0x101;

	}
}

BMS_t BMS0x102(const CANMSG_T CAN_MESSAGE) {
	BMS_t BMS0x102 = { 0 };
	BMS0x102.BMSstart = 0xAA;
	BMS0x102.BMSend = 0xFF;
	if (CAN_MESSAGE.dlc < 7) {
		Error_Handler();
		return BMS0x102;
	}

	if (CRC_CHECKSUM(&CAN_MESSAGE) != 1) {
		Error_Handler();
		return BMS0x102;
	} else {
		BMS0x102.BMSpackResistance = be16u(&CAN_MESSAGE.data[0]);
		BMS0x102.BMSmaxPackVoltage = be16s(&CAN_MESSAGE.data[2]);
		BMS0x102.BMStotalPackCycles = CAN_MESSAGE.data[4];
		BMS0x102.BMSinternalTemp = CAN_MESSAGE.data[5];
		BMS0x102.BMSreqFanSpeed = CAN_MESSAGE.data[6];
		/*
		 const uint16_t BMSpackResistance = be16u(&CAN_MESSAGE.data[0]); // [mOhm]
		 const float BMSmaxPackVoltage = be16u(&CAN_MESSAGE.data[2])
		 * VOLT_CURR_SCALE; 										// V
		 const uint8_t BMStotalPackCycles = CAN_MESSAGE.data[4];	// cycles
		 const int8_t BMSinternalTemp = CAN_MESSAGE.data[5]; 			// C
		 const uint8_t BMSreqFanSpeed = CAN_MESSAGE.data[6]; 		// 0-6
		 static char buf[255];
		 int n = 0;
		 if (RUN_MODE == DEBUG_MODE) {
		 n =
		 snprintf(buf, sizeof(buf),
		 "BMS id=0x%03lX Pack Resistance=%d Max Pack Voltage =%0.1f V mOhm Total Cycles=%d Internal Temperature=%d C Requested Fan Speed=%d \r\n",
		 (unsigned long) CAN_MESSAGE.id,
		 BMSpackResistance, BMSmaxPackVoltage,
		 BMStotalPackCycles, BMSinternalTemp,
		 BMSreqFanSpeed);

		 }
		 if (RUN_MODE == TEST_MODE_UART_1) {
		 n = snprintf(buf, sizeof(buf), "0x%03lX %d %0.1f %d %d %d",
		 (unsigned long) CAN_MESSAGE.id, BMSpackResistance,
		 BMSmaxPackVoltage, BMStotalPackCycles, BMSinternalTemp,
		 BMSreqFanSpeed);
		 }
		 if (RUN_MODE == TEST_MODE_UART_2) {
		 n = snprintf(buf, sizeof(buf),
		 "%03lX %02X %02X %02X %02X %02X %02X %02X %02X %02X ",
		 (unsigned long) CAN_MESSAGE.id,
		 (unsigned int) CAN_MESSAGE.dlc, CAN_MESSAGE.data[0],
		 CAN_MESSAGE.data[1], CAN_MESSAGE.data[2],
		 CAN_MESSAGE.data[3], CAN_MESSAGE.data[4],
		 CAN_MESSAGE.data[5], CAN_MESSAGE.data[6],
		 CAN_MESSAGE.data[7]);
		 }
		 if (n < 0)
		 return;

		 if (HAL_UART_Transmit(&huart4, (uint8_t*) buf, (uint16_t) n,
		 HAL_MAX_DELAY) != HAL_OK) {

		 Error_Handler();
		 }*/
		return BMS0x102;

	}
} /*
BMS_t BMS0x010(const CANMSG_T CAN_MESSAGE) {
	if (CAN_MESSAGE.dlc < 7) {
		Error_Handler();
	}

	if (CRC_CHECKSUM(&CAN_MESSAGE) != 1) {
		Error_Handler();
	} else {

		const uint16_t BMScclByte = le16u(&CAN_MESSAGE.data[0]);
		const uint8_t BMSrelayState = CAN_MESSAGE.data[2];
		const uint8_t BMSflag_0 = CAN_MESSAGE.data[3];
		const uint8_t BMSflag_1 = CAN_MESSAGE.data[4];
		const uint8_t BMSflag_2 = CAN_MESSAGE.data[5];
		const uint8_t BMSflag_3 = CAN_MESSAGE.data[5];

		LimitAnalysis_t status = analyze_current_limit_causes(BMScclByte,
				BMSrelayState);

		if (status.is_ccl_limited) {
			if (status.ccl_reasons & CLS_CCL_HIGH_SOC)
				Error_Handler();
			//etc etc de implementat
			// to do something

		}
		if (status.is_dcl_limited) {
			if (status.dcl_reasons & CLS_DCL_LOW_SOC)
				Error_Handler();
			//etc etc de implementat
			// to do something

		}

	}

}
*/
