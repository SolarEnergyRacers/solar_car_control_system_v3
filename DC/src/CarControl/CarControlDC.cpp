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

#include <ADC.h>
#include <CANBus.h>
#include <CarControl.h>
// #include <CarSpeed.h>
#include <CarState.h>
// #include <ConfigFile.h>
#include <Console.h>
#include <DAC.h>
#include <Helper.h>
#include <I2CBus.h>
// #include <IOExt.h>
// #include <MCP23017.h>

extern ADC adc;
extern CANBus canBus;
extern CarControl carControl;
// extern CarSpeed carSpeed;
extern CarState carState;
extern Console console;
extern DAC dac;
extern I2CBus i2cBus;
// extern IOExt ioExt;

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
// --------------------

void CarControl::_handleValueChanged() {
  // TODO
}

bool CarControl::read_reference_cell_data() {
  carState.ReferenceSolarCell = 0; // adc.REFERENCE_CELL; // TODO: no free ADC available
  return true;
}

bool CarControl::read_potentiometer() {
  int value = adc.switch_potentiometer;
  if (carState.Potentiometer != value) {
    carState.Potentiometer = value;
    carState.ConstSpeedIncrease = transformArea(1, 20, 0, INT16_MAX, value);
    carState.ConstPowerIncrease = transformArea(1, 500, 0, INT16_MAX, value);
    return true;
  }
  return false;
}

bool CarControl::read_speed() {
  float diameter = 0.50;                                  // wheel diameter [m]
  int16_t value = adc.motor_speed;                        // native input
  float voltage = value * adc.get_multiplier();           // voltage multiplier
  float rpm = 370 * voltage;                              // round per minute
  int speed = round(3.1416 * diameter * rpm * 6. / 100.); // [km/h]
  if (carState.Speed != speed) {
    carState.Speed = speed;
    return true;
  }
  return false;
}

int CarControl::calculate_acceleration_display(int valueDec, int valueAcc) {
  if (valueDec > 0) {
    return -transformArea(0, MAX_ACCELERATION_DISPLAY_VALUE, 0, INT16_MAX, valueDec);
    // return transformArea(MAX_ACCELERATION_DISPLAY_VALUE, 0, 0, UINT16_MAX, valueDec);
  } else {
    return transformArea(0, MAX_ACCELERATION_DISPLAY_VALUE, 0, INT16_MAX, valueAcc);
  }
}

// bool CarControl::read_PLUS_MINUS() {
//   //   bool hasChanged = false;
//   if (carState.BreakPedal) {
//     _set_dec_acc_values(DAC_MAX, 0, ADC_MAX, 0, -64);
//     return true;
//   }
//   bool plusButton = false;  // ioExt.getPort(carState.getPin(PinIncrease)->port) == 0;  // currently pressed?
//   bool minusButton = false; // ioExt.getPort(carState.getPin(PinDecrease)->port) == 0; // currently pressed?
//   if (!plusButton && !minusButton)
//     return false;

//   carState.ConstantModeOn = true;
//   int8_t acceleration; // -99 ... +99
//   if (carState.Deceleration > 0) {
//     acceleration = -carState.Deceleration;
//   } else {
//     acceleration = carState.Acceleration;
//   }

//   if (minusButton) {
//     if (acceleration - carState.ButtonControlModeIncrease > -99)
//       acceleration -= carState.ButtonControlModeIncrease;
//     else
//       acceleration = -99;
//   } else if (plusButton) {
//     if (acceleration + carState.ButtonControlModeIncrease < 99)
//       acceleration += carState.ButtonControlModeIncrease;
//     else
//       acceleration = 99;
//   }

// int8_t valueDec = 0;
// int8_t valueAcc = 0;
// if (acceleration < 0) {
//   valueDec = -acceleration;
// } else if (acceleration > 0) {
//   valueAcc = acceleration;
// }

// int valueDisplay = valueDec > 0 ? -valueDec : valueAcc;
// // prepare and write motor acceleration and recuperation values to DigiPot
// int valueDecPot;
// int valueAccPot;
// if (valueDisplay < 0) {
//   valueDecPot = -(int)(((float)valueDisplay / MAX_ACCELERATION_DISPLAY_VALUE) * DAC_MAX);
//   valueAccPot = 0;
// } else {
//   valueDecPot = 0;
//   valueAccPot = (int)(((float)valueDisplay / MAX_ACCELERATION_DISPLAY_VALUE) * DAC_MAX);
// }

