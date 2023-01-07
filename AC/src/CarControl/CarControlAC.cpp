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
// #include <CarSpeed.h>
#include <CarState.h>
// #include <ConfigFile.h>
#include <Console.h>
#include <DriverDisplay.h>
#include <EngineerDisplay.h>
#include <Helper.h>
#include <I2CBus.h>
// #include <IOExt.h>
// #include <SDCard.h>

extern CANBus canBus;
extern CarControl carControl;
extern CarState carState;
extern Console console;
// extern DriverDisplay driverDisplay;
// extern EngineerDisplay engineerDisplay;
extern I2CBus i2cBus;
// extern IOExt ioExt;
// extern SDCard sdCard;

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

// bool CarControl::read_reference_cell_data() {
//   carState.ReferenceSolarCell = adc.REFERENCE_CELL; // TODO
//   return true;
// }

bool CarControl::read_potentiometer() {
  // int value = adc.SWITCH_POTENTIOMENTER;
  // if (carState.Potentiometer != value) {
  //   carState.Potentiometer = value;
  //   return true;
  // }
  return false;
}

bool CarControl::read_speed() {
  // float diameter = 0.50;           // m
  // int16_t value = adc.MOTOR_SPEED; // native input
  // float voltage = value * adc.get_multiplier();
  // float rpm = 370 * voltage;                              // round per minute
  // int speed = round(3.1415 * diameter * rpm * 6. / 100.); // unit: km/h
  // if (carState.Speed != speed) {
  //   carState.Speed = speed;
  //   return true;
  // }
  return false;
}

// int CarControl::calculate_acceleration_display(int valueDec, int valueAcc) {
//   if (valueDec > 0) {
//     return -_transform(0, MAX_ACCELERATION_DISPLAY_VALUE, 0, UINT16_MAX, valueDec);
//   } else {
//     return _transform(0, MAX_ACCELERATION_DISPLAY_VALUE, 0, UINT16_MAX, valueAcc);
//   }
// }

bool CarControl::read_PLUS_MINUS() {
  bool hasChanged = false;
  // if (carState.BreakPedal) {
  //   _set_dec_acc_values(DAC_MAX, 0, ADC_MAX, 0, -64);
  //   return true;
  // }

  // bool plusButton = false;  // ioExt.getPort(carState.getPin(PinIncrease)->port) == 0;  // currently pressed?
  // bool minusButton = false; // ioExt.getPort(carState.getPin(PinDecrease)->port) == 0; // currently pressed?
  // if (!plusButton && !minusButton)
  //   return false;

  // int8_t acceleration; // -99 ... +99
  // if (carState.Deceleration > 0) {
  //   acceleration = -carState.Deceleration;
  // } else {
  //   acceleration = carState.Acceleration;
  // }

  // if (minusButton) {
  //   if (acceleration - carState.ButtonControlModeIncrease > -99)
  //     acceleration -= carState.ButtonControlModeIncrease;
  //   else
  //     acceleration = -99;
  // } else if (plusButton) {
  //   if (acceleration + carState.ButtonControlModeIncrease < 99)
  //     acceleration += carState.ButtonControlModeIncrease;
  //   else
  //     acceleration = 99;
  // }

  // int8_t valueDec = 0;
  // int8_t valueAcc = 0;
  // if (acceleration < 0) {
  //   valueDec = -acceleration;
  // } else if (acceleration > 0) {
  //   valueAcc = acceleration;
  // }

  // int valueDisplay = valueDec > 0 ? -valueDec : valueAcc;
  // // prepare and write motor acceleration and recuperation values to DigiPot
  // int valueDecPot = 0;
  // int valueAccPot = 0;
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
  // if (verboseMode) {
  //   console << fmt::format(
  //       "button mode: acc={:3d} --> valueDecPot={:4d}, valueAccPot={:4d} | valueDec={:3d}, valueAcc={:3d}, valueDisplay={:3d}\n",
  //       acceleration, valueDecPot, valueAccPot, valueDec, valueAcc, valueDisplay);
  // }
  return hasChanged;
}

bool CarControl::read_paddles() {
  bool hasChanged = false;
  // if (carState.BreakPedal) {
  //   _set_dec_acc_values(DAC_MAX, 0, ADC_MAX, 0, -64);
  //   if (verboseMode) {
  //     console << fmt::format(
  //         "paddle mode BREAK PEDAL: valueDecPot={:4d}, valueAccPot={:4d} | valueDec={:3d}, valueAcc={:3d}, valueDisplay={:3d}\n",
  //         DAC_MAX, 0, ADC_MAX, 0, -64);
  //   }
  //   return true;
  // }
  // int16_t valueDec = adc.STW_DEC;
  // int16_t valueAcc = adc.STW_ACC;
  // // check if change is in damping
  // // if (valueAcc != 0 && valueDec != 0)
  // //   if (abs(accelLast - valueAcc) < carState.PaddleDamping && abs(recupLast - valueDec) < carState.PaddleDamping)
  // //     return false;

  // // #SAFETY#: Reset constant mode on deceleration paddel touched
  // if (valueDec > carState.PaddleDamping) {
  //   carState.ConstantModeOn = false;
  // }

  // int valueDisplay = calculate_displayvalue_acc_dec(valueDec, valueAcc);
  // // prepare and write motor acceleration and recuperation values to DigiPot
  // int valueDecPot = 0;
  // int valueAccPot = 0;
  // if (valueDisplay < 0) {
  //   valueDecPot = -(int)(((float)valueDisplay / MAX_ACCELERATION_DISPLAY_VALUE) * DAC_MAX);
  //   valueAccPot = 0;
  // } else {
  //   valueDecPot = 0;
  //   valueAccPot = (int)(((float)valueDisplay / MAX_ACCELERATION_DISPLAY_VALUE) * DAC_MAX);
  // }

  // if (valueDisplayLast != valueDisplay) {
  //   hasChanged = true;
  // }
  // _set_dec_acc_values(valueDecPot, valueAccPot, valueDec, valueAcc, valueDisplay);
  // if (verboseMode && hasChanged) {
  //   console << fmt::format(
  //       "paddle mode            : valueDecPot={:4d}, valueAccPot={:4d} | valueDec={:3d}, valueAcc={:3d}, valueDisplay={:3d}\n",
  //       valueDecPot, valueAccPot, valueDec, valueAcc, valueDisplay);
  // }
  return hasChanged;
}

