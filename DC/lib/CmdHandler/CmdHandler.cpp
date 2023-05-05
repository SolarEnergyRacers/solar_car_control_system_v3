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

#include <ADC_SER.h>
#include <CANBus.h>
#include <CarControl.h>
#include <CarState.h>
#include <CarStatePin.h>
#include <CmdHandler.h>
#include <Console.h>
#include <ConstSpeed.h>
#include <DAC.h>
#include <Helper.h>
#include <I2CBus.h>
#include <IOExt.h>
#include <IOExtHandler.h>
// #include <RTC.h>
#include <System.h>

extern ADC adc;
extern CANBus can;
extern I2CBus i2cBus;
extern IOExt ioExt;
extern CarState carState;
extern CANBus canBus;
extern ConstSpeed constSpeed;
extern CarControl carControl;
extern Console console;
extern DAC dac;
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
      while (Serial.available()) {
        // read the incoming chars:
        String input = "";
        if (Serial.available()) {
          input = Serial.readString();
          Serial.flush();
        }
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
          console << "Received: '" << input.c_str() << "' --> ";
          if (input[1] == 's') {
            // console << "Received: '" << input.c_str() << "' -->  i2cBus.scan_i2c_devices()\n";
            i2cBus.scan_i2c_devices();
          } else if (input[1] == 'i') {
            ioExt.verboseModeDIn = !ioExt.verboseModeDIn;
            console << "set verboseModeDIn: " << ioExt.verboseModeDIn << NL;
          } else if (input[1] == 'o') {
            ioExt.verboseModeDOut = !ioExt.verboseModeDOut;
            console << "set verboseModeDOut: " << ioExt.verboseModeDOut << NL;
          } else if (input[1] == 'h') {
            ioExt.verboseModeDInHandler = !ioExt.verboseModeDInHandler;
            ioExt.readAndHandlePins(PinHandleMode::FORCED);
            console << "set verboseModeDInHandler: " << ioExt.verboseModeDInHandler << NL;
          } else if (input[1] == 'a') {
            adc.verboseModeADC = !adc.verboseModeADC;
            console << "set verboseModeADC: " << adc.verboseModeADC << NL;
          } else if (input[1] == 'd') {
            dac.verboseModeDAC = !dac.verboseModeDAC;
            console << "set verboseModeDAC: " << dac.verboseModeDAC << NL;
          } else if (input[1] == 'c') {
            carControl.verboseMode = !carControl.verboseMode;
            console << "set verboseMode for acc-/dec-controls: " << carControl.verboseMode << NL;
          } else if (input[1] == 'R') {
            console << ioExt.re_init() << NL;
            msg = ioExt.re_init();
            console << msg << NL;
          } else {
            console << "Read all IOs:" << NL;
            ioExt.readAllPins();
            console << carState.printIOs("", true, false) << NL;
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
          } else {
            string arr[4];
            splitString(arr, &input[1]);
            float batCurrent = atof(arr[0].c_str());
            float batVoltage = atof(arr[1].c_str());
            int address = BMS_BASE_ADDR | 0xFA;
            uint64_t data = 0;
            CANPacket packet = CANPacket(address, data);
            packet.setData_i32(0, batVoltage);
            packet.setData_i32(1, batCurrent);
            canBus.handle_rx_packet(packet);
            console << "CAN inject for AdrId[" << address << "]: batCurrent=" << batCurrent << ", batVoltage=" << batVoltage << NL;
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
            console << "Received: '" << input.c_str() << "' --> DateTime: " << asctime(&t) << NL;
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
            console << "Received: '" << input.c_str() << "' --> Set dateTime to: " << asctime(&t) << NL;
          }
#else
          console << "RTC deactivated\n";
#endif
        } break;
        case 'K':
          console << "Received: '" << input.c_str() << "' --> ";
          if (input[1] == 'v') {
            constSpeed.verboseModePID = !constSpeed.verboseModePID;
            console << "set verboseModePID: " << constSpeed.verboseModePID << NL;
          } else {
            string arr[4];
            int count = splitString(arr, &input[1]);
            if (count == 0) {
              console << "PID parameters: ";
            } else {
              float Kp = atof(arr[0].c_str());
              float Ki = atof(arr[1].c_str());
              float Kd = atof(arr[2].c_str());
              constSpeed.update_pid(Kp, Ki, Kd);
              console << "PID set parameters: ";
            }
            console << "Kp=" << carState.Kp << ", Ki=" << carState.Ki << ", Kd=" << carState.Kd << NL;
          }
          break;
          //-------- DRIVER INFO COMMANDS --------------------

        default:
          console << "ERROR:: Unknown command '" << input.c_str() << "' \n" << helpText << NL;
          break;
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
  ss << fmt::format("DAC: POT-0 (accel)= {:4d}, POT-1 (recup)= {:4d}\n", dac.get_pot(DAC::pot_chan::POT_CHAN0_ACC),
                    dac.get_pot(DAC::pot_chan::POT_CHAN1_DEC));
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
