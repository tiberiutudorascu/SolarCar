# SolarCar 

Salut, asta e repo-ul pe care lucrez acum pentru partea de CAN pe low voltage.  
Momentan am facut doar partea de baza ca sa putem citi mesaje de la BMS.
Se downloadeaza Platformio (extensie pe VSCode) -> New Project -> Se adauga in platformio.ini urmatoarea chestie:
```
[env:disco_l4s5i_iot01a]
platform = ststm32
board = disco_l4s5i_iot01a
framework = arduino

lib_deps =
  https://github.com/pazi88/STM32_CAN

build_flags = 
  -DHAL_CAN_MODULE_ENABLED

monitor_speed = 115200
```
Biblioteca CAN https://github.com/pazi88/STM32_CAN in caz ca aveti nevoie de resurse / clasa e in proiect

## De ce fac asta de acum?
BMS-ul are o gramada de semnale si ideea e sa il configuram astfel incat sa ne dea doar ce ne trebuie pentru masina, nu tot ce are el.
<img width="3480" height="1405" alt="image" src="https://github.com/user-attachments/assets/5067c58c-e3ee-4f24-b65f-9255a7376679" />
Nu am gasit mesaje standardizate asa ca momentan, o sa imi inchipui ca o sa configuram noi BMS ul incat sa ne dea doar strictul necesar. more research to come
In plus, pe langa BMS vor mai fi integrate si alte lucruri de safety si control: butoanele, pedalele, masurarea vitezei, PDM-ul si dashboard-ul. Toate astea trebuie sa vorbeasca intre ele prin CAN. 


## Ce merge acum (IN TEORIE)
- Initializam magistrala CAN pe STM32 si am testat ca functioneaza in loopback  
- Avem tratament de erori la filtru, transmitere si primire  
- Citim mesajul cu ID 0x200 de la BMS si il decodam:
  - tensiunea pachetului (0.1 V pe LSB)  
  - curentul pachetului (0.1 A pe LSB, cu semn)  
  - SOC (0.5% pe LSB)  
  - SOH (1% pe LSB)  
- Valorile decodate le scoatem pe Serial ca sa le vedem usor  

## Ce urmeaza
- Sa adaugam suport si pentru alte mesaje CAN  
- Sa separam partea de parsare de partea de afisare ca sa putem salva/loga mai usor  
- Sa documentam clar structura mesajelor in repo
- Efectiv tot restul softwareului ca asta nu e nici 1%
  may god have mercy
  Linkuri de ajutor moral si tehnic: https://www.orionbms.com/downloads/misc/editing_canbus_messages.pdf
  Biblia cu toate datele :           https://www.orionbms.com/manuals/utility/
