//
// I2C Bus
//
#include <definitions.h>

#include <fmt/core.h>
#include <iostream>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h> // semaphore

#include <Wire.h> // Arduino I2C library

#include <lib/CarState/CarState.h>
#include <lib/Console/Console.h>
#include <lib/I2CBus/I2CBus.h>

extern Console console;
extern CarState carState;

using namespace std;

string I2CBus::re_init() { return init(); }

string I2CBus::init(void) {
  bool hasError = false;
  console << "[  ] Init 'I2CBus'...\n";

  // devices["Mst"] = "0x00 | ESP32 I2C master                | AC + DC";
  // devices["Ext"] = "0x20 | MCP23017, Extended digital IOs  | DC";
  // devices["DAC"] = "0x28 | DS1803, digital analog coder    | DC";
  // devices["ADC"] = "0x48 | ADS1115, analog digital coder   | DC";
  // devices["RTC"] = "0x68 | DS1307, real time clock         | AC";

  mutex = xSemaphoreCreateMutex();
  // init i2c wire library
  uint32_t frequency = I2C_FREQ;           // make to int to find the correct signature
  Wire.begin(I2C_SDA, I2C_SCL, frequency); // frequency in kHz
  xSemaphoreGive(mutex);
  console << "     I2C inited: I2C_SDA=" << I2C_SDA << ", I2C_SCL=" << I2C_SCL << ", I2C_FREQ=" << I2C_FREQ << ".\n";
  scan_i2c_devices();
  return fmt::format("[{}] I2CBus initialized.", hasError ? "--" : "ok");
}

// test if the i2c bus is available and ready for transaction at address adr
bool I2CBus::i2c_available(uint8_t adr) {
  uint32_t timeout = millis();
  bool ready = false;

  xSemaphoreTake(mutex, portMAX_DELAY);
  while ((millis() - timeout < 100) && (!ready)) {
    Wire.beginTransmission(adr);
    ready = (Wire.endTransmission() == 0);
  }
  xSemaphoreGive(mutex);

  return ready;
}

uint8_t I2CBus::scan_i2c_devices() {
  /* I2C slave Address Scanner
   * for 5V bus
   * Connect a 4.7k resistor between SDA and Vcc
   * Connect a 4.7k resistor between SCL and Vcc
   * for 3.3V bus
   * Connect a 2.4k resistor between SDA and Vcc
   * Connect a 2.4k resistor between SCL and Vcc
   */
  console << "     Scanning I2C addresses:\n     ";
  i2cDevices = 0;
  string msg;

  for (uint8_t addr = 0; addr < 0x80; addr++) {
    uint8_t ec = -1;
    xSemaphoreTake(mutex, portMAX_DELAY);
    try {
      Wire.beginTransmission(addr);
      ec = Wire.endTransmission(true);
    } catch (exception &ex) {
      // do nothing
    }
    xSemaphoreGive(mutex);
    if (ec == 0) {
      msg = fmt::format("{:02x} ", addr);
      devices[addr] = true;
      i2cDevices++;
    } else {
      msg = "-- ";
    }
    if ((addr & 0x0f) == 0x0f) {
      msg.append("\n     ");
    }
    console << msg;
  }

  console << "Scan completed: " << fmt::format("{}", i2cDevices) << " I2C devices found.\n";

  msg = "     Expected addresses:\n";
  msg += "     Address | Device                               | Location      \n";
  msg += "     ------- | ------------------------------------ | --------------\n";
  msg += "      0x00   | ESP32 I2C master                     | AC + DC    \n";
  msg += "      0x20   | MCP23017, Extended digital IOs       | DC    \n";
  msg += "      0x28   | DS1803, digital analog coder         | DC    \n";
  msg += "      0x48   | ADS1115, analog digital coder        | DC    \n";
  msg += "      0x68   | DS1307, real time clock              | AC    \n";
  // msg += "      0x19   | BMI088, 6-axis inertial sensor, acc  | main board    \n";
  // msg += "      0x21   | MCP23017, Extended digital IOs       | steering wheel\n";
  // msg += "      0x49   | ADS1115, analog digital coder        | main board    \n";
  // msg += "      0x4a   | ADS1115, analog digital coder        | steering wheel\n";
  // msg += "      0x69   | BMI088, 6-axis inertial sensor, gyro | main board    \n";
  console << msg;
  return i2cDevices;
}
