#ifndef __BMSCANDISPATCH_H
#define __BMSCANDISPATCH_H

#include <stdbool.h> // Rezolva eroarea cu 'bool'
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx_hal.h" // Important pentru tipurile HAL

#define BMS_CF0_INTERNAL_COMM_FAULT      (1u << 0)  // Bit 0: Internal Communication Fault
#define BMS_CF0_INTERNAL_CONV_FAULT      (1u << 1)  // Bit 1: Internal Conversion Fault
#define BMS_CF0_WEAK_CELL_FAULT          (1u << 2)  // Bit 2: Weak Cell Fault
#define BMS_CF0_LOW_CELL_VOLT_FAULT      (1u << 3)  // Bit 3: Low Cell Voltage Fault
#define BMS_CF0_OPEN_CELL_VOLT_FAULT     (1u << 4)  // Bit 4: Open Cell Voltage Fault
#define BMS_CF0_CURRENT_SENSOR_FAULT     (1u << 5)  // Bit 5: Current Sensor Fault
#define BMS_CF0_PACK_VOLT_SENSOR_FAULT   (1u << 6)  // Bit 6: Pack Voltage Sensor Fault
#define BMS_CF0_VOLT_REDUNDANCY_FAULT    (1u << 7)  // Bit 7: Voltage Redundancy Fault

#define BMS_CF0_ALL_MASK ( \
    BMS_CF0_INTERNAL_COMM_FAULT    | \
    BMS_CF0_INTERNAL_CONV_FAULT    | \
    BMS_CF0_WEAK_CELL_FAULT        | \
    BMS_CF0_LOW_CELL_VOLT_FAULT    | \
    BMS_CF0_OPEN_CELL_VOLT_FAULT   | \
    BMS_CF0_CURRENT_SENSOR_FAULT   | \
    BMS_CF0_PACK_VOLT_SENSOR_FAULT | \
    BMS_CF0_VOLT_REDUNDANCY_FAULT    \
)

// byte-ul "Custom Flag 1"

#define BMS_CF1_WEAK_PACK_FAULT          (1u << 0)  // Bit 0 (0x01): Weak Pack Fault
#define BMS_CF1_FAN_MONITOR_FAULT        (1u << 1)  // Bit 1 (0x02): Fan Monitor Fault
#define BMS_CF1_THERMISTOR_FAULT         (1u << 2)  // Bit 2 (0x04): Thermistor Fault
#define BMS_CF1_COMMUNICATION_FAULT      (1u << 3)  // Bit 3 (0x08): Communication Fault
#define BMS_CF1_ALWAYS_ON_SUPPLY_FAULT   (1u << 4)  // Bit 4 (0x10): Always-On Supply Fault
#define BMS_CF1_HV_ISOLATION_FAULT       (1u << 5)  // Bit 5 (0x20): High Voltage Isolation Fault
#define BMS_CF1_12V_SUPPLY_FAULT         (1u << 6)  // Bit 6 (0x40): 12V Power Supply Fault
#define BMS_CF1_CHARGE_ENABLE_RELAY_FAULT (1u << 7) // Bit 7 (0x80): Charge-Enable Relay Fault

#define BMS_CF1_ALL_MASK ( \
    BMS_CF1_WEAK_PACK_FAULT           | \
    BMS_CF1_FAN_MONITOR_FAULT         | \
    BMS_CF1_THERMISTOR_FAULT          | \
    BMS_CF1_COMMUNICATION_FAULT       | \
    BMS_CF1_ALWAYS_ON_SUPPLY_FAULT    | \
    BMS_CF1_HV_ISOLATION_FAULT        | \
    BMS_CF1_12V_SUPPLY_FAULT          | \
    BMS_CF1_CHARGE_ENABLE_RELAY_FAULT   \
)

// byte-ul "Custom Flag 2"

#define BMS_CF2_DISCHARGE_RELAY_FAULT    (1u << 0)  // Bit 0 (0x01): Discharge-Enable Relay Fault
#define BMS_CF2_CHARGER_SAFETY_RELAY_FAULT (1u << 1) // Bit 1 (0x02): Charger Safety Relay Fault
#define BMS_CF2_INTERNAL_MEMORY_FAULT    (1u << 2)  // Bit 2 (0x04): Internal Memory Fault
#define BMS_CF2_INTERNAL_THERMISTOR_FAULT (1u << 3) // Bit 3 (0x08): Internal Thermistor Fault
#define BMS_CF2_INTERNAL_LOGIC_FAULT     (1u << 4)  // Bit 4 (0x10): Internal Logic Fault
#define BMS_CF2_BALANCING_ACTIVE         (1u << 5)  // Bit 5 (0x20): Balancing Active
#define BMS_CF2_MP_OUTPUT_ACTIVE         (1u << 6)  // Bit 6 (0x40): Multi-Purpose Output
#define BMS_CF2_RELAY_FAILSAFE           (1u << 7)  // Bit 7 (0x80): Relay Failsafe

