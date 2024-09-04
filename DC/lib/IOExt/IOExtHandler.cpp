//
// MCP23017 I/O Extension over I2C
//
#include "../definitions.h"

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
#include <ConstSpeed.h>
#include <Helper.h>
#include <I2CBus.h>
#include <IOExt.h>
#include <IOExtHandler.h>

extern Console console;
extern I2CBus i2cBus;
extern IOExt ioExt;
extern CarState carState;
extern CarControl carControl;
extern ConstSpeed constSpeed;
extern bool SystemInited;

void breakPedalHandler() {
  if (!SystemInited)
    return;

  carState.BreakPedal = carState.getPin(PinDI_Break)->value == 0;
  carControl.read_paddles(); // read peaddels and handels breeak

  if (ioExt.verboseModeDInHandler)
    console << "Break pedal pressed " << (carState.BreakPedal ? "pressed" : "released") << NL;
}

void buttonSetHandler() {
  if (!SystemInited)
    return;
  if (carState.getPin(PinDI_Button_Set)->value != 0)
    return;
  carState.ConstantModeOn = !carState.ConstantModeOn; // #SAFETY#: deceleration unlock const mode
  if (ioExt.verboseModeDInHandler)
    console << "Set constant mode " << CONSTANT_MODE_str[(int)(carState.ConstantMode)] << ", target Speed: " << carState.TargetSpeed
            << "km/h / " << carState.TargetPower << "W.\n";
}

void buttonConfirmDriverInfoHandler() {
  if (!SystemInited)
    return;
  if (carState.getPin(PinDI_Button_Confirm)->value != 0)
    return;
  carState.ConfirmDriverInfo = true;
  carState.DriverInfo = "";
  if (ioExt.verboseModeDInHandler)
    console << "ConfirmDriverInfo: " << carState.ConfirmDriverInfo<< " - buttenConfirmDriverInfoResetHandler " << NL;
}

void buttonMinusHandler() {
  if (!SystemInited)
    return;
  if (!carState.ConstantModeOn) {
    carState.ConstantModeOn = true;
    carState.TargetSpeed = carState.Speed;                                       // unit: km/h
    carState.TargetPower = carState.MotorCurrent * carState.MotorVoltage / 1000; // unit: kW
    if (ioExt.verboseModeDInHandler)
      console << "Set (-) constant mode " << CONSTANT_MODE_str[(int)(carState.ConstantMode)] << ", target Speed: " << carState.TargetSpeed
              << "km/h | " << carState.TargetPower << "W.(" << carState.ConstSpeedIncrease << "km/h|" << carState.ConstPowerIncrease
              << "W)\n";
    return;
  }
  if (carState.ConstantMode == CONSTANT_MODE::SPEED) {
    carState.TargetSpeed -= carState.ConstSpeedIncrease;
    if (carState.TargetSpeed < 0)
      carState.TargetSpeed = 0;
  } else { // CONSTANT_MODE::POWER
    carState.TargetPower -= carState.ConstPowerIncrease;
    if (carState.TargetPower < 0)
      carState.TargetPower = 0;
  }
  if (ioExt.verboseModeDInHandler)
    console << "MINUS, mode " << CONSTANT_MODE_str[(int)(carState.ConstantMode)] << ", target Speed: " << carState.TargetSpeed << "km/h | "
            << carState.TargetPower << "W (" << carState.ConstSpeedIncrease << "km/h|" << carState.ConstPowerIncrease << "W)\n";
}

void buttonPlusHandler() {
  if (!SystemInited)
    return;
  if (!carState.ConstantModeOn) {
    carState.ConstantModeOn = true;
    carState.TargetSpeed = carState.Speed;                                       // unit: km/h
    carState.TargetPower = carState.MotorCurrent * carState.MotorVoltage / 1000; // unit: kW
    carState.ConstantMode = CONSTANT_MODE::SPEED;
    if (ioExt.verboseModeDInHandler)
      console << "Set (+) constant mode " << CONSTANT_MODE_str[(int)(carState.ConstantMode)] << ", target Speed: " << carState.TargetSpeed
              << "km/h | " << carState.TargetPower << "W (" << carState.ConstSpeedIncrease << "km/h|" << carState.ConstPowerIncrease
              << "W)\n";
    return;
  }
  if (carState.ConstantMode == CONSTANT_MODE::SPEED) {
    carState.TargetSpeed += carState.ConstSpeedIncrease;
    if (carState.TargetSpeed > 111) // only until 111km/h
      carState.TargetSpeed = 111;
  } else { // CONSTANT_MODE::POWER
    carState.TargetPower += carState.ConstPowerIncrease;
    if (carState.TargetPower > 4500) // only until 5kW
      carState.TargetPower = 4500;
  }
  if (ioExt.verboseModeDInHandler)
    console << "PLUS,  mode " << CONSTANT_MODE_str[(int)(carState.ConstantMode)] << ", target Speed: " << carState.TargetSpeed << "km/h | "
            << carState.TargetPower << "W (" << carState.ConstSpeedIncrease << "km/h|" << carState.ConstPowerIncrease << "W)\n";
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
