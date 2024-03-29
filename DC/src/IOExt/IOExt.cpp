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

// ------------------
// FreeRTOS functions

volatile bool ioInterruptRequest;
void IRAM_ATTR ioExt_interrupt_handler() { ioInterruptRequest = true; };

string IOExt::re_init() { return init(); }

string IOExt::init() {
  bool hasError = false;
  console << "[  ] Init IOExt devices...\n";
  for (int devNr = 0; devNr < MCP23017_NUM_DEVICES; devNr++) {
    console << "     DeviceNr [" << devNr << "]:\n";
    xSemaphoreTakeT(i2cBus.mutex);
    ioExt.IOExtDevs[devNr].init();
    ioExt.IOExtDevs[devNr].writeRegister(MCP23017Register::GPIO_A, 0x00);
    ioExt.IOExtDevs[devNr].writeRegister(MCP23017Register::GPIO_B, 0x00);
    ioExt.IOExtDevs[devNr].writeRegister(MCP23017Register::IPOL_A, 0x00);
    ioExt.IOExtDevs[devNr].writeRegister(MCP23017Register::IPOL_B, 0x00);
    ioExt.IOExtDevs[devNr].interruptMode(MCP23017InterruptMode::Or);
    ioExt.IOExtDevs[devNr].interrupt(MCP23017Port::A, CHANGE); // FALLING);
    ioExt.IOExtDevs[devNr].interrupt(MCP23017Port::B, CHANGE); // FALLING);
    ioExt.IOExtDevs[devNr].clearInterrupts();
    xSemaphoreGive(i2cBus.mutex);
    for (int pinNr = 0; pinNr < MCP23017_NUM_PORTS; pinNr++) {
      CarStatePin *pin = carState.getPin(devNr, pinNr);
      carState.idxOfPin.insert(pair<string, int>{pin->name, getIdx(devNr, pinNr)});
      setPortMode(pin->port, pin->mode);
    }
    console << "     ok " << getName() << "[" << devNr << "]\n";
  }
  // inital read the io pins
  readAllPins();
  // setup inerrupt handling
  ioInterruptRequest = false;
  isInInputHandler = false;
  pinMode(I2C_INTERRUPT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(I2C_INTERRUPT), ioExt_interrupt_handler, CHANGE);
  return fmt::format("[{}] IOExt initialized.", hasError ? "--" : "ok");
}

void IOExt::exit(void) {
  // TODO
}
// ------------------

// void IOExt::handleIoInterrupt() {
//   if (isInInterruptHandler) {
//     // console << fmt::format("Jump out of interrupt handler at {}\n", millis());
//     return;
//   }
//   isInInterruptHandler = true;
//   // console << fmt::format("Do interrupt handler at {}}\n", millis());
//   readAllPins();
//   //readAll(true, false, "", false);
//   // console << fmt::format("Clear interrupts   {}\n", millis());
//   xSemaphoreTakeT(i2cBus.mutex);
//   for (int devNr = 0; devNr < MCP23017_NUM_DEVICES; devNr++) {
//     uint8_t portA, portB;
//     // uint8_t allPins = ioExt.IOExtDevs[devNr].read();
//     ioExt.IOExtDevs[devNr].clearInterrupts(portA, portB);
//     // console << fmt::format("Jump in interrupt handler at {:ld}::\tdevNr: {} -- pA {:02x}, pB {:02x}\n", millis(), devNr, portA,
//     portB); ioExt.IOExtDevs[devNr].clearInterrupts();
//   }
//   xSemaphoreGive(i2cBus.mutex);
//   isInInterruptHandler = false;
// }

void IOExt::setPortMode(int port, uint8_t mode) {
  int devNr = port >> 4;
  int pinNr = port & 0xf;
  xSemaphoreTakeT(i2cBus.mutex);
  ioExt.IOExtDevs[devNr].pinMode(pinNr, mode);
  xSemaphoreGive(i2cBus.mutex);
}

void IOExt::setPort(int port, bool value) {
  int devNr = port >> 4;
  int pinNr = port & 0xf;
  xSemaphoreTakeT(i2cBus.mutex);
  ioExt.IOExtDevs[devNr].digitalWrite(pinNr, value ? 1 : 0);
  xSemaphoreGive(i2cBus.mutex);
  if (verboseModeDOut) {
    CarStatePin *pin = carState.getPin(devNr, pinNr);
    console << fmt::format("Set BOOL -- 0x{:02x}, [{:x}|{:x}]: {} --> {}, inited: {:5}  <-- {:18s} \t({}ms)\n", pin->port, devNr, pinNr,
                           pin->oldValue, pin->value, pin->inited, pin->name, millis());
  }
}

