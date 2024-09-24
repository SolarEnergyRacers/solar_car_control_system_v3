//
// Car State with all car information
//
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

#include <CarState.h>
// #include <ConfigFile.h>
#include <Console.h>
// #include <ESP32Time.h>
#include <Helper.h>
#include <IOExt.h>
// #include <RTC.h>
#include "../definitions.h"

using namespace std;

extern CarState carState;
extern Console console;
extern IOExt ioExt;
// extern RTC rtc;
// extern ESP32Time esp32time;

int CarState::getIdx(const string pinName) { return idxOfPin.find(pinName)->second; }
CarStatePin *CarState::getPin(int devNr, int pinNr) { return &(carState.pins[IOExt::getIdx(devNr, pinNr)]); }
CarStatePin *CarState::getPin(int port) { return &(carState.pins[IOExt::getIdx(port)]); }
CarStatePin *CarState::getPin(const string pinName) { return &(carState.pins[carState.getIdx(pinName)]); }

void CarState::init_values() {
  // values read from sensors
  Speed = 0;
  Potentiometer = 0;
  Acceleration = 0;
  Deceleration = 0;
  BatteryVoltage = 0;
  BatteryCurrent = 0;
  PhotoVoltaicCurrent = 0;
  ReferenceSolarCell = 0;
  MotorCurrent = 0;
  SdCardDetect = false;

  // values set by control elements or CarControl
  DriveDirection = DRIVE_DIRECTION::FORWARD;
  ConstantMode = CONSTANT_MODE::SPEED; // #SAFETY#: deceleration unlock const mode
  ConstantModeOn = false;
  ConfirmDriverInfo = false;

  TargetSpeed = 0;
  TargetPower = 0;
  DriverInfo = "Acceleration\nstill locked!";
  DriverInfoType = INFO_TYPE::STATUS;
  GreenLight = false;

  // read from SER4CONFIG.INI file
  initalize_config();
  console << print("State after reading config") << NL;
}

bool CarState::initalize_config() {
  // try {
  //   ConfigFile cf = ConfigFile(FILENAME_SER4CONFIG);
  //   // [Main]
  //   LogFilename = cf.get("Main", "LogFilename", "/SER4DATA.CSV");
  //   LogFilePeriod = cf.get("Main", "LogFilePeriod", 1000);
  //   LogInterval = cf.get("Main", "LogInterval", 1);
  //   // [PID]
  //   Kp = cf.get("PID", "Kp", 15);
  //   Ki = cf.get("PID", "Ki", 5);
  //   Kd = cf.get("PID", "Kd", 0.05);
  //   // [Dynamic]
  //   PaddleDamping = cf.get("Dynamic", "PaddleDamping", 10);
  //   ConstSpeedIncrease = cf.get("Dynamic", "ConstSpeedIncrease", 1.0);
  //   ConstPowerIncrease = cf.get("Dynamic", "ConstPowerIncrease", 0.5);
  //   ButtonControlModeIncreaseLow = cf.get("Dynamic", "ButtonControlModeIncreaseLow", 2);
  //   ButtonControlModeIncreaseHeigh = cf.get("Dynamic", "ButtonControlModeIncreaseHeigh", 10);
  //   ButtonControlModeIncrease = ButtonControlModeIncreaseLow;
  //   // [Communication]
  //   CarDataSendPeriod = cf.get("Communication", "CarDataSendPeriod", 3000);
  //   Serial1Baudrate = cf.get("Communication", "Serail1Baudrate", 115200);
  //   Serial2Baudrate = cf.get("Communication", "Serial2Baudrate", 9600);
  //   // [Telemetry]
  //   SendInterval = cf.get("Telemetry", "", 1000);
  //   MaxCachedRecords = cf.get("Telemetry", "MaxCachedRecords", 100);

  // } catch (exception &ex) {
  //   console << "WARN: No config file: '" << FILENAME_SER4CONFIG << "' found or readable: " << ex.what() << NL;
  //   return false;
  // }
  return true;
}

const char *getCleanString(string str) {
  replace(str.begin(), str.end(), '\n', '.');
  return str.c_str();
}

