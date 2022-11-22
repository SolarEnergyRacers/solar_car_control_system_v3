//
// Digital to Analog Converter
//

#include <definitions.h>

#include <fmt/core.h>
#include <fmt/printf.h>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Arduino.h>
#include <Wire.h> // I2C

#include <lib/CarState/CarState.h>
#include <lib/Console/Console.h>
#include <lib/DAC/DAC.h>
//#include <DriverDisplay.h>
#include <lib/Helper/Helper.h>
#include <lib/I2CBus/I2CBus.h>
//#include <SPI.h>

#define BASE_ADDR_CMD 0xA8

extern CarState carState;
// extern DriverDisplay driverDisplay;
extern I2CBus i2c;
extern Console console;
extern bool dacInited;

string DAC::re_init() {
  // reset_and_lock_pot();
  reset_pot();
  return "";
}

string DAC::init() {
  console << "[  ] Init 'DAC'...\n";
  dacInited = reset_pot();
  console << fmt::format("     DAC initialisation with I2C_ADDRESS_DS1803={:02x} {}.\n", I2C_ADDRESS_DS1803,
                         dacInited ? "successful" : "failed");
  return fmt::format("[{}] DAC initialized.", dacInited ? "ok" : "--");
}

uint8_t DAC::get_cmd(pot_chan channel) {
  uint8_t command = BASE_ADDR_CMD;
  switch (channel) {
  case POT_CHAN0:
    command |= 0x1;
    break;
  case POT_CHAN1:
    command |= 0x2;
    break;
  case POT_CHAN_ALL:
    command |= 0x3;
    break;
  default:
    command |= 0x1;
    break;
  }
  return command;
}

void DAC::reset_and_lock_pot() {
  lock_acceleration();
  reset_pot();
}

void DAC::lock_acceleration() {
  // #SAFETY#: acceleration lock
  isLocked = true;
  carState.AccelerationLocked = true;
};

bool DAC::reset_pot() {
  bool success = false;
  uint8_t command = get_cmd(POT_CHAN_ALL);
  //#SAFTY#
  xSemaphoreTakeT(i2c.mutex);
  try {
    Wire.beginTransmission(I2C_ADDRESS_DS1803);
    if (Wire.write(command) > 0) {
      success = true;
    }
    if (Wire.write(0) > 0) {
      success = true;
    } // first pot value
    if (Wire.write(0) > 0) {
      success = true;
    } // second pot value
    Wire.endTransmission();
  } catch (exception &ex) {
    success = false;
  }
  xSemaphoreGive(i2c.mutex);
  return success;
}

bool DAC::set_pot(uint8_t val, pot_chan channel) {
  if (!dacInited)
    return false;
  bool success = true;
  // #SAFETY#: acceleration lock
  if (isLocked) {
    if (!carState.PaddlesAdjusted || carState.AccelerationDisplay != 0)
      return false;

    // release unlock state and take over into to car state
    unlock_acceleration();
    carState.AccelerationLocked = false;
    string s = "DAC unlocked.\n";
    console << s;
    // if (driverDisplay.get_DisplayStatus() == DISPLAY_STATUS::DRIVER_RUNNING) {
    //   driverDisplay.print(s.c_str());
    // }
  }
  // setup command
  uint8_t command = get_cmd(channel);
  uint8_t oldValue = get_pot(channel);
  if (oldValue != val) {
    try {
      xSemaphoreTakeT(i2c.mutex);
      Wire.beginTransmission(I2C_ADDRESS_DS1803);
      if (Wire.write(command) == 0) {
        success = false;
      }
      if (Wire.write(val) == 0) {
        success = false;
      } // first pot value
      if (channel == POT_CHAN_ALL) {
        if (Wire.write(val) == 0) {
          success = false;
        } // second pot value
      }
      Wire.endTransmission();
    } catch (exception &ex) {
      success = false;
    }
    xSemaphoreGive(i2c.mutex);
    if (verboseModeDAC) {
      console << fmt::format("dac:    {:02x}-chn | {:5d}-val | {:5d}-acc | {:5d}-dec  | {:5d}-accD | {}-lck\n", channel, val,
                             carState.Acceleration, carState.Deceleration, carState.AccelerationDisplay, carState.AccelerationLocked);
    }
  }
  return success;
}

uint16_t DAC::get_pot(pot_chan channel) {
  if (!dacInited)
    return 0;
  uint8_t pot0 = 0; // get pot0
  uint8_t pot1 = 0; // get pot1
  xSemaphoreTakeT(i2c.mutex);
  Wire.requestFrom(I2C_ADDRESS_DS1803, 2); // request 2 bytes
  pot0 = Wire.read();
  pot1 = Wire.read();
  xSemaphoreGive(i2c.mutex);

  if (channel == POT_CHAN_ALL) {
    return pot0 | (pot1 << 8);
  } else if (channel == POT_CHAN0) {
    return pot0;
  } else { // POT_CHAN1
    return pot1;
  }
}
