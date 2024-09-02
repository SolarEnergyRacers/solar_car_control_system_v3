# solar_car_control_system_v5

[solarenergyracers.ch](https://www.solarenergyracers.ch) - control system (two controller based control system for SER5)

![steering wheel](drafts/SteeringWheel-v0_20221016.png)

## Folder Structure

```Text
.
├── AC
│   ├── CMakeLists.txt
│   ├── dependencies.lock
│   ├── lib
│   │   ├── CarControl
│   │   │   ├── CarControlAC.cpp
│   │   │   └── CarControl.h
│   │   ├── CarState
│   │   │   ├── CarState.cpp
│   │   │   ├── CarState.h
│   │   │   ├── CarStateRadio.cpp
│   │   │   └── CarStateRadio.h
│   │   ├── CmdHandler
│   │   │   ├── CmdHandler.cpp
│   │   │   └── CmdHandler.h
│   │   ├── ConfigReader
│   │   │   ├── ConfigFile.cpp
│   │   │   ├── ConfigFile.h
│   │   │   ├── readme.md
│   │   │   └── SER4CNFG.INI
│   │   ├── Console
│   │   │   ├── Console.cpp
│   │   │   └── Console.h
│   │   ├── definitions.h
│   │   ├── Display
│   │   │   ├── Display.cpp
│   │   │   ├── Display.h
│   │   │   └── Display.md
│   │   ├── DisplayValue
│   │   │   └── DisplayValue.h
│   │   ├── DriverDisplay
│   │   │   ├── DriverDisplay.cpp
│   │   │   └── DriverDisplay.h
│   │   ├── EngineerDisplay
│   │   │   ├── EngineerDisplay.cpp
│   │   │   └── EngineerDisplay.h
│   │   ├── GPIO
│   │   │   ├── GPIO.cpp
│   │   │   └── GPIO.h
│   │   ├── interfaces
│   │   │   ├── AbstractTask.cpp
│   │   │   └── AbstractTask.h
│   │   ├── LocalFunctionsAndDevices.h
│   │   ├── OneWireBus
│   │   │   ├── OneWireBus.cpp
│   │   │   └── OneWireBus.h
│   │   ├── placeholder.txt
│   │   ├── SDCard
│   │   │   ├── SDCard.cpp
│   │   │   └── SDCard.h
│   │   └── SPIBus
│   │       ├── SPIBus.cpp
│   │       └── SPIBus.h
│   ├── platformio.ini
│   ├── sdkconfig
│   ├── sdkconfig.defaults
│   ├── sdkconfig.esp32dev
│   ├── sdkconfig.esp32dev-linux
│   ├── sdkconfig.esp32dev-windows
│   ├── ser5_v5_AC.code-workspace
│   ├── src
│   │   ├── CMakeLists.txt
│   │   └── main.cpp
│   └── upload_no_build.py
├── archive_logs.py
├── data
│   └── README.md
├── datasheets
│   ├── ads1115--ADC.pdf
│   ├── ds1307--RTC.pdf
│   ├── ds1803--DAC.pdf
│   ├── Elmar_Race--MPPT.pdf
│   ├── esp32_datasheet.pdf
│   ├── esp32_feather_mcu.pdf
│   ├── esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf
│   ├── ili9341--DISPLAY.pdf
│   ├── mcp23017_ExtIO.pdf
│   ├── Prohelion_Assembly-Procedure-CMU-Cell-Wiring.pdf
│   ├── Prohelion_-BMS-BMU-Communications-Protocol.pdf
│   ├── Prohelion_BMS.pdf
│   ├── Prohelion_BMS-Users-Manual.pdf
│   ├── Prohelion_BMU-Wiring-Diagram.pdf
│   ├── sdcard2.pdf
│   ├── sdcard.pdf
│   └── sn65hvd232_cantransceiver.pdf
├── DC
│   ├── CMakeLists.txt
│   ├── dependencies.lock
│   ├── lib
│   │   ├── ADC
│   │   │   ├── ADC_SER.cpp
│   │   │   └── ADC_SER.h
│   │   ├── CAN
│   │   │   └── CANBusHandlerDC.cpp
│   │   ├── CarControl
│   │   │   ├── CarControlDC.cpp
│   │   │   └── CarControl.h
│   │   ├── CarState
│   │   │   ├── CarState.cpp
│   │   │   └── CarState.h
│   │   ├── CmdHandler
│   │   │   ├── CmdHandler.cpp
│   │   │   └── CmdHandler.h
│   │   ├── ConstSpeed
│   │   │   ├── ConstSpeed.cpp
│   │   │   └── ConstSpeed.h
│   │   ├── DAC
│   │   │   ├── DAC.cpp
│   │   │   └── DAC.h
│   │   ├── definitions.h
│   │   ├── GPIO
│   │   │   ├── GPIO.cpp
│   │   │   └── GPIO.h
│   │   ├── IOExt
│   │   │   ├── IOExt.cpp
│   │   │   ├── IOExt.h
│   │   │   ├── IOExtHandler.cpp
│   │   │   ├── IOExtHandler.h
│   │   │   └── IOExtPins.cpp
│   │   ├── LocalFunctionsAndDevices.h
│   │   ├── OneWireBus
│   │   │   ├── OneWireBus.cpp
│   │   │   └── OneWireBus.h
│   │   └── SPIBus
│   │       ├── SPIBus.cpp
│   │       └── SPIBus.h
│   ├── platformio.ini
│   ├── sdkconfig
│   ├── sdkconfig.defaults
│   ├── sdkconfig.esp32dev
│   ├── sdkconfig.esp32dev-linux
│   ├── sdkconfig.esp32dev-windows
│   ├── ser5_v5_DC.code-workspace
│   ├── src
│   │   ├── CMakeLists.txt
│   │   └── main.cpp
│   └── upload_no_build.py
├── docs
│   ├── 2024-08-21-Note-22-19.xopp
│   ├── AC_PCB_2022-11-20.svg
│   ├── AC_Schematic_2022-11-20.svg
│   ├── assets.ser4-drivers-manual
│   │   ├── image-20230506130251806.png
│   │   ├── image-20230506130443195.png
│   │   ├── image-20230506132037797.png
│   │   └── image-20230506134445977.png
│   ├── ChangeRequests.md
│   ├── DC_PCB_2023-01-04.svg
│   ├── DC_Schematic_2022-11-20.svg
│   ├── Elmar_Solar_MPPT.pdf
│   ├── HowTos.md
│   ├── MOSFET_Schematic_2022-11-20.svg
│   ├── OberdeckConn_PCB_2023-01-04.svg
│   ├── OberdeckConn_Schematic_2023-01-04.svg
│   ├── PRH67.010v2-BMS-BMU-Communications-Protocol.pdf
│   ├── ser5-chaseCar-manual.md
│   ├── ser5-drivers-manual.md
│   ├── SignalFlows_CAN-Definitions.md
│   ├── SwitchBoard_2022-11-20.svg
│   ├── SwitchBoard_PCB_2023-01-04.svg
│   ├── SwitchBoard_Schematic_2023-01-04.svg
│   └── XBee-Locations-Thun-2023-06-11.xopp
├── drafts
│   ├── ...
├── error_report-1.txt
├── extras
│   ├── archive_logs.py
│   ├── automation
│   │   ├── captive_portal.py
        ...
│   │   └── README.md
│   ├── dev_envDCAC-new.py
│   ├── dev_envDCAC.py
│   ├── format.sh
│   └── start-vscode-for-AC-DC-workspace.py
├── fixes_for_librarys
│   ├── core.h
│   ├── fixes.md
│   └── rtc.h
├── howto_reorganize.md
├── lib_common
│   ├── CAN
│   │   ├── CANBus.cpp
│   │   ├── CANBus.h
│   │   ├── CANBusHandlerAC.cpp
│   │   ├── CANBusHandlerDC.cpp
│   │   ├── CANPacket.cpp
│   │   ├── CANPacket.h
│   │   ├── CANRxBuffer.cpp
│   │   └── CANRxBuffer.h
│   ├── Console
│   │   ├── Console.cpp
│   │   └── Console.h
│   ├── Helper
│   │   ├── Helper.cpp
│   │   └── Helper.h
│   ├── I2CBus
│   │   ├── I2CBus.cpp
│   │   └── I2CBus.h
│   ├── include
│   │   ├── CarStatePin.h
│   │   └── global_definitions.h
│   ├── interfaces
│   │   ├── AbstractTask.cpp
│   │   └── AbstractTask.h
│   ├── RTC
│   │   ├── RTC_SER.cpp
│   │   └── RTC_SER.h
│   ├── Serial
│   │   ├── Serial.cpp
│   │   └── Serial.h
│   └── System
│       ├── System.cpp
│       └── System.h
├── platformio.txt
├── README.md
├── reset_usb_numbering.py
└── start-vscode-for-AC-DC-workspace.py

```

## Draft for Electro Schema

- ![signal 2](drafts/signal-2022-10-04-140540_002.jpeg)
- ![signal 3](drafts/signal-2022-10-04-140540_003.jpeg)
- ![signal 4](drafts/signal-2022-10-04-140540_004.jpeg)
- ![signal 5](drafts/signal-2022-10-04-140540_005.jpeg)

## Documentation and Data Sheets

- [Signal Flows](docs/SignalFlows.md)
- [CAN Signal](docs/CAN_Signals.md)
- ![AC schema](docs/AC_Sheet_1_2022-11-20.svg)
- ![AC PCB](docs/AC_PCB_2022-11-20.svg)
- ![Connetor board](docs/MOSFET_Sheet_1_2022-11-20.svg)
- ![DC schema](docs/DC_Sheet_1_2022-11-20.svg)
- ![Switch Board](docs/SwitchBoard_2022-11-20.svg)

