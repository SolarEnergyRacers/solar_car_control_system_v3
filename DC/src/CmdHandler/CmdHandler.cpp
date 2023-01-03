//
// Command Receiver and Handler
//
// reads commands from serial console and deploy it
#include <definitions.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdio.h>
#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Arduino.h>
#if RTC_ON
#include <ESP32Time.h>
#include <RtcDateTime.cpp>
#endif

#include <Wire.h> // I2C

#include <CANBus.h>
#include <CarControl.h>
#if CARSPEED_ON
#include <CarSpeed.h>
#endif
#include <CANBus.h>
#include <CarState.h>
#include <CarStatePin.h>
#include <CmdHandler.h>
#include <Console.h>
#include <Helper.h>
#include <I2CBus.h>
// #include <IOExt.h>
// #include <IOExtHandler.h>
// #include <RTC.h>
#include <System.h>

extern CANBus can;
extern I2CBus i2cBus;
extern I2CBus i2cBus;
extern CarState carState;
extern CANBus canBus;
// extern CarSpeed carSpeed;
extern CarControl carControl;
extern Console console;
#if RTC_ON
extern RTC rtc;
extern ESP32Time esp32time;
#endif

using namespace std;

// ------------------
// FreeRTOS functions

string CmdHandler::re_init() { return init(); }

string CmdHandler::init() {
  bool hasError = false;
  console << "[  ] Init 'CmdHandler'...\n";
  // nothing to do, i2c bus is getting initialized externally
  return fmt::format("[{}] CmdHandler initialized.", hasError ? "--" : "ok");
}

void CmdHandler::exit() {
  // TODO
}

