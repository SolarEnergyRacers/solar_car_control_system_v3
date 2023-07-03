# Signals and Media in SER4 V3

Version 2023.01.01

[github: issues](https://github.com/SolarEnergyRacers/solar_car_control_system_v3/issues/1)

## Communication between Parts
### Communication AC - DC

| AC (auxiliary controller)        | Dir | Type        | DC (drive controller)         |
|----------------------------------|-----|-------------|-------------------------------|
| HAL-paddle Acceleration          | >   | analog 0-5V | carStateDC ➡ MC               |
| HAL-paddle Deceleration          | >   | analog 0-5V | carStateDC ➡ MC               |
| Button Level Plus                | >   | binary 0/1  | carStateDC                    |
| Button Level Minus               | >   | binary 0/1  | carStateDC                    |
| carStateAC -> Display Speed      | <   | CAN         | current speed ⬅ MC            |
| Console input PID parameters     | >   | CAN         | carConfigDC? carStateDC       |
| Button Set Constant Mode         | <   | CAN         | carStateDC                    |
| Button Reset Constant Mode       | <   | CAN         | carStateDC                    |
| Button Constant Mode speed/power | >   | CAN         | carStateDC                    |
| Break pedal state                | <   | CAN         | carStateDC                    |
| Step width Plus/Minus            | <   | CAN         | carStateDC ⬅ Switchboard Poti |

### Communication SwitchBoard - DC

| SwitchBoard                 | Dir | Type        | DC (drive controller) |
|-----------------------------|-----|-------------|-----------------------|
| Potentiometer (step width?) | >   | analog 0-5V | carStateDC ➡ MC       |

### Communication MC - DC

| MC (motor controller) | Dir | Type        | DC (drive controller) |
|-----------------------|-----|-------------|-----------------------|
| Acceleration          | >   | anlaog 0-5V |                       |
| Deceleration          | >   | analog 0-5V |                       |
| Speed                 | <   | analog 0-5V |                       |

>  look below for MC-CAN packets

### Communication BMS - MC

[BMS Comm Documentation](https://github.com/SolarEnergyRacers/solar_car_control_system_v3/blob/add_comm_documentation/docs/PRH67.010v2-BMS-BMU-Communications-Protocol.pdf)

| BMS (battery management system) | Dir | Type | DC (drive controller) |
|---------------------------------|-----|------|-----------------------|
| (see docs)                      | >   | CAN  |                       |

### Communication MPPT - MC
[MPPT Documentation](https://github.com/SolarEnergyRacers/solar_car_control_system_v3/blob/add_comm_documentation/docs/Elmar_Solar_MPPT.pdf)
| MPPT (maximum power point tracker) | Dir | Type | DC (drive controller) |
|------------------------------------|-----|------|-----------------------|
| (see docs)                         | >   | CAN  |                       |

### Communication AC - ChaseCar

#### Envelope, Encoding
Forward some CAN Frames, ~~encoded as ASCII chars~~
The id (11 bits) and data (64 bits) parts of the CAN frame get transmitted

The id is encoded into two chars, aligned by least significant bit.
The 5 unused bits are set to 1 to make the start of the CAN frame easier to identify.

\[ char 0 \]  \[ char 1 \]

  11111abc    defghijk

a = MSB

k = LSB

#### Data Frames

The CAN frames to be transmitted can be defined in property `radio_packages` of class `CarStateRadio`.

| AC | Dir | Type | ChaseCar |
|------------------------------------|-----|-------|-----------------------|
| MPPT Outputs (MPPT Base+1)         | >   | CAN*  |                       |
| BMS min/max Voltage (BMS Base+F8)  | >   | CAN*  |                       |
| BMS min/max Temp (BMS Base+F9)     | >   | CAN*  |                       |
| BMS voltage & current (BMS Base+FA)| >   | CAN*  |                       |
| BMS extended status (BMS Base+FD)  | >   | CAN*  |                       |
| DC Speed, Accel., buttons (0x661)  | >   | CAN*  |                       |
| AC Display Data (0x630)            | >   | CAN*  |                       |
|                                    | < > | Serial(text)| CommandHandler cmds |

##### nice-to-haves

| AC | Dir | Type | ChaseCar |
|------------------------------------|-----|-------|-----------------------|
| MPPT Inputs (MPPT Base+0)          | >   | CAN*  |                       |
| MPPT Status (MPPT Base+3)          | >   | CAN*  |                       |
| BMS cell Voltages (BMS Base+[1..]) | >   | CAN*  |                       |

\* CAN frames encoded to ASCII chars, transmitted over serial

## DC Data

- PID parameters 
- Setpoint speed
- Setpoint power
- Current speed
- Current acceleration
- Current decceleration

## AC Data

### Display

- Speed + Speed unit (km/h)
- Acceleration/Deceleration (with unit %)
- Setpoint speed (km/h) or power (kW) + Setpoint type (speed/power showed by the unit)
- Driver info lines
- Drive direction
- Light mode
- PV on/off + PV current (A)
- Battery on/off + Battery voltage (V)
- Battery max, temperature (°C)



### Direct Signals

- Locking button for left/right/hazard warn indicators 
- LEDs for left/right/hazard warn indicators 
- Horn
- HAL sensors for paddles (to DC)
- buttons for plus/minus (to DC)
- Break pedal (to DC)


## CAN Signals

### BMS _ Battery Management System

BMS_BASE_ADDR: **0x700**

### MPPT

MPPT1_BASE_ADDR: **0x600**
MPPT2_BASE_ADDR: **0x610**
MPPT3_BASE_ADDR: **0x620**

### DC - Drive Controller

DC_BASE_ADDR: **0x660**

#### CAN Address Offset: 0x00 - Speed Control

Interval: 1000ms

Format | IdxFmt | Index8 | Meaning
-------|--------|--------|----------------------------------
u_16   | [0]    | 0,1    | LifeSign
u_8   | [2]    | 2    | Kp 
u_8   | [3]    | 3      | Ki 
u_8   | [4]    | 4      | Kd 
b_33 | [33] | 5 | ConstantMode: true-SPEED, false-POWER 

#### CAN Address Offset: 0x01 - Speed, Acceleration, buttons

Interval: 1000ms

Format | IdxFmt | Index8 | Meaning
-------|--------|--------|----------------------------------------------
i_8    | [0]    | 0      | Display Acceleration
u_8    | [1]    | 1      | Display Speed
u_16   | [1]    | 2,3    | Target Speed  [float as value\*1000]
u_16   | [2]    | 4,5    | Target Power  [float as value\*1000]
u_8    | [6]    | 6      | Constant Mode OFF [0] / Speed [1] / Power [2]
b      | [56]   | 7      | Fwd [1] / Bwd [0]
b      | [57]   | 7      | Button Lvl Brake Pedal
b      | [58]   | 7      | MC Off [0] / On [1]
b      | [59]   | 7      | 
b      | [60]   | 7      | 
b      | [61]   | 7      | 
b      | [62]   | 7      | 
b      | [63]   | 7      | 

#### CAN Address Offset: 0x02 - Motor, Bat, PV

Interval: ?ms

Format | IdxFmt | Index8 | Meaning
-------|--------|--------|-----------------------
u_16   | [1]    | 0,1    | Motor current
u_16   | [2]    | 2,3    | Battery voltage
u_16   | [3]    | 4,5    | PV voltage
b      | [48]   | 6      | Motor On [1] / Off [0]
b      | [49]   | 6      | Bat On [1] / Off [0]
b      | [51]   | 6      | PV On [1] / Off [0]

### AC - Auxiliary Controller

AC_BASE_ADDR: **0x630**

#### CAN Address Offset: 0x00 - Display Data

Interval: 1000ms

Format | IdxFmt | Index8 | Meaning
-------|--------|--------|----------------------------------
u_16   | [0]    | 0,1    | LifeSign
u_8   | [2]    | 2    | Kp [int (float * 100)] 
u_8   | [3]    | 3      | Ki [int (float * 100)] 
u_8   | [4]    | 4      | Kd [int (float * 100)] 
b_33 | [33] | 5 | ConstantMode: true-SPEED, false-POWER 

### MC - Motor Controller

MC_BASE_ADDR: **0x500**

Intervall: 1000ms

MC CAN description: [github.com/vedderb/bldc/blob/master/documentation/comm_can.md](https://github.com/vedderb/bldc/blob/master/documentation/comm_can.md#status-commands)

| **Command Name** | **Command Id** | **Content** |
|--------------|------------|---------|
| CAN_PACKET_STATUS   |  9 (0x09) | ERPM, Current, Duty Cycle |
| CAN_PACKET_STATUS_2 | 14 (0x0e) | Ah Used, Ah Charged |
| CAN_PACKET_STATUS_3 | 15 (0x0f) | Wh Used, Wh Charged |
| CAN_PACKET_STATUS_4 | 16 (0x10) | Temp Fet, Temp Motor, Current In, PID position |
| CAN_PACKET_STATUS_5 | 27 (0x1b) | Tachometer, Voltage In |
| ~~CAN_PACKET_STATUS_6~~ | ~~28 (0x1c)~~ | ~~ADC1, ADC2, ADC3, PPM~~ |

The content of the status messages is encoded as follows:

**CAN_PACKET_STATUS**

| **Byte** | **Data** | **Unit** | **Scale** |
|------|------|------|-------|
| B0 - B3 | ERPM | RPM | 1 |
| B4 - B5 | Current | A | 10 |
| B6 - B7 | Duty Cycle | % / 100 | 1000 |

**CAN_PACKET_STATUS_2**

| **Byte** | **Data** | **Unit** | **Scale** |
|------|------|------|-------|
| B0 - B3 | Amp Hours | Ah | 10000 |
| B4 - B7 | Amp Hours Chg | Ah | 10000 |

**CAN_PACKET_STATUS_3**

| **Byte** | **Data** | **Unit** | **Scale** |
|------|------|------|-------|
| B0 - B3 | Watt Hours | Wh | 10000 |
| B4 - B7 | Watt Hours Chg | Wh | 10000 |

**CAN_PACKET_STATUS_4**

| **Byte** | **Data** | **Unit** | **Scale** |
|------|------|------|-------|
| B0 - B1 | Temp FET | DegC | 10 |
| B2 - B3 | Temp Motor | DegC | 10 |
| B4 - B5 | Current In | A | 10 |
| B6 - B7 | PID Pos | Deg | 50 |

**CAN_PACKET_STATUS_5**

| **Byte** | **Data** | **Unit** | **Scale** |
|------|------|------|-------|
| B0 - B3 | Tachometer | EREV | 6 |
| B4 - B5 | Volts In | V | 10 |

~~**CAN_PACKET_STATUS_6**~~

| ~~**Byte**~~ | ~~**Data**~~ | ~~**Unit**~~ | ~~**Scale**~~ |
|------|------|------|-------|
| ~~B0 - B1~~ | ~~ADC1~~ | ~~V~~ | ~~1000~~ |
| ~~B2 - B3~~ | ~~ADC2~~ | ~~V~~ | ~~1000~~ |
| ~~B4 - B5~~ | ~~ADC3~~ | ~~V~~ | ~~1000~~ |
| ~~B6 - B7~~ | ~~PPM~~ | ~~% / 100~~ | ~~1000~~ |

### Convert MC-CAN Address

```
===============
14:19:48.187 > C0-474-[I:04|117,O:00|07]=R=Id=0x 624-data: 40e0000042e2cccd -- 40 - e0 - 00 - 00 - 42 - e2 - cc - cd
---------------
14:19:48.209 > C0-474-[I:06|117,O:00|07]=R=Id=0x 950-data: 0000000000000000 -- 00 - 00 - 00 - 00 - 00 - 00 - 00 - 00
14:19:48.209 > C0-474-[I:05|117,O:00|07]=R=Id=0x e50-data: 0000000000000000 -- 00 - 00 - 00 - 00 - 00 - 00 - 00 - 00
14:19:48.209 > C0-474-[I:04|117,O:00|07]=R=Id=0x f50-data: 0000000000000000 -- 00 - 00 - 00 - 00 - 00 - 00 - 00 - 00
14:19:48.232 > C0-474-[I:03|117,O:00|07]=R=Id=0x1050-data: 7e360000fc030001 -- 7e - 36 - 00 - 00 - fc - 03 - 00 - 01
14:19:48.232 > C0-474-[I:02|117,O:00|07]=R=Id=0x1b50-data: 0000ea0301000000 -- 00 - 00 - ea - 03 - 01 - 00 - 00 - 00
---------------
14:19:48.254 > C0-474-[I:01|117,O:00|07]=R=Id=0x7f8-data: 060400010e3b0e2e -- 06 - 04 - 00 - 01 - 0e - 3b - 0e - 2e
===============

Eingestellt MC: 0x50 

1b50
 51b
       5     1    b  
11111xxx  xxxx xxxx
```