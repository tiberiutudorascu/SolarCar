#include <Arduino.h>
#include <STM32_CAN.h> //Biblioteca CAN https://github.com/pazi88/STM32_CAN
#include "headers.hpp"

#define RXPIN PA11
#define TXPIN PA12

STM32_CAN CANBUS(RXPIN, TXPIN);
CAN_message_t tx{}, rx{};
BMS myBMS(96, 3.0f, 4.2f, 150.0f, 0.5f, -20.0f, 60.0f, 70.0f, "Li-ion", 5000.0f);


void problem(ERORI eroare);

void setup()
{

  Serial.begin(115200);
  CANBUS.enableLoopBack(true);
  CANBUS.begin();
  CANBUS.setBaudRate(500000);

  // verific filtrul
  if (!CANBUS.setFilter(0, true, STM32_CAN::FILTER_ACTION::STORE_FIFO0))
  {
    bug = EROARE_FILTRU;
    problem(bug);
  }

  tx.id = 0x123;
  tx.len = 1;
  tx.buf[0] = 0xAA;
  if (!CANBUS.write(tx))
  {
    bug = EROARE_TRIMITERE;
    problem(bug);
  }
  unsigned long startUpTime = millis();
  bool recieved = false;
  while (millis() - startUpTime < 50) // TRB FREERTOS DAR MOMENTAN NU STIU SA L FOL BINE.
  {
    if (CANBUS.read(rx))
    {
      recieved = true;
      break;
    }
  }
  if (!recieved)
  {
    bug = EROARE_PRIMIRE;
    problem(bug);
  }
  if (bug == NICIO_EROARE)
    Serial.println("Initializarea CANBUS a fost realizata cu succes! doamne ajuta de aici");
}

void loop()
{
  if (CANBUS.read(rx))
  {
    switch (rx.id)
    {
    case 0x03B:
      myBMS.PACK_DATA(rx);
      break;

    default:
      break;
    }
  }
}
