#ifndef __CANDISPATCH_H
#define __CANDISPATCH_H

    static const float VOLT_CURR_SCALE = 0.1f;     // 1 LSB = 0.1 V
    static const float SOC_SCALE = 0.5f;      // 1 LSB = 0.5 %
    static const float SOH_TEMP_SCALE = 1.0f; // 1 LSB = 1 % sau 1 grad C in unele frame-uri
    static const float FAN_V_SCALE = 0.01f;

static inline int16_t be16s(const uint8_t *p) {
	return (int16_t) (((uint16_t) (p[0]) << 8) | (uint16_t) (p[1]));
}
// Functii pentru a interpreta doi octeti (big-endian) ca numar uint16_t fara semn/cu semn
static inline uint16_t be16u(const uint8_t *p) {
	return ((uint16_t) (p[0]) << 8) | (uint16_t) (p[1]);
}

void BMS0x100(const CANMSG_T CAN_MESSAGE);

#endif
