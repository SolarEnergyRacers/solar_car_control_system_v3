//
// CarControlx: Main control module of SER4
//
#include "../definitions.h"

#include <fmt/core.h>
#include <fmt/printf.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <CAN.h>
#include <Wire.h> // I2C

#include <ADC_SER.h>
#include <CANBus.h>
#include <CarControl.h>
// #include <CarSpeed.h>
#include <CarState.h>
#include <ConstSpeed.h>
// #include <ConfigFile.h>
#include <Console.h>
#include <DAC.h>
#include <Helper.h>
#include <I2CBus.h>
#include <IOExt.h>
// #include <MCP23017.h>

extern ADC adc;
extern CANBus canBus;
extern CarControl carControl;
extern ConstSpeed constSpeed;
extern CarState carState;
extern Console console;
extern DAC dac;
extern I2CBus i2cBus;
extern IOExt ioExt;

extern bool SystemInited;

using namespace std;

unsigned long millisNextCanSend = millis();
unsigned long millisNextLifeSignIncrement = millis();
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

#define MAX_POTENTIOMETER_VALUE 22000
bool CarControl::read_potentiometer() {
  int value = adc.switch_potentiometer;
  if (carState.Potentiometer != value) {
    carState.Potentiometer = value;
    carState.ConstSpeedIncrease = transformArea(1, 21, 0, MAX_POTENTIOMETER_VALUE, value);
    carState.ConstPowerIncrease = transformArea(1, 501, 0, MAX_POTENTIOMETER_VALUE, value);
    if (verboseModeDebug)
      console << carState.ConstSpeedIncrease << "km/h|" << carState.ConstPowerIncrease << "W\n";

    return true;
  }
  return false;
}

bool CarControl::read_speed() {
  float diameter = 0.55;                                  // wheel diameter [m]
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
  } else {
    return transformArea(0, MAX_ACCELERATION_DISPLAY_VALUE, 0, INT16_MAX, valueAcc);
  }
}

bool CarControl::read_paddles() {
  if (carState.BreakPedal) {
    // #SAFETY#: on break pedal -> deccelerate
    carState.ConstantModeOn = false;
    carState.Acceleration = 0;
    uint16_t dec_procent = ads_max_dec * 0.64;
    if (carState.Deceleration < dec_procent) {
      carState.Deceleration = dec_procent;
    }
    carState.AccelerationDisplay = -64; //calculate_acceleration_display(carState.Deceleration, carState.Acceleration);
    if (carControl.verboseMode) {
      console << fmt::format("paddle with BREAK PEDAL: Decl={:6d}, {:6d} | Accl={:6d}, {:6d} | => [{:4d}]\n", carState.Deceleration, adc.stw_dec, carState.Acceleration, adc.stw_acc, carState.AccelerationDisplay);
    }
  } else {
    carState.Deceleration = normalize_0_UINT16(ads_min_dec, ads_max_dec, adc.stw_dec);
    carState.Acceleration = normalize_0_UINT16(ads_min_acc, ads_max_acc, adc.stw_acc);
    
    // #SAFETY#: Reset constant mode on deceleration paddel touched
    if (carState.Deceleration > 0) {
      carState.ConstantModeOn = false;
      carState.Acceleration = 0;
      carState.AccelerationDisplay = calculate_acceleration_display(carState.Deceleration, carState.Acceleration);
    } else {
      if (carState.ConstantModeOn) {
        int8_t temp = calculate_acceleration_display(carState.Deceleration, carState.Acceleration);
        if (carState.AccelerationDisplay < temp && carState.Acceleration > carState.PaddleDamping) {
          carState.AccelerationDisplay = temp;
        }
      } else {
        carState.AccelerationDisplay = calculate_acceleration_display(carState.Deceleration, carState.Acceleration);
      }
    }
  }

  if (accelerationDisplayLast != carState.AccelerationDisplay) {
    accelerationDisplayLast = carState.AccelerationDisplay;
    if (carControl.verboseMode) {
      console << fmt::format("paddle w/o  BREAK PEDAL: Decl={:6d}, {:6d} | Accl={:6d}, {:6d} | => [{:4d}]\n", carState.Deceleration, adc.stw_dec, carState.Acceleration, adc.stw_acc, carState.AccelerationDisplay);
    }
    return true;
  }
  return false;
}

