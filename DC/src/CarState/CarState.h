//
// Car State with all car information
//

#ifndef CARSTATE_H
#define CARSTATE_H

#include <cJSON.h>
#include <list>
#include <map>
#include <sstream>
#include <string>

#include <definitions.h>

#include <CarStatePin.h>

using namespace std;

// public structures and structure texts
static const char *BOOL_str[] = {"false", "true"};

enum class INFO_TYPE { INFO, STATUS, WARN, ERROR };
static const char *INFO_TYPE_str[] = {"INFO", "STATUS", "WARN", "ERROR"};

enum class SPEED_ARROW { OFF, INCREASE, DECREASE };
static const char *SPEED_ARROW_str[] = {"OFF", "INCREASE", "DECREASE"};

enum class CONSTANT_MODE { SPEED, POWER };
static const char *CONSTANT_MODE_str[] = {"SPEED", "POWER"};

enum class DRIVE_DIRECTION { FORWARD, BACKWARD };
static const char *DRIVE_DIRECTION_str[] = {"fwd", "bwd"};

enum class LIGHT { OFF, L1, L2 };
static const char *LIGHT_str[] = {"OFF", "L1", "L2"};

enum class DISPLAY_STATUS {
  DRIVER_HALTED,
  DRIVER_SETUP,
  DRIVER_BACKGROUND,
  DRIVER_RUNNING,
  DRIVER_DEMOSCREEN,
  ENGINEER_CONSOLE,
  ENGINEER_HALTED,
  ENGINEER_SETUP,
  ENGINEER_BACKGROUND,
  ENGINEER_RUNNING
};
static const char *DISPLAY_STATUS_str[] = {
    "DRIVER_HALTED",       // no action on this display
    "DRIVER_SETUP",        // driver screen setup
    "DRIVER_BACKGROUND",   // create background for driver screen
    "DRIVER_RUNNING",      // driver mode active
    "DRIVER_DEMOSCREEN",   // demo
    "ENGINEER_CONSOLE",    // console mode (engineer screen)
    "ENGINEER_HALTED"      // no action on this display
    "ENGINEER_SETUP",      // engineerscreen setup
    "ENGINEER_BACKGROUND", // create background for engineer screen
    "ENGINEER_RUNNING"     // enineer mode active
};

enum class PRECHARGE_STATE { ERROR, IDLE, MEASURE, PRECHARGE, RUN, ENABLE_PACK };
static const char *PRECHARGE_STATE_str[] = {"ERROR", "IDLE", "MEASURE", "PRECHARGE", "RUN", "ENABLE_PACK"};

enum class BATTERY_ERROR {
  CELL_OVER_VOLTAGE,
  CELL_UNDER_VOLTAGE,
  CELL_OVER_TEMP,
  MEASUREMENT_UNTRUSTED,
  CMU_COMM_TIMEOUT,
  VEHICLE_COMM_TIMEOUT,
  BMU_IN_SETUP_MODE,
  CMU_CAN_POWER,
  PACK_ISOLATION_TEST_FAIL,
  SOC_MEASUREMENT_INVALID,
  CAN_12V_LOW,
  CONTACTOR_STUCK,
  EXTRA_CELL_DETECTED
};
static const char *BATTERY_ERROR_str[] = {
    "CELL_OVER_VOLTAGE",        //
    "CELL_UNDER_VOLTAGE",       //
    "CELL_OVER_TEMP",           //
    "MEASUREMENT_UNTRUSTED",    //
    "CMU_COMM_TIMEOUT",         //
    "VEHICLE_COMM_TIMEOUT",     //
    "BMU_IN_SETUP_MODE",        //
    "CMU_CAN_POWER",            //
    "PACK_ISOLATION_TEST_FAIL", //
    "SOC_MEASUREMENT_INVALID",  //
    "CAN_12V_LOW",              //
    "CONTACTOR_STUCK",          //
    "EXTRA_CELL_DETECTED"       //
};

class CarState {

public:
  CarState() {
    Speed = 0;
    Acceleration = 0;
    Deceleration = 0;
    BatteryVoltage = 0;
    BatteryCurrent = 0;
    PhotoVoltaicCurrent = 0;
    MotorCurrent = 0;

    DriveDirection = DRIVE_DIRECTION::FORWARD;
    ConstantMode = CONSTANT_MODE::SPEED; // #SAFETY#: deceleration unlock const mode
    ConstantModeOn = false;

    TargetSpeed = 0;
    TargetPower = 0;
    DriverInfo = "starting...";
    DriverInfoType = INFO_TYPE::STATUS;
    Light = LIGHT::OFF;
    Fan = false;

    // init state flags
    // #SAFETY#: acceleration lock
    AccelerationLocked = true;
    GreenLight = false;
    // BEGIN prevent stupid compiler warnings "defined but not used"
    (void)BOOL_str;
    (void)INFO_TYPE_str;
    (void)SPEED_ARROW_str;
    (void)CONSTANT_MODE_str;
    (void)DRIVE_DIRECTION_str;
    (void)LIGHT_str;
    (void)DISPLAY_STATUS_str;
    (void)PRECHARGE_STATE_str;
    (void)BATTERY_ERROR_str;
    // BEGIN prevent stupid compiler warnings "defined but not used"
  }
  ~CarState(){};
  void init_values();
  bool initalize_config();

