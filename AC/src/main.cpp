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
#include <CmdHandler.h>
#include <Console.h>
#include <Display.h>
#include <DriverDisplay.h>
#include <EngineerDisplay.h>
#if RTC_ON
#include <ESP32Time.h>
#endif
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

int base_offset_suspend = 150;
bool SystemInited = false;
bool SystemJustInited = true;
uint64_t life_sign = 0;

CANBus canBus;
CarControl carControl;
CarState carState;
CmdHandler cmdHandler;
Console console;
DriverDisplay driverDisplay;
EngineerDisplay engineerDisplay;
GPInputOutput gpio; // I2C Interrupts, GPInputOutput pin settings
I2CBus i2cBus;
// OneWireBus oneWireBus;
SPIBus spiBus;
Uart uart; // SERIAL

static void canBusTask(void *pvParams) { canBus.task(pvParams); }
static void carControlTask(void *pvParams) { carControl.task(pvParams); }
static void cmdHandlerTask(void *pvParams) { cmdHandler.task(pvParams); }
static void engineerDisplayTask(void *pvParams) { engineerDisplay.task(pvParams); }
static void driverDisplayTask(void *pvParams) { driverDisplay.task(pvParams); }

void app_main(void) {
  string msg;
  // init console IO and radio console
  msg = uart.init();
  console << NL << msg << NL;
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

  // Engineer Display
  msg = engineerDisplay.init_t(1, 1, 10000, base_offset_suspend + 300);
  console << msg << NL;
  engineerDisplay.verboseMode = false;
  console << "[  ] Create " << engineerDisplay.getName() << " ...";
  xTaskCreatePinnedToCore(engineerDisplayTask,             /* task function. */
                          engineerDisplay.getInfo(),       /* name of task. */
                          engineerDisplay.getStackSize(),  /* stack size of task */
                          NULL,                            /* parameter of the task */
                          engineerDisplay.getPriority(),   /* priority of the task */
                          engineerDisplay.getTaskHandle(), /* task handle to keep track of created task */
                          engineerDisplay.getCoreId());    /* pin task to core id */
  engineerDisplay.clear_screen(0xffff);
  console << " done." << NL;
  msg = carControl.report_task_init();
  console << msg << NL;
  engineerDisplay.print(msg + "\n");
  // CAN Bus
  msg = canBus.init_t(1, 1, 10000, base_offset_suspend + 90);
  console << msg << NL;
  canBus.verboseModeCanIn = false;
  canBus.verboseModeCanInNative = false;
  canBus.verboseModeCanOut = false;
  canBus.verboseModeCanOutNative = false;
  console << "[  ] Create " << canBus.getName() << " task ...";
  xTaskCreatePinnedToCore(canBusTask,             /* task function. */
                          canBus.getInfo(),       /* name of task. */
                          canBus.getStackSize(),  /* stack size of task */
                          NULL,                   /* parameter of the task */
                          canBus.getPriority(),   /* priority of the task */
                          canBus.getTaskHandle(), /* task handle to keep track of created task */
                          canBus.getCoreId());    /* pin task to core id */
  console << " done." << NL;
  msg = canBus.report_task_init();
  console << msg << NL;
  engineerDisplay.print(msg + "\n");
#if COMMANDHANDLER_ON
  // CMD Handler
  msg = cmdHandler.init_t(1, 1, 10000, base_offset_suspend + 300);
  console << msg << NL;
  console << "[  ] Create " << cmdHandler.getName() << " task ...";
  xTaskCreatePinnedToCore(cmdHandlerTask,             /* task function. */
                          cmdHandler.getInfo(),       /* name of task. */
                          cmdHandler.getStackSize(),  /* stack size of task */
                          NULL,                       /* parameter of the task */
                          cmdHandler.getPriority(),   /* priority of the task */
                          cmdHandler.getTaskHandle(), /* task handle to keep track of created task */
                          cmdHandler.getCoreId());    /* pin task to core id */
  console << " done." << NL;
  msg = cmdHandler.report_task_init();
  console << msg << NL;
  engineerDisplay.print(msg + "\n");
#endif
  // Car Control AC
  msg = carControl.init_t(1, 10, 10000, base_offset_suspend + 100);
  console << msg << NL;
  carControl.verboseMode = false;
  carControl.verboseModeDebug = false;
  console << "[  ] Create " << carControl.getName() << " task ...";
  xTaskCreatePinnedToCore(carControlTask,             /* task function. */
                          carControl.getInfo(),       /* name of task. */
                          carControl.getStackSize(),  /* stack size of task */
                          NULL,                       /* parameter of the task */
                          carControl.getPriority(),   /* priority of the task */
                          carControl.getTaskHandle(), /* task handle to keep track of created task */
                          carControl.getCoreId());    /* pin task to core id */
  console << " done." << NL;
  msg = carControl.report_task_init();
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
  msg = driverDisplay.init_t(1, 1, 10000, base_offset_suspend + 110);
  console << msg << NL;
  driverDisplay.verboseMode = false;
  driverDisplay.set_DisplayStatus(DISPLAY_STATUS::DRIVER_SETUP);
  console << "[  ] Create " << driverDisplay.getName() << " task ...";
  xTaskCreatePinnedToCore(driverDisplayTask,             /* task function. */
                          driverDisplay.getInfo(),       /* name of task. */
                          driverDisplay.getStackSize(),  /* stack size of task */
                          NULL,                          /* parameter of the task */
                          driverDisplay.getPriority(),   /* priority of the task */
                          driverDisplay.getTaskHandle(), /* task handle to keep track of created task */
                          driverDisplay.getCoreId());    /* pin task to core id */
  console << " done." << NL;
  msg = carControl.report_task_init();
  console << msg << driverDisplay.get_DisplayStatus_text() << NL;
  engineerDisplay.print(msg + "\n");
  delay(1000);

  console << "------------------------------------------------------------" << NL;
  if (i2cBus.isDC()) {
    console << "Initialization ready as DriveController" << NL;
  } else {
    console << "Initialization ready as AuxiliaryController" << NL;
  }
  console << fmt::format("- i2cBus.verboseModeI2C         = {}", i2cBus.verboseModeI2C) << NL;
  console << fmt::format("- canBus.verboseModeCanIn       = {}", canBus.verboseModeCanIn) << NL;
  console << fmt::format("- canBus.verboseModeCanInNative = {}", canBus.verboseModeCanInNative) << NL;
  console << fmt::format("- canBus.verboseModeCanOut      = {}", canBus.verboseModeCanOut) << NL;
  console << fmt::format("- canBus.verboseModeCanOutNative= {}", canBus.verboseModeCanOutNative) << NL;
  console << fmt::format("- carControl.verboseMode        = {}", carControl.verboseMode) << NL;
  console << fmt::format("- carControl.verboseModeDebug   = {}", carControl.verboseModeDebug) << NL;
  console << "------------------------------------------------------------" << NL;
  delay(1000);
  SystemInited = true;
}