const string CarState::print(const string msg, bool withColors) {
  stringstream ss(msg);
  ss << "====SER4 Car Status DC====" << VERSION << "==";
  // ss << t.tm_year << "." << t.tm_mon << "." << t.tm_mday << "_" << t.tm_hour << ":" << t.tm_min << ":" << t.tm_sec;
  ss << "====uptime:" << getTimeStamp() << "s====" << getDateTime() << "==\n";
  if (msg.length() > 0)
    ss << msg << NL;
  ss << "Display Status ........ " << DISPLAY_STATUS_str[(int)displayStatus] << NL;
  ss << "Potentiometer ......... " << (int)Potentiometer << NL;
  ss << "Speed ................. " << (int)Speed << NL;
  ss << "Acceleration locked ... " << BOOL_str[(int)(AccelerationLocked)] << NL;
  ss << "Acceleration .......... " << (int)Acceleration << NL;
  ss << "Deceleration .......... " << (int)Deceleration << NL;
  ss << "Acceleration Display .. " << (int)AccelerationDisplay << NL;
  ss << "Break pedal pressed ... " << BOOL_str[(int)(BreakPedal)] << NL;
  ss << "Battery On............. " << BatteryOn << NL;
  ss << "Battery Voltage ....... " << BatteryVoltage << NL;
  ss << "Battery Current........ " << BatteryCurrent << NL;
  ss << "Battery Errors ........." << batteryErrorsAsString(true) << NL;
  ss << "Battery Precharge State " << PRECHARGE_STATE_str[(int)(PrechargeState)] << NL;
  ss << "Photo Voltaic On ...... " << PhotoVoltaicOn << NL;
  ss << "MPPT1 Current ......... " << Mppt1Current << NL;
  ss << "MPPT2 Current ......... " << Mppt2Current << NL;
  ss << "MPPT3 Current ......... " << Mppt3Current << NL;
  ss << "Photo Voltaic Current . " << PhotoVoltaicCurrent << NL;
  ss << "Photo Reference Cell .. " << ReferenceSolarCell << NL;
  ss << "Break pedal pressed ... " << BOOL_str[(int)(BreakPedal)] << NL;
  ss << "Photo Voltaic On ...... " << PhotoVoltaicOn << NL;
  ss << "Motor On .............. " << MotorOn << NL;
  ss << "Motor Current ......... " << MotorCurrent << NL;
  ss << "Drive Direction ....... " << DRIVE_DIRECTION_str[(int)(DriveDirection)] << NL;
  ss << "Green Light ........... " << GreenLight << NL;
  ss << NL;
  ss << "Constant Mode ......... " << CONSTANT_MODE_str[(int)(ConstantMode)] << NL;
  ss << "Target Speed .......... " << TargetSpeed << NL;
  ss << "Target Power .......... " << TargetPower << NL;
  ss << "Speed Arrow ........... " << SPEED_ARROW_str[(int)SpeedArrow] << NL;
  ss << "IO .................... " << printIOs("", false) << NL;
  ss << NL;
  // [PID]
  ss << "Kp .................... " << Kp << NL;
  ss << "Ki .................... " << Ki << NL;
  ss << "Kd .................... " << Kd << NL;
  // [Dynamic]
  ss << "Paddle damping ........ " << PaddleDamping << NL;
  ss << "Paddle offset ......... " << "acc " << carState.StartOffset_acc << ", dec "<<carState.StartOffset_dec << NL;
  ss << "Const speed increase .. " << ConstSpeedIncrease << NL;
  ss << "Const power invrease .. " << ConstPowerIncrease << NL;
  ss << NL;
  // [Communication]
  ss << "Serial 1 baud rate .... " << Serial1Baudrate << NL;
  ss << "========================================================================" << NL;
  return ss.str();
}

