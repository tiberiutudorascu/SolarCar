#ifndef __CANDISPATCH_H
#define __CANDISPATCH_H

#include <stdbool.h> // Rezolva eroarea cu 'bool'
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx_hal.h" // Important pentru tipurile HAL

typedef struct {
	uint16_t id;
	uint8_t dlc;
	uint8_t data[8];
	uint8_t flags;
} CANMSG_T;

// Constante
static const float VOLT_CURR_SCALE = 0.1f;     // 1 LSB = 0.1 V
static const float SOC_SCALE = 0.5f;           // 1 LSB = 0.5 %
static const float SOH_TEMP_SCALE = 1.0f;      // 1 LSB = 1 % sau 1 grad C
static const float FAN_V_SCALE = 0.01f;

extern UART_HandleTypeDef huart1;

// Functii Helper
static inline int16_t be16s(const uint8_t *p) {
	return (int16_t) (((uint16_t) (p[0]) << 8) | (uint16_t) (p[1]));
}

static inline uint16_t be16u(const uint8_t *p) {
	return ((uint16_t) (p[0]) << 8) | (uint16_t) (p[1]);
}

// Prototipuri functii
bool CRC_CHECKSUM(const CANMSG_T *CAN_MESSAGE);
void BMS0x100(const CANMSG_T CAN_MESSAGE);
void BMS0x101(const CANMSG_T CAN_MESSAGE);
void BMS0x102(const CANMSG_T CAN_MESSAGE);

#endif
