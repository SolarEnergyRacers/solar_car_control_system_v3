//
// Software Serial (Interconnection to Telemetry / PC)
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_SERIAL_H
#define SOLAR_CAR_CONTROL_SYSTEM_SERIAL_H

#include <AbstractTask.h>
#include <SoftwareSerial.h>

class Uart : public AbstractTask {
public:
  // RTOS task
  #if SERIAL_RADIO_ON
string getName() { return "SerialWire-SerialRadio"; };
#else
string getName() { return "Serial"; };
#endif
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void * pvParams);
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_SERIAL_H
