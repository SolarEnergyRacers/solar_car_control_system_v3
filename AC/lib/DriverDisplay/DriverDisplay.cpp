/***
 *
 * Driver's Display
 *
 ***/

#include <global_definitions.h>
#include "../definitions.h"

// standard libraries
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <AbstractTask.h>
#include "../LocalFunctionsAndDevices.h"

// #include <ADC.h>
#include <CarState.h>
#include <Console.h>
#include <Display.h>
#include <DriverDisplay.h>
#include <Helper.h>

#include <Adafruit_GFX.h>     // graphics library
#include <Adafruit_ILI9341.h> // display
// #include <Fonts/FreeMono12pt7b.h>
// #include <Fonts/FreeMono18pt7b.h>
// #include <Fonts/FreeMono24pt7b.h>
// #include <Fonts/FreeMono9pt7b.h>
// #include <Fonts/FreeMonoBold24pt7b.h>
// #include <Fonts/FreeSans12pt7b.h>
// #include <Fonts/FreeSans18pt7b.h>
// #include <Fonts/FreeSans24pt7b.h>
// #include <Fonts/FreeSans9pt7b.h>

extern Console console;
extern SPIBus spiBus;
extern CarState carState;
extern Display display;
// extern Adafruit_ILI9341 tft;

DriverDisplay::DriverDisplay() { display.bgColor = ILI9341_BLACK; };
DriverDisplay::~DriverDisplay(){};

string DriverDisplay::init() {
  console << "     Init '" << getName() << "'..." << NL;
  return re_init();
}

string DriverDisplay::re_init(void) { return display_setup(); }

string DriverDisplay::display_setup() {
  bool hasError = false;
  return fmt::format("[{}] {} initialized.  Screen 'ILI9341' {}x{}.     Status: {}", hasError ? "--" : "ok", getName(), display.height,
                     display.width, DISPLAY_STATUS_str[(int)carState.displayStatus]);
}

int DriverDisplay::getColorForInfoType(INFO_TYPE type) {
  int color;
  switch (type) {
  case INFO_TYPE::ERROR:
    color = ILI9341_RED;
    break;

  case INFO_TYPE::WARN:
    color = ILI9341_GREENYELLOW;
    break;

  case INFO_TYPE::STATUS:
    color = ILI9341_GREEN;
    break;

  case INFO_TYPE::INFO:
  default:
    color = ILI9341_WHITE;
    break;
  }
  return color;
}

void DriverDisplay::speedCheck(int speed) {
  if (speed < 50) {
    arrow_increase(true);
  } else {
    arrow_increase(false);
  }
  if (speed > 80) {
    arrow_decrease(true);
  } else {
    arrow_decrease(false);
  }
}

// write color of the border of the main display
void DriverDisplay::draw_display_border(int color) {
  xSemaphoreTakeT(spiBus.mutex);
  display.tft->drawRoundRect(0, mainFrameX, display.tft->width(), display.tft->height() - mainFrameX, 8, color);
  xSemaphoreGive(spiBus.mutex);
}

// write color of the border of the speed display
#define SPEED_UNIT "km/h"
void DriverDisplay::draw_speed_border(int color) {
  int vWidth = strlen(SPEED_UNIT) * speedUnitTextSize * 6;
  speedUnitX = speedFrameX + speedFrameSizeX - vWidth;
  xSemaphoreTakeT(spiBus.mutex);
  display.tft->drawRoundRect(speedFrameX, speedFrameY, speedFrameSizeX, speedFrameSizeY, 4, color);
  display.tft->setCursor(speedUnitX, speedUnitSpeedY);
  display.tft->setTextSize(speedUnitTextSize);
  display.tft->setTextColor(ILI9341_GREENYELLOW);
  display.tft->print(SPEED_UNIT);
  xSemaphoreGive(spiBus.mutex);
}

