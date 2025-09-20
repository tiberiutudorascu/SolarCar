#include <Arduino.h>
#include <STM32_CAN.h> //Biblioteca CAN https://github.com/pazi88/STM32_CAN
#include <LoRa.h>
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

uint8_t BMS::getNumCells() const { return numCells; }
float BMS::getMinCellVoltage() const { return minCellVoltage; }
float BMS::getMaxCellVoltage() const { return maxCellVoltage; }
float BMS::getMaxChargeCurrent() const { return maxChargeCurrent; }
float BMS::getMinChargeCurrent() const { return minChargeCurrent; }
float BMS::getMinChargeTemp() const { return minChargeTemp; }
float BMS::getMaxChargeTemp() const { return maxChargeTemp; }
float BMS::getMaxDischargeTemp() const { return maxDischargeTemp; }
const char *BMS::getCellType() const { return cellType; }
float BMS::getCapacityWh() const { return capacityWh; }

void BMS::PACK_DATA(const CAN_message_t &rx, DATA_t &BMSDATA)
{
  // Formatul dat de tine: 2B V + 2B I + 1B SOC + 1B SOH = 6 bytes
  if (rx.len < 7)
  {
    Serial.println("Payload prea scurt");
  }

  // Voltage (unsigned, big-endian)
  const uint16_t v_raw = be16u(&rx.buf[0]);
  // Current8 (daca e cu semn, schimba pe be16s + int16_t)
  const uint16_t i_raw = be16u(&rx.buf[2]);
  // SOC si SOH pe cate 1 byte
  const uint8_t soc_raw = rx.buf[4];
  const uint8_t soh_raw = rx.buf[5];

  BMSDATA.BMSpackVoltage = v_raw * VOLT_SCALE; // [V]
  BMSDATA.BMSpackCurrent = i_raw * CURR_SCALE; // [A]
  BMSDATA.BMSsoc = soc_raw * SOC_SCALE;        // [%]
  BMSDATA.BMSsoh = soh_raw * SOH_TEMP_SCALE;   // [%]
}
void BMS::THERMID(const CAN_message_t &rx, DATA_t &BMSDATA)
{
  if (rx.len < 7)
  {
    Serial.println("Payload prea scurt.");
  }

  BMSDATA.BMShighestTemperature = rx.buf[0] * BMS::SOH_TEMP_SCALE;
  BMSDATA.BMSlowestTemperature = rx.buf[2] * BMS::SOH_TEMP_SCALE;
  BMSDATA.BMSaverageTemperature = rx.buf[4] * BMS::SOH_TEMP_SCALE;

  if (rx.buf[1] <= BMSDATA.BMSlowestThermistorID)
    BMSDATA.BMSlowestThermistorID = rx.buf[1];
  if (rx.buf[3] >= BMSDATA.BMShighestThermistorID)
    BMSDATA.BMShighestThermistorID = rx.buf[3];
}
void BMS::FANDATA(const CAN_message_t &rx, DATA_t &BMSDATA)
{
  if (rx.len < 7)
  {
    Serial.println("Payload prea scurt.");
  }
  BMSDATA.BMSfanSpeed = rx.buf[5];
  BMSDATA.BMSfanVoltage = rx.buf[6] * BMS::FAN_V_SCALE;
}