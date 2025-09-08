#include <Arduino.h>
#include <STM32_CAN.h> //Biblioteca CAN https://github.com/pazi88/STM32_CAN
#include "headers.hpp"

void problem(ERORI eroare)
{
  if (eroare == EROARE_FILTRU)
    Serial.println("Problema la filtrul magistralei CANBUS");
  if (eroare == EROARE_TRIMITERE)
    Serial.println("Problema la trimiterea mesajului pe magistrala CANBUS");
  if (eroare == EROARE_PRIMIRE)
    Serial.println("Eroare la primirea mesajelor pe magistrala CANBUS");
}
static inline int16_t be16s(const uint8_t *p) { return int16_t((uint16_t(p[0]) << 8) | uint16_t(p[1])); }
static inline uint16_t be16u(const uint8_t *p) { return (uint16_t(p[0]) << 8) | uint16_t(p[1]); }

BMS::BMS(uint8_t numCells_param,
         float minCellVoltage_param,
         float maxCellVoltage_param,
         float maxChargeCurrent_param,
         float minChargeCurrent_param,
         float minChargeTemp_param,
         float maxChargeTemp_param,
         float maxDischargeTemp_param,
         const char *cellType_param,
         float capacityWh_param)
    : numCells(numCells_param),
      minCellVoltage(minCellVoltage_param),
      maxCellVoltage(maxCellVoltage_param),
      maxChargeCurrent(maxChargeCurrent_param),
      minChargeCurrent(minChargeCurrent_param),
      minChargeTemp(minChargeTemp_param),
      maxChargeTemp(maxChargeTemp_param),
      maxDischargeTemp(maxDischargeTemp_param),
      capacityWh(capacityWh_param)
{
  strncpy(cellType, cellType_param, sizeof(cellType) - 1);
  cellType[sizeof(cellType) - 1] = '\0'; // siguranță pentru terminator
}
uint8_t BMS::getNumCells() const
{
  return numCells;
}

float BMS::getMinCellVoltage() const
{
  return minCellVoltage;
}

float BMS::getMaxCellVoltage() const
{
  return maxCellVoltage;
}

float BMS::getMaxChargeCurrent() const
{
  return maxChargeCurrent;
}

float BMS::getMinChargeCurrent() const
{
  return minChargeCurrent;
}

float BMS::getMinChargeTemp() const
{
  return minChargeTemp;
}

float BMS::getMaxChargeTemp() const
{
  return maxChargeTemp;
}

float BMS::getMaxDischargeTemp() const
{
  return maxDischargeTemp;
}

const char *BMS::getCellType() const
{
  return cellType;
}

float BMS::getCapacityWh() const
{
  return capacityWh;
}

void BMS::PACK_DATA(const CAN_message_t &rx)
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
  const float SOC = rx.buf[4] * SOC_SCALE;                  // [%]
  const float SOH = rx.buf[5] * SOH_SCALE;                  // [%]

  // (optional) sanity checks
  // if (SOC > 100.0) { /* trateaza cazuri non-standard }

  /* exemplu de log dar probabil ar trb comparat cu
  Serial.print("V=");
  Serial.print(packVoltage, 2);
  Serial.print("V, I=");
  Serial.print(packCurrent, 2);
  Serial.print("A, SOC=");
  Serial.print(SOC, 1);
  Serial.print("%, SOH=");
  Serial.print(SOH, 1);
  Serial.println("%"); */
}