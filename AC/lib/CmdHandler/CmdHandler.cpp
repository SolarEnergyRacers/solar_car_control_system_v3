//
// Command Receiver and Handler
//
// reads commands from serial console and deploy it
#include <global_definitions.h>
#include "../definitions.h"

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
#include <RTC_SER.h>
#include <RtcDateTime.h>
#endif

#include <Wire.h> // I2C

#include <CANBus.h>
#include <CarControl.h>
#include <CarState.h>
#include <CarStatePin.h>
#include <CarStateRadio.h>
#include <CmdHandler.h>
#include <Console.h>
#include <Display.h>
#include <DriverDisplay.h>
#include <EngineerDisplay.h>
#include <Helper.h>
#include <I2CBus.h>
// #include <RTC.h>
#include <SDCard.h>
#include <Serial.h>
#include <System.h>

extern CANBus can;
extern I2CBus i2cBus;
extern CarState carState;
extern CarStateRadio carStateRadio;
extern CANBus canBus;
extern CarControl carControl;
extern Display display;
extern DriverDisplay driverDisplay;
extern EngineerDisplay engineerDisplay;
extern SDCard sdCard;
extern Console console;
extern Uart uart;
#if RTC_ON
extern GlobalTime globalTime;
#endif

extern bool SystemInited;

using namespace std;

// ------------------
// FreeRTOS functions

string CmdHandler::re_init() { return init(); }

string CmdHandler::init() {
  bool hasError = false;
  return fmt::format("[{}] CmdHandler initialized.", hasError ? "--" : "ok");
}

void CmdHandler::exit() {
  // TODO
}

