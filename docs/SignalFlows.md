# Signals and Media in SER4 V3

Version 2022.10.18

## Communication AC - DC

| AC (auxiliary controller)        | Dir  | Type        | DC (drive controller)   |
| -------------------------------- | ---- | ----------- | ----------------------- |
| HAL-paddle Acceleration          | >    | analog 0-5V | carStateDC ➡ MC         |
| HAL-paddle Deceleration          | >    | analog 0-5V | carStateDC ➡ MC         |
| Button Level Plus                | >    | binary 0/1  | carStateDC              |
| Button Level Minus               | >    | binary 0/1  | carStateDC              |
| carStateAC -> Display Speed      | <    | CAN         | current speed ⬅ MC      |
| Console input PID parameters     | >    | CAN         | carConfigDC? carStateDC |
| Button Constant Mode on/off      | >    | CAN         | carStateDC              |
| Button Constant Mode speed/power | >    | CAN         | carStateDC              |
| Break pedal state                | <    | CAN         | carStateDC              |

## Communication MC - DC

| MC (motor controller) | Dir  | Type        | DC (drive controller) |
| --------------------- | ---- | ----------- | --------------------- |
| Acceleration          | >    | anlaog 0-5V |                       |
| Deceleration          | >    | analog 0-5V |                       |
| Speed                 | <    | analog 0-5V |                       |

## Communication BMS - MC

| BMS (battery management system) | Dir  | Type | DC (drive controller) |
| ------------------------------- | ---- | ---- | --------------------- |
| (see c++)                       | >    | CAN  |                       |

## Communication MPPT - MC

| MPPT (maximum power point tracker) | Dir  | Type | DC (drive controller) |
| ---------------------------------- | ---- | ---- | --------------------- |
|                                    | >    | CAN  |                       |

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



## Direct Signals

- Locking button for left/right/hazard warn indicators 
- LEDs for left/right/hazard warn indicators 
- Horn
- HAL sensors for paddles (to DC)
- buttons for plus/minus (to DC)
- Break pedal (to DC)


https://github.com/SolarEnergyRacers/solar_car_control_system_v3/issues/1