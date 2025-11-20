* Repository overview
  * Name
      * SolarCar – Low Voltage CAN Stack
  * Scope
      * This repository contains the Low Voltage (LV) / CAN communication stack
        for the SolarCar project.
      * It is focused on:
        * receiving and decoding CAN messages from the BMS and other LV devices
        * dispatching data to other subsystems (PDM, dashboard, safety blocks)
        * experimenting with different boards and configurations for LV CAN

* Repository structure
  * BLUESOLARTEST_ITCAN2
      * Main STM32CubeIDE project.
      * Contains:
        * core source files (e.g. `Core/Src/main.c`, `Core/Src/candispatch.c`)
        * HAL drivers (`Drivers/STM32F1xx_HAL_Driver/...`)
        * STM32CubeIDE metadata for the project.
      * This is where active development happens for:
        * CAN and other peripherals driver configuration
        * message filtering and dispatching
        * decoding BMS frames (voltage, current, SOC, SOH, etc.)
  * OLDARDUINOPROJECT
      * Legacy project using PlatformIO + Arduino framework on STM32.
      * Kept for historical reference only:
        * early experiments with CAN
        * first versions of BMS data decoding
      * Not maintained; new work should NOT be added here.
  * Other files and folders
      * `esp32.ino`
        * ESP32 sketch that is used on an ESP32 C3-SuperMini acting as a CAN Node using a 8MHz MCP2515.
        * Should be modified only during RX/TX tests.
      * `README.md`
        * This file.
      * Future documentation files
        * `Documentation.pdf` (recommended)
        * diagrams or protocol descriptions for CAN / LV
  * The software used to create and configure the BMS messages is the official Orion BMS software