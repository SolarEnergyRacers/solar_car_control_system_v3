/*
 * MCP23017 I/O Extension over I2C  !!! UNTESTED !!!
 */

#ifndef SER_IOEXT_H
#define SER_IOEXT_H

#include <definitions.h>

#include <MCP23017.h>
#include <list>
#include <map>
#include <string>

#include <AbstractTask.h>

#include <CarState.h>
#include <CarStatePin.h>

// IOExtDev
#define PinDI_MCONOFF "DI_MCONOFF-GPA7"
#define PinDI_FWD_BWD "DI_FWD_BWD-GPA6"
#define PinDO_BreakLight "DO_BreakLight-GPA5"
#define PinDI_Break "DI_Break-GPA4"
#define PinDI_Button_Set "DI_Button_Set-GPA3"
#define PinDI_Button_Minus "DI_Button_Minus-GPA2"
#define PinDI_Button_Reset "DI_Button_Reset-GPA1"
#define PinDI_Button_Plus "DI_Button_Plus-GPA0"
#define PinGPB7 "GPB7"
#define PinGPB6 "GPB6"
#define PinGPB5 "GPB5"
#define PinGPB4 "GPB4"
#define PinGPB3 "GPB3"
#define PinGPB2 "GPB2"
#define PinGPB1 "GPB1"
#define PinGPB0 "GPB0"

enum class PinHandleMode { NORMAL, FORCED };

class IOExt : public AbstractTask {
public:
  // RTOS task
  string getName(void) { return "IOExt"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void *pvParams);

  // Class member and functions
  int getPort(int port);
  void setPort(int port, bool value);
  void writeAllPins(PinHandleMode mode = PinHandleMode::NORMAL);
  void readAllPins();
  bool readAndHandlePins(PinHandleMode mode = PinHandleMode::NORMAL);

  static int getIdx(int devNr, int pin) { return devNr * 16 + pin; };
  static int getIdx(int port) { return (port >> 4) * 16 + (port & 0x0F); };

  bool verboseModeDIn = false;
  bool verboseModeDInHandler = false;
  bool verboseModeDOut = false;

private:
  // bool isInInterruptHandler = false;
  bool isInInputHandler = false;

  void setPortMode(int port, uint8_t mode);
  void handleIoInterrupt();

  MCP23017 IOExtDevs[MCP23017_NUM_DEVICES] = {MCP23017(I2C_ADDRESS_MCP23017_IOExt0)};
};
#endif // SER_IOEXT_H
