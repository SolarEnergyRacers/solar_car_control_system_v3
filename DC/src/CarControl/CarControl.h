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
  bool read_reference_cell_data();
  bool read_potentiometer();
  bool read_speed(); // in km/h

  int calculate_acceleration_display(int valueDec, int valueAcc);
  void set_DAC();
  
  bool read_paddles();
  bool read_PLUS_MINUS();
  bool verboseMode = false;
  bool verboseModeDebug = false;

private:
  int ads_min_dec = 4800;
  int ads_min_acc = 5200;
  int ads_max_dec = 26000;
  int ads_max_acc = 26000;
  int accelLast = INT_MAX;
  int recupLast = INT_MAX;
  int accelerationDisplayLast = INT_MAX;
  bool justInited = true;

  //void _set_dec_acc_values(int valueDecPot, int valueAccPot, int16_t valueDec, int16_t valueAcc, int valueDisplay);

  bool isInValueChangedHandler = false;
  void _handleValueChanged();
};
#endif // SER_CAR_CONTROL_H
