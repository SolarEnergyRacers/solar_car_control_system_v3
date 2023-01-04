//
// MCP23017 I/O Extension over I2C
//
#include <definitions.h>

#include <stdio.h>

// standard libraries
#include <fmt/core.h>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <MCP23017.h> // MCP23017
#include <Wire.h>     // I2C

#include <CarControl.h>
#include <Console.h>
// #include <DriverDisplay.h>
// #include <EngineerDisplay.h>
#include <Helper.h>
#include <I2CBus.h>
#include <IOExt.h>
#include <IOExtHandler.h>

extern Console console;
extern I2CBus i2cBus;
extern IOExt ioExt;
extern CarState carState;
extern CarControl carControl;
extern bool SystemInited;

void breakPedalHandler() {
  if (!SystemInited)
    return;

  carState.BreakPedal = carState.getPin(PinDI_Break)->value == 0;
  if (!carState.BreakPedal) { // break pedal released
    carControl.reset_acceleration_values();
    carState.ConstantModeOn = false;
  }
  if (ioExt.verboseModeDInHandler)
    console << "Break pedal pressed " << (carState.BreakPedal ? "pressed" : "released") << NL;
}

void buttonSetHandler() {
  if (!SystemInited)
    return;
  if (carState.getPin(PinDI_Button_Set)->value != 0)
    return;

  carState.ConstantModeOn = true;                                              // #SAFETY#: deceleration unlock const mode
  carState.TargetSpeed = carState.Speed;                                       // unit: km/h
  carState.TargetPower = carState.MotorCurrent * carState.MotorVoltage / 1000; // unit: kW
  if (ioExt.verboseModeDInHandler)
    console << "ConstantMode ON"
            << " - overtake speed " << carState.TargetSpeed << " / power " << carState.TargetPower << NL;
}

void buttenResetHandler() {
  if (!SystemInited)
    return;
  if (carState.getPin(PinDI_Button_Reset)->value != 0)
    return;

  carState.ConstantModeOn = false; // #SAFETY#: deceleration unlock const mode
  if (ioExt.verboseModeDInHandler)
    console << "ConstantMode OFF"
            << " - buttenResetHandler " << NL;
}

// void constantModeHandler() {
//   if (!SystemInited)
//     return;
// //   if (carState.getPin(PinConstantMode)->value != 0)
// //     return;

// //   if (ioExt.verboseModeDInHandler)
// //     console << "Constant mode toggle\n";
//   if (carState.ConstantMode == CONSTANT_MODE::POWER) {
//     carState.ConstantMode = CONSTANT_MODE::SPEED;
//   } else {
//     carState.ConstantMode = CONSTANT_MODE::POWER;
//   }
//   carState.TargetSpeed = carState.Speed;                                       // unit: km/h
//   carState.TargetPower = carState.MotorCurrent * carState.MotorVoltage / 1000; // unit: kW
// }

void buttonPlusHandler() {
  if (!SystemInited || !carState.ConstantModeOn) // || carState.ControlMode == CONTROL_MODE::BUTTONS)
    return;
  carState.TargetSpeed -= carState.ConstSpeedIncrease;
  if (carState.TargetSpeed < 0)
    carState.TargetSpeed = 0;

  carState.TargetPower -= carState.ConstPowerIncrease;
  if (carState.TargetPower < 0)
    carState.TargetPower = 0;
  if (ioExt.verboseModeDInHandler)
    console << "Decrease constant mode target to " << carState.TargetSpeed << "km/h / " << carState.TargetPower << "W.\n";
}

void buttonMinusHandler() {
  if (!SystemInited || !carState.ConstantModeOn) // || carState.ControlMode == CONTROL_MODE::BUTTONS)
    return;
  carState.TargetSpeed += carState.ConstSpeedIncrease;
  if (carState.TargetSpeed > 111) // only until 111km/h
    carState.TargetSpeed = 111;

  carState.TargetPower += carState.ConstPowerIncrease;
  if (carState.TargetPower > 4500) // only until 5kW
    carState.TargetPower = 4500;
  if (ioExt.verboseModeDInHandler)
    console << "Increase constant mode target to " << carState.TargetSpeed << "km/h / " << carState.TargetPower << " W.\n";
}

void fwdBwdHandler() {
  carState.DriveDirection = carState.getPin(PinDI_FWD_BWD)->value == 1 ? DRIVE_DIRECTION::FORWARD : DRIVE_DIRECTION::BACKWARD;
  if (ioExt.verboseModeDInHandler)
    console << "Direction " << (carState.DriveDirection == DRIVE_DIRECTION::FORWARD ? "Forward" : "Backward") << "\n";
}

void mcOnOffHandler() {
  carState.MotorOn = carState.getPin(PinDI_MCONOFF)->value == 1;
  if (ioExt.verboseModeDInHandler)
    console << "MC " << (carState.MotorOn ? "On" : "Off") << "\n";
}
