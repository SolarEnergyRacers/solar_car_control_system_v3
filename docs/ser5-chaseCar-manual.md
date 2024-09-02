# SER4 Chase Car Manual

[[_TOC_]]

## Preparation

## Control Elements

### Mainboard

(nothing to do by the technicians)

### Emergency Controls

### Switch Board Preparation

### Steering Wheel Preparation

### Console Commands AC

```ini
Available commands (REDPSVJMUFBICOTKi:!sc?):
-------- SYSTEM COMMANDS -------------------------
R _ _ _ _ _ _ _ _ _ _ reset and reinit driver display
E _ _ _ _ _ _ _ _ _ _ switch to engineer screen
D _ _ _ _ _ _ _ _ _ _ switch to driver display
P _ _ _ _ _ _ _ _ _ _ print directory of sdcard
S _ _ _ _ _ _ _ _ _ _ print status
  [a]                   - print status and all values
V [|+]  _ _ _ _ _ _ _ write_log to sdcard | with header (+)
M _ _ _ _ _ _ _ _ _ _ mount sdcard and eneable logging
U _ _ _ _ _ _ _ _ _ _ unmount sdcard and disable logging
H _ _ _ _ _ _ _ _ _ _ memory_info
B [v|m|<rate>]  _ _ _ _ show or set baud rate of Serial2, v-verbose mode
F _ _ _ _ _ _ _ _ _ _ Re-read configuration
I _ _ _ _ _ _ _ _ _ _ read and show IOs
  [s]                   - scan and show I2C devices
  [R]                   - reinit
C _ _ _ _ _ _ _ _ _ _ set CAN verbose mode
  [i]                   - verbose CAN in
  [I]                   - verbose CAN in native packages
  [o]                   - verbose CAN out
  [O]                   - verbose CAN out native packages
  [S]                   - verbose sd card
  motC batV             - inject motor current [A] and battery voltage [V]
O _ _ _ _ _ _ _ _ _ _ set CarControl verbose mode
  [o]                   - verbose
  [O]                   - verbose debug
T _ _ _ _ _ _ _ _ _ _ get system time
  [yyyy mm dd hh MM ss] - set RTC date and time
K [|kp ki kd] _ _ _ _ show / update PID constants
i _ _ _ _ _ _ _ _ _ _ minimal drive to console
-------- DRIVER INFO COMMANDS --------------------
:<text> _ _ _ _ _ _ _ display driver info text
!<text> _ _ _ _ _ _ _ display driver warn text
s [|+|-]  _ _ _ _ _ _ speed arrow off, green up (+), red down (-)
-------- Driver SUPPORT COMMANDS -----------------
c [-|+|s|p] _ _ _ _ _ _ constant mode on (-:off|+:on|s:speed|p:power)
```

### Console Commands DC

```ini
Available commands (SVMUICOTK:!sch?):
-------- SYSTEM COMMANDS -------------------------
S _ _ _ _ _ _ _ _ _ _ print status
  [a]                 - print status and all values
H _ _ _ _ _ _ _ _ _ _ memory_info
I _ _ _ _ _ _ _ _ _ _ read and show IOs
  [s]                 - scan and show I2C devices
  [i|o]               - show in|out continuously
  [h]                 - show in handler results
  [a|d]               - show ADC|DAC continuously
  [c]                 - show controls acc/dec continuously, breakPedal
  [R]                 - reinit
C _ _ _ _ _ _ _ _ _ _ set CAN verbose mode
  [i]                 - verbose CAN in
  [I]                 - verbose CAN in native packages
  [o]                 - verbose CAN out
  [O]                 - verbose CAN out native packages
O _ _ _ _ _ _ _ _ _ _ set CarControl verbose mode
  [o]                 - verbose
  [O]                 - verbose debug
T [yyyy mm dd hh MM]  get/set RTC date and time
K [|kp ki kd] _ _ _ _ show / update PID constants
-------- DRIVER INFO COMMANDS --------------------
:<text> _ _ _ _ _ _ _ display driver info text
!<text> _ _ _ _ _ _ _ display driver warn text
s [|+|-]  _ _ _ _ _ _ speed arrow off, green up (+), red down (-)
-------- Driver SUPPORT COMMANDS -----------------
c [-|s|p] _ _ _ _ _ _ constant mode on (-:off|s:speed|p:power)
```

#### SYSTEM COMMANDS

| command          | Function                                                                                |
| :--------------- | :-------------------------------------------------------------------------------------- |
| R                | reset and reinit driver display                                                         |
| C                | switch to console screen                                                                |
| E                | switch to engineer screen                                                               |
| D                | switch to driver display                                                                |
| T yy mm dd hh MM | set rtc date and time                                                                   |
| t                | get date and time                                                                       |
| -                | adjust paddels                                                                          |
| P                | print directory of sdcard                                                               |
| s                | print status of all values                                                              |
| S                | print status                                                                            |
| J                | write JSON status to sdcard                                                             |
| v or V           | Log state as csv to sd card, V: with header (write csv data row into `/SER4DATA.CSV`) |
| M                | mount sdcard and eneable logging                                                        |
| P                | print directory of sdcard                                                               |
| U                | unmount sdcard and disable logging                                                      |
| ~H~             | ~memory_info~                                                                          |
| k kp ki kd       | update PID constants                                                                    |
| I                | scan I2C devices                                                                        |
| i                | IO ReadAll                                                                              |
| ii               | verbose IO input view                                                                   |
| io               | verbose IO output view                                                                  |
|                  |                                                                                         |
| iR               | re-init IO                                                                              |

#### DRIVER INFO COMMANDS

| command     | Function                        |
| ----------- | ------------------------------- |
| :`<text>` | display driver info text        |
| !`<text>` | display driver warn text        |
| u [-]       | green speed up arrow (- => off) |
| d [-]       | red speed down arrow (- => off) |

### Console Procedures

Mount SD card and start logging:

- Switch to Engineer Screen with button `R1`
- plug in the SD card
- Press button `R3`

The Engineer Info at the bottom of the Engineer Screen 

## Car Start

### Boot Screen

The bootscreen with blue font on white background shows the initialization of all devices controlled by the car control system.

The last display is a 4 Seconds countdown to allow an additional view to the values.

### Adjust Paddles

During each start the paddles gets be adjusted automatically. 

> Make sure that the paddles are in the home position, i.e. the position in which the paddles are set to partially tighten and then fully release. **Never push the paddles towards the front of the vehicle!**

If you release both paddles and the `acceleration/deceleration` display doesn't show `+ 0` the adjust procedure must be repeated by restarting DC.

If the control range of the paddles is to rough then the adjust procedure must be repeated by restarting DC.

## Car Operate

### Steering Wheel Drive

### Buttons

### Paddles

### Display
