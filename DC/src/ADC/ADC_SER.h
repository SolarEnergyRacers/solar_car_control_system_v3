/*
 * Analog to Digital Converter
 */

#ifndef SOLAR_CAR_CONTROL_SYSTEM_ADC_SER_H
#define SOLAR_CAR_CONTROL_SYSTEM_ADC_SER_H

#include <string>

#include <ADS1X15.h>

#include <AbstractTask.h>
#include <definitions.h>

class ADC : public AbstractTask {

public:
  // RTOS task
  string getName(void) { return "ADC"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void *pvParams);

  // Class member and functions
private:
  ADS1115 adsDevice;

public:
  enum Pin { // high nibble: device number, low nibble: port
    // ADC0
    MOTOR_SPEED_PORT = 0x00,
    SWITCH_POTENTIOMETER_PORT = 0x01,
    STW_ACC_PORT = 0x02,
    STW_DEC_PORT = 0x03,
  };
  // ADC native values
  int16_t motor_speed = 0;
  int16_t switch_potentiometer = -1;
  int16_t stw_acc = 0;
  int16_t stw_dec = 0;

  int16_t read(Pin pin);
  float get_multiplier();
  bool verboseModeADC = false;
  bool verboseModeADCDebug = false;
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_ADC_SER_H
