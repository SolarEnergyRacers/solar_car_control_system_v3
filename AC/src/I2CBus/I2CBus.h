//
// I2C Bus
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_I2CBUS_H
#define SOLAR_CAR_CONTROL_SYSTEM_I2CBUS_H

#include <fmt/core.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h> // semaphore
#include <iostream>

#include <Wire.h> // Arduino I2C library

#include <definitions.h>

#include <CarState.h>
#include <Console.h>
#include <I2CBus.h>

using namespace std;

// #define _Master_ 0x00
// #define _ExtIO_ I2C_ADDRESS_MCP23017_IOExt0
// #define _DAC_ I2C_ADDRESS_DS1803
// #define _ADC_ I2C_ADDRESS_ADS1x15
// #define _RTC_ I2C_ADDRESS_DS1307

class I2CBus {
private:
  uint8_t i2cDevices = 0;
  std::map<int, bool> devices;

public:
  SemaphoreHandle_t mutex = NULL;
  string getName() { return "I2C Bus"; }
  string init();
  string re_init();
  bool i2c_available(uint8_t adr);
  uint8_t scan_i2c_devices();
  uint8_t get_number_of_i2c_devices() { return i2cDevices; }
  bool hasMaster() { return devices[_Master_]; }
  bool hasExtIO() { return devices[_ExtIO_]; }
  bool hasDAC() { return devices[_DAC_]; }
  bool hasADC() { return devices[_ADC_]; }
  bool isAC() { return !isDC(); }
  bool isDC() { return hasADC() && hasDAC() && hasExtIO(); }
  bool verboseModeI2C = false;

  enum Address {
    _Master_ = 0x00,
    _ExtIO_  = I2C_ADDRESS_MCP23017_IOExt0,
    _DAC_    = I2C_ADDRESS_DS1803,
    _ADC_    = I2C_ADDRESS_ADS1x15,
    _RTC_    = I2C_ADDRESS_DS1307
  };
};


/** 
 * @brief try to take mux within timeout. 
 * @brief **Warning: needs this.ok() to check if successful**
 */
class RAII_mux {
private:
  SemaphoreHandle_t mux;
  bool _ok;
public:
  RAII_mux(SemaphoreHandle_t mux, TickType_t timeout)
  :mux(mux) {
    _ok = xSemaphoreTake(mux, timeout);
  }
  ~RAII_mux() {xSemaphoreGive(mux);}
  bool ok() {return _ok;}
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_I2CBUS_H
