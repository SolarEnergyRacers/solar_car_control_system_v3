//
// Analog to Digital Converter
//
#include <definitions.h>

#include <fmt/core.h>
#include <fmt/printf.h>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <ADS1X15.h>
#include <Wire.h> // I2C

#include <ADC.h>
#include <CarState.h>
#include <Console.h>
#include <I2CBus.h>
//#include <DAC.h>
//#include <DriverDisplay.h>
#include <Abstract_task.h>
#include <Helper.h>

extern Console console;
extern CarState carState;
extern I2CBus i2cBus;
extern ADC adc;
extern bool adcInited;
// extern DAC dac;
// extern DriverDisplay driverDisplay;

using namespace std;

string ADC::re_init() { return init(); }

string ADC::init() {
  console << fmt::format("     Init 'ADC' with address {:#04x} ...", I2C_ADDRESS_ADS1x15);
  xSemaphoreTakeT(i2cBus.mutex);
  adsDevice = ADS1115(I2C_ADDRESS_ADS1x15);
  adcInited = adsDevice.begin();
  if (!adcInited) {
    xSemaphoreGive(i2cBus.mutex);
    console << fmt::format("\n        ERROR: Wrong ADS1x15 at address: {:#04x}.\n", I2C_ADDRESS_ADS1x15);
  } else {
    console << NL;
    // set gain amplifier value
    // 2/3x gain +/- 6.144V
    // 1 bit = 3mV (ADS1015) / 0.1875mV (ADS1115)
    adsDevice.setGain(0);
    adsDevice.setMode(1);
    adsDevice.setDataRate(3);

    // conversion factor:
    // bit-value -> mV: 2/3x gain +/- 6.144V
    // 1 bit = 3mV (ADS1015) 0.1875mV (ADS1115)
    float multiplier = adsDevice.toVoltage(1); // voltage factor
    xSemaphoreGive(i2cBus.mutex);
    console << "        Max voltage=" << adsDevice.getMaxVoltage() << " with multiplier=" << multiplier << NL;
    // read all inputs & report
    for (int i = 0; i < 4; i++) {
      xSemaphoreTakeT(i2cBus.mutex);
      int16_t value = adsDevice.readADC(i);
      xSemaphoreGive(i2cBus.mutex);
      console << "          [ADS1x15] AIN" << i << " --> " << value << ": " << multiplier * value << "mV\n";
    }
    console << fmt::format("     ok ADC at 0x{:x} inited.\n", I2C_ADDRESS_ADS1x15);
  }
  return fmt::format("[{}] ADC initialized.", adcInited ? "ok" : "--");
}

void ADC::exit(void) {
  // TODO
}

int16_t ADC::read(Pin pin) {
  if (!adcInited)
    return 0;

  xSemaphoreTakeT(i2cBus.mutex);
  int16_t value = adsDevice.readADC(pin & 0xf);
  xSemaphoreGive(i2cBus.mutex);
  return value;
}

float ADC::get_multiplier() {
  if (!adcInited)
    return 0;

  return adsDevice.toVoltage(1);
}

void ADC::task() {
  // polling loop
  while (1) {
    // ADC0
    int motor_s = read(Pin::MOTOR_SPEED_PORT);
    int potenti = read(Pin::SWITCH_POTENTIOMENTER_PORT);
    int stw_acc = read(Pin::STW_ACC_PORT);
    int stw_dec = read(Pin::STW_DEC_PORT);

    if ((abs(MOTOR_SPEED - motor_s) > 2 || abs(SWITCH_POTENTIOMENTER - potenti) > 2 || abs(STW_ACC - stw_acc) > 2 ||
         abs(STW_DEC - stw_dec) > 2)) {
      if (verboseModeADC)
        console << fmt::format("ADC: speed={:3d} | acc={:5d} | dec={:5d} | pot {:5d}\n", motor_s, stw_acc, stw_dec, potenti);

      MOTOR_SPEED = motor_s;
      SWITCH_POTENTIOMENTER = potenti;
      STW_ACC = stw_acc;
      STW_DEC = stw_dec;
    }
    // sleep
    vTaskDelay(sleep_polling_ms / portTICK_PERIOD_MS);
  }
}
