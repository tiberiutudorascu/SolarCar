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