// write color of the border of the acceleration display
#define ACCELERATION_UNIT "%"
void DriverDisplay::draw_acceleration_border(int color) {
  accFrameSizeX = speedFrameX - 3;
  int accUnitX = accFrameX + accFrameSizeX - 4 - display.getPixelWidthOfTexts(constantModeTextSize, ACCELERATION_UNIT, ACCELERATION_UNIT);
  ;
  xSemaphoreTakeT(spiBus.mutex);
  display.tft->drawRoundRect(accFrameX, accFrameY, accFrameSizeX, accFrameSizeY, 4, color);
  display.tft->setCursor(accUnitX, speedUnitAccelY);
  display.tft->setTextSize(speedUnitTextSize);
  display.tft->setTextColor(ILI9341_GREENYELLOW);
  display.tft->print(ACCELERATION_UNIT);
  xSemaphoreGive(spiBus.mutex);
}

// write color of the border of the constand mode target display
void DriverDisplay::draw_target_value_border(int color) {
  targetValueFrameX = speedFrameX + speedFrameSizeX + 1;
  targetValueFrameSizeX = display.tft->width() - accFrameSizeX - speedFrameSizeX - 6;
  xSemaphoreTakeT(spiBus.mutex);
  display.tft->drawRoundRect(targetValueFrameX, targetValueFrameY, targetValueFrameSizeX, targetValueFrameSizeY, 4, color);
  xSemaphoreGive(spiBus.mutex);
}

void DriverDisplay::draw_display_background() {
  // xSemaphoreTakeT(spiBus.mutex);
  // infoFrameSizeX = display.tft->width();
  // xSemaphoreGive(spiBus.mutex);
  infoFrameSizeX = display.width;
  speedFrameX = (infoFrameSizeX - speedFrameSizeX) / 2;

  draw_display_border(ILI9341_GREEN);
  draw_speed_border(ILI9341_YELLOW);
  draw_acceleration_border(ILI9341_YELLOW);
  draw_target_value_border(ILI9341_YELLOW);
}

void DriverDisplay::_arrow_increase(int color) {
  int x = speedFrameX;
  int y = speedFrameY - 3;

  xSemaphoreTakeT(spiBus.mutex);
  display.tft->fillTriangle(x, y, x + speedFrameSizeX / 2, y, x + speedFrameSizeX / 4, y - 10, color);
  xSemaphoreGive(spiBus.mutex);
}

void DriverDisplay::_arrow_decrease(int color) {
  int x = speedFrameX;
  int y = speedFrameY + speedFrameSizeY + 3;

  xSemaphoreTakeT(spiBus.mutex);
  display.tft->fillTriangle(x, y, x + speedFrameSizeX / 2, y, x + speedFrameSizeX / 4, y + 10, color);
  xSemaphoreGive(spiBus.mutex);
}

// show the slower arrow (red under the speed display)
void DriverDisplay::arrow(SPEED_ARROW incDecOff) {
  switch (incDecOff) {
  default:
  case SPEED_ARROW::OFF:
    arrow_increase(false);
    arrow_decrease(false);
    break;
  case SPEED_ARROW::INCREASE:
    arrow_increase(true);
    break;
  case SPEED_ARROW::DECREASE:
    arrow_decrease(true);
    break;
  }
}

void DriverDisplay::arrow_decrease(bool on) {
  int color = display.bgColor;
  if (on) {
    arrow_increase(false);
    color = ILI9341_RED;
  }
  _arrow_decrease(color);
}

// show the faster arrow (green above the speed display)
void DriverDisplay::arrow_increase(bool on) {
  int color = display.bgColor;
  if (on) {
    arrow_decrease(false);
    color = ILI9341_YELLOW;
  }
  _arrow_increase(color);
}

#define SPEED_STRING "Target SPEED"
#define POWER_STRING "Target POWER"
void DriverDisplay::constant_drive_mode_show() {
  CONSTANT_MODE mode = ConstantMode.get_recent_overtake_last();
  bool isOn = ConstantModeOn.get_recent_overtake_last();
  int width = display.getPixelWidthOfTexts(constantModeTextSize, SPEED_STRING, POWER_STRING) + 4;
  if (!isOn) {
    xSemaphoreTakeT(spiBus.mutex);
    display.tft->fillRoundRect(constantModeX - 2, constantModeY - 2, width, 11, 3, ILI9341_BLACK);
    xSemaphoreGive(spiBus.mutex);
    return;
  }

  xSemaphoreTakeT(spiBus.mutex);
  display.tft->setCursor(constantModeX, constantModeY);
  display.tft->setTextSize(constantModeTextSize);
  display.tft->setTextColor(ILI9341_BLACK);
  if (mode == CONSTANT_MODE::POWER) {
    display.tft->fillRoundRect(constantModeX - 2, constantModeY - 2, width, 11, 3, ILI9341_GREEN);
    display.tft->print(POWER_STRING);
  } else {
    display.tft->fillRoundRect(constantModeX - 2, constantModeY - 2, width, 11, 3, ILI9341_YELLOW);
    display.tft->print(SPEED_STRING);
  }
  xSemaphoreGive(spiBus.mutex);
}

