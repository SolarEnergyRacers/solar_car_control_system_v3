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

class CarControl : public AbstractTask {

public:
  // RTOS task
  string getName(void) { return "CarControlAC"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void *pvParams);

  // Class member and functions
  bool read_nextScreenButton();
  bool update_sd_card_detect();
  bool read_sd_card_detect();
  bool read_const_mode_and_mountrequest();
  bool verboseModeCarControl = false;
  bool verboseModeCarControlDebug = false;

private:
  int valueDisplayLast = INT_MAX;
  bool justInited = true;

  bool isInValueChangedHandler = false;
  void _handleValueChanged();
};
#endif // SER_CAR_CONTROL_H
