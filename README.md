# SolarCar 

Salut, asta e repo-ul pe care lucram acum pentru partea de CAN pe low voltage.  
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

## Ce merge acum
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
  may god have mercy
  Linkuri de ajutor moral si tehnic: https://www.orionbms.com/downloads/misc/editing_canbus_messages.pdf
  Biblia cu toate datele :           https://www.orionbms.com/manuals/utility/
