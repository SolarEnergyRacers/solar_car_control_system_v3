/*
 *  SER - Solar Car Control System - Main Routine for setting up tasks,
 * initialize devices, ..
 *
 * clang style:
 *    ./extras/format.sh
 */

#include <fmt/core.h>

// local definitions
#include <definitions.h>

// standard libraries
#include <Streaming.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

// FreeRTOS / Arduino
#include <Arduino.h>
#include <ESP32Time.h>
#include <SoftwareSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/FreeRTOSConfig.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

// project variables
//#include <sdkconfig.h>

// local libs
#include <ADC/ADC.h>
#include <CANBus/CANBus.h>
#include <CarSpeed/CarSpeed.h>
#include <CmdHandler/CmdHandler.h>
#include <Console/Console.h>
#include <DAC/DAC.h>
#include <Display/Display.h>
#include <DriverDisplay/DriverDisplay.h>
#include <ESP32Time.h>
#include <EngineerDisplay/EngineerDisplay.h>
#include <GPIO/GPIO.h>
#include <I2CBus/I2CBus.h>
#include <IOExt/IOExt.h>
#include <IOExt/IOExtHandler.h>
#include <MCP23017.h>
#include <OneWireBus/OneWireBus.h>
#include <PWM/PWM.h>
#include <RTC/RTC.h>
#include <SDCard/SDCard.h>
#include <SPIBus/SPIBus.h>
#include <Serial/Serial.h>
#include <System/System.h>
#include <Temp/Temp.h>

#include <LocalFunctionsAndDevices.h>
#include <abstract_task.h>

#include <CarControl/CarControl.h>
#include <CarState/CarState.h>
#include <CarStatePin.h>

// add C linkage definition
extern "C" {
void app_main(void);
}

using namespace std;

// Global objects (not possible to deactivate)
ADC adc;
CANBus canBus;
CarControl carControl;
CarState carState;
CmdHandler cmdHandler;
Console console;
DAC dac;
DriverDisplay driverDisplay;
EngineerDisplay engineerDisplay;
ESP32Time esp32time(0);
GPInputOutput gpio;
I2CBus i2cBus;
IOExt ioExt;
OneWireBus oneWireBus;
RTC rtc;
SDCard sdCard;
SPIBus spiBus;
// Global objects (possibly to deactivated)
#if DS_ON
Temp ds; // temperature
#endif
#if PWM_ON
PWM pwm;
#endif
#if CARSPEED_ON
CarSpeed carSpeed;
#endif

bool startOk = true;
bool systemOk = false;

string msg;

