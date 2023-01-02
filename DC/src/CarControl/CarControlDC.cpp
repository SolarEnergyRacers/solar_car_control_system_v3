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
// #include <DriverDisplay.h>
// #include <EngineerDisplay.h>
#include <Helper.h>
#include <I2CBus.h>
// #include <IOExt.h>
// #include <Indicator.h>
// #include <MCP23017.h>
// #include <SDCard.h>

extern ADC adc;
extern CANBus canBus;
extern CarControl carControl;
// extern CarSpeed carSpeed;
extern CarState carState;
extern Console console;
extern DAC dac;
// extern DriverDisplay driverDisplay;
// extern EngineerDisplay engineerDisplay;
extern I2CBus i2cBus;
// extern Indicator indicator;
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
  int value = adc.switch_potentiometer;
  if (carState.Potentiometer != value) {
    carState.Potentiometer = value;
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

int CarControl::calculate_displayvalue_acc_dec(int valueDec, int valueAcc) {
  int16_t valueDecNorm = 0;
  int16_t valueAccNorm = 0;
  int valueDisplay = 0;
  valueDecNorm = CarControl::_normalize(0, MAX_ACCELERATION_DISPLAY_VALUE, ads_min_dec, ads_max_dec, valueDec);
  if (valueDecNorm > 0) {
    valueDisplay = -valueDecNorm;
  } else {
    valueAccNorm = CarControl::_normalize(0, MAX_ACCELERATION_DISPLAY_VALUE, ads_min_acc, ads_max_acc, valueAcc);
    if (valueAccNorm > 0) {
      valueDisplay = valueAccNorm;
    } else {
      valueDisplay = 0;
    }
  }
  return valueDisplay;
}

bool CarControl::read_PLUS_MINUS() {
  bool hasChanged = false;
  if (carState.BreakPedal) {
    _set_dec_acc_values(DAC_MAX, 0, ADC_MAX, 0, -64);
    return true;
  }

  bool plusButton = false;  // ioExt.getPort(carState.getPin(PinIncrease)->port) == 0;  // currently pressed?
  bool minusButton = false; // ioExt.getPort(carState.getPin(PinDecrease)->port) == 0; // currently pressed?
  if (!plusButton && !minusButton)
    return false;

  int8_t acceleration; // -99 ... +99
  if (carState.Deceleration > 0) {
    acceleration = -carState.Deceleration;
  } else {
    acceleration = carState.Acceleration;
  }

  if (minusButton) {
    if (acceleration - carState.ButtonControlModeIncrease > -99)
      acceleration -= carState.ButtonControlModeIncrease;
    else
      acceleration = -99;
  } else if (plusButton) {
    if (acceleration + carState.ButtonControlModeIncrease < 99)
      acceleration += carState.ButtonControlModeIncrease;
    else
      acceleration = 99;
  }

  int8_t valueDec = 0;
  int8_t valueAcc = 0;
  if (acceleration < 0) {
    valueDec = -acceleration;
  } else if (acceleration > 0) {
    valueAcc = acceleration;
  }

  int valueDisplay = valueDec > 0 ? -valueDec : valueAcc;
  // prepare and write motor acceleration and recuperation values to DigiPot
  int valueDecPot = 0;
  int valueAccPot = 0;
  if (valueDisplay < 0) {
    valueDecPot = -(int)(((float)valueDisplay / MAX_ACCELERATION_DISPLAY_VALUE) * DAC_MAX);
    valueAccPot = 0;
  } else {
    valueDecPot = 0;
    valueAccPot = (int)(((float)valueDisplay / MAX_ACCELERATION_DISPLAY_VALUE) * DAC_MAX);
  }

  if (valueDisplayLast != valueDisplay) {
    hasChanged = true;
    _set_dec_acc_values(valueDecPot, valueAccPot, valueDec, valueAcc, valueDisplay);
  }
  if (carControl.verboseMode) {
    console << fmt::format(
        "button mode: acc={:5d} --> valueDecPot={:5d}, valueAccPot={:5d} | valueDec={:5d}, valueAcc={:5d}, valueDisplay={:5d}\n",
        acceleration, valueDecPot, valueAccPot, valueDec, valueAcc, valueDisplay);
  }
  return hasChanged;
}

bool CarControl::read_paddles() {
  bool hasChanged = false;
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
  int16_t valueDec = adc.stw_dec;
  int16_t valueAcc = adc.stw_acc;
  carState.Deceleration = adc.stw_dec;
  carState.Acceleration = adc.stw_acc;
  // check if change is in damping
  // if (valueAcc != 0 && valueDec != 0)
  //   if (abs(accelLast - valueAcc) < carState.PaddleDamping && abs(recupLast - valueDec) < carState.PaddleDamping)
  //     return false;

  // #SAFETY#: Reset constant mode on deceleration paddel touched
  if (valueDec > carState.PaddleDamping) {
    carState.ConstantModeOn = false;
  }

  int valueDisplay = calculate_displayvalue_acc_dec(valueDec, valueAcc);
  // prepare and write motor acceleration and recuperation values to DigiPot
  int valueDecPot = 0;
  int valueAccPot = 0;
  if (valueDisplay < 0) {
    valueDecPot = -(int)(((float)valueDisplay / MAX_ACCELERATION_DISPLAY_VALUE) * DAC_MAX);
    valueAccPot = 0;
  } else {
    valueDecPot = 0;
    valueAccPot = (int)(((float)valueDisplay / MAX_ACCELERATION_DISPLAY_VALUE) * DAC_MAX);
  }

  if (valueDisplayLast != valueDisplay) {
    hasChanged = true;
  }
  _set_dec_acc_values(valueDecPot, valueAccPot, valueDec, valueAcc, valueDisplay);
  if (carControl.verboseMode && hasChanged) {
    console << fmt::format(
        "paddle mode            : valueDecPot={:5d}, valueAccPot={:5d} | valueDec={:5d}, valueAcc={:5d}, valueDisplay={:5d}\n", valueDecPot,
        valueAccPot, valueDec, valueAcc, valueDisplay);
  }
  return hasChanged;
}

void CarControl::_set_dec_acc_values(int valueDecPot, int valueAccPot, int16_t valueDec, int16_t valueAcc, int valueDisplay) {
  // // if (carState.ConstantModeOn && valueDec == 0 && valueAcc == 0)
  // //   return;
  // dac.set_pot(valueDecPot, DAC::pot_chan::POT_CHAN1);
  // dac.set_pot(valueAccPot, DAC::pot_chan::POT_CHAN0);
  // carState.Deceleration = valueDec;
  // carState.Acceleration = valueAcc;
  carState.AccelerationDisplay = valueDisplay;
  valueDisplayLast = valueDisplay;
}

int CarControl::_normalize(int minDisplayValue, int maxDisplayValue, int minValue, int maxValue, int value) {
  float displArea = maxDisplayValue - minDisplayValue;
  float valueArea = maxValue - minValue;
  if (value < minValue)
    value = minValue;
  if (value > maxValue)
    value = maxValue;
  float retValue = (value - minValue) * displArea / valueArea;

  return round(retValue);
}

volatile int CarControl::valueChangeRequest = 0;
void CarControl::task(void *pvParams) {
  // polling loop
  while (1) {
    if (SystemInited) {
      // read values from ADC/IO
      // update OUTPUT pins
      // ioExt.writeAllPins(PinHandleMode::FORCED);

      // handle changed INPUT pins
      bool someThingChanged = false;
      someThingChanged |= read_paddles();
      someThingChanged |= read_PLUS_MINUS();
      // someThingChanged |= read_reference_cell_data();
      someThingChanged |= read_speed();
      someThingChanged |= read_potentiometer();
      carState.LifeSign++;
      canBus.writePacket(DC_BASE_ADDR | 0x00, carState.LifeSign, carState.Potentiometer, carState.Acceleration, carState.Deceleration);
      //canBus.writePacket(DC_BASE_ADDR | 0x02, carState.Speed, 0x1002, 0x1003, 0x10024);
      // canBus.writePacket(DC_BASE_ADDR | 0x00, 0x1234, 0x5678, 0x9abc, 0xdef0);
      // canBus.writePacket(DC_BASE_ADDR | 0x01, 0x6677, 0x4455, 0x2233, 0xaa11);
      // canBus.writePacket(DC_BASE_ADDR | 0x01, counter);
      // if (carControl.verboseModeCarControlMax)
      //   console << fmt::format(
      //                  "{} [{:02d}|{:02d}] CAN.PacketId=0x{:03x}-S-data: lifeSign={:4x}, speed={:5d}, decl={:5d}, accl={:5d}, poti={:5d},
      //                  ", counter, canBus.availiblePackets(), canBus.getMaxPacketsBufferUsage(), DC_BASE_ADDR | 0x00, carState.LifeSign,
      //                  carState.Speed, carState.Deceleration, carState.Acceleration, carState.Potentiometer)
      //           << NL;
      // counter++;
      // canBus.writePacket(DC_BASE_ADDR | 0x01, carState.Speed, carState.AccelerationDisplay, carState.Deceleration,
      // carState.Potentiometer); if (carControl.verboseModeCan)
      //   console << fmt::format("[{:02d}|{:02d}] CAN.PacketId=0x{:03x}-S-data:dummy={:5d}, speed={:5d}, decl={:5d}, accl={:5d}",
      //                          canBus.availiblePackets(), canBus.getMaxPacketsBufferUsage(), DC_BASE_ADDR | 0x01, carState.Speed,
      //                          carState.Deceleration, carState.Acceleration, carState.Potentiometer)
      //           << NL;
      // one data row per second
      // if ((millis() > millisNextStampCsv) || (millis() > millisNextStampSnd)) {
      //   // if (sdCard.isReadyForLog() && millis() > millisNextStampCsv) {
      //   //   sdCard.write(record);
      //   //   millisNextStampCsv = millis() + carState.LogInterval;
      //   // }
      //   if (carControl.verboseModeCarControlMax) {
      //     string record = carState.csv();
      //     if (millis() > millisNextStampSnd) {
      //       console << "d: " + record << NL;
      //       millisNextStampSnd = millis() + carState.CarDataSendPeriod;
      //     }
      //   }
      // }
    }
    taskSuspend();
  }
}
