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

uint8_t BMS::getNumCells() const
{
  return numCells;
}
float BMS::getMinCellVoltage() const { return minCellVoltage; }
float BMS::getMaxCellVoltage() const { return maxCellVoltage; }
float BMS::getMaxChargeCurrent() const { return maxChargeCurrent; }
float BMS::getMinChargeCurrent() const { return minChargeCurrent; }
float BMS::getMinChargeTemp() const { return minChargeTemp; }
float BMS::getMaxChargeTemp() const { return maxChargeTemp; }
float BMS::getMaxDischargeTemp() const { return maxDischargeTemp; }
const char *BMS::getCellType() const { return cellType; }
float BMS::getCapacityWh() const { return capacityWh; }

BMS::DATA_t BMS::PACK_DATA(const CAN_message_t &rx)
{
    // Formatul dat de tine: 2B V + 2B I + 1B SOC + 1B SOH = 6 bytes
    if (rx.len < 6) {
        Serial.println("Payload prea scurt.");
        return BMSDATA; // returnează ultimul / default
    }

    // Voltage (unsigned, big-endian)
    const uint16_t v_raw = be16u(&rx.buf[0]);
    // Current (daca e cu semn, schimba pe be16s + int16_t)
    const uint16_t i_raw = be16u(&rx.buf[2]);
    // SOC si SOH pe câte 1 byte
    const uint8_t soc_raw = rx.buf[4];
    const uint8_t soh_raw = rx.buf[5];

    BMSDATA.BMSpackVoltage = v_raw * VOLT_SCALE;         // [V]
    BMSDATA.BMSpackCurrent = i_raw * CURR_SCALE;         // [A]
    BMSDATA.BMSsoc         = soc_raw * SOC_SCALE;        // [%]
    BMSDATA.BMSsoh         = soh_raw * SOH_TEMP_SCALE;   // [%]

    return BMSDATA; //  returneaza structul completat
}
uint8_t BMS::thermID(const CAN_message_t &rx) const
{
  if (rx.len < 8)
  {
    Serial.println("Payload prea scurt.");
    return;
  }
  if (rx.buf[1] <= BMS::lowestThemistor)
    BMS::lowestThemistor = rx.buf[1];
  if (rx.buf[3] >= BMS::highestThermistor)
    BMS::highestThermistor = rx.buf[3];
}