void app_main(void) {

  console << "\n";

#if SERIAL_RADIO_ON
  // init console IO and radio console
  Uart uart; // SERIAL
  msg = uart.init();
  console << msg << "\n";
#endif

#if TEST_SERIAL2
  // Testcode for buffered Serial2 transfer
  console << "a:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5f6f7f8f9x1x\n";
  console << "b:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5f6f7f8f9x1\n";
  console << "c:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5f6f7f8f9x\n";
  console << "d:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5f6f7f8f9\n";
  console << "e:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5f6f7f8f\n";
  console << "f:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5f6f7f8\n";
  console << "g:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5f6f7\n";
  console << "h:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5f6f\n";
  console << "i:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5f6\n";
  console << "j:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5f\n";
  console << "k:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f5\n";
  console << "l:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4f\n";
  console << "m:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f4\n";
  console << "n:a1a2a3a4a5a6a7a8a9b1b2b3b4b5b6b7b8b9c1c2c3c4c5vc6c7c8c9d1d2d3d4d5d6d7d8d9e1e2e3e4e5e6e7e8e9f1f2f3f\n";
  console << "o:a1\n";
  console << "\n";
  console << "p:a1a2\n";
#endif

  console << "\n--------------------\n";
  console << "esp32dev + free RTOS\n";
  console << "Solar Energy Car Racers SER4 Controller: " << VERSION;
  console << "\n--------------------\n";

  // init arduino library
  initArduino();

  // report chip info
  console << "-chip info -------------------\n";
  // ???cchip_info();
  console << "-gpio pin settings ----------\n";
  msg = gpio.init();
  delay(200);
  console << msg << "\n";
  console << "-init bus systems ------------\n";
  // init buses
  msg = spiBus.init();
  console << msg << "\n";
  msg = oneWireBus.init();
  console << msg << "\n";
  msg = i2cBus.init();
  console << msg << "\n";
  //   msg = rtc.init();
  //   console << msg << "\n";
  //   engineerDisplay.print(msg + "\n");
  msg = engineerDisplay.init();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
// ---- init modules ----
#if INT_ON
  gpio.register_gpio_interrupt();
#endif

  // IOEXT
  msg = ioExt.init();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
  // console << "     Reread all IOs in foreced mode:";
  // ioExt.readAndHandlePins(PinHandleMode::FORCED);
  // console << carState.printIOs("", true, false) << "\n";

#if SD_ON
  sdCardDetectHandler();
  msg = sdCard.init();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#endif

  //--- SD card available ----------------------------
  carState.init_values();
  sdCard.open_log_file();
  //------from now config ini values can be used------

#if COMMANDHANDLER_ON
  msg = cmdHandler.init();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#endif
  msg = rtc.init();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
  msg = dac.init();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
  msg = adc.init();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#if DS_ON
  msg = ds.init();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#endif
#if PWM_ON
  msg = pwm.init();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#endif
  msg = canBus.init();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
  // #if CARCONTROL_ON
  //   carControl.init();
  //   console << msg << "\n";
  //   engineerDisplay.print(msg + "\n");
  // #endif
  // #if CARSPEED_ON
  //   msg = carSpeed.init();
  //   console << msg << "\n";
  //   engineerDisplay.print(msg + "\n");
  // #endif

  if (!startOk) {
    console << "ERROR in init sequence(s). System halted!\n";
    exit(0);
  }

  engineerDisplay.print("Startup sequence successful, creating FreeRTOS tasks.\n");
  console << "\n";
  console << "-----------------------------------------------------------------\n";
  console << "Startup sequence(s) successful. System creating FreeRTOS tasks...\n";
  console << "-----------------------------------------------------------------\n\n";
  // ---- create tasks ----

  // IOEXT
  carState.Indicator = INDICATOR::OFF;
  carState.ConstantModeOn = false; // #SAFETY#: deceleration unlock const mode
  carState.SdCardDetect = false;
  carState.ConstantMode = CONSTANT_MODE::SPEED;
  carState.Light = LIGHT::OFF;
  msg = ioExt.create_task(20, 100, 8000);
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");

#if DS_ON
  msg = ds.create_task();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#endif
  msg = adc.create_task(21, 50);
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#if PWM_ON
  msg = pwm.create_task();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#endif
#if INT_ON
  msg = gpio.create_task();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#endif
#if COMMANDHANDLER_ON
  msg = cmdHandler.create_task(15, 300, 8000);
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#endif
  msg = canBus.create_task();
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#if CARCONTROL_ON
  msg = carControl.init();
  console << msg << "\n";
  msg = carControl.create_task(10, 100, 6000);
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#endif
#if CARSPEED_ON
  msg = carSpeed.init();
  console << msg << "\n";
  msg = carSpeed.create_task(10, 250, 3000);
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
#endif
  string actTime = formatDateTime(rtc.read_rtc_datetime());
  msg = rtc.create_task(3, 1000 * 60 * 10); // 10 minutes
  console << msg << " RTC DateTime: " << actTime << "\n";
  engineerDisplay.print(fmt::format("{} {}\n", msg, getDateTime()));

  engineerDisplay.print("\nready");
  engineerDisplay.print(fmt::format(" at {}", getDateTime()));
  //--let the bootscreen visible for a moment ------------------
  engineerDisplay.print(".\nWaiting for start of life display: ");
  int waitAtConsoleView = 5;
  while (waitAtConsoleView-- > 0) {
    engineerDisplay.print(to_string(waitAtConsoleView));
    delay(1000);
    engineerDisplay.print("-");
  }
  engineerDisplay.print("start");
  engineerDisplay.set_DisplayStatus(DISPLAY_STATUS::ENGINEER_HALTED);
  //------------------------------------------------------------
  msg = engineerDisplay.create_task(10);
  console << msg << "\n";
  engineerDisplay.print(msg + "\n");
  driverDisplay.init();
  driverDisplay.set_DisplayStatus(DISPLAY_STATUS::DRIVER_SETUP);
  msg = driverDisplay.create_task(16);
  console << msg << driverDisplay.get_DisplayStatus_text() << "\n";
  engineerDisplay.print(msg + "\n");
  delay(1000);
  systemOk = true;

  console << "-----------------------------------------------------------------\n";
  console << "FreeRTOS tasks successfully created. System running.\n";
  console << "-----------------------------------------------------------------\n";

  ioExt.readAndHandlePins(PinHandleMode::FORCED);
}
