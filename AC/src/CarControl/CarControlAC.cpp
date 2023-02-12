//
// CarControlx: Main control module of SER4
//
#include <definitions.h>

#include <fmt/core.h>
#include <fmt/printf.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <CAN.h>
#include <Wire.h> // I2C

#include <CANBus.h>
#include <CarControl.h>
// #include <CarSpeed.h>
#include <CarState.h>
// #include <ConfigFile.h>
#include <Console.h>
#include <DriverDisplay.h>
#include <EngineerDisplay.h>
#include <Helper.h>
#include <I2CBus.h>
#include <SDCard.h>

extern CANBus canBus;
extern CarControl carControl;
extern CarState carState;
extern Console console;
// extern DriverDisplay driverDisplay;
// extern EngineerDisplay engineerDisplay;
extern I2CBus i2cBus;
extern SDCard sdCard;

extern bool SystemInited;

using namespace std;

unsigned long millisNextStampCsv = millis();
unsigned long millisNextStampSnd = millis();

// ------------------
// FreeRTOS functions

string CarControl::re_init() { return init(); }

string CarControl::init() {
  bool hasError = false;
  console << "[  ] Init 'CarControl'...";
  justInited = true;
  // mutex = xSemaphoreCreateMutex();
  // xSemaphoreGive(mutex);
  carState.AccelerationDisplay = -99;
  console << "done.\n";
  return fmt::format("[{}] CarControl initialized.", hasError ? "--" : "ok");
}

void CarControl::exit(void) {
  // TODO
}

bool CarControl::read_nextScreenButton() {
  if (!SystemInited)
    return false;

  int button1pressed = !digitalRead(ESP32_AC_BUTTON_AC_NEXT);
  if (!button1pressed)
    return false;

  switch (carState.displayStatus) {
  case DISPLAY_STATUS::ENGINEER_RUNNING:
    carState.displayStatus = DISPLAY_STATUS::DRIVER_SETUP;
    console << "Switch Next Screen toggle: switch from eng --> driver\n";
    break;
  case DISPLAY_STATUS::DRIVER_RUNNING:
    carState.displayStatus = DISPLAY_STATUS::ENGINEER_SETUP;
    console << "Switch Next Screen toggle: switch from driver --> engineer\n";
    break;
  default:
    break;
  }
  return true;
}

bool CarControl::read_sd_card_detect() {
  if (!SystemInited)
    return false;

  bool sdCardDetectOld = carState.SdCardDetect;
  carState.SdCardDetect = digitalRead(ESP32_AC_SD_DETECT);

  if (carState.SdCardDetect && !sdCardDetectOld) {
    console << "SD card detected, try to start logging...\n";
    string msg = sdCard.init();
    console << msg << "\n";
    string state = carState.csv("Recent State", true); // with header
    sdCard.write(state);
    sdCard.open_log_file();
  } else if (!carState.SdCardDetect && sdCardDetectOld) {
    console << "SD card removed.\n";
  }
  return carState.SdCardDetect;
}

bool CarControl::read_const_mode() {
  if (!SystemInited)
    return false;

  bool button2pressed = !digitalRead(ESP32_AC_BUTTON_CONST_MODE); // switch constant mode (Speed, Power)
  if (!button2pressed)
    return false;

  carState.ConstantMode = (carState.ConstantMode == CONSTANT_MODE::POWER) ? CONSTANT_MODE::SPEED : CONSTANT_MODE::POWER;

  return true;
}

void CarControl::task(void *pvParams) {
  while (1) {
    if (SystemInited) {

      bool button2pressed = read_nextScreenButton();
      bool sd_detected = read_sd_card_detect();
      read_const_mode();

      delay(1);

      uint8_t constantMode = carState.ConstantMode == CONSTANT_MODE::SPEED ? 0 : 1;
      canBus.writePacket(AC_BASE_ADDR | 0x00,
                         carState.LifeSign,      // LifeSign
                         (uint16_t)constantMode, // switch constant mode Speed / Power
                         (uint16_t)0,            // empty
                         (uint16_t)0             // empty
      );
      delay(1);
      if (carControl.verboseModeDebug)
        console << fmt::format("[{:02d}|{:02d}] CAN.PacketId=0x{:03x}-S-data:LifeSign={:4x}, button2 = {:1x} ", canBus.availiblePackets(),
                               canBus.getMaxPacketsBufferUsage(), AC_BASE_ADDR | 0x00, carState.LifeSign, button2pressed)
                << NL;

      // one data row per second
      if ((millis() > millisNextStampCsv) || (millis() > millisNextStampSnd)) {
        // console << fmt::format("ready:{},next={}, millis={}\n", sdCard.isReadyForLog(), millisNextStampCsv, millis());
        string record = carState.csv();
        if (sdCard.isReadyForLog() && millis() > millisNextStampCsv) {
          if (sdCard.verboseModeDebug)
            console << "d: " << record << NL;
          sdCard.write(record);
          millisNextStampCsv = millis() + carState.LogInterval;
        }
        if (sdCard.verboseModeDebug) {
          if (millis() > millisNextStampSnd) {
            millisNextStampSnd = millis() + carState.CarDataSendPeriod;
          }
        }
      }
    }
    taskSuspend();
  }
}