#define MAX_POTENTIOMETER_VALUE 22000
void DriverDisplay::step_width_show() {
  carState.ConstSpeedIncrease = transformArea(1, 21, 0, MAX_POTENTIOMETER_VALUE, carState.Potentiometer);
  carState.ConstPowerIncrease = transformArea(1, 501, 0, MAX_POTENTIOMETER_VALUE, carState.Potentiometer);
  string valueString = fmt::format("{:2d}km/h {:4d}W", carState.ConstSpeedIncrease, carState.ConstPowerIncrease);
  int width = display.getPixelWidthOfText(constantModeTextSize, valueString) + 6;

  xSemaphoreTakeT(spiBus.mutex);
  display.tft->fillRoundRect(controlModeStepX - 2, controlModeStepY - 2, width, 11, 3, ILI9341_BLACK);
  display.tft->setCursor(controlModeStepX, controlModeStepY);
  display.tft->setTextSize(controlModeStepTextSize);
  display.tft->setTextColor(ILI9341_GREENYELLOW);
  display.tft->print(valueString.c_str());
  xSemaphoreGive(spiBus.mutex);
}
// #define ECO_MODE_STRING " eco"
// #define PWR_MODE_STRING " power"
// void DriverDisplay::eco_power_mode_show() {
//   bool isEco = EcoModeOn.get_recent_overtake_last();
//   int width = getPixelWidthOfTexts(ecoPwrModeTextSize, ECO_MODE_STRING, PWR_MODE_STRING) + 4;

//   xSemaphoreTakeT(spiBus.mutex);
//   display.tft->fillRoundRect(ecoPwrModeX - 2, ecoPwrModeY - 2, width, 18, 3, ILI9341_BLACK);
//   display.tft->setCursor(ecoPwrModeX, ecoPwrModeY);
//   display.tft->setTextSize(ecoPwrModeTextSize);
//   if (isEco) {
//     display.tft->setTextColor(ILI9341_GREEN);
//     display.tft->print(ECO_MODE_STRING);
//   } else {
//     display.tft->setTextColor(ILI9341_BLUE);
//     display.tft->print(PWR_MODE_STRING);
//   }
//   xSemaphoreGive(spiBus.mutex);
// }

// #define LIGHT1_STRING "Light"
// #define LIGHT2_STRING "LIGHT"
// void DriverDisplay::_hide_light() {
//   int width = getPixelWidthOfTexts(lightTextSize, LIGHT1_STRING, LIGHT2_STRING) + 4;
//   xSemaphoreTakeT(spiBus.mutex);
//   display.tft->fillRoundRect(lightX - 2, lightY - 2, width, 18, 3, ILI9341_BLACK);
//   xSemaphoreGive(spiBus.mutex);
// }

// void DriverDisplay::show_light() {
//   LIGHT light = Light.get_recent_overtake_last();
//   if (light == LIGHT::OFF) {
//     _hide_light();
//     return;
//   }
// int width = getPixelWidthOfTexts(lightTextSize, LIGHT1_STRING, LIGHT2_STRING) + 4;
// xSemaphoreTakeT(spiBus.mutex);
// display.tft->setCursor(lightX, lightY);
// display.tft->setTextSize(lightTextSize);
// display.tft->fillRoundRect(lightX - 2, lightY - 2, width, 18, 3, ILI9341_BLACK);
// if (light == LIGHT::L1) {
//   display.tft->setTextColor(ILI9341_YELLOW);
//   display.tft->print(LIGHT1_STRING);
// } else {
//   display.tft->setTextColor(ILI9341_BLUE);
//   display.tft->print(LIGHT2_STRING);
// }
// xSemaphoreGive(spiBus.mutex);
// }

