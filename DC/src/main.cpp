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
#include <ADC.h>
#include <CANBus.h>
#include <CarControl.h>
#include <CmdHandler.h>
#include <Console.h>
#include <DAC.h>
#if RTC_ON
#include <ESP32Time.h>
#endif
#include <GPIO.h>
#include <I2CBus.h>
#include <IOExt.h>
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
bool SystemJustInited = true;
bool adcInited = false;
bool dacInited = false;
uint64_t life_sign = 0;

ADC adc;
CANBus canBus;
CarControl carControl;
CarState carState;
CmdHandler cmdHandler;
Console console;
DAC dac;
GPInputOutput gpio; // I2C Interrupts, GPInputOutput pin settings
I2CBus i2cBus;
IOExt ioExt;
// OneWireBus oneWireBus;
SPIBus spiBus;
Uart uart; // SERIAL

static void adcTask(void *pvParams) { adc.task(pvParams); }
static void canBusTask(void *pvParams) { canBus.task(pvParams); }
static void carControlTask(void *pvParams) { carControl.task(pvParams); }
static void cmdHandlerTask(void *pvParams) { cmdHandler.task(pvParams); }
static void ioExtTask(void *pvParams) { ioExt.task(pvParams); }

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
  // NOT available on DC
  // CAN Bus
  msg = canBus.init_t(0, 1, 10000, 100);
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
#if COMMANDHANDLER_ON
  // CMD Handler
  msg = cmdHandler.init_t(1, 1, 10000, 200);
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
#endif
  // Car Control AC
  msg = carControl.init_t(1, 10, 10000, 150);
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
  // DAC
  msg = dac.init();
  console << msg << NL;
  dac.verboseModeDAC = false;
  // IOExt
  msg = ioExt.init_t(1, 10, 10000, 100);
  console << msg << NL;
  ioExt.verboseModeDIn = false;
  ioExt.verboseModeDInHandler = false;
  ioExt.verboseModeDOut = false;
  console << "[  ] Create " << ioExt.getName() << " task ...";
  xTaskCreatePinnedToCore(ioExtTask,             /* task function. */
                          ioExt.getInfo(),       /* name of task. */
                          ioExt.getStackSize(),  /* stack size of task */
                          NULL,                  /* parameter of the task */
                          ioExt.getPriority(),   /* priority of the task */
                          ioExt.getTaskHandle(), /* task handle to keep track of created task */
                          ioExt.getCoreId());    /* pin task to core id */
  console << " done." << NL;
  msg = ioExt.report_task_init();
  console << msg << NL;
  // ADC
  msg = adc.init_t(1, 12, 10000, 150);
  console << msg << NL;
  adc.verboseModeADC = false;
  adc.verboseModeADCDebug = false;
  console << "[  ] Create " << adc.getName() << " task ...";
  xTaskCreatePinnedToCore(adcTask,             /* task function. */
                          adc.getInfo(),       /* name of task. */
                          adc.getStackSize(),  /* stack size of task */
                          NULL,                /* parameter of the task */
                          adc.getPriority(),   /* priority of the task */
                          adc.getTaskHandle(), /* task handle to keep track of created task */
                          adc.getCoreId());    /* pin task to core id */
  console << " done." << NL;
  msg = adc.report_task_init();
  console << msg << NL;

  // Driver Display
  // NOT available on DC
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
  console << fmt::format("- canBus.verboseModeDIn         = {}", ioExt.verboseModeDIn) << NL;
  console << fmt::format("- canBus.verboseModeDInHandler  = {}", ioExt.verboseModeDInHandler) << NL;
  console << fmt::format("- canBus.verboseModeDOut        = {}", ioExt.verboseModeDOut) << NL;
  console << fmt::format("- carControl.verboseMode        = {}", carControl.verboseMode) << NL;
  console << fmt::format("- carControl.verboseModeDebug   = {}", carControl.verboseModeDebug) << NL;
  console << "------------------------------------------------------------" << NL;
  SystemInited = true;
}
