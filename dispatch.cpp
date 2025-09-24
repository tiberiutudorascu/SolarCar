#include <Arduino.h>
#include <STM32_CAN.h> // Biblioteca pentru comunicatia CAN, foloseste STM32_CAN (link: https://github.com/pazi88/STM32_CAN)
#include <LoRa.h>      // Biblioteca LoRa pentru transmisii wireless
#include "headers.hpp" // Header 

// Functie pentru afisarea erorilor aparute pe magistrala CANBUS
void problem(ERORI eroare)
{
  if (eroare == EROARE_FILTRU)
    Serial.println("Problema la filtrul magistralei CANBUS");
  if (eroare == EROARE_TRIMITERE)
    Serial.println("Problema la trimiterea mesajului pe magistrala CANBUS");
  if (eroare == EROARE_PRIMIRE)
    Serial.println("Eroare la primirea mesajelor pe magistrala CANBUS");
}

// Functie pentru a interpreta doua octeti (big-endian) ca numar int16_t cu semn
static inline int16_t be16s(const uint8_t *p) { return int16_t((uint16_t(p[0]) << 8) | uint16_t(p[1])); }
// Functie pentru a interpreta doi octeti (big-endian) ca numar uint16_t fara semn
static inline uint16_t be16u(const uint8_t *p) { return (uint16_t(p[0]) << 8) | uint16_t(p[1]); }

// Getteri pentru parametrii BMS (returneaza valori private din clasa BMS)
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


// Functie pentru extragerea datelor de pachet (tensiune, curent, SOC, SOH) dintr un mesaj CAN
void BMS::PACK_DATA(const CAN_message_t &rx, DATA_t &BMSDATA)
{
  // Structura: 2B Voltage + 2B Current + 1B SOC + 1B SOH = minim 6 bytes
  if (rx.len < 7)
  {
    Serial.println("Payload prea scurt");
  }

  // Citire valori brute din buffer (big-endian)
  const uint16_t v_raw = be16u(&rx.buf[0]); // primii 2 octeti = Voltage
  const uint16_t i_raw = be16u(&rx.buf[2]); // urmatorii 2 octeti = Current
  const uint8_t soc_raw = rx.buf[4];        // octetul 5 = SOC
  const uint8_t soh_raw = rx.buf[5];        // octetul 6 = SOH

  // Aplicare factori de scalare pentru conversie la unitati fizice
  BMSDATA.BMSpackVoltage = v_raw * VOLT_CURR_SCALE; // [V]
  BMSDATA.BMSpackCurrent = i_raw * VOLT_CURR_SCALE; // [A]
  BMSDATA.BMSsoc = soc_raw * SOC_SCALE;        // [%]
  BMSDATA.BMSsoh = soh_raw * SOH_TEMP_SCALE;   // [%]
}

// Functie pentru procesarea datelor de temperatura dintr un mesaj CAN
void BMS::THERM_DATA(const CAN_message_t &rx, DATA_t &BMSDATA)
{
  if (rx.len < 7)
  {
    Serial.println("Payload prea scurt.");
  }

  // Temperaturi citite si scalate
  BMSDATA.BMShighestTemperature = rx.buf[0] * BMS::SOH_TEMP_SCALE;
  BMSDATA.BMSlowestTemperature = rx.buf[2] * BMS::SOH_TEMP_SCALE;
  BMSDATA.BMSaverageTemperature = rx.buf[4] * BMS::SOH_TEMP_SCALE;

  // ID-uri termistori asociate temperaturilor extreme
  if (rx.buf[1] <= BMSDATA.BMSlowestThermistorID)
    BMSDATA.BMSlowestThermistorID = rx.buf[1];
  if (rx.buf[3] >= BMSDATA.BMShighestThermistorID)
    BMSDATA.BMShighestThermistorID = rx.buf[3];
}

// Functie pentru extragerea datelor legate de ventilator (viteza, tensiune)
void BMS::FAN_DATA(const CAN_message_t &rx, DATA_t &BMSDATA)
{
  if (rx.len < 7)
  {
    Serial.println("Payload prea scurt.");
  }

  // Viteza ventilator si tensiune (cu scalare)
  BMSDATA.BMSfanSpeed = rx.buf[5];
  BMSDATA.BMSfanVoltage = rx.buf[6] * BMS::FAN_V_SCALE;
}

//Functie pentru extragerea statusului BMS ului, max voltage, amphours etc
void BMS::PACK_STATUS(const CAN_message_t &rx, DATA_t &BMSDATA, LimitReasons &CLS)
{
  if(rx.len < 7)
  {
    Serial.println("Payload prea scurt.");
 
  }
  BMSDATA.BMSMaximumPackVoltage = rx.buf[0] * BMS::VOLT_CURR_SCALE;
  BMSDATA.BMSPackResistance = rx.buf[1];
  BMSDATA.BMSInternalTemperature = be16s(&rx.buf[2]);
  uint16_t flags = be16s(&rx.buf[4]);
  BMSDATA.BMSFlags = flags;
  LimitReasons CLS = decode_limit_flags(flags);
  // Mai trebuie si ampHours da nu inteleg exact cum o fac

}