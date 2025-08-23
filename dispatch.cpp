#include <Arduino.h>
#include <STM32_CAN.h>
#include "headers.hpp"

static inline int16_t be16s(const uint8_t *p) { return int16_t((uint16_t(p[0]) << 8) | uint16_t(p[1])); }
static inline uint16_t be16u(const uint8_t *p) { return (uint16_t(p[0]) << 8) | uint16_t(p[1]); }

constexpr double VOLT_SCALE = 0.1; // 1 LSB = 0.1 V
constexpr double CURR_SCALE = 0.1; // 1 LSB = 0.1 A
constexpr double SOC_SCALE = 0.5;  // 1 LSB = 0.5 %
constexpr double SOH_SCALE = 1.0;  // 1 LSB = 1 %

void PACK_VI_SOCH(const CAN_message_t &rx)
{
  // 2B V + 2B I + 1B SOC + 1B SOH = 6 bytes min
  if (rx.len < 6)
  {
    Serial.println("Payload prea scurt.");
    return;
  }

  // tensiunea e aproape mereu UNSIGNED
  const float packVoltage = be16u(&rx.buf[0]) * VOLT_SCALE; // [V]
  const float packCurrent = be16s(&rx.buf[2]) * CURR_SCALE; //[A] poate sa fie negativa i guess?
  const float SOC = rx.buf[4] * SOC_SCALE; // [%]
  const float SOH = rx.buf[5] * SOH_SCALE; // [%]

  // (optional) sanity checks
  // if (SOC > 100.0) { /* trateaza cazuri non-standard }

  // exemplu de log
  Serial.print("V=");
  Serial.print(packVoltage, 2);
  Serial.print("V, I=");
  Serial.print(packCurrent, 2);
  Serial.print("A, SOC=");
  Serial.print(SOC, 1);
  Serial.print("%, SOH=");
  Serial.print(SOH, 1);
  Serial.println("%");
}
