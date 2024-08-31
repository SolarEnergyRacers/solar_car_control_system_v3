//
// Analog to Digital Converter
//
#include "../definitions.h"
#include <global_definitions.h>

#include <fmt/core.h>
#include <fmt/printf.h>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <ADS1X15.h>
#include <Wire.h> // I2C

#include <ADC_SER.h>
#include <AbstractTask.h>
#include <CarState.h>
#include <Console.h>
#include <Helper.h>
#include <I2CBus.h>

extern Console console;
extern CarState carState;
extern I2CBus i2cBus;
extern ADC adc;
extern bool adcInited;
extern bool SystemInited;

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
    // do not adjust in case of flying restart! int mot = read(Pin::MOTOR_SPEED_PORT);
    // do not adjust: potentiometer static set! int pot = read(Pin::SWITCH_POTENTIOMETER_PORT);
    carState.StartOffset_acc = read(Pin::STW_ACC_PORT) + carState.PaddleDamping;
    carState.StartOffset_dec = read(Pin::STW_DEC_PORT) + carState.PaddleDamping;
    
    console << fmt::format("       startOffset_acc={}", carState.StartOffset_acc) << NL;
    console << fmt::format("       startOffset_dec={}", carState.StartOffset_dec) << NL;
    console << fmt::format("     ADC at 0x{:x} inited.", I2C_ADDRESS_ADS1x15) << NL;
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
  return value < 0 ? 0 : value;
}

float ADC::get_multiplier() {
  if (!adcInited)
    return 0;

  return adsDevice.toVoltage(1);
}

void ADC::task(void *pvParams) {
  while (1) {
    if (SystemInited) {
      // ADC0
      int mot = read(Pin::MOTOR_SPEED_PORT);
      int pot = read(Pin::SWITCH_POTENTIOMETER_PORT);
      int acc = read(Pin::STW_ACC_PORT) - carState.StartOffset_acc;
      int dec = read(Pin::STW_DEC_PORT) - carState.StartOffset_dec;
      if(acc < 0) acc = 0;
      if(dec < 0) dec = 0;
      if (abs(motor_speed - mot) > 2 || abs(switch_potentiometer - pot) > 2 || abs(stw_acc - acc) > 1 || abs(stw_dec - dec) > 1) {
        if (adc.verboseModeADC)
          console << fmt::format("ADC: speed={:6d} | dec={:6d} | acc={:6d} | poti= {:6d}\n", mot, dec, acc, pot);

        motor_speed = mot;
        switch_potentiometer = pot;
        stw_acc = acc;
        stw_dec = dec;
      }
    }
    taskSuspend();
  }
}