  uint16_t LifeSign;
  // Buttons
  bool ButtonPlus;
  bool ButtonMinus;
  bool ButtonConstantModeOn;
  bool ButtonConstantModeOFF;
  bool ButtonConstant_v_P;

  // physical car data (measurement values)
  uint8_t Speed;          // ADC
  uint16_t Potentiometer; // ADC potentiometer from switch board
  uint16_t Acceleration;  // ADC Steering Wheel
  uint16_t Deceleration;  // ADC Steering Wheel
  // #SAFETY#: acceleration lock
  bool AccelerationLocked;    // DAC lock
  int8_t AccelerationDisplay; // Display Value (-99...+99)
  // #SAFETY-END#

  bool BatteryOn;      // IO-In
  bool PhotoVoltaicOn; // IO-in
  bool MotorOn;        // IO-In
  bool EcoOn;          // IO-In

  PRECHARGE_STATE PrechargeState;    // CAN
  list<BATTERY_ERROR> BatteryErrors; // CAN

  float BatteryVoltage;      // CAN
  float BatteryCurrent;      // CAN
  float PhotoVoltaicCurrent; // ADC
  float MotorVoltage;        // ADC
  float MotorCurrent;        // ADC

  float Mppt1Current; // CAN
  float Mppt2Current; // CAN
  float Mppt3Current; // CAN

  float Umin; // CAN
  float Uavg; // CAN
  float Umax; // CAN

  float T1; // MPPT1
  float T2; // MPPT2
  float T3; // MPPT3
  // float T4; // Motorcontroller
  float Tmin; // max. cell temperatur from BMS
  float Tmax; // max. cell temperatur from BMS

  bool BreakPedal = false;

  int ReferenceSolarCell;

  // logical car data (values set by driver or chase car)
  DISPLAY_STATUS displayStatus;
  DRIVE_DIRECTION DriveDirection;
  CONSTANT_MODE ConstantMode;
  bool ConstantModeOn; // #SAFETY#: deceleration unlock const mode
  bool SdCardDetect;

  float TargetSpeed;
  float TargetRecuperation;
  float TargetPower;
  string DriverInfo;
  SPEED_ARROW SpeedArrow;
  INFO_TYPE DriverInfoType;
  LIGHT Light;
  bool GreenLight;
  bool Fan;

  // All IO pins
  static CarStatePin pins[IOExtPINCOUNT];
  int getIdx(string pinName);
  CarStatePin *getPin(int devNr, int pinNr);
  CarStatePin *getPin(int port);
  CarStatePin *getPin(string pinName);

  std::map<string, int> idxOfPin;
  // std::map<int, Pin> pins; // pins by index

  // auxiliary variables - read from ser4config.ini
  // [Main]
  string LogFilename; // telemetry data: %stamp% get replaced by datetime stamp if period != 0
  int LogFilePeriod;  // [h], after that a new log file is created, 0 - never
  int LogInterval;    // [ms]

  // [TaskTimings]
  int SleepTimeIOExt = 400; // [ms]

  // [PID]
  double Kp = 1.0;  // proportional
  double Ki = 1.0;  // integral
  double Kd = 0.01; // differential

  // [Dynamic]
  int PaddleDamping = 200;            // 0...ca.30000
  int PaddleOffset;                   // 0 ... 65535: offset when paddle recognize not 0 values
  int ButtonControlModeIncrease;      // on click means ButtonControlModeIncrease units
  int ButtonControlModeIncreaseLow;   // ButtonControlModeIncrease low mode
  int ButtonControlModeIncreaseHeigh; // ButtonControlModeIncrease hight mode
  int ConstSpeedIncrease;             // [km/h] per click
  int ConstPowerIncrease;             // [W] per click

  // [Communication]
  int CarDataSendPeriod;        // [ms]
  int Serial1Baudrate = 115200; // baud
  int Serial2Baudrate = 9600;   // baud

  // [Telemetry]
  int SendInterval;     // [ms]
  int MaxCachedRecords; // number of telemetry records hold in cache in case of trasmit errors

  // tools
  const string print(string msg, bool withColors = true);
  const string printIOs(string msg, bool withColors = true, bool deltaOnly = false);
  const string serialize(string msg = "");
  const string csv(string msg = "", bool withHeader = false);
  const string batteryErrorsAsString(bool verbose = false);
};

#endif // CARSTATE_H