void CmdHandler::task(void *pvParams) {
  string state, msg;
  while (1) {
    try {
      while (Serial.available()) { // || Serial2.available()) {
        // read the incoming chars:
        String input = "";
        if (Serial.available()) {
          input = Serial.readString();
          Serial.flush();
        }
        console << input << NL;

#if SERIAL_RADIO_ON
        if (Serial2.available()) {
          input = Serial2.readString();
          Serial2.flush();
        }
#endif
        if (input.endsWith("\n")) {
          input = input.substring(0, input.length() - 1);
        }
        if (input.endsWith("\r")) {
          input = input.substring(0, input.length() - 1);
        }

        if (input.length() > 0 && commands.find(input[0], 0) == -1) {
          input = "h"; // help
        }

        if (input.length() == 0)
          break;

        switch (input[0]) {
        // ---------------- controller commands
        case '\n':
          console << NL;
          break;
        case 'S':
          if (input[1] == 'a') {
            console << printSystemValues();
          } else {
            console << carState.print("Recent State") << NL;
          }
          break;
        case 'V':
          state = carState.csv("Recent State", input[1] == '+' ? true : false); // with header
          console << "Not implemented yet!" << NL;
          break;
        case 'H':
          console << "Not implemented yet!" << NL;
          // memory_info();
          break;
        case 'I':
          // console << "Received: '" << input << "' --> ";
          // if (input[1] == 's') {
          //   //console << "Received: '" << input << "' -->  i2cBus.scan_i2c_devices()\n";
          //   i2cBus.scan_i2c_devices();
          // } else if (input[1] == 'i') {
          //   ioExt.verboseModeDigitalIn = !ioExt.verboseModeDigitalIn;
          //   console << "set verboseModeDigitalIn: " << ioExt.verboseModeDigitalIn << NL;
          // } else if (input[1] == 'o') {
          //   ioExt.verboseModeDigitalOut = !ioExt.verboseModeDigitalOut;
          //   console << "set verboseModeDigitalOut: " << ioExt.verboseModeDigitalOut << NL;
          // } else if (input[1] == 'a') {
          //   adc.verboseModeADC = !adc.verboseModeADC;
          //   console << "set verboseModeADC: " << adc.verboseModeADC << NL;
          // } else if (input[1] == 'd') {
          //   dac.verboseModeDAC = !dac.verboseModeDAC;
          //   console << "set verboseModeDAC: " << dac.verboseModeDAC << NL;
          // } else if (input[1] == 'c') {
          //   carControl.verboseMode = !carControl.verboseMode;
          //   console << "set verboseMode for acc-/dec-controls: " << carControl.verboseMode << NL;
          // } else if (input[1] == 'R') {
          //   console << ioExt.re_init() << NL;
          //   msg = ioExt.re_init();
          //   console << msg << NL;
          // } else {
          //   ioExt.readAllPins();
          //   console << carState.printIOs("", true, false) << NL;
          // }
          break;
        // -------------- chase car commands
        case 'C':
          if (input[1] == 'i') {
            canBus.verboseModeCanIn = !canBus.verboseModeCanIn;
            console << "set verboseModeCanIn: " << canBus.verboseModeCanIn << NL;
          } else if (input[1] == 'I') {
            canBus.verboseModeCanInNative = !canBus.verboseModeCanInNative;
            console << "set verboseModeCanInNative: " << canBus.verboseModeCanInNative << NL;
          } else if (input[1] == 'o') {
            canBus.verboseModeCanOut = !canBus.verboseModeCanOut;
            console << "set verboseModeCanOut: " << canBus.verboseModeCanOut << NL;
          } else if (input[1] == 'O') {
            canBus.verboseModeCanOutNative = !canBus.verboseModeCanOutNative;
            console << "set verboseModeCanOutNative: " << canBus.verboseModeCanOutNative << NL;
          } else {
            console << "set verboseModeCan needs a specifier: i,I,o,O." << NL;
          }
          break;
        case 'O':
          if (input[1] == 'o') {
            carControl.verboseMode = !carControl.verboseMode;
            console << "set verboseMode: " << carControl.verboseMode << NL;
          } else if (input[1] == 'O') {
            carControl.verboseModeDebug = !carControl.verboseModeDebug;
            console << "set verboseModeDebug: " << carControl.verboseModeDebug << NL;
          } else {
            console << "set verboseMode CarControl needs a specifier: o,O." << NL;
          }
          break;
        case 'T': {
#if RTC_ON
          string arr[6];
          int count = splitString(arr, &input[1]);
          if (count == 0) {
            time_t theTime = time(NULL);
            struct tm t = *localtime(&theTime);
            console << "Received: '" << input << "' --> DateTime: " << asctime(&t) << NL;
          } else {
            int yy = atof(arr[0].c_str());
            int mm = atof(arr[1].c_str());
            int dd = atof(arr[2].c_str());
            int hh = atof(arr[3].c_str());
            int MM = atof(arr[4].c_str());
            int ss = atof(arr[5].c_str());
            uint16_t days = DaysSinceFirstOfYear2000<uint16_t>(yy, mm, dd);
            uint64_t seconds = SecondsIn<uint64_t>(days, hh, MM, ss);
            RtcDateTime dateTime = RtcDateTime(seconds);
            rtc.write_rtc_datetime(dateTime);
            esp32time.setTime(ss, MM, hh, dd, mm, yy);
            time_t theTime = time(NULL);
            struct tm t = *localtime(&theTime);
            console << "Received: '" << input << "' --> Set dateTime to: " << asctime(&t) << NL;
          }
#else
          console << "RTC deactivated\n";
#endif
        } break;
        case 'K':
#if CARSPEED_ON
          console << "Received: '" << input << "' --> ";
          if (input[1] == 'v') {
            carSpeed.verboseModePID = !carSpeed.verboseModePID;
          } else {
            string arr[4];
            int count = splitString(arr, &input[1]);
            if (count == 0) {
              console << "PID parameters: ";
            } else {
              float Kp = atof(arr[0].c_str());
              float Ki = atof(arr[1].c_str());
              float Kd = atof(arr[2].c_str());
              carSpeed.update_pid(Kp, Ki, Kd);
              console << "PID set parameters: ";
            }
            console << "Kp=" << carState.Kp << ", Ki=" << carState.Ki << ", Kd=" << carState.Kd << NL;
          }
#else
          console << "Car speed control deactivated\n";
#endif
          break;
        //-------- DRIVER INFO COMMANDS --------------------
        // TODO: Implement and send by CAN
        // case 's':
        //   if (input[1] == 'u') {
        //     carState.SpeedArrow = SPEED_ARROW::INCREASE;
        //   } else if (input[1] == 'd') {
        //     carState.SpeedArrow = SPEED_ARROW::DECREASE;
        //   } else if (input[1] == 'o') {
        //     carState.SpeedArrow = SPEED_ARROW::OFF;
        //   }
        //   console << "Received: '" << input << "' -->  carState.SpeedArrow=" << SPEED_ARROW_str[(int)(carState.SpeedArrow)] << NL;
        //   break;
        // case ':':
        //   carState.DriverInfoType = INFO_TYPE::INFO;
        //   carState.DriverInfo = &input[1];
        //   console << "Received: '" << input << "' -->  carState.DriverInfo " << INFO_TYPE_str[(int)carState.DriverInfoType] << ": "
        //           << carState.DriverInfo << NL;
        //   break;
        // case '!':
        //   carState.DriverInfoType = INFO_TYPE::WARN;
        //   carState.DriverInfo = &input[1];
        //   console << "Received: '" << input << "' -->  carState.DriverInfo " << INFO_TYPE_str[(int)carState.DriverInfoType] << ": "
        //           << carState.DriverInfo << NL;
        //   break;
        // // -------- Driver SUPPORT COMMANDS -----------------
        // case 'c':
        //   if (input[2] == '-') {
        //     carState.ConstantModeOn = false; // #SAFETY#: deceleration unlock const mode
        //   } else if (input[2] == 's') {
        //     carState.ConstantMode = CONSTANT_MODE::SPEED;
        //     carState.ConstantModeOn = true; // #SAFETY#: deceleration unlock const mode
        //   } else if (input[2] == 'p') {
        //     carState.ConstantMode = CONSTANT_MODE::POWER;
        //     carState.ConstantModeOn = true; // #SAFETY#: deceleration unlock const mode
        //   } else {
        //     carState.ConstantModeOn = true; // #SAFETY#: deceleration unlock const mode
        //   }
        //   console << "Received: '" << input << "' -->  carState.ConstantMode - " << CONSTANT_MODE_str[(int)(carState.ConstantMode)]
        //           << " On: " << carState.ConstantModeOn << NL;
        //   break;
        default:
          console << "Received: '" << input << "'\n";
          console << "ERROR:: Unknown command '" << input << "'" << NL;
          // break;
        // -------- Command Help -----------------
        case 'h':
          console << helpText << NL;
          break;
        }
      }
    } catch (exception &ex) {
      console << "Exception:" << ex.what() << NL;
    }
    taskSuspend();
  }
}

string CmdHandler::printSystemValues() {
  stringstream ss("");
#if ADC_ON
  int16_t valueDec = adc.STW_DEC;
  int16_t valueAcc = adc.STW_ACC;
  ss << fmt::format("ADC: dec={:5d}  acc={:5d}\n", valueDec, valueAcc);
#endif
#if DAC_ON
  ss << fmt::format("DAC: POT-0 (accel)= {:4d}, POT-1 (recup)= {:4d}\n", dac.get_pot(DAC::pot_chan::POT_CHAN0),
                    dac.get_pot(DAC::pot_chan::POT_CHAN1));
#endif
  // for (int devNr = 0; devNr < MCP23017_NUM_DEVICES; devNr++) {
  //   for (int pinNr = 0; pinNr < MCP23017_NUM_PORTS; pinNr++) {
  //     CarStatePin *pin = carState.getPin(devNr, pinNr);
  //     ss << fmt::format("{:20s} {:#04x}: {%10s} Pin ", pin->name, pin->port, pin->mode);
  //     if (pin->value == 0) {
  //       ss << "SET\n";
  //     } else {
  //       ss << "UNSET\n";
  //     }
  //   }
  // }
  return ss.str();
}