#define BMS_CF2_ALL_MASK ( \
    BMS_CF2_DISCHARGE_RELAY_FAULT      | \
    BMS_CF2_CHARGER_SAFETY_RELAY_FAULT | \
    BMS_CF2_INTERNAL_MEMORY_FAULT      | \
    BMS_CF2_INTERNAL_THERMISTOR_FAULT  | \
    BMS_CF2_INTERNAL_LOGIC_FAULT       | \
    BMS_CF2_BALANCING_ACTIVE           | \
    BMS_CF2_MP_OUTPUT_ACTIVE           | \
    BMS_CF2_RELAY_FAILSAFE               \
)

// byte-ul "Custom Flag 3"

#define BMS_CF3_CHARGE_POWER_SIGNAL      (1u << 0)  // Bit 0 (0x01): Charge Power Signal
#define BMS_CF3_DEPLETED                 (1u << 1)  // Bit 1 (0x02): Depleted
#define BMS_CF3_MODE_DENIED              (1u << 2)  // Bit 2 (0x04): Mode Denied
#define BMS_CF3_CHARGE_INTERLOCK         (1u << 3)  // Bit 3 (0x08): Charge Interlock
#define BMS_CF3_BAT_MODE_HIGH            (1u << 4)  // Bit 4 (0x10): Battery Mode High
#define BMS_CF3_BAT_MODE_MID             (1u << 5)  // Bit 5 (0x20): Battery Mode Mid
#define BMS_CF3_BAT_MODE_LOW             (1u << 6)  // Bit 6 (0x40): Battery Mode Low
#define BMS_CF3_CURRENT_FAILSAFE         (1u << 7)  // Bit 7 (0x80): Current Failsafe

#define BMS_CF3_ALL_MASK ( \
    BMS_CF3_CHARGE_POWER_SIGNAL        | \
    BMS_CF3_DEPLETED                   | \
    BMS_CF3_MODE_DENIED                | \
    BMS_CF3_CHARGE_INTERLOCK           | \
    BMS_CF3_BAT_MODE_HIGH              | \
    BMS_CF3_BAT_MODE_MID               | \
    BMS_CF3_BAT_MODE_LOW               | \
    BMS_CF3_CURRENT_FAILSAFE             \
)

#define CLS_DCL_LOW_SOC                     (1u << 0)  // Bit 0: DCL Reduced Due To Low SOC
#define CLS_DCL_HIGH_CELL_RESISTANCE        (1u << 1)  // Bit 1: DCL Reduced Due To High Cell Resistance
#define CLS_DCL_TEMPERATURE                 (1u << 2)  // Bit 2: DCL Reduced Due To Temperature
#define CLS_DCL_LOW_CELL_VOLTAGE            (1u << 3)  // Bit 3: DCL Reduced Due To Low Cell Voltage
#define CLS_DCL_LOW_PACK_VOLTAGE            (1u << 4)  // Bit 4: DCL Reduced Due To Low Pack Voltage
#define CLS_DCLCCL_VOLTAGE_FAILSAFE         (1u << 6)  // Bit 6: DCL and CCL Reduced Due To Voltage Failsafe
//#define CLS_DCLCCL_COMM_FAILSAFE            (1u << 7)  // Bit 7: DCL and CCL Reduced Due To Communication Failsafe
#define CLS_CCL_HIGH_SOC                    (1u << 9)   // Bit 9:  CCL Reduced Due To High SOC
#define CLS_CCL_HIGH_CELL_RESISTANCE        (1u << 10)  // Bit 10: CCL Reduced Due To High Cell Resistance
#define CLS_CCL_TEMPERATURE                 (1u << 11)  // Bit 11: CCL Reduced Due To Temperature
#define CLS_CCL_HIGH_CELL_VOLTAGE           (1u << 12)  // Bit 12: CCL Reduced Due To High Cell Voltage
#define CLS_CCL_HIGH_PACK_VOLTAGE           (1u << 13)  // Bit 13: CCL Reduced Due To High Pack Voltage
#define CLS_CCL_CHARGER_LATCH               (1u << 14)  // Bit 14: CCL Reduced Due To Charger Latch
#define CLS_CCL_ALT_CURRENT_LIMIT_MPI       (1u << 15)  // Bit 15: CCL Reduced Due To Alternate Current Limit [MPI]
#define MASK_DCL_CAUSES ( \
    CLS_DCL_LOW_SOC | CLS_DCL_HIGH_CELL_RESISTANCE | CLS_DCL_TEMPERATURE | \
    CLS_DCL_LOW_CELL_VOLTAGE | CLS_DCL_LOW_PACK_VOLTAGE | CLS_DCLCCL_VOLTAGE_FAILSAFE )