const string CarState::serialize(const string msg) {
  string timeStamp = getDateTime();
  // timeStamp.erase(timeStamp.end() - 1);
  string tempStr = getCleanString(DriverInfo);

  cJSON *carData = cJSON_CreateObject();
  cJSON *dynData = cJSON_CreateObject();
  cJSON *ctrData = cJSON_CreateObject();

  cJSON_AddItemToObject(carData, "dynamicData", dynData);
  cJSON_AddStringToObject(dynData, "timeStamp", timeStamp.c_str());
  cJSON_AddStringToObject(dynData, "uptime", getTimeStamp().c_str()); // getUpTime().c_str()); // getTimeStamp().c_str());
  cJSON_AddStringToObject(dynData, "msg", msg.c_str());
  cJSON_AddNumberToObject(dynData, "potentiometer", Potentiometer);
  cJSON_AddNumberToObject(dynData, "speed", Speed);
  cJSON_AddNumberToObject(dynData, "acceleration", Acceleration);
  cJSON_AddNumberToObject(dynData, "deceleration", Deceleration);
  cJSON_AddNumberToObject(dynData, "accelerationDisplay", AccelerationDisplay);
  cJSON_AddBoolToObject(dynData, "batteryOn", BatteryOn);
  cJSON_AddNumberToObject(dynData, "batteryVoltage", floor(BatteryVoltage * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "batteryCurrent", floor(BatteryCurrent * 1000.0 + .5) / 1000.0);
  cJSON_AddBoolToObject(dynData, "pvOn", PhotoVoltaicOn);
  cJSON_AddNumberToObject(dynData, "pvCurrent", floor(PhotoVoltaicCurrent * 1000.0 + .5) / 1000.0);
  cJSON_AddBoolToObject(dynData, "motorOn", MotorOn);
  cJSON_AddNumberToObject(dynData, "motorCurrent", floor(MotorCurrent * 1000.0 + .5) / 1000.0);

  cJSON_AddNumberToObject(dynData, "mppt1Current", floor(Mppt1Current * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "mppt2Current", floor(Mppt2Current * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "mppt3Current", floor(Mppt3Current * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "voltageMin", floor(Umin * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "voltageAvg", floor(Uavg * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "voltageMax", floor(Umax * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "t1", floor(T1 * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "t2", floor(T2 * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "t3", floor(T3 * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "tmin", floor(Tmin * 1000.0 + .5) / 1000.0);
  cJSON_AddNumberToObject(dynData, "tmax", floor(Tmax * 1000.0 + .5) / 1000.0);

  cJSON_AddStringToObject(dynData, "sdCardDetect", BOOL_str[(int)(SdCardDetect)]);

  cJSON_AddItemToObject(carData, "controlData", ctrData);
  cJSON_AddStringToObject(dynData, "driveDirection", DRIVE_DIRECTION_str[(int)(DriveDirection)]);
  cJSON_AddStringToObject(ctrData, "displayStatus", DISPLAY_STATUS_str[(int)displayStatus]);
  cJSON_AddStringToObject(ctrData, "constantMode", CONSTANT_MODE_str[(int)(ConstantMode)]);
  cJSON_AddNumberToObject(ctrData, "targetSpeed", TargetSpeed);
  cJSON_AddNumberToObject(ctrData, "targetPower", TargetPower);
  cJSON_AddStringToObject(ctrData, "driverInfo", fmt::format("[{}] {}", INFO_TYPE_str[(int)DriverInfoType], tempStr).c_str());
  cJSON_AddStringToObject(ctrData, "speedArrow", SPEED_ARROW_str[(int)SpeedArrow]);
  cJSON_AddBoolToObject(dynData, "greenLight", GreenLight);
  cJSON_AddStringToObject(ctrData, "io:", printIOs("", false).c_str());
  return fmt::format("{}\n", cJSON_PrintUnformatted(carData));
}

const string CarState::csv(const string msg, bool withHeader) {
  string timeStamp = getDateTime();
  // timeStamp.erase(timeStamp.end() - 1);
  string tempStr = getCleanString(DriverInfo);

  stringstream ss("");
  if (withHeader) {
    // header
    ss << "Epoch, ";
    ss << "uptime, ";
    ss << "msg, ";
    ss << "potentiomenter, ";
    ss << "speed, ";
    ss << "acceleration, ";
    ss << "deceleration, ";
    ss << "accelerationDisplay, ";

    ss << "batteryOn, ";
    ss << "batteryVoltage, ";
    ss << "batteryCurrent, ";
    ss << "batteryErrors, ";
    ss << "batteryPrechargeState, ";
    ss << "pvOn, ";
    ss << "pvCurrent, ";
    ss << "motorOn, ";
    ss << "motorCurrent, ";

    ss << "mppt1Current, ";
    ss << "mppt2Current, ";
    ss << "mppt3Current, ";
    ss << "voltageMin, ";
    ss << "voltageAvg, ";
    ss << "voltageMax, ";
    ss << "T1, ";
    ss << "T2, ";
    ss << "T3, ";
    ss << "Tmin, ";
    ss << "Tmax, ";

    ss << "driveDirection, ";
    ss << "constantMode, ";
    ss << "sdCardDetected, ";

    ss << "displayStatus, ";

    ss << "targetSpeed, ";
    ss << "Kp, ";
    ss << "Ki, ";
    ss << "Kd, ";
    ss << "targetPower, ";
    ss << "driverInfo, ";
    ss << "ConfirmDriverInfo, ";
    ss << "speedArrow, ";
    ss << "light, ";
    ss << "greenLight, ";
    ss << "fan, ";
    ss << "io, ";
    ss << "timeStampDate, ";
    ss << "timeStampTime, ";
    ss << NL;
  }
  // data
  ss << "XXXXXXX"; // ss << esp32time.getEpoch() << ", " ;
  ss << millis() / 1000 << ", ";
  ss << msg.c_str() << ", ";
  ss << Potentiometer << ", ";
  ss << Speed << ", ";
  ss << Acceleration << ", ";
  ss << Deceleration << ", ";
  ss << AccelerationDisplay << ", ";

  ss << BatteryOn << ", ";
  ss << floor(BatteryVoltage * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(BatteryCurrent * 1000.0 + .5) / 1000.0 << ", ";
  ss << batteryErrorsAsString() << ", ";
  ss << PRECHARGE_STATE_str[(int)(PrechargeState)] << ", ";
  ss << PhotoVoltaicOn << ", ";
  ss << floor(PhotoVoltaicCurrent * 1000.0 + .5) / 1000.0 << ", ";
  ss << MotorOn << ", ";
  ss << floor(MotorCurrent * 1000.0 + .5) / 1000.0 << ", ";

  ss << floor(Mppt1Current * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(Mppt2Current * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(Mppt3Current * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(Umin * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(Uavg * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(Umax * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(T1 * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(T2 * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(T3 * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(Tmin * 1000.0 + .5) / 1000.0 << ", ";
  ss << floor(Tmax * 1000.0 + .5) / 1000.0 << ", ";

  ss << DRIVE_DIRECTION_str[(int)(DriveDirection)] << ", ";
  ss << CONSTANT_MODE_str[(int)(ConstantMode)] << ", ";
  ss << SdCardDetect << ", ";

  ss << DISPLAY_STATUS_str[(int)displayStatus] << ", ";

  ss << TargetSpeed << ", ";
  ss << Kp << ", ";
  ss << Ki << ", ";
  ss << Kd << ", ";
  ss << TargetPower << ", ";
  ss << fmt::format("\"{}: {}\"", INFO_TYPE_str[(int)DriverInfoType], tempStr) << ", ";
  ss << ConfirmDriverInfo << ", ";
  ss << SPEED_ARROW_str[(int)SpeedArrow] << ", ";
  ss << GreenLight << ", ";
  ss << printIOs("", false).c_str() << ", ";
  ss << timeStamp.c_str() << ", ";
  ss << NL;
  return ss.str();
}

const string CarState::printIOs(const string msg, bool withColors, bool deltaOnly) {
  string normalColor = "";
  string highLightColorOut = "";
  string highLightColorChg = "";
  if (withColors) {
    normalColor = "\033[0;39m";       // white
    highLightColorOut = "\033[1;31m"; // red
    highLightColorChg = "\033[1;36m"; // blue
  }
  stringstream ss(msg);
  if (msg.length() > 0)
    ss << msg << NL;

  bool hasDelta = false;
  for (int devNr = 0; devNr < MCP23017_NUM_DEVICES; devNr++) {
    ss << "devNr " << devNr << ": ";
    for (int pinNr = 0; pinNr < MCP23017_NUM_PORTS; pinNr++) {
      CarStatePin *pin = carState.getPin(devNr, pinNr);
      if (pin->mode == OUTPUT) {
        if (pin->value != pin->oldValue) {
          hasDelta = true;
          ss << highLightColorChg << pin->value << normalColor;
        } else {
          ss << highLightColorOut << pin->value << normalColor;
        }
      } else {
        if (pin->value != pin->oldValue) {
          hasDelta = true;
          ss << highLightColorChg << pin->value << normalColor;
        } else {
          ss << pin->value;
        }
      }
      int idx = IOExt::getIdx(devNr, pinNr);
      if (idx < MCP23017_NUM_DEVICES * MCP23017_NUM_PORTS - 1) {
        if ((idx + 1) % 8 == 0)
          ss << " | ";
        else if ((idx + 1) % 4 == 0)
          ss << "-";
      }
    }
  }

  // ss << NL;
  if (hasDelta || !deltaOnly)
    return ss.str();
  else
    return "";
}

const string CarState::batteryErrorsAsString(bool verbose /*= false*/) {
  stringstream ss;

  ss << "[";

  if (verbose) {
    for (auto const &battErr : BatteryErrors) {
      ss << BATTERY_ERROR_str[(int)(battErr)] << "-";
    }
  } else {
    for (auto const &battErr : BatteryErrors) {
      ss << (int)(battErr) << "-";
    }
  }

  ss << "]";

  return ss.str();
}