// if (valueDisplayLast != valueDisplay) {
//   hasChanged = true;
//   _set_dec_acc_values(valueDecPot, valueAccPot, valueDec, valueAcc, valueDisplay);
// }
// if (carControl.verboseMode) {
//   console << fmt::format(
//       "button mode: acc={:5d} --> valueDecPot={:5d}, valueAccPot={:5d} | valueDec={:5d}, valueAcc={:5d}, valueDisplay={:5d}\n",
//       acceleration, valueDecPot, valueAccPot, valueDec, valueAcc, valueDisplay);
// }
//   return hasChanged;
// }

bool CarControl::read_paddles() {
  if (carState.BreakPedal) {
    // #SAFETY#: on break pedal -> deccelerate
    //_set_dec_acc_values(DAC_MAX, 0, ADC_MAX, 0, -64);
    if (carControl.verboseMode) {
      console << fmt::format(
          "paddle mode BREAK PEDAL: valueDecPot={:5d}, valueAccPot={:5d} | valueDec={:5d}, valueAcc={:5d}, valueDisplay={:5d}\n", DAC_MAX,
          0, ADC_MAX, 0, -64);
    }
    return true;
  }
  // carState.Deceleration = transformArea(ads_min_dec, ads_max_dec, 0, INT16_MAX, adc.stw_dec);
  // carState.Acceleration = transformArea(ads_min_acc, ads_max_acc, 0, INT16_MAX, adc.stw_acc);
  carState.Deceleration = _normalize_0_UINT16(ads_min_dec, ads_max_dec, adc.stw_dec);
  carState.Acceleration = _normalize_0_UINT16(ads_min_acc, ads_max_acc, adc.stw_acc);

  // check if change is in damping
  // if (abs(accelLast - carState.Acceleration) < carState.PaddleDamping && abs(recupLast - carState.Deceleration) <
  // carState.PaddleDamping)
  //   return false;

  accelLast = carState.Acceleration;
  recupLast = carState.Deceleration;

  // #SAFETY#: Reset constant mode on deceleration paddel touched
  if (carState.Deceleration > carState.PaddleDamping) {
    carState.ConstantModeOn = false;
  }

  carState.AccelerationDisplay = calculate_acceleration_display(carState.Deceleration, carState.Acceleration);

  // console << fmt::format("Accl={:5d} |{:5d}, Decl={:5d} |{:5d} => {:4d}\n", carState.Acceleration, adc.stw_acc, carState.Deceleration,
  //                        adc.stw_dec, carState.AccelerationDisplay);

  if (valueDisplayLast != carState.AccelerationDisplay) {
    valueDisplayLast = carState.AccelerationDisplay;
    if (carControl.verboseMode)
      console << fmt::format("Accl={:5d} |{:5d}, Decl={:5d} |{:5d} => {:4d}\n", carState.Acceleration, adc.stw_acc, carState.Deceleration,
                             adc.stw_dec, carState.AccelerationDisplay);
    return true;
  }
  return false;
}

// prepare and write motor acceleration and recuperation values to DigiPot
void CarControl::_set_DAC() {
  int setpoint = (int)(((float)carState.AccelerationDisplay / MAX_ACCELERATION_DISPLAY_VALUE) * DAC_MAX);
  int valueDecPot = carState.AccelerationDisplay < 0 ? -setpoint : 0;
  int valueAccPot = carState.AccelerationDisplay < 0 ? 0 : setpoint;
  dac.set_pot(valueDecPot, DAC::pot_chan::POT_CHAN1);
  dac.set_pot(valueAccPot, DAC::pot_chan::POT_CHAN0);

  if (carControl.verboseMode) {
    console << fmt::format("valueDecPot={:5d}, valueAccPot={:5d} | valueDec={:5d}, valueAcc={:5d}, valueDisplay={:5d}\n", valueDecPot,
                           valueAccPot, carState.Deceleration, carState.Acceleration, carState.AccelerationDisplay);
  }
}

