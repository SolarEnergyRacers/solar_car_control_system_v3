/*
 * Car controll loops
 */

#ifndef SER_CAR_CONTROL_H
#define SER_CAR_CONTROL_H

#include <list>
#include <map>
#include <string>

// #include <MCP23017.h>

#include <AbstractTask.h>
#include <definitions.h>

const int MAX_ACCELERATION_DISPLAY_VALUE = 99; // absolute of minimal or maximal value for acceleration scaling

class CarControl : public AbstractTask {

public:
  // RTOS task
  string getName(void) { return "CarControl"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void *pvParams);

  // Class member and functions
  static void valueChangedHandler() { valueChangeRequest++; };

  bool read_potentiometer();
  bool read_speed(); // in km/h

  bool read_paddles();
  bool read_PLUS_MINUS();
  void reset_acceleration_values() { _set_dec_acc_values(0, 0, 0, 0, 0); }
  bool read_nextScreenButton();
  bool read_ConstantModeButton();
  bool verboseMode = false;
  bool verboseModeDebug = false;

private:
  int valueDisplayLast = INT_MAX;
  bool justInited = true;

  void _set_dec_acc_values(int valueDecPot, int valueAccPot, int16_t valueDec, int16_t valueAcc, int valueDisplay);

  static volatile int valueChangeRequest;
  bool isInValueChangedHandler = false;
  void _handleValueChanged();

  unsigned int _normalize_0_UINT16(int minOriginValue, int maxOriginValue, int value);
  int _transform(int minDisplayValue, int maxDisplayValue, int minValue, int maxValue, int value);
};
#endif // SER_CAR_CONTROL_H
