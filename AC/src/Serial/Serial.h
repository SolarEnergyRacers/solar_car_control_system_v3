//
// Software Serial (Interconnection to Telemetry / PC)
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_SERIAL_H
#define SOLAR_CAR_CONTROL_SYSTEM_SERIAL_H

#include <Abstract_task.h>
#include <SoftwareSerial.h>

class Uart : public Abstract_task {
public:
  // RTOS task
  string getName() { return "SerialWire-SerialRadio"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void);
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_SERIAL_H
