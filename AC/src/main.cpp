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
#include <fmt/core.h>
#include <fmt/printf.h>
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
#include <AbstractTask.h>
#include <LocalFunctionsAndDevices.h>
// local components
#include <CANBus.h>
#include <CarControl.h>
#include <Console.h>
#include <Display.h>
#include <DriverDisplay.h>
#include <EngineerDisplay.h>
// #include <ESP32Time.h>
#include <GPIO.h>
#include <I2CBus.h>
#include <OneWire.h>
#include <OneWireBus.h>
#include <SPIBus.h>
#include <Serial.h>
#include <System.h>

// add C linkage definition
extern "C" {
void app_main(void);
}

using namespace std;

bool SystemInited = false;
uint64_t life_sign = 0;

CANBus canBus;
CarControl carControl;
CarState carState;
Console console;
GPInputOutput gpio; // I2C Interrupts, GPInputOutput pin settings
I2CBus i2cBus;
// OneWireBus oneWireBus;
SPIBus spiBus;

EngineerDisplay engineerDisplay;
DriverDisplay driverDisplay;

static void canBusTask(void *pvParams) { canBus.task(pvParams); }
static void carControlTask(void *pvParams) { carControl.task(pvParams); }
static void engineerDisplayTask(void *pvParams) { engineerDisplay.task(pvParams); }
static void driverDisplayTask(void *pvParams) { driverDisplay.task(pvParams); }