#define FORWARD_STRING ""
#define BACKWARD_STRING "Backward"
void DriverDisplay::write_drive_direction() {
  DRIVE_DIRECTION direction = DriveDirection.get_recent_overtake_last();
  int width = display.getPixelWidthOfTexts(driveDirectionTextSize, FORWARD_STRING, BACKWARD_STRING) + 4;

  xSemaphoreTakeT(spiBus.mutex);
  display.tft->fillRoundRect(driveDirectionX - 2, driveDirectionY - 2, width, 18, 3, ILI9341_BLACK);
  display.tft->setTextSize(driveDirectionTextSize);
  display.tft->setCursor(driveDirectionX, driveDirectionY);
  if (direction == DRIVE_DIRECTION::FORWARD) {
    display.tft->setTextColor(ILI9341_YELLOW);
    display.tft->print(FORWARD_STRING);
  } else {
    display.tft->setTextColor(ILI9341_RED);
    display.tft->print(BACKWARD_STRING);
  }
  xSemaphoreGive(spiBus.mutex);
}

// Write the speed in the centre frame: 0...999
void DriverDisplay::write_speed() {
  int value = Speed.get_recent_overtake_last();
  if (value >= 0 || value <= 999) {
    if (justInited)
      speedLast = -1;
    speedLast = display.write_nat_999(speedFrameX + 9, speedFrameY + 10, speedLast, value, speedTextSize, ILI9341_WHITE);
  }
}

// acceleration value: -99...+99
void DriverDisplay::write_acceleration() {
  int value = Acceleration.get_recent_overtake_last();
  if (value <= -MAX_ACCELERATION_DISPLAY_VALUE)
    value = -99;
  if (value > MAX_ACCELERATION_DISPLAY_VALUE)
    value = 99;
  accelerationLast =
      display.write_ganz_99(accFrameX + 4, accFrameY + 4, accelerationLast, value, accTextSize, justInited, ILI9341_GREENYELLOW);
}

// commented out code is preparation for font usage
void DriverDisplay::write_driver_info() {
  if (DriverInfo.Value != DriverInfo.ValueLast || justInited) {
    string msg = DriverInfo.get_recent_overtake_last();
    int color = getColorForInfoType(carState.DriverInfoType);
    int len = msg.length();
    int textSize = infoTextSize;
    if (len > 2 * 17)
      textSize = 2;
    if (len > 4 * 26)
      textSize = 1;
    if (len > 7 * 53)
      msg = msg.substr(0, 7 * 53 - 3) + "...";
    xSemaphoreTakeT(spiBus.mutex);
    display.tft->fillRect(infoFrameX, infoFrameY, infoFrameSizeX, infoFrameSizeY, display.bgColor);
    // display.tft->setFont(&FreeSans18pt7b);
    display.tft->setTextSize(textSize);
    display.tft->setTextWrap(true);
    display.tft->setTextColor(color);
    display.tft->setCursor(infoFrameX, infoFrameY);
    display.tft->print(msg.c_str());
    xSemaphoreGive(spiBus.mutex);
  }
}