// prepare and write motor acceleration and recuperation values to DigiPot
void CarControl::set_DAC() {
  int setpoint = (int)(((float)carState.AccelerationDisplay / MAX_ACCELERATION_DISPLAY_VALUE) * DAC_MAX);
  int valueDAC_dec = carState.AccelerationDisplay < 0 ? -setpoint : 0;
  int valueDAC_acc = carState.AccelerationDisplay < 0 ? 0 : setpoint;
  dac.set_pot(valueDAC_dec, DAC::pot_chan::POT_CHAN1_DEC);
  dac.set_pot(valueDAC_acc, DAC::pot_chan::POT_CHAN0_ACC);

  if (carControl.verboseMode) {
    console << fmt::format("set DAC:: valueDAC_dec={:6d}, valueDAC_acc={:6d} | valueDec={:6d}, valueAcc={:6d} [valueDisplay={:4d}] > Speed={:3d} ({:6d})\n", 
                            valueDAC_dec, valueDAC_acc, carState.Deceleration, carState.Acceleration, carState.AccelerationDisplay, carState.Speed, adc.motor_speed);
  }
}

void CarControl::switch_break_light() {
  // -10 means to switch on brak light at approx. 4km/h / s (1.1m/s^2)
  carState.getPin(PinDO_BreakLight)->value = carState.AccelerationDisplay < -10 ? 1 : 0;
}

void CarControl::task(void *pvParams) {
  while (1) {
    if (SystemInited) {
      // bool force = false;
      bool force = true;
      unsigned long cur_millis = millis();
      if (cur_millis > millisNextCanSend) {
        millisNextCanSend = cur_millis + 1000;
        force = true;
      }
      if (cur_millis > millisNextLifeSignIncrement) {
        millisNextLifeSignIncrement = cur_millis + 1000;
        carState.LifeSign++;
        force = true;
      }

      // read_reference_cell_data();
      // vTaskDelay(10);
      read_speed();
      vTaskDelay(10);
      read_potentiometer();
      vTaskDelay(10);
      if (read_paddles())
        set_DAC();
      switch_break_light();
      // update OUTPUT pins
      ioExt.writeAllPins(PinHandleMode::FORCED);
      constSpeed.update_pid();

      for(int i=0; i<20; ++i)
      canBus.writePacket(DC_BASE_ADDR | 0x00,
                         carState.LifeSign,      // LifeSign
                         carState.Potentiometer, // Potentiometer value
                         carState.Acceleration,  // HAL-paddle Acceleration ADC value
                         carState.Deceleration,  // HAL-paddle Deceleration ADC value
                         force                   // force or not
      );

      bool driveDirection = carState.DriveDirection == DRIVE_DIRECTION::FORWARD ? 1 : 0;
      for(int i=0; i<20; ++i)
      canBus.writePacket(DC_BASE_ADDR | 0x01,
                         (uint16_t)carState.TargetSpeed,          // Target Speed [float as value\*1000]
                         (uint16_t)(carState.TargetPower * 1000), // Target Power [float as value\*1000]
                         carState.AccelerationDisplay,            // Display Acceleration
                         0,                                       // empty
                         carState.Speed,                          // Display Speed
                         driveDirection,                          // Fwd [1] / Bwd [0]
                         carState.BreakPedal,                     // Button Lvl Brake Pedal
                         carState.MotorOn,                        // MC Off [0] / On [1]
                         carState.ConstantModeOn,                 // Constant Mode Off [false], On [true]
                         carState.ConfirmDriverInfo,              // Confirm driver had read info [true]
                         false,                                   // empty
                         false,                                   // empty
                         false,                                   // empty
                         force                                    // force or not
      );

      // vTaskDelay(10);

      if (carControl.verboseModeDebug) {
        console << fmt::format("[I:{:02d}|{:02d},O:{:02d}|{:02d}] P.Id=0x{:03x}-S-data:lifesign={:5d}, poti={:5d}, decl={:5d}, accl={:5d}",
                               canBus.availiblePacketsIn(), canBus.getMaxPacketsBufferInUsage(), canBus.availiblePacketsOut(),
                               canBus.getMaxPacketsBufferOutUsage(), DC_BASE_ADDR | 0x00, carState.LifeSign, carState.Potentiometer,
                               carState.Acceleration, carState.Deceleration)
                << NL;
        console << fmt::format("        P.Id=0x{:03x}-S-data:tgtSpeed={:5d}, Powr={:5d}, accD={:5d}, speed={:3d}, "
                               "direct={:1d}, break={}, MotorOn={}, ConstandModeOn={}, ConfirmDriverInfo={}",
                               DC_BASE_ADDR | 0x01, (uint16_t)(carState.TargetSpeed * 1000), (uint16_t)(carState.TargetPower * 1000),
                               carState.AccelerationDisplay, carState.Speed, driveDirection, carState.BreakPedal,
                               carState.MotorOn, carState.ConstantModeOn, carState.ConfirmDriverInfo)
                << NL;
      }
    }
    taskSuspend();
  }
}