void app_main(void) {
  string msg;
  console << NL;

#if SERIAL_RADIO_ON
  // init console IO and radio console
  Uart uart; // SERIAL
  msg = uart.init();
  console << msg << NL;
#endif

  delay(1000);
  console << NL << "------------------------------------------------------------" << NL;
  console << "esp32dev + free RTOS" << NL;
  console << "Solar Energy Car Racers SER4 Controller: v" << VERSION << ", build time: " << __DATE__ << " " << __TIME__ << NL;
  console << "ARDUINO:              " << ARDUINO << NL;
  console << "SER4TYPE:             " << SER4TYPE << NL;
  console << "Main running on core: " << xPortGetCoreID() << NL;
  console << "------------------------------------------------------------" << NL;
  // init arduino library
  // initArduino();
  console << "------------------------------------------------------------" << NL;
  chip_info();
  console << "------------------------------------------------------------" << NL;
  console << "-- gpio pin settings ---------------------------------------" << NL;
  msg = gpio.init();
  delay(200);
  console << msg << NL;
  // init buses
  console << "-- init bus systems ----------------------------------------" << NL;
  msg = spiBus.init();
  console << msg << NL;
  // msg = oneWireBus.init();
  // console << msg << NL;
  msg = i2cBus.init();
  console << msg << NL;
  i2cBus.verboseModeI2C = false;
  delay(200);

  if (i2cBus.isDC()) {
    console << "-- Drive Controller recognized -" << NL;
  } else {
    console << "-- Auxiliary Controller recognized -" << NL;
  }
  // Engineer Display
  msg = engineerDisplay.init_t(1, 1, 10000, 300);
  console << msg << NL;
  engineerDisplay.verboseMode = false;
  console << "[  ] Create " << engineerDisplay.getName() << " ...";
  xTaskCreatePinnedToCore(engineerDisplayTask,             /* task function. */
                          engineerDisplay.getInfo(),       /* name of task. */
                          engineerDisplay.getStackSize(),      /* stack size of task */
                          NULL,                            /* parameter of the task */
                          engineerDisplay.getPriority(),        /* priority of the task */
                          engineerDisplay.getTaskHandle(), /* task handle to keep track of created task */
                          engineerDisplay.getCoreId());        /* pin task to core id */
  console << " done." << NL;
  msg = carControl.report_task_init(&engineerDisplay);
  console << msg << NL;
  engineerDisplay.print(msg + "\n");
  // CAN Bus
  msg = canBus.init_t(0, 1, 10000, 200);
  console << msg << NL;
  canBus.verboseModeCan = false;
  canBus.verboseModeCanIn = true;
  canBus.verboseModeCanOut = false;
  canBus.verboseModeCanDebug = false;
  console << "[  ] Create " << canBus.getName() << " task ...";
  xTaskCreatePinnedToCore(canBusTask,             /* task function. */
                          canBus.getInfo(),       /* name of task. */
                          canBus.getStackSize(),      /* stack size of task */
                          NULL,                   /* parameter of the task */
                          canBus.getPriority(),        /* priority of the task */
                          canBus.getTaskHandle(), /* task handle to keep track of created task */
                          canBus.getCoreId());        /* pin task to core id */
  console << " done." << NL;
  msg = canBus.report_task_init(&canBus);
  console << msg << NL;
  engineerDisplay.print(msg + "\n");
  // Car Control AC
  msg = carControl.init_t(1, 10, 10000, 200);
  console << msg << NL;
  carControl.verboseMode = true;
  console << "[  ] Create " << carControl.getName() << " task ...";
  xTaskCreatePinnedToCore(carControlTask,             /* task function. */
                          carControl.getInfo(),       /* name of task. */
                          carControl.getStackSize(),      /* stack size of task */
                          NULL,                       /* parameter of the task */
                          carControl.getPriority(),        /* priority of the task */
                          carControl.getTaskHandle(), /* task handle to keep track of created task */
                          carControl.getCoreId());        /* pin task to core id */
  console << " done." << NL;
  msg = carControl.report_task_init(&carControl);
  console << msg << NL;
  engineerDisplay.print(msg + "\n");

  //--let the bootscreen visible for a moment ------------------
  engineerDisplay.print(".\nWaiting for start of life display: ");
  int waitAtConsoleView = 3;
  while (waitAtConsoleView-- > 0) {
    engineerDisplay.print(to_string(waitAtConsoleView));
    delay(1000);
    engineerDisplay.print("-");
  }
  engineerDisplay.print("start");
  engineerDisplay.set_DisplayStatus(DISPLAY_STATUS::ENGINEER_HALTED);
  //------------------------------------------------------------
  // Driver Display
  msg = driverDisplay.init_t(1, 1, 10000, 300);
  console << msg << NL;
  driverDisplay.verboseMode = false;
  driverDisplay.set_DisplayStatus(DISPLAY_STATUS::DRIVER_SETUP);
  console << "[  ] Create " << driverDisplay.getName() << " task ...";
  xTaskCreatePinnedToCore(driverDisplayTask,             /* task function. */
                          driverDisplay.getInfo(),       /* name of task. */
                          driverDisplay.getStackSize(),      /* stack size of task */
                          NULL,                          /* parameter of the task */
                          driverDisplay.getPriority(),        /* priority of the task */
                          driverDisplay.getTaskHandle(), /* task handle to keep track of created task */
                          driverDisplay.getCoreId());        /* pin task to core id */
  console << " done." << NL;
  msg = carControl.report_task_init(&driverDisplay);
  console << msg << driverDisplay.get_DisplayStatus_text() << NL;
  engineerDisplay.print(msg + "\n");
  delay(1000);

  console << "------------------------------------------------------------" << NL;
  if (i2cBus.isDC()) {
    console << "Initialization ready as DriveController" << NL;
  } else {
    console << "Initialization ready as AuxiliaryController" << NL;
  }
  console << fmt::format("- i2cBus.verboseModeI2C      = {}", i2cBus.verboseModeI2C) << NL;
  console << fmt::format("- canBus.verboseModeCan      = {}", canBus.verboseModeCan) << NL;
  console << fmt::format("- canBus.verboseModeCanDebug = {}", canBus.verboseModeCanDebug) << NL;
  console << fmt::format("- carControl.verboseMode     = {}", carControl.verboseMode) << NL;
  console << "------------------------------------------------------------" << NL;
  SystemInited = true;
}