#define MASK_CCL_CAUSES ( \
    CLS_DCLCCL_VOLTAGE_FAILSAFE | CLS_CCL_HIGH_SOC | CLS_CCL_HIGH_CELL_RESISTANCE | \
    CLS_CCL_TEMPERATURE | CLS_CCL_HIGH_CELL_VOLTAGE | CLS_CCL_HIGH_PACK_VOLTAGE | \
    CLS_CCL_CHARGER_LATCH | CLS_CCL_ALT_CURRENT_LIMIT_MPI )



#define RELAY_STATE_DISCHARGE_ENABLED        (1u << 0)  // Bit 1: Discharge relay enabled
#define RELAY_STATE_CHARGE_ENABLED           (1u << 1)  // Bit 2: Charge relay enabled
#define RELAY_STATE_CHARGER_SAFETY_ENABLED   (1u << 2)  // Bit 3: Charger safety enabled
#define RELAY_STATE_MIL_ACTIVE               (1u << 3)  // Bit 4: Malfunction indicator active (DTC status)
#define RELAY_STATE_MP_INPUT_ACTIVE          (1u << 4)  // Bit 5: Multi-Purpose Input signal status
#define RELAY_STATE_ALWAYS_ON_ACTIVE         (1u << 5)  // Bit 6: Always-on signal status
#define RELAY_STATE_IS_READY                 (1u << 6)  // Bit 7: Is-Ready signal status
#define RELAY_STATE_IS_CHARGING              (1u << 7)  // Bit 8: Is-Charging signal status
#define RELAY_STATE_CAUSES (RELAY_STATE_DISCHARGE_ENABLED | RELAY_STATE_CHARGE_ENABLED | RELAY_STATE_CHARGER_SAFETY_ENABLED | \
RELAY_STATE_MIL_ACTIVE | RELAY_STATE_MP_INPUT_ACTIVE | RELAY_STATE_ALWAYS_ON_ACTIVE | RELAY_STATE_IS_READY | RELAY_STATE_IS_CHARGING)

typedef struct {
	bool is_dcl_limited;
	bool is_ccl_limited;
	bool relay_state_fault;

	uint16_t dcl_reasons;
	uint16_t ccl_reasons;
	uint8_t relay_reasons;
	uint8_t flag_0;
	uint8_t flag_1;
	uint8_t flag_2;

} LimitAnalysis_t;

typedef struct {
	uint16_t id;
	uint8_t dlc;
	uint8_t data[8];
	uint8_t flags;
} CANMSG_T;

typedef enum {
	DEBUG_MODE, TEST_MODE_UART_1, TEST_MODE_UART_2
} SystemMode_t;


typedef struct __attribute__((packed)){
	int8_t BMSstart;
	int16_t BMSpackVoltage;
	int16_t BMSpackCurrent;
	uint8_t BMSsoc;
	uint8_t BMSsoh;
	uint8_t BMSamphours;

	int8_t BMShighTemperature;
	int8_t BMShighTermID;
	int8_t BMSlowTemperature;
	uint8_t BMSlowTermID;
	uint8_t BMSaverageTemp;
	int8_t BMSfanSpeed;
	int8_t BMSfanVoltage;

	uint16_t BMSpackResistance;
	int16_t BMSmaxPackVoltage;
	uint8_t BMStotalPackCycles;
	int8_t  BMSinternalTemp;
	uint8_t BMSreqFanSpeed;
	int8_t BMSend;


}BMS_t;
// Constante
static const float VOLT_CURR_SCALE = 0.1f;     // 1 LSB = 0.1 V
static const float SOC_SCALE = 0.5f;           // 1 LSB = 0.5 %
static const float SOH_TEMP_SCALE = 1.0f;      // 1 LSB = 1 % sau 1 grad C
static const float FAN_V_SCALE = 0.01f;


// Functii Helper
static inline int16_t be16s(const uint8_t *p) {
	return (int16_t) (((uint16_t) (p[0]) << 8) | (uint16_t) (p[1]));
}

static inline uint16_t be16u(const uint8_t *p) {
	return ((uint16_t) (p[0]) << 8) | (uint16_t) (p[1]);
}

static inline int16_t le16s(const uint8_t *p) {
	return (int16_t) (((uint16_t) (p[1]) << 8) | (uint16_t) (p[0]));
}
// Functii pentru a interpreta doi octeti (little-endian) ca numar uint16_t fara semn/cu semn
static inline uint16_t le16u(const uint8_t *p) {
	return ((uint16_t) (p[1]) << 8) | (uint16_t) (p[0]);
}


// Prototipuri functii
LimitAnalysis_t analyze_current_limit_causes(uint16_t raw_status_ccl,
		uint8_t raw_relay_state);
bool CRC_CHECKSUM(const CANMSG_T *CAN_MESSAGE);
BMS_t BMS0x100(const CANMSG_T CAN_MESSAGE);
BMS_t BMS0x101(const CANMSG_T CAN_MESSAGE);
BMS_t BMS0x102(const CANMSG_T CAN_MESSAGE);
BMS_t BMS0x010(const CANMSG_T CAN_MESSAGE);



#endif
