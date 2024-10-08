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
#include <CarState.h>
#include <CarStateRadio.h>
#include <CmdHandler.h>
#include <Console.h>
#include <Display.h>
#include <DriverDisplay.h>
#include <EngineerDisplay.h>
#include <GPIO.h>
#include <I2CBus.h>
#include <OneWire.h>
#include <OneWireBus.h>
#include <RTC_SER.h>
#include <SDCard.h>
#include <SPIBus.h>
#include <Serial.h>
#include <System.h>

// #include <Wire.h>
#include <array>
// #include <vector>

// add C linkage definition
extern "C" {
void app_main(void);
}

using namespace std;

int base_offset_suspend = 10;
bool SystemInited = false;
bool SystemJustInited = true;
uint64_t life_sign = 0;

CANBus canBus;
CarControl carControl;
CarState carState;
CarStateRadio carStateRadio;
CmdHandler cmdHandler;
Console console;
DriverDisplay driverDisplay;
EngineerDisplay engineerDisplay;
GPInputOutput gpio;
I2CBus i2cBus;
SDCard sdCard;
SPIBus spiBus;
Uart uart;
GlobalTime globalTime = GlobalTime(&i2cBus);

static void canBusTask(void *pvParams) { canBus.task(pvParams); }
static void carControlTask(void *pvParams) { carControl.task(pvParams); }
static void cmdHandlerTask(void *pvParams) { cmdHandler.task(pvParams); }
static void engineerDisplayTask(void *pvParams) { engineerDisplay.task(pvParams); }
static void driverDisplayTask(void *pvParams) { driverDisplay.task(pvParams); }
auto compiletime = RtcDateTime(__DATE__, __TIME__);

// Adafruit_ILI9341 ili9341 = Adafruit_ILI9341(SPI_CS_TFT, SPI_DC, SPI_MOSI, SPI_CLK, SPI_RST, SPI_MISO);
Adafruit_ILI9341 ili9341 = Adafruit_ILI9341(&(spiBus.spi), SPI_DC, SPI_CS_TFT, SPI_RST);
Display display = Display(&ili9341);

