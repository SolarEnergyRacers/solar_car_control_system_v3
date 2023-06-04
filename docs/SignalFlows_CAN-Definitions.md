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

### DC - Drive Controller

DC_BASE_ADDR: **0x660**

#### CAN id: 0x00 - Speed Control

Interval: ?ms

Format | IdxFmt | Index8 | Meaning
-------|--------|--------|----------------------------------
u_16   | [0]    | 0,1    | LifeSign
u_16   | [2]    | 4,5    | Potentiometer value
u_16   | [6]    | 6      | HAL-paddle Acceleration ADC value
u_16   | [7]    | 7      | HAL-paddle Deceleration ADC value

#### CAN id: 0x01 - Speed, Acceleration, buttons

Interval: ?ms

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

~~b    | [48]   | 6      | Button Lvl Plus~~
~~b    | [49]   | 6      | Button Lvl Minus~~
~~b    | [50]   | 6      | Button Lvl Const Mode Set~~
~~b    | [51]   | 6      | Button Lvl Const Mode Reset~~
~~b    | [52]   | 6      | Button Lvl Const Mode v/P~~

#### CAN id: 0x02 - Motor, Bat, PV

Interval: ?ms

Format | IdxFmt | Index8 | Meaning
-------|--------|--------|-----------------------
u_16   | [1]    | 0,1    | Motor current
u_16   | [2]    | 2,3    | Battery voltage
u_16   | [3]    | 4,5    | PV voltage
b      | [48]   | 6      | Motor On [1] / Off [0]
b      | [49]   | 6      | Bat On [1] / Off [0]
b      | [51]   | 6      | PV On [1] / Off [0]

#### CAN id: 0x02 - PID

Interval: ?ms

Format | IdxFmt | Index8 | Meaning
-------|--------|--------|--------------------------
f_16   | [1]    | 0,1    | Kp [float as value\*1000]
f_16   | [2]    | 2,3    | Ki [float as value\*1000]
f_16   | [3]    | 4,5    | Kd [float as value\*1000]

### AC - Auxiliary Controller

AC_BASE_ADDR: **0x630**

#### CAN id: 0x00 - Display Data

Interval: ?ms

Format | IdxFmt   | Index8 | Meaning
-------|----------|--------|------------------------------------------
u_16   | [0]      | 0,1    | LifeSign
f_16   | [1]      | 2,3    | Target Power DAC value [0-65535]
f_16   | [2]      | 4,5    | Target Speed DAC value [0-65535]
~~b~~  | ~~[48]~~ | ~~6~~  | ~~Constant Mode On [0] / Off [1]~~
b      | [49]     | 6      | Constant Mode Type Speed [0] / Power [1]
~~b~~  | ~~[50]~~ | ~~6~~  | ~~Button 1~~ Next Screen
b      | [51]     | 6      | Button 2 Constant Mode toggle Power/Speed

#### CAN id: 0x01 - SET PID Params

Interval: ?ms

Format | IdxFmt | Index8 | Meaning
-------|--------|--------|-------------------------
f_16   | [0]    | 0,1    | Kp [float as value*1000]
f_16   | [1]    | 2,3    | Ki [float as value*1000]
f_16   | [2]    | 4,5    | Kd [float as value*1000]
