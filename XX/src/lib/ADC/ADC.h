/*
 * Analog to Digital Converter
 */

#ifndef SOLAR_CAR_CONTROL_SYSTEM_ADC_H
#define SOLAR_CAR_CONTROL_SYSTEM_ADC_H

#include <string>

#include <ADS1X15.h>

#include <Abstract_task.h>
#include <definitions.h>

class ADC : public Abstract_task {

public:
  // RTOS task
  string getName(void) { return "ADC"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void);

  // Class member and functions
private:
  ADS1115 adsDevice;

public:
  enum Pin { // high nibble: device number, low nibble: port
    // ADC0
    MOTOR_SPEED_PORT = 0x00,
    Reserve_PORT = 0x01,
    STW_ACC_PORT = 0x02,
    STW_DEC_PORT = 0x03,
  };
  // ADC native values
  int16_t MOTOR_SPEED;
  int16_t RESERVE;
  int16_t STW_ACC;
  int16_t STW_DEC;

  int16_t read(Pin pin);
  float get_multiplier();
  bool verboseModeADC = false;
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_ADC_H
