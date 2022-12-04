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
#include <CANBus.h>
#include <CarControl.h>
#include <Console.h>
#include <Display.h>
#include <DriverDisplay.h>
//#include <ESP32Time.h>
#include <EngineerDisplay.h>
#include <GPIO.h>
#include <I2CBus.h>
#include <OneWire.h>
#include <OneWireBus.h>
#include <SPIBus.h>
#include <Serial.h>
#include <System.h>

#include <fmt/core.h>

// add C linkage definition
extern "C" {
void app_main(void);
}

using namespace std;

CANBus canBus;
CarControl carControl;
CarState carState;
Console console;
DriverDisplay driverDisplay;
EngineerDisplay engineerDisplay;
// ESP32Time esp32time(0);
GPInputOutput gpio; // I2C Interrupts, GPInputOutput pin settings
I2CBus i2cBus;
OneWireBus oneWireBus;
// RTC rtc;
// SDCard sdCard;
SPIBus spiBus;

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

  if (i2cBus.isAC()) {
    console << "-Auxiliary Controller specific initialization ------------\n";
    // #if RTC_ON
    //   msg = rtc.init();
    //   console << msg << NL;
    //   engineerDisplay.print(msg + "\n");
    // #endif
    msg = engineerDisplay.init();
    console << msg << NL;
    engineerDisplay.print(msg + "\n");
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

  msg = carControl.init();
  console << msg << NL;
  // engineerDisplay.print(msg + "\n");
  msg = carControl.create_task(10, 200, 8000);
  console << msg << NL;
  // engineerDisplay.print(msg + "\n");
  carControl.verboseMode = true;

  if (i2cBus.isAC()) {
    msg = engineerDisplay.create_task(10);
    console << msg << "\n";
    engineerDisplay.print(msg + "\n");
    driverDisplay.init();
    driverDisplay.set_DisplayStatus(DISPLAY_STATUS::DRIVER_SETUP);
    msg = driverDisplay.create_task(16);
    console << msg << driverDisplay.get_DisplayStatus_text() << NL;
    engineerDisplay.print(msg + "\n");
    delay(1000);
  }
  console << "------------------------------------------------------------\n";
  if (i2cBus.isDC()) {
    console << "Initialization ready as DriveController\n";
  } else {
    console << "Initialization ready as AuxiliaryController\n";
  }
  console << "------------------------------------------------------------\n";
}
