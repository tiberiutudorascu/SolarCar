
#ifndef INC_CANDISPATCH_H_
#define INC_CANDISPATCH_H_

#include "main.h"
#include <stdbool.h>
#include <stdio.h>

static inline int16_t be16s(const uint8_t *p) {
	return (int16_t) (((uint16_t) (p[0]) << 8) | (uint16_t) (p[1]));
}
// Functii pentru a interpreta doi octeti (big-endian) ca numar uint16_t fara semn/cu semn
static inline uint16_t be16u(const uint8_t *p) {
	return ((uint16_t) (p[0]) << 8) | (uint16_t) (p[1]);
}

void BMS0x100(const CANMSG_T CAN_MESSAGE);
bool CRC_CHECKSUM(uint8_t crc_val_b);

#endif
