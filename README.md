# solar_car_control_system_v3

[solarenergyracers.ch](https://www.solarenergyracers.ch) - control system (two controller based control system for SER4)

## Folder Structure

```Text
.
├── AC
│   --> here comes the Auxiliary Controller
├── data
├── datasheets
├── DC
│   --> here comes the Drive Controller
├── docs
├── drafts
├── extras
│   └── automation
└── XX --> minimal development area before divideing ing in AC and DC
    ├── include
    ├── interfaces
    └── src
        └── lib
            ├── ADC
            ├── CAN
            ├── CarControl
            ├── CarState
            ├── Console
            ├── DAC
            ├── Helper
            ├── I2CBus
            └── System
```

## Draft for Electro Schema

![signal 2](drafts/signal-2022-10-04-140540_002.jpeg)
![signal 3](drafts/signal-2022-10-04-140540_003.jpeg)
![signal 4](drafts/signal-2022-10-04-140540_004.jpeg)
![signal 5](drafts/signal-2022-10-04-140540_005.jpeg)
![steering wheel](drafts/SteeringWheel-v0_20221016.png)

## Documentation and Data Sheets
### Signal Flows

- [Signal Flows](docs/SignalFlows_CAN-Definitions.md)

### Electrical schema
  ![AC schema](docs/AC_Schematic_2022-11-20.svg)
  ![AC PCB](docs/AC_PCB_2022-11-20.svg)
  ![DC schema](docs/DC_Schematic_2022-11-20.svg)
  ![DC PCB](docs/DC_PCB_2023-01-04.svg)
  ![Connetor schema](docs/OberdeckConn_Schematic_2023-01-04.svg)
  ![Connetor PCB](docs/OberdeckConn_PCB_2023-01-04.svg)
  ![Switch Board Schema](docs/SwitchBoard_Schematic_2023-01-04.svg)
  ![Switch Board PCB](docs/SwitchBoard_PCB_2023-01-04.svg)