void CmdHandler::task(void *pvParams) {
  string state, msg;
  while (1) {
    try {
      while (SystemInited && (Serial.available()
                              // #if SERIAL_RADIO_ON
                              //                               || Serial2.available()
                              // #endif
                              )) {
        // read the incoming chars:
        String input = "";
        if (Serial.available()) {
          input = Serial.readString();
          Serial.flush();
          // #if SERIAL_RADIO_ON
          //         } else if (Serial2.available()) {
          //           input = Serial2.readString();
          //           Serial2.flush();
          // #endif
        }
        if (input.length() == 0)
          break;
        if (input.endsWith("\n")) {
          input = input.substring(0, input.length() - 1);
        }
        if (input.endsWith("\r")) {
          input = input.substring(0, input.length() - 1);
        }
        if (input.length() == 0)
          break;

        switch (input[0]) {
        // ---------------- controller commands
        case '\n':
          console << NL;
          break;
        case 'R':
          display.set_DisplayStatus(DISPLAY_STATUS::ENGINEER_HALTED);
          driverDisplay.re_init();
          break;
        case 'E':
          display.set_DisplayStatus(DISPLAY_STATUS::ENGINEER_CONSOLE);
          display.clear_screen(ILI9341_WHITE);
          break;
        case 'D':
          display.set_DisplayStatus(DISPLAY_STATUS::ENGINEER_HALTED);
          display.set_DisplayStatus(DISPLAY_STATUS::DRIVER_SETUP);
          break;
        case 'S':
          if (input[1] == 'a') {
            console << printSystemValues();
          } else {
            console << carState.print("Recent State (CMD-S)") << NL;
          }
          break;
        case 'J':
          state = carState.serialize("Recent State (CMD-J)");
          console << state;
          break;
        case 'V':
          state = carState.csv("Recent State (CMD-V)", input[1] == '+' ? true : false); // +: with header
          sdCard.write_log(state);
          console << state;
          break;
        case 'M':
          sdCard.mount();
          break;
        case 'P':
          sdCard.directory();
          break;
        case 'U':
          sdCard.unmount();
          break;
        case 'F':
          carState.initalize_config();
          console << carState.print("State after reading SER4CNFG.INI") << NL;
          break;
        case 'H':
          console << "Not implemented yet!" << NL;
          // memory_info();
          break;
        case 'B':
          if (input[1] == '\0') {
            console << "Serial2 baudrate=" << carState.Serial2Baudrate << NL;
          } else if (input[1] == 'v') {
            carStateRadio.verboseModeRadioSend = !carStateRadio.verboseModeRadioSend;
            console << "set verboseModeRadioSend: " << carStateRadio.verboseModeRadioSend << NL;
          } else if (input[1] == 'm') {
            if (carStateRadio.mode == SEND_MODE::ASCII)
              carStateRadio.mode = SEND_MODE::BINARY;
            else
              carStateRadio.mode = SEND_MODE::ASCII;
            console << "set radio send: " << SEND_MODE_str[(int)carStateRadio.mode] << NL;
          } else if (input[1] == 'r') {
            carState.Serial2Baudrate = atof(&input[2]);
            Serial2.end();
            Serial2.begin(carState.Serial2Baudrate, SERIAL_8N1, SERIAL2_RX, SERIAL2_TX);
          }
          console << "Serial2(radio) baudrate=" << carState.Serial2Baudrate << ", send mode: " << SEND_MODE_str[(int)carStateRadio.mode]
                  << NL;
          break;
        case 'I':
          if (input[1] == 's') {
            console << "Received: '" << input << "' -->  i2cBus.scan_i2c_devices()\n";
            i2cBus.scan_i2c_devices();
          }
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
          } else if (input[1] == 'L') {
            canBus.verboseModeCanBusLoad = !canBus.verboseModeCanBusLoad;
            console << "set verboseModeCanBusLoad: " << canBus.verboseModeCanBusLoad << NL;
          } else if (input[1] == 'S') {
            sdCard.verboseModeSdCard = !sdCard.verboseModeSdCard;
            console << "set verboseModeSdCard: " << sdCard.verboseModeSdCard << NL;
          } else {
            string arr[4];
            splitString(arr, &input[1]);
            float motCurrent = atof(arr[0].c_str());
            float batVoltage = atof(arr[1].c_str());
            float batCurrent = atof(arr[2].c_str());
            carState.MotorCurrent = motCurrent;
            // Injection into BMS-CAN!!
            int packetId = BMS_BASE_ADDR | 0xFA;
            CANPacket packet = CANPacket(packetId, (uint64_t)0);
            packet.setData_i32(0, batVoltage * 1000);
            packet.setData_i32(1, batCurrent * 1000);
            canBus.pushIn(packet);
            console << fmt::format("CarState direct set motCurrent={}; CAN inject for AdrId[{:3x}]: batCurrent={}, batVoltage={}\n",
                                   motCurrent, packetId, batCurrent, batVoltage);
          }
          break;
        case 'O':
          if (input[1] == 'o') {
            carControl.verboseModeCarControl = !carControl.verboseModeCarControl;
            console << "set verboseModeCarControl: " << carControl.verboseModeCarControl << NL;
          } else if (input[1] == 'O') {
            carControl.verboseModeCarControlDebug = !carControl.verboseModeCarControlDebug;
            console << "set verboseModeCarControlDebug: " << carControl.verboseModeCarControlDebug << NL;
          } else {
            console << "set verboseModeCarControl CarControl needs a specifier: o,O." << NL;
          }
          break;
        case 'T': {
          string arr[6];
          int count = splitString(arr, &input[1]);
          if (count == 0) {
            console << "Received: '" << input.c_str() << "' --> DateTime: " << globalTime.strTime("%X %F (%a)")
                    << ", Uptime: " << globalTime.strUptime(true) << NL;
          } else {
            int yy = atof(arr[0].c_str());
            int mm = atof(arr[1].c_str());
            int dd = atof(arr[2].c_str());
            int hh = atof(arr[3].c_str());
            int MM = atof(arr[4].c_str());
            int ss = atof(arr[5].c_str());
            RtcDateTime dateTime = RtcDateTime(yy, mm, dd, hh, MM, ss);
            globalTime.set_RTC(dateTime);
            console << "Received: '" << input.c_str() << "' --> Onboard time now: " << globalTime.strTime("%X %F (%a)") << NL;
          }
        } break;
        case 'K': {
#if CARSPEED_ON
          console << "Received: '" << input.c_str() << "' --> ";
          string arr[4];
          int count = splitString(arr, &input[1]);
          if (count == 0) {
            console << "PID parameters: ";
          } else {
            carState.Kp = atof(arr[0].c_str());
            carState.Ki = atof(arr[1].c_str());
            carState.Kd = atof(arr[2].c_str());
            // later Kp,Ki,Kd will be sent by CAN to DC
            console << "PID set parameters: ";
          }
          console << "Kp=" << carState.Kp << ", Ki=" << carState.Ki << ", Kd=" << carState.Kd << NL;
          // }
#else
          console << "Car speed control settings only on DC possible yet\n";
#endif
        } break;
        case 'i':
          carControl.verboseModeCarControl = !carControl.verboseModeCarControl;
          console << "set verboseModeCarControl: " << carControl.verboseModeCarControl << NL;
          break;
        //-------- DRIVER INFO COMMANDS --------------------
        case 's':
          if (input[1] == '+')
            carState.SpeedArrow = SPEED_ARROW::INCREASE;
          else if (input[1] == '-')
            carState.SpeedArrow = SPEED_ARROW::DECREASE;
          else
            carState.SpeedArrow = SPEED_ARROW::OFF;
          console << "Received: '" << input.c_str() << "' -->  carState.SpeedArrow=" << SPEED_ARROW_str[(int)(carState.SpeedArrow)] << NL;
          break;
        case ':':
          carState.DriverInfoType = INFO_TYPE::INFO;
          carState.DriverInfo = &input[1];
          console << "Received: '" << input.c_str() << "' -->  carState.DriverInfo " << INFO_TYPE_str[(int)carState.DriverInfoType] << ": "
                  << carState.DriverInfo << NL;
          break;
        case '!':
          carState.DriverInfoType = INFO_TYPE::WARN;
          carState.DriverInfo = &input[1];
          console << "Received: '" << input.c_str() << "' -->  carState.DriverInfo " << INFO_TYPE_str[(int)carState.DriverInfoType] << ": "
                  << carState.DriverInfo << NL;
          break;
        // -------- Driver SUPPORT COMMANDS -----------------
        case 'c':
          if (input[1] == '-') {
            carState.ConstantModeOn = false;              // #SAFETY#: deceleration unlock const mode
          } else if (input[1] == '+') {
            carState.ConstantModeOn = true;               // #SAFETY#: deceleration unlock const mode
          } else if (input[1] == 's') {
            carState.ConstantMode = CONSTANT_MODE::SPEED; // #SAFETY#: deceleration unlock const mode
          } else if (input[1] == 'p') {
            carState.ConstantMode = CONSTANT_MODE::POWER; // #SAFETY#: deceleration unlock const mode
          } else {
            // show state
          }
          console << "Received: '" << input.c_str() << "' -->  carState.ConstantMode - " << CONSTANT_MODE_str[(int)(carState.ConstantMode)]
                  << NL;
          break;
        default:
          console << "ERROR:: Unknown command '" << input.c_str() << "' \n" << helpText << NL;
          break;
        // -------- Command Help -----------------
        case 'h':
        case '?':
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
  stringstream ss("nothing to show");
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
