//
// EngineeringDisplay
//

#include <iostream>

#include <Adafruit_GFX.h>     // graphics library
#include <Adafruit_ILI9341.h> // display
#include <SPIBus.h>

#include <AbstractTask.h>
#include "../LocalFunctionsAndDevices.h"
#include <global_definitions.h>
#include "../definitions.h"

#include <CarState.h>
#include <Console.h>
// #include <ADC.h>
#include <Display.h>
#include <EngineerDisplay.h>
#include <Helper.h>

extern Console console;
extern SPIBus spiBus;
extern CarState carState;
extern Display display;
// extern Adafruit_ILI9341 tft;

EngineerDisplay::EngineerDisplay() { display.bgColor = ILI9341_LIGHTGREY; };
EngineerDisplay::~EngineerDisplay(){};

string EngineerDisplay::init() {
  console << "     Init '" << getName() << "'..." << NL;
  return re_init();
}

string EngineerDisplay::re_init(void) { return display_setup(); }

string EngineerDisplay::display_setup() {
  bool hasError = false;
  return fmt::format("[{}] {} initialized.  Screen 'ILI9341' {}x{}.     Status: {}", hasError ? "--" : "ok", getName(), display.height,
                     display.width, DISPLAY_STATUS_str[(int)carState.displayStatus]);
}

void EngineerDisplay::write_engineer_info(bool force) {
  if (EngineerInfo.Value != EngineerInfo.ValueLast || justInited || force) {
    string msg = EngineerInfo.get_recent_overtake_last();
    int color = EngineerInfo.getTextColor(); // getColorForInfoType(carState.EngineerInfoType);
    int len = msg.length();
    int textSize = EngineerInfo.getTextSize();
    if (len > 7 * 53)
      msg = msg.substr(0, 7 * 53 - 3) + "...";
    xSemaphoreTakeT(spiBus.mutex);
    display.tft->fillRect(4, 212, 316, 28, EngineerInfo.getBgColor());
    // display.tft->setFont(&FreeSans18pt7b);
    display.tft->setTextSize(textSize);
    display.tft->setTextWrap(true);
    display.tft->setTextColor(color);
    display.tft->setCursor(4, 212);
    display.tft->print(msg.c_str());
    xSemaphoreGive(spiBus.mutex);
  }
}

void EngineerDisplay::draw_display_background() {
  PhotoVoltaicOn.showLabel(display.tft);
  MotorOn.showLabel(display.tft);
  // BatteryOn.showLabel(display.tft);
  Mppt1.showLabel(display.tft);
  Mppt2.showLabel(display.tft);
  Mppt3.showLabel(display.tft);
  // Mppt4.showLabel(display.tft);
  EngineerInfo.showLabel(display.tft);
  BatteryStatus.showLabel(display.tft);
  BmsStatus.showLabel(display.tft);
  Temperature1.showLabel(display.tft);
  Temperature2.showLabel(display.tft);
  Temperature3.showLabel(display.tft);
  // Temperature4.showLabel(display.tft);
  TemperatureMin.showLabel(display.tft);
  TemperatureMax.showLabel(display.tft);
  BatteryCurrent.showLabel(display.tft);
  BatteryVoltage.showLabel(display.tft);
  VoltageAvg.showLabel(display.tft);
  VoltageMin.showLabel(display.tft);
  VoltageMax.showLabel(display.tft);
  EngineerInfo.showLabel(display.tft);
}

// void EngineerDisplay::print(string msg) { Display::print(msg); }

void EngineerDisplay::task(void *pvParams) {
  while (1) {
    switch (carState.displayStatus) {
    case DISPLAY_STATUS::ENGINEER_CONSOLE:
      break;

      // initializing states:
    case DISPLAY_STATUS::ENGINEER_SETUP:
      re_init();
      display_setup();
      justInited = true;
      carState.displayStatus = DISPLAY_STATUS::ENGINEER_BACKGROUND;
      break;
    case DISPLAY_STATUS::ENGINEER_BACKGROUND:
      display.clear_screen(ILI9341_ORANGE);
      draw_display_background();
      set_sleep_polling(500);
      carState.displayStatus = DISPLAY_STATUS::ENGINEER_RUNNING;
      break;

    // working state:
    case DISPLAY_STATUS::ENGINEER_RUNNING:
      display.lifeSign();
      // BatteryOn.Value = carState.BatteryOn;
      PhotoVoltaicOn.Value = carState.PhotoVoltaicOn;
      MotorOn.Value = carState.MotorOn;
      BatteryVoltage.Value = carState.BatteryVoltage;
      BatteryCurrent.Value = carState.BatteryCurrent;
      Temperature1.Value = carState.T1;
      Temperature2.Value = carState.T2;
      Temperature3.Value = carState.T3;
      // Temperature4.Value = carState.T4;
      EngineerInfo.Value = carState.EngineerInfo;
      TemperatureMin.Value = carState.Tmin;
      TemperatureMax.Value = carState.Tmax;
      Mppt1.Value = carState.Mppt1Current;
      Mppt2.Value = carState.Mppt2Current;
      Mppt3.Value = carState.Mppt3Current;

      VoltageMin.Value = carState.Umin;
      VoltageAvg.Value = carState.Uavg;
      VoltageMax.Value = carState.Umax;

      // BatteryOn.showValue(display.tft);
      PhotoVoltaicOn.showValue(display.tft);
      MotorOn.showValue(display.tft);

      Mppt1.showValue(display.tft);
      Mppt2.showValue(display.tft);
      Mppt3.showValue(display.tft);
      // Mppt4.showValue(display.tft);

      BatteryStatus.showValue(display.tft);
      BmsStatus.showValue(display.tft);

      BatteryCurrent.showValue(display.tft);
      BatteryVoltage.showValue(display.tft);
      VoltageAvg.showValue(display.tft);
      VoltageMin.showValue(display.tft);
      VoltageMax.showValue(display.tft);

      Temperature1.showValue(display.tft);
      Temperature2.showValue(display.tft);
      Temperature3.showValue(display.tft);
      // Temperature4.showValue(display.tft);
      TemperatureMin.showValue(display.tft);
      TemperatureMax.showValue(display.tft);

      // EngineerInfo.showValue(display.tft, true);
      write_engineer_info();

      justInited = false;
      break;

    default:
      // ignore others
      break;
    }
    taskSuspend();
  }
}
