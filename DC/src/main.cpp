/*
 *  SER - Solar Car Control System - Main Routine for setting up tasks,
 * initialize devices, ..
 *
 * clang style:
 *    ../extras/format.sh
 */
#ifndef ARDUINO
#define ARDUINO 10805
#endif

// project variables
#include <sdkconfig.h>

// local definitions
#include <definitions.h>
// standard libraries
#include <Streaming.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
// Arduino
#include <Arduino.h>
#include <SoftwareSerial.h>
// FreeRTOS
#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
// local includes
#include <Abstract_task.h>
#include <LocalFunctionsAndDevices.h>
// local libs
#include <ADC.h>
#include <CANBus.h>
#include <CarControl.h>
#include <Console.h>
#include <DAC.h>
//#include <ESP32Time.h>
#include <GPIO.h>
#include <I2CBus.h>
#include <OneWire.h>
#include <OneWireBus.h>
#include <SPIBus.h>
#include <System.h>

#include <fmt/core.h>

// add C linkage definition
extern "C" {
void app_main(void);
}

using namespace std;

ADC adc;
CANBus canBus;
CarControl carControl;
CarState carState;
Console console;
DAC dac;
GPInputOutput gpio; // I2C Interrupts, GPInputOutput pin settings
I2CBus i2cBus;
OneWireBus oneWireBus;
SPIBus spiBus;
bool adcInited = false;
bool dacInited = false;

void app_main(void) {
  string msg;
  console << "\n";

#if SERIAL_RADIO_ON
  // init console IO and radio console
  Uart uart; // SERIAL
  msg = uart.init();
  console << msg << "\n";
#endif

  delay(1000);
  console << "\n------------------------------------------------------------\n";
  console << "esp32dev + free RTOS\n";
  console << "Solar Energy Car Racers SER4 Controller: v" << VERSION << ", build time: " << __DATE__ << " " << __TIME__ << NL;
  console << "ARDUINO: " << ARDUINO << NL;
  console << "------------------------------------------------------------\n";
  // init arduino library
  initArduino();
  console << "------------------------------------------------------------\n";
  chip_info();
  console << "------------------------------------------------------------\n";

  console << "-gpio pin settings -----------------------------------------\n";
  msg = gpio.init();
  delay(200);
  console << msg << NL;

  console << "-init bus systems ------------------------------------------\n";
  // init buses
  msg = spiBus.init();
  console << msg << NL;
  msg = oneWireBus.init();
  console << msg << NL;
  msg = i2cBus.init();
  console << msg << NL;
  delay(200);

  if (i2cBus.isDC()) {
    console << "-Drive Controller recognized-\n";
  } else {
    console << "-Auxiliary Controller recognized-\n";
  }

  i2cBus.verboseModeI2C = true;
  msg = canBus.init();
  console << msg << NL;
  // engineerDisplay.print(msg + "\n");
  msg = canBus.create_task(15, 600, 8000);
  console << msg << NL;
  // engineerDisplay.print(msg + "\n");
  canBus.verboseModeCan = true;
  canBus.verboseModeCanDebug = true;

  if (i2cBus.isDC()) {
    console << "-Drive Controller specific initialization ----------------\n";
    msg = dac.init();
    console << msg << NL;
    // engineerDisplay.print(msg + "\n");
    dac.verboseModeDAC = true;

    msg = adc.init();
    console << msg << NL;
    // engineerDisplay.print(msg + "\n");
    msg = adc.create_task(6, 300, 8000);
    console << msg << NL;
    // engineerDisplay.print(msg + "\n");
    adc.verboseModeADC = true;
  }

  msg = carControl.init();
  console << msg << NL;
  // engineerDisplay.print(msg + "\n");
  msg = carControl.create_task(10, 200, 8000);
  console << msg << NL;
  // engineerDisplay.print(msg + "\n");
  carControl.verboseMode = true;

  console << "------------------------------------------------------------\n";
  if (i2cBus.isDC()) {
    console << "Initialization ready as DriveController\n";
  } else {
    console << "Initialization ready as AuxiliaryController\n";
  }
  console << "------------------------------------------------------------\n";
}
