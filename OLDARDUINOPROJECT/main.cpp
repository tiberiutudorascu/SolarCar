#include <Arduino.h> // Frameworkul Arduino pentru usurinta
#include <STM32_CAN.h> // Biblioteca pentru comunicarea CAN, foloseste STM32_CAN (link: https://github.com/pazi88/STM32_CAN)
#include <LoRa.h> //Biblioteca pentru LoRa

#include "headers.hpp" //Includerea fisierului de headere
//Declararea pinilor de pe placa
#define RXPIN PA11 //Pinul RX de pe STM32
#define TXPIN PA12 //Pinul TX de pe STM32

STM32_CAN CANBUS(RXPIN, TXPIN); //Definirea celor 2 pini pentru a primii mesajele
CAN_message_t tx{}, rx{}; //Definirea unui mesaj tipic
BMS myBMS(96, 3.0f, 4.2f, 150.0f, 0.5f, -20.0f, 60.0f, 70.0f, "Li-ion", 5000.0f); //Parametrii pentru BMS
ERORI bug = NICIO_EROARE; //Starea initiala de fara erori 
BMS::DATA_t BMSupdatedDATA; //Pachetul de date pentru trimiterea prin LoRa
LimitReasons CLS;

void problem(ERORI eroare); //Prototipul functiei pentru error handling

void setup() //Setup
{

  Serial.begin(115200); //Initierea comunicarii Serial
  CANBUS.enableLoopBack(true); //Activarea functiei de loopback pe CanBUS
  CANBUS.begin(); //Initirea magistralei CANBUS
  CANBUS.setBaudRate(500000); //Baudrate de 500kbps

  if (!CANBUS.setFilter(0, true, STM32_CAN::FILTER_ACTION::STORE_FIFO0)) //Verificarea functiei de filtru pe CANBUS
  {
    bug = EROARE_FILTRU; //Bug primeste EROARE_FILTRU daca functia returneaza 0
    problem(bug); //Apelarea functiei problem cu parametrul erorii de filtru
  }

  tx.id = 0x123; //Setarea unui ID oarecare pentru verificarea scrierea pe CanBUS
  tx.len = 1; //Setarea unei dimensiuni minime pentru verificarea scrierea pe CanBUS
  tx.buf[0] = 0xAA; //Setarea unui payload minim pentru verificarea scrierea pe CanBUS
  if (!CANBUS.write(tx)) //Scrierea mesajului pe CanBUS
  {
    bug = EROARE_TRIMITERE; //Daca nu a fost primit mesajul prin loopback
    problem(bug);//Apelarea functiei problem cu parametrul de EROARE_TRIMITERE
  }
  unsigned long startUpTime = millis(); //Setarea unui timer
  bool recieved = false; //Flag pentru primirea mesajului pe CanBUS
  while (millis() - startUpTime < 50) //Masurarea intervalului de 50ms // TRB FREERTOS DAR MOMENTAN NU STIU SA L FOL BINE.
  {
    if (CANBUS.read(rx)) //Verificarea primirii mesajului
    {
      recieved = true; //Flagul devine true daca mesajul s a primit
      break;
    }
  }
  if (!recieved)//Altfel acesta este fals
  {
    bug = EROARE_PRIMIRE;//Bug primeste EROARE_PRIMIRE
    problem(bug);//Apelarea functiei problem cu parametrul erorii de primire
  }
  if (bug == NICIO_EROARE) //Daca bug inca nu s a modificat, CanBUS ul functioneaza in parametrii siguri
    Serial.println("Initializarea CANBUS a fost realizata cu succes! doamne ajuta de aici"); // WOHOOO
}

void loop() //Loop
{
  if (CANBUS.read(rx)) //Daca un mesaj este citit 
  {
    switch (rx.id) //Switch case
    {
    case 0x03B: //Id 0x03B
    {
      myBMS.PACK_DATA(rx, BMSupdatedDATA); //Apelam functia PACK_DATA si updatam datele
      break;
    }
    case 0x03C: // Id 0x03C
    {
      myBMS.THERM_DATA(rx, BMSupdatedDATA); //Apelam functia THERM_DATA si updatam datele
      myBMS.FAN_DATA(rx,BMSupdatedDATA); //Apelam functia FAN_DATA si updatam datele
      break;
    }
    case 0x03D: //Id 0x03D
      myBMS.PACK_STATUS(rx,BMSupdatedDATA,CLS); //Apelam functia scalam cateva lucruri dar ne da si flaguri de eroare si de aici jonglam cu ele ce face VCU ul prin CLS
      break;
    default:
      break;
    }
  }
}