void IOExt::writeAllPins(PinHandleMode mode) {
  for (auto &pin : carState.pins) {
    if (pin.mode == OUTPUT && (pin.oldValue != pin.value || !pin.inited || mode == PinHandleMode::FORCED)) {
      setPort(pin.port, pin.value);
      pin.oldValue = pin.value;
      pin.inited = true;
    }
  }
}

void IOExt::readAllPins() {

  xSemaphoreTakeT(i2cBus.mutex);
  int16_t io0A = IOExtDevs[0].readPort(MCP23017Port::A);
  int16_t io0B = IOExtDevs[0].readPort(MCP23017Port::B);
  xSemaphoreGive(i2cBus.mutex);

  for (CarStatePin &pin : carState.pins) {
    if (pin.mode != OUTPUT) {
      if (pin.port < 0x08) {
        pin.value = (io0A & pin.mask) > 0 ? 1 : 0;
      } else if (pin.port < 0x10) {
        pin.value = (io0B & pin.mask) > 0 ? 1 : 0;
      } else {
        SystemInited = false;
        console << "ERROR: Wrong ioExt configuration! System halted." << NL;
      }
    }
  }
  if (verboseModeDIn) {
    console << fmt::format("IOExt ({}ms)\tA:{:02x}, B:{:02x}", millis(), io0A, io0B)
            << ", Car state: " << carState.printIOs("", true, false) << "\n";
  }
}

bool IOExt::readAndHandlePins(PinHandleMode mode) {
  if (isInInputHandler)
    return false;
  isInInputHandler = true;
  bool hasChanges = false;
  readAllPins();
  list<void (*)()> pinHandlerList;
  for (CarStatePin &pin : carState.pins) {
    if (pin.mode != OUTPUT && (pin.handlerFunction != NULL)) {
      unsigned long timestamp = millis();
      // button: debounced time and value == 0 --> activate Handler
      // toggles: debounced time == 0, value 0|1 --> activate handler
      if ((pin.value == 0 || (pin.debounceTime_ms == 0 && pin.oldValue != pin.value)) &&
          (timestamp > pin.timestamp + pin.debounceTime_ms) &&
          (pin.continouseMode || pin.oldValue != pin.value || !pin.inited || mode == PinHandleMode::FORCED)) {
        if (ioExt.verboseModeDIn)
          console << fmt::format("Get BOOL -- 0x{:02x}: {} --> {}, inited: {}  <-- {:18s} \t({})\t -> handle ({}ms)\n", pin.port,
                                 pin.oldValue, pin.value, pin.inited, pin.name, millis(), timestamp - pin.timestamp);
        pinHandlerList.push_back(pin.handlerFunction);
        pin.inited = true;
        if (pin.oldValue != pin.value)
          hasChanges = true;
        pin.oldValue = pin.value;
        pin.timestamp = timestamp;
      }
      if (pin.value == 1)
        pin.oldValue = 1;
    }
  }
  // avoid multi registration:
  pinHandlerList.unique();
  // call all handlers for changed pins
  if (SystemInited) {
    // xSemaphoreTakeT(i2cBus.mutex);
    for (void (*pinHandler)() : pinHandlerList) {
      pinHandler();
    }
    // xSemaphoreGive(i2cBus.mutex);
  }
  pinHandlerList.clear();
  isInInputHandler = false;
  return hasChanges;
}

int IOExt::getPort(int port) {
  // get device & port
  int devNr = port >> 4;
  int pin = port & 0xf;
  xSemaphoreTakeT(i2cBus.mutex);
  int value = ioExt.IOExtDevs[devNr].digitalRead(pin);
  xSemaphoreGive(i2cBus.mutex);
  // console << fmt::format("port 0x{:02x} [{}|{}]: {} -- {}ms\n", port, devNr, pin, value, millis());
  return value;
}

void IOExt::task(void *pvParams) {
  while (1) {
    if (SystemInited) {
      readAndHandlePins(); // PinHandleMode::FORCED);
    }
    taskSuspend();
  }
}
