//
// DriverDisplay
//

#ifndef SER_DRIVER_DISPLAY_C_H
#define SER_DRIVER_DISPLAY_C_H

#include <ADS1X15.h>
#include <Adafruit_ILI9341.h>

#include <CarState.h>
#include <Display.h>
#include <DisplayValue.h>
#include <LocalFunctionsAndDevices.h>

using namespace std;

class DriverDisplay : public AbstractTask {
public:
  DriverDisplay();
  ~DriverDisplay();
  // RTOS task
  string init(void);
  string re_init(void);
  void exit(void){};
  void task(void *pvParams);

  string getName(void) { return "DriverDisplay"; };
  bool verboseModeDriver = false;

private:
  // int x, int y, string label, string format, string unit, int textColor, int bgColor, int textSize
  DisplayValue<string> DriverInfo = DisplayValue<string>(0, 0, "", "%s", "");
  DisplayValue<INFO_TYPE> DriverInfoType = DisplayValue<INFO_TYPE>(0, 0, "", "%s", "");
  DisplayValue<DRIVE_DIRECTION> DriveDirection = DisplayValue<DRIVE_DIRECTION>(0, 0, "", "%s", "");
  DisplayValue<LIGHT> Light = DisplayValue<LIGHT>(0, 0, "", "%s", "");
  DisplayValue<CONSTANT_MODE> ConstantMode = DisplayValue<CONSTANT_MODE>(0, 0, "", "%s", "");
  DisplayValue<bool> ConstantModeOn = DisplayValue<bool>(0, 0, "", "%s", "");

  DisplayValue<bool> EcoModeOn = DisplayValue<bool>(0, 0, "", "%s", "");

  DisplayValue<int> Speed = DisplayValue<int>(0, 0, "", "%d", "", ILI9341_WHITE, ILI9341_BLACK);
  DisplayValue<int> Acceleration = DisplayValue<int>(0, -1, "", "%d", "", ILI9341_WHITE, ILI9341_BLACK);
  DisplayValue<int> StepSize = DisplayValue<int>(10, 200, "", "%s", "", ILI9341_YELLOW, ILI9341_BLACK, 1);
  // DisplayValue<string> DateTimeStamp = DisplayValue<string>(10, 168, "", "%s", "", ILI9341_YELLOW, ILI9341_BLACK, 1);
  // this format will be changed dynamically in IOExt event handler in dependency of CONSTANT_MODE:
  DisplayValue<float> TargetSpeedPower = DisplayValue<float>(240, 130, " ", "%5.0f", "", ILI9341_WHITE, ILI9341_BLACK, 2);

  DisplayValue<float> MotorCurrent = DisplayValue<float>(10, 180, "Motor:", "%5.1f", "A", ILI9341_ORANGE, ILI9341_BLACK);
  DisplayValue<bool> MotorOn = DisplayValue<bool>(160, 180, "-", "%3s", "", ILI9341_MAROON, ILI9341_BLACK);
  DisplayValue<float> BatteryVoltage = DisplayValue<float>(10, 200, "Bat  :", "%5.1f", "V", ILI9341_ORANGE, ILI9341_BLACK);
  // DisplayValue<bool> BatteryOn = DisplayValue<bool>(160, 200, "-", "%3s", "", ILI9341_MAROON, ILI9341_BLACK);
  DisplayValue<float> PhotoVoltaicCurrent = DisplayValue<float>(10, 220, "PV   :", "%5.1f", "A", ILI9341_ORANGE, ILI9341_BLACK);
  DisplayValue<bool> PhotoVoltaicOn = DisplayValue<bool>(160, 220, "-", "%3s", "", ILI9341_MAROON, ILI9341_BLACK);

  //==== display cache =====================
  // ... to avoid flickering
  int speedLast = 1000;
  int accelerationLast = 1000;
  int targetValueLast = 1000;
  bool justInited = true;
  //=======================================

  //==== Driver Display definitions ==== START
  // display formats and sizes
  const int infoFrameX = 0;
  const int infoFrameY = 0;
  int infoFrameSizeX = -1; // full tft width, calculated beow
  const int infoFrameSizeY = 64;
  const int infoTextSize = 3;

  // frame around value display (exclude text message lines)
  const int mainFrameX = infoFrameSizeY;

  // speed display
  int speedFrameX = -1; // get calculated later: (sizeX - speedFrameCx) / 2;
  const int speedFrameY = 80;
  const int speedFrameSizeX = 156;
  const int speedFrameSizeY = 76;
  const int speedTextSize = 8;
  int speedUnitX = 0; // get claculated later
  const int speedUnitAccelY = 108;
  const int speedUnitSpeedY = 72;
  const int speedUnitTextSize = 1;

  // target speed/power display
  int targetValueFrameX = -1; // get calculated later: (speedFrameX + speedFrameSizeX + 1;
  const int targetValueFrameY = 118;
  int targetValueFrameSizeX = 40; // get recalculated later
  const int targetValueFrameSizeY = 38;
  const int targetValueTextSize = 4;

  // acceleration display
  const int accFrameX = 2;
  const int accFrameY = 118;    // speedFrameY + speedFrameSizeY / 2
  int accFrameSizeX = -1;       // get calculated later: speedFrameX - 4
  const int accFrameSizeY = 38; // speedFrameSizeY / 2
  const int accTextSize = 4;    // speedTextSize / 2

  // ---- voltage and current displays ---- START
  const int labelLen = 9; // label length for all 3 voltage/current displays

  // battery voltage display
  const int batFrameX = 10;
  const int batFrameY = 180;
  const int batTextSize = 2;
  const int lightTextSize = 2;

  // photovoltaics voltage display
  const int pvFrameX = 10;
  const int pvFrameY = 200;
  const int pvTextSize = 2;

  // motor current display
  const int motorFrameX = 10;
  const int motorFrameY = 220;
  const int motorTextSize = 2;
  // ---- voltage and current displays ---- END

  // constant mode speed or power display
  const int constantModeX = 242;
  const int constantModeY = 169;
  const int constantModeTextSize = 1;

  // step width for constant mode display
  const int controlModeStepX = 242;
  const int controlModeStepY = 158;
  const int controlModeStepTextSize = 1;

  // drive direction display
  const int driveDirectionX = 220;
  const int driveDirectionY = 180;
  const int driveDirectionTextSize = 2;

  const int lightX = 252;
  const int lightY = 198;

  // eco/pwr mode display
  const int ecoPwrModeX = 242;
  const int ecoPwrModeY = 218;
  const int ecoPwrModeTextSize = 2;

  //==== Driver Display definition ==== END

public:
  // DISPLAY_STATUS display_task();
  //==== overwrites from base class ==== END

  void _arrow_increase(int color);
  void _arrow_decrease(int color);
  void _hide_light();
  void _turn_Left(int color);
  void _turn_Right(int color);

  bool init_driver_display(void);
  void draw_display_border(int color);
  void draw_speed_border(int color);
  void draw_acceleration_border(int color);
  void draw_target_value_border(int color);
  void draw_display_background();

  void step_width_show();
  void constant_drive_mode_show();
  void eco_power_mode_show();

  void write_drive_direction();
  void write_driver_info();
  void write_speed();
  void write_acceleration();
  void write_target_value();

  void show_light();

  void speedCheck(int speed);
  void arrow(SPEED_ARROW incDecOff);
  void arrow_increase(bool on);
  void arrow_decrease(bool on);

  int getColorForInfoType(INFO_TYPE type);
  string display_setup();
};

#endif // #ifndef SER_DRIVER_DISPLAY_C_H