void CarControl::_set_dec_acc_values(int valueDecPot, int valueAccPot, int16_t valueDec, int16_t valueAcc, int valueDisplay) {
  dac.set_pot(valueDecPot, DAC::pot_chan::POT_CHAN1);
  dac.set_pot(valueAccPot, DAC::pot_chan::POT_CHAN0);
  valueDisplayLast = valueDisplay;
}

unsigned int CarControl::_normalize_0_UINT16(int minOriginValue, int maxOriginValue, int value) {
  float k = UINT16_MAX / (maxOriginValue - minOriginValue);
  value = value < minOriginValue ? minOriginValue : value;
  value = value > UINT16_MAX ? UINT16_MAX : value;
  return (unsigned int)round((value - minOriginValue) * k);
}
volatile int CarControl::valueChangeRequest = 0;

void CarControl::task(void *pvParams) {
  while (1) {
    if (SystemInited) {
      // update OUTPUT pins
      // ioExt.writeAllPins(PinHandleMode::FORCED);
      // read values from ADC/IO
      read_paddles();
      read_reference_cell_data();
      read_speed();
      read_potentiometer();
      _set_DAC();
      carState.LifeSign++;

      canBus.writePacket(DC_BASE_ADDR | 0x00,
                         carState.LifeSign,      // LifeSign
                         carState.Potentiometer, // Potentiometer value
                         carState.Acceleration,  // HAL-paddle Acceleration ADC value
                         carState.Deceleration   // HAL-paddle Deceleration ADC value
      );
      bool driveDirection = carState.DriveDirection == DRIVE_DIRECTION::FORWARD ? 1 : 0;
      canBus.writePacket(DC_BASE_ADDR | 0x01,
                         (uint16_t)carState.TargetSpeed, // Target Speed [float as value\*1000]
                         (uint16_t)(carState.TargetPower * 1000), // Target Power [float as value\*1000]
                         carState.AccelerationDisplay,            // Display Acceleration
                         0,                                       // empty
                         carState.Speed,                          // Display Speed
                         driveDirection,                          // Fwd [1] / Bwd [0]
                         carState.BreakPedal,                     // Button Lvl Brake Pedal
                         carState.MotorOn,                        // MC Off [0] / On [1]
                         carState.ConstantModeOn,                 // Constant Mode Off [false], On [true]
                         false,                                   // empty
                         false,                                   // empty
                         false,                                   // empty
                         false                                    // empty
      );

      if (carControl.verboseModeDebug) {
        console << fmt::format("[{:02d}|{:02d}] P.Id=0x{:03x}-S-data:lifesign={:5d}, poti={:5d}, decl={:5d}, accl={:5d}",
                               canBus.availiblePackets(), canBus.getMaxPacketsBufferUsage(), DC_BASE_ADDR | 0x01, carState.LifeSign,
                               carState.Potentiometer, carState.Acceleration, carState.Deceleration)
                << NL;
        console << fmt::format("        P.Id=0x{:03x}-S-data:tgtSpeed={:5d}, Powr={:5d}, accD={:5d}, cMod={:1d}, speed={:3d}, "
                               "direct={:1d}, break={}, MotorOn={}, ConstandModeOn={}",
                               DC_BASE_ADDR | 0x01, (uint16_t)(carState.TargetSpeed * 1000), (uint16_t)(carState.TargetSpeed * 1000),
                               carState.AccelerationDisplay, carState.ConstantModeOn, carState.Speed, driveDirection, carState.BreakPedal,
                               carState.MotorOn,carState.ConstantModeOn)
                << NL;
      }
      //  one data row per second
      //  if ((millis() > millisNextStampCsv) || (millis() > millisNextStampSnd)) {
      //    // if (sdCard.isReadyForLog() && millis() > millisNextStampCsv) {
      //    //   sdCard.write(record);
      //    //   millisNextStampCsv = millis() + carState.LogInterval;
      //    // }
      //    if (carControl.verboseModeCarControlMax) {
      //      string record = carState.csv();
      //      if (millis() > millisNextStampSnd) {
      //        console << "d: " + record << NL;
      //        millisNextStampSnd = millis() + carState.CarDataSendPeriod;
      //      }
      //    }
      //  }
    }
    taskSuspend();
  }
}