void DriverDisplay::task(void *pvParams) {
  while (1) {
    switch (carState.displayStatus) {
    case DISPLAY_STATUS::ENGINEER_CONSOLE:
      break;

    // initializing states:
    case DISPLAY_STATUS::DRIVER_SETUP:
      display_setup();
      justInited = true;
      carState.displayStatus = DISPLAY_STATUS::DRIVER_BACKGROUND;
      break;

    case DISPLAY_STATUS::DRIVER_BACKGROUND:
      display.clear_screen(display.bgColor);
      draw_display_background();
      BatteryVoltage.showLabel(display.tft);
      BatteryVoltage.set_epsilon(0.1);
      // BatteryOn.showLabel(tft);
      PhotoVoltaicCurrent.showLabel(display.tft);
      PhotoVoltaicCurrent.set_epsilon(0.1);
      PhotoVoltaicOn.showLabel(display.tft);
      MotorCurrent.showLabel(display.tft);
      MotorCurrent.set_epsilon(0.1);
      MotorOn.showLabel(display.tft);
      TargetSpeedPower.showLabel(display.tft);
      // DateTimeStamp.showLabel(display.tft);
      set_sleep_polling(600);
      carState.displayStatus = DISPLAY_STATUS::DRIVER_RUNNING;
      break;

    // working state:
    case DISPLAY_STATUS::DRIVER_RUNNING:
      display.lifeSign();
      DriverInfo.Value = carState.DriverInfo;
      if (DriverInfo.Value != DriverInfo.ValueLast || justInited) {
        write_driver_info();
      }
      Speed.Value = carState.Speed;
      if (Speed.is_changed() || justInited) {
        write_speed();
      }
      if (carState.SpeedArrow == SPEED_ARROW::DECREASE) {
        arrow_increase(false);
        arrow_decrease(true);
      } else if (carState.SpeedArrow == SPEED_ARROW::INCREASE) {
        arrow_increase(true);
        arrow_decrease(false);
      } else {
        arrow_increase(false);
        arrow_decrease(false);
      }
      Acceleration.Value = carState.AccelerationDisplay;
      if (Acceleration.is_changed() || justInited) {
        write_acceleration();
      }
      TargetSpeedPower.Value = carState.ConstantMode == CONSTANT_MODE::SPEED ? carState.TargetSpeed : carState.TargetPower;
      if (TargetSpeedPower.is_changed() || justInited) {
        // write_target_value()
        if (carState.ConstantMode == CONSTANT_MODE::SPEED)
          TargetSpeedPower.change_format("%5.0f");
        else
          TargetSpeedPower.change_format("%3.1f");
        TargetSpeedPower.showValue(display.tft);
      }

      DriveDirection.Value = carState.DriveDirection;
      if (DriveDirection.Value != DriveDirection.ValueLast || justInited) {
        write_drive_direction();
      }
      // Light.Value = carState.Light;
      // if (Light.Value != Light.ValueLast || justInited) {
      //   show_light();
      // }
      ConstantMode.Value = carState.ConstantMode;
      ConstantModeOn.Value = carState.ConstantModeOn;
      if (ConstantMode.Value != ConstantMode.ValueLast || ConstantModeOn.Value != ConstantModeOn.ValueLast || justInited) {
        constant_drive_mode_show();
      }

      // if (carState.Speed + 1 < carState.TargetSpeed)
      //   arrow(SPEED_ARROW::INCREASE);
      // else if (carState.Speed - 1 > carState.TargetSpeed)
      //   arrow(SPEED_ARROW::DECREASE);
      // else
      //   arrow(SPEED_ARROW::OFF);

      StepSize.Value = carState.Potentiometer;
      if (StepSize.Value != StepSize.ValueLast || justInited) {
        StepSize.get_recent_overtake_last();
        step_width_show();
      }

      // EcoModeOn.Value = carState.EcoOn;
      // if (EcoModeOn.Value != EcoModeOn.ValueLast || justInited) {
      //   eco_power_mode_show();
      // }

      BatteryVoltage.Value = carState.BatteryVoltage;
      if (BatteryVoltage.is_changed() || justInited) {
        BatteryVoltage.showValue(display.tft);
      }
      // BatteryOn.Value = carState.BatteryOn;
      // if (BatteryOn.is_changed() || justInited) {
      //   BatteryOn.showValue(tft);
      // }
      // TODO: PhotoVoltaicCurrent.Value = carState.Mppt1Current + carState.Mppt2Current + carState.Mppt3Current;
      PhotoVoltaicCurrent.Value = carState.PhotoVoltaicCurrent;
      if (PhotoVoltaicCurrent.is_changed() || justInited) {
        PhotoVoltaicCurrent.showValue(display.tft);
      }
      PhotoVoltaicOn.Value = carState.PhotoVoltaicOn;
      if (PhotoVoltaicOn.is_changed() || justInited) {
        PhotoVoltaicOn.showValue(display.tft);
      }
      MotorCurrent.Value = carState.MotorCurrent;
      if (MotorCurrent.is_changed() || justInited) {
        MotorCurrent.showValue(display.tft);
      }
      MotorOn.Value = carState.MotorOn;
      if (MotorOn.is_changed() || justInited) {
        MotorOn.showValue(display.tft);
      }
      // DateTimeStamp.Value = getTime();
      // DateTimeStamp.showValue(tft);
      justInited = false;
      break;

    default:
      // ignore others
      break;
    }
    taskSuspend();
  }
}
