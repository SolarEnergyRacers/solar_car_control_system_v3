/*
 *  SER - Solar Car Control System - Main Routine for setting up tasks,
 * initialize devices, ..
 *
 * clang style:
 *    ./extras/format.sh
 */
#ifndef ARDUINO
#define ARDUINO 10805
#endif

#include <fmt/core.h>

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
#include <CarControl.h>
#include <Console.h>

//#include <Serial.h>
#include <ADC.h>
#include <CANBus.h>
#include <DAC.h>
#include <I2CBus.h>
#include <System.h>

// add C linkage definition
extern "C" {
void app_main(void);
}

using namespace std;

Console console;
CANBus canBus;
CarControl carControl;
CarState carState;
I2CBus i2c;
ADC adc;
bool adcInited = false;
DAC dac;
bool dacInited = false;

void app_main(void) {
  string msg;
  delay(1000);
  console << "\n------------------------------------------------------------\n";
  console << "esp32dev + free RTOS\n";
  console << "Solar Energy Car Racers SER4 Controller: v" << VERSION << ", build time: " << __DATE__ << " " << __TIME__ << "\n";
  console << "ARDUINO: " << ARDUINO << "\n";
  chip_info();
  console << "------------------------------------------------------------\n";

  msg = i2c.init();
  console << msg << "\n";
  // engineerDisplay.print(msg + "\n");
  i2c.verboseModeI2C = true;

  msg = canBus.init();
  console << msg << "\n";
  // engineerDisplay.print(msg + "\n");
  msg = canBus.create_task();
  console << msg << "\n";
  // engineerDisplay.print(msg + "\n");
  canBus.verboseModeCan = true;

  if (i2c.isDC()) {
    msg = dac.init();
    console << msg << "\n";
    // engineerDisplay.print(msg + "\n");
    dac.verboseModeDAC = true;

    msg = adc.init();
    console << msg << "\n";
    // engineerDisplay.print(msg + "\n");
    msg = adc.create_task();
    console << msg << "\n";
    // engineerDisplay.print(msg + "\n");
    adc.verboseModeADC = false;
  }

  msg = carControl.init();
  console << msg << "\n";
  // engineerDisplay.print(msg + "\n");
  msg = carControl.create_task();
  console << msg << "\n";
  // engineerDisplay.print(msg + "\n");
  carControl.verboseMode = true;
}
