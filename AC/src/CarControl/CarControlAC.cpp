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
#include <CarState.h>
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
unsigned long millisNextEngineerInfoCleanup = millis();

// ------------------
// FreeRTOS functions

string CarControl::re_init() { return init(); }

string CarControl::init() {
  bool hasError = false;
  justInited = true;
  carState.AccelerationDisplay = -99;
  return fmt::format("[{}] {} initialized.", hasError ? "--" : "ok", getName());
}

void CarControl::exit(void) {
  // TODO
}

bool CarControl::read_nextScreenButton() {
  if (!SystemInited)
    return false;

  int button_nextScreen_pressed = !digitalRead(ESP32_AC_BUTTON_AC_NEXT);
  if (!button_nextScreen_pressed)
    return false;

  switch (carState.displayStatus) {
  case DISPLAY_STATUS::ENGINEER_RUNNING:
    carState.displayStatus = DISPLAY_STATUS::DRIVER_SETUP;
    console << "Switch Next Screen toggle: switch from eng --> driver" << NL;
    break;
  case DISPLAY_STATUS::DRIVER_RUNNING:
    carState.displayStatus = DISPLAY_STATUS::ENGINEER_SETUP;
    console << "Switch Next Screen toggle: switch from driver --> engineer" << NL;
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
  sdCard.update_sd_card_detect();
  //console << "." << carState.SdCardDetect << sdCardDetectOld << "_";

  if (carState.SdCardDetect && !sdCardDetectOld) {
    carState.EngineerInfo = "SD card detected, try to start logging...";
    console << "     " << carState.EngineerInfo << NL;
    string msg = sdCard.init();
    console << msg << NL;
    if (sdCard.check_log_file()) {
      string state = carState.csv("Recent State", true); // with header
      sdCard.write_log_line(state);
    }
  } else if (!carState.SdCardDetect && sdCardDetectOld) {
    carState.EngineerInfo = "SD card removed.";
    console << "     " << carState.EngineerInfo << NL;
  }

  return carState.SdCardDetect;
}

bool CarControl::read_const_mode_and_mountrequest() {
  if (!SystemInited)
    return false;

  bool button_constMode_pressed = !digitalRead(ESP32_AC_BUTTON_CONST_MODE); // switch constant mode (Speed, Power)
  if (!button_constMode_pressed)
    return false;

  switch (carState.displayStatus) {
  case DISPLAY_STATUS::ENGINEER_RUNNING:
    if (sdCard.isMounted()) {
      sdCard.unmount();
    } else {
      sdCard.mount();
      string state = carState.csv("Recent State just after mounting", true); // with header
      sdCard.write_log(state);
    }
    break;
  case DISPLAY_STATUS::DRIVER_RUNNING:
    carState.ConstantMode = (carState.ConstantMode == CONSTANT_MODE::POWER) ? CONSTANT_MODE::SPEED : CONSTANT_MODE::POWER;
    break;
  default:
    break;
  }

  return true;
}

int cyclecounter = 0;
unsigned long carStateLifeSignLast = 0;
uint8_t carStateConstantModeLast = 0;
string carStateEngineerInfoLast = "";

void CarControl::task(void *pvParams) {
  while (1) {
    if (SystemInited) {
      cyclecounter++;
      if (cyclecounter > 50) {
        // console << "." << NL;
        cyclecounter = 0;
      }
      bool button_nextScreen_pressed = read_nextScreenButton();
      // vTaskDelay(10);
      read_sd_card_detect();
      // vTaskDelay(10);
      read_const_mode_and_mountrequest();
      // vTaskDelay(10);
#ifndef SUPRESS_CAN_OUT_AC
      uint8_t constantMode = carState.ConstantMode == CONSTANT_MODE::SPEED ? 0 : 1;
      if (carStateLifeSignLast != carState.LifeSign || carStateConstantModeLast != constantMode) {
        canBus.writePacket(AC_BASE_ADDR | 0x00,
                           carState.LifeSign,      // LifeSign
                           (uint16_t)constantMode, // switch constant mode Speed / Power
                           (uint16_t)0,            // empty
                           (uint16_t)0             // empty
        );
        carStateLifeSignLast = carState.LifeSign;
        carStateConstantModeLast = constantMode;
      }
      // vTaskDelay(10);
#endif
      if (carControl.verboseModeCarControlDebug)
        console << fmt::format("[{:02d}|{:02d}] CAN.PacketId=0x{:03x}-S-data:LifeSign={:4x}, button2 = {:1x} ", canBus.availiblePackets(),
                               canBus.getMaxPacketsBufferUsage(), AC_BASE_ADDR | 0x00, carState.LifeSign, button_nextScreen_pressed)
                << NL;
      // self destroying engineer info
      if (carState.EngineerInfo.compare(carStateEngineerInfoLast) != 0) {
        carStateEngineerInfoLast = carState.EngineerInfo;
        millisNextEngineerInfoCleanup = millis() + 4000;
      }
      if (millis() > millisNextEngineerInfoCleanup && carState.EngineerInfo.length() > 0) {
        carStateEngineerInfoLast = carState.EngineerInfo = "";
      }
      //  log file one data row per LogInterval
      if ((millis() > millisNextStampCsv) || (millis() > millisNextStampSnd)) {
        string record = carState.csv();
        if (sdCard.isMounted() && millis() > millisNextStampCsv) {
          millisNextStampCsv = millis() + carState.LogInterval;
          if (sdCard.verboseModeSdCard)
            console << "d: Interval=" << carState.LogInterval << ", Rec: " << record << NL;
          sdCard.write_log_line(record);
        }
        // vTaskDelay(10);
        // if (verboseModeRadioSend) {
        //   if (millis() > millisNextStampSnd) {
        //     // send serail2 --> radio
        //     millisNextStampSnd = millis() + carState.CarDataSendPeriod;
        //   }
        // }
      }
    }
    taskSuspend();
  }
}
