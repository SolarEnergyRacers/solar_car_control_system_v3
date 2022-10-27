# CAN Signals

## Drive Controller
Base Address: 0x100

### CAN id: 0x00    Speed Control
Interval: ?ms
* u_16[0]     Speed ADC value
* u_8[3]      HAL-paddle Acceleration ADC value
* u_8[4]      HAL-paddle Deceleration	ADC value
* b[32]       Button Lvl Plus 
* b[33]       Button Lvl Minus
* b[34]       Button Lvl Const Mode Set
* b[35]       Button Lvl Const Mode Reset
* b[36]       Button Lvl Const Mode v/P
* b[37]       Button Lvl Brake Pedal

### CAN id: 0x01    PID 1
Interval: ?ms
* f_32[0]:    Kp
* f_32[1]:    Ki

### CAN id: 0x02    PID 2
Interval: ?ms
* f_32[2]:    Kd

## Aux. Controller
Base Address: 0x200

### CAN id: 0x00    Display Data
Interval: ?ms
* f_32[0]     Speed km/h

### CAN id: 0x01    SET PID Params 1
* f_32[0]:    Kp
* f_32[1]:    Ki

### CAN id: 0x02    SET PID Params 2
* f_32[0]:    Kd