void CarControl::_set_dec_acc_values(int valueDecPot, int valueAccPot, int16_t valueDec, int16_t valueAcc, int valueDisplay) {
  // if (carState.ConstantModeOn && valueDec == 0 && valueAcc == 0)
  //   return;
  // dac.set_pot(valueDecPot, DAC::pot_chan::POT_CHAN1);
  // dac.set_pot(valueAccPot, DAC::pot_chan::POT_CHAN0);
  carState.Deceleration = valueDec;
  carState.Acceleration = valueAcc;
  carState.AccelerationDisplay = valueDisplay;
  valueDisplayLast = valueDisplay;
}

bool CarControl::read_nextScreenButton() {
  if (!SystemInited)
    return false;

  int button1pressed = !digitalRead(ESP32_AC_BUTTON_1);
  if (!button1pressed)
    return false;

  switch (carState.displayStatus) {
  case DISPLAY_STATUS::ENGINEER_RUNNING:
    carState.displayStatus = DISPLAY_STATUS::DRIVER_SETUP;
    console << "Switch Next Screen toggle: switch from eng --> driver\n";
    break;
  case DISPLAY_STATUS::DRIVER_RUNNING:
    carState.displayStatus = DISPLAY_STATUS::ENGINEER_SETUP;
    console << "Switch Next Screen toggle: switch from driver --> engineer\n";
    break;
  default:
    break;
  }
  return true;
}

bool CarControl::read_ConstantModeButton() {
  if (!SystemInited)
    return false;

  int button1pressed = !digitalRead(ESP32_AC_BUTTON_2);
  if (!button1pressed)
    return false;

  if (carState.ConstantMode == CONSTANT_MODE::POWER) {
    carState.ConstantMode = CONSTANT_MODE::SPEED;
  } else {
    carState.ConstantMode = CONSTANT_MODE::POWER;
  }
  carState.TargetSpeed = carState.Speed;                                       // unit: km/h
  carState.TargetPower = carState.MotorCurrent * carState.MotorVoltage / 1000; // unit: kW
  return true;
}

volatile int CarControl::valueChangeRequest = 0;

void CarControl::task(void *pvParams) {
  while (1) {
    if (SystemInited) {
      // console << "CarControl Task running on core " << xPortGetCoreID() << NL;

      // handle changed INPUT pins
      bool someThingChanged = false;
      // someThingChanged |= read_paddles();
      // someThingChanged |= read_PLUS_MINUS();
      // someThingChanged |= read_speed();
      // someThingChanged |= read_potentiometer();
      // someThingChanged |= read_reference_cell_data();
      // someThingChanged |= read_break_pedal();

      someThingChanged |= read_nextScreenButton();
      someThingChanged |= read_ConstantModeButton();

      bool button2pressed = !digitalRead(ESP32_AC_BUTTON_2); // switch constant mode (Off, Speed, Power)

      canBus.writePacket(DC_BASE_ADDR | 0x00,
                         carState.LifeSign,        // LifeSign
                         (uint16_t)button2pressed, // switch constant mode Speed / Power
                         (uint16_t)0,              // empty
                         (uint16_t)0               // empty
      );

      if (carControl.verboseMode)
        console << fmt::format("[{:02d}|{:02d}] CAN.PacketId=0x{:03x}-S-data:LifeSign={:4x}, button2 = {:1x} ", canBus.availiblePackets(),
                               canBus.getMaxPacketsBufferUsage(), AC_BASE_ADDR | 0x00, carState.LifeSign, button2pressed)
                << NL;

      // // one data row per second
      // if ((millis() > millisNextStampCsv) || (millis() > millisNextStampSnd)) {
      //   // if (sdCard.isReadyForLog() && millis() > millisNextStampCsv) {
      //   //   sdCard.write(record);
      //   //   millisNextStampCsv = millis() + carState.LogInterval;
      //   // }
      //   if (verboseModeCarControlMax) {
      //     string record = carState.csv();
      //     if (millis() > millisNextStampSnd) {
      //       console << "d: " + record;
      //       millisNextStampSnd = millis() + carState.CarDataSendPeriod;
      //     }
      //   }
      // }
    }
    taskSuspend();
  }
}