void app_main(void) {
  string msg;
  carState.init_values();
  carStateRadio.verboseModeRadioSend = false;

  // init console IO and radio console
  // msg = uart.init();
  msg = uart.init_t(1, 60, 1000, base_offset_suspend + 100);
  console << msg << NL;
  console << "------------------------------------------------------------" << NL;
  console << "-- gpio pin settings ---------------------------------------" << NL;
  msg = gpio.init();
  delay(200);
  console << msg << NL;
  // init buses
  console << "-- init bus systems ----------------------------------------" << NL;
  msg = spiBus.init();
  console << msg << NL;
  msg = i2cBus.init();
  console << msg << NL;
  i2cBus.verboseModeI2C = false;
  delay(200);

  std::array<std::string, 7> wd = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; // todo: rm
  console << fmt::format("main.o compiletime: {}:{}:{}  {}-{}-{} ({})\n", compiletime.Hour(), compiletime.Minute(), compiletime.Second(),
                         compiletime.Day(), compiletime.Month(), compiletime.Year(), wd[compiletime.DayOfWeek()]);
  int RTC_err = globalTime.init(DS1307SquareWaveOut_Low, 1);
  console << "RTC init errorcode: " << RTC_err << "\n";
  console << "RTC time: " << globalTime.strTime("%H:%M:%S %Y-%m-%d (%a)") << "\n";
  // sleep(5);
  // console << "time 5s: " << globalTime.strTime("%H:%M:%S %Y-%m-%d (%a)") << "\n";
  // sleep(60);
  // console << "time 5s: " << globalTime.strTime("%H:%M:%S %Y-%m-%d (%a)") << "\n";

  //------------------------------------------------------------
  // Global Display
  msg = display.init();
  console << msg << NL;
  display.clear_screen(0xffff);
  display.print("Start AC Init Procedure:\n\n");

  //------------------------------------------------------------
  // CAN Bus
  msg = canBus.init_t(0, 22, 10000, base_offset_suspend + 10);
  console << msg << NL;
  canBus.verboseModeCanIn = false;
  canBus.verboseModeCanInNative = false;
  canBus.verboseModeCanOut = false;
  canBus.verboseModeCanOutNative = false;
  canBus.verboseModeCanBusLoad = false;
  console << "[  ] " << canBus.getName() << " create task ..." << NL;
  xTaskCreatePinnedToCore(canBusTask,             /* task function. */
                          canBus.getInfo(),       /* name of task. */
                          canBus.getStackSize(),  /* stack size of task */
                          NULL,                   /* parameter of the task */
                          canBus.getPriority(),   /* priority of the task */
                          canBus.getTaskHandle(), /* task handle to keep track of created task */
                          canBus.getCoreId());    /* pin task to core id */
  msg = canBus.report_task_init();
  console << msg << NL;
  display.print(msg + "\n");
  // vTaskDelay(10);

  //------------------------------------------------------------
  // CMD Handler
  msg = cmdHandler.init_t(1, 1, 10000, base_offset_suspend + 300);
  console << msg << NL;
  console << "[  ] " << cmdHandler.getName() << " create task ..." << NL;
  xTaskCreatePinnedToCore(cmdHandlerTask,             /* task function. */
                          cmdHandler.getInfo(),       /* name of task. */
                          cmdHandler.getStackSize(),  /* stack size of task */
                          NULL,                       /* parameter of the task */
                          cmdHandler.getPriority(),   /* priority of the task */
                          cmdHandler.getTaskHandle(), /* task handle to keep track of created task */
                          cmdHandler.getCoreId());    /* pin task to core id */
  msg = cmdHandler.report_task_init();
  console << msg << NL;
  display.print(msg + "\n");
  // vTaskDelay(10);

  //------------------------------------------------------------
  // Engineer Display
  engineerDisplay.verboseModeEngineer = false;
  msg = engineerDisplay.init_t(1, 1, 10000, base_offset_suspend + 20);
  console << msg << NL;
  console << "[  ] " << engineerDisplay.getName() << " create task ..." << NL;
  xTaskCreatePinnedToCore(engineerDisplayTask,             /* task function. */
                          engineerDisplay.getInfo(),       /* name of task. */
                          engineerDisplay.getStackSize(),  /* stack size of task */
                          NULL,                            /* parameter of the task */
                          engineerDisplay.getPriority(),   /* priority of the task */
                          engineerDisplay.getTaskHandle(), /* task handle to keep track of created task */
                          engineerDisplay.getCoreId());    /* pin task to core id */
  msg = engineerDisplay.report_task_init();
  console << msg << NL;
  display.print(msg + "\n");
  // vTaskDelay(10);

  //------------------------------------------------------------
  // Driver Display
  driverDisplay.verboseModeDriver = false;
  msg = driverDisplay.init_t(1, 1, 10000, base_offset_suspend + 20);
  console << msg << NL;
  console << "[  ] " << driverDisplay.getName() << " create task ..." << NL;
  xTaskCreatePinnedToCore(driverDisplayTask,             /* task function. */
                          driverDisplay.getInfo(),       /* name of task. */
                          driverDisplay.getStackSize(),  /* stack size of task */
                          NULL,                          /* parameter of the task */
                          driverDisplay.getPriority(),   /* priority of the task */
                          driverDisplay.getTaskHandle(), /* task handle to keep track of created task */
                          driverDisplay.getCoreId());    /* pin task to core id */
  msg = driverDisplay.report_task_init();
  console << msg << NL;
  display.print(msg + "\n");
  // vTaskDelay(10);

  //------------------------------------------------------------
  // Car Control AC
  msg = carControl.init_t(1, 10, 10000, base_offset_suspend + 10);
  console << msg << NL;
  carControl.verboseModeCarControl = false;
  carControl.verboseModeCarControlDebug = false;
  console << "[  ] " << carControl.getName() << " create task ..." << NL;
  xTaskCreatePinnedToCore(carControlTask,             /* task function. */
                          carControl.getInfo(),       /* name of task. */
                          carControl.getStackSize(),  /* stack size of task */
                          NULL,                       /* parameter of the task */
                          carControl.getPriority(),   /* priority of the task */
                          carControl.getTaskHandle(), /* task handle to keep track of created task */
                          carControl.getCoreId());    /* pin task to core id */
  msg = carControl.report_task_init();
  console << msg << NL;
  display.print(msg + NL);
  // vTaskDelay(10);

  //------------------------------------------------------------
  // SC card
  msg = sdCard.init();
  console << msg << NL;
  display.print(msg + NL);
  //--- SD card available
  carState.initalize_config();
  console << carState.print("State after reading SER4CNFG.INI") << NL;
  sdCard.check_log_file();
  //------from now config ini values can be used

  stringstream ss;
  ss << NL;
  ss << "----------------------------------------------------" << NL;
  ss << "Initialization ready as AuxiliaryController" << NL;
  ss << fmt::format("- i2cBus.verboseModeI2C              = {}", i2cBus.verboseModeI2C) << NL;
  ss << fmt::format("- canBus.verboseModeCanIn            = {}", canBus.verboseModeCanIn) << NL;
  ss << fmt::format("-        verboseModeCanInNative      = {}", canBus.verboseModeCanInNative) << NL;
  ss << fmt::format("-        verboseModeCanOut           = {}", canBus.verboseModeCanOut) << NL;
  ss << fmt::format("-        verboseModeCanOutNative     = {}", canBus.verboseModeCanOutNative) << NL;
  ss << fmt::format("-        verboseModeCanBusLoad       = {}", canBus.verboseModeCanBusLoad) << NL;
  ss << fmt::format("- carControl.verboseModeCC           = {}", carControl.verboseModeCarControl) << NL;
  ss << fmt::format("-            verboseModeCCDebug      = {}", carControl.verboseModeCarControlDebug) << NL;
  ss << fmt::format("- engineerDisplay.verboseModeED      = {}", engineerDisplay.verboseModeEngineer) << NL;
  ss << fmt::format("- driverDisplay.verboseModeDD        = {}", driverDisplay.verboseModeDriver) << NL;
  ss << fmt::format("- carStateRadio.verboseModeRadioSend = {}", carStateRadio.verboseModeRadioSend) << NL;
  ss << fmt::format("- sdCard.verboseModeSdCard           = {}", sdCard.verboseModeSdCard) << NL;
  ss << "----------------------------------------------------" << NL;
  // vTaskDelay(10);
  console << ss.str();
  // display.print(ss.str());
  //--let the bootscreen visible for a moment ------------------
  display.print("\nWaiting for start of life system: ");
  int waitAtConsoleView = 3;
  while (waitAtConsoleView-- > 0) {
    display.print(to_string(waitAtConsoleView));
    delay(1000);
    display.print("-");
  }
  console << NL;
  display.print("start\n");
  display.set_DisplayStatus(DISPLAY_STATUS::DRIVER_SETUP);
  SystemInited = true;
}
