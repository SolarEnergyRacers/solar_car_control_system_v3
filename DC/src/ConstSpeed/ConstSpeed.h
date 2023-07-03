//
// Constant Car Speed PID Control
//
//  - For an ultra-detailed explanation of why the code is the way it is, please visit:
//    http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/
//
//  - For function documentation see:  http://playground.arduino.cc/Code/PIDLibrary

#ifndef SOLAR_CAR_CONTROL_SYSTEM_CONSTSPEED_H
#define SOLAR_CAR_CONTROL_SYSTEM_CONSTSPEED_H

#include <stdio.h>

#include <AbstractTask.h>
#include <CarState.h>
#include <PID_v1.h>

extern CarState carState;

class ConstSpeed : public AbstractTask {
public:
  // RTOS task
  string getName(void) { return "ConstSpeed"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void *pvParams);

  // Class functions and members
private:
  double input_value;
  double output_setpoint;
  double target_speed;          // in m/s
  double speed_increment = 1.0; // in m/s
  PID pid = PID(&input_value, &output_setpoint, &target_speed, carState.Kp, carState.Ki, carState.Kd, DIRECT);

public:
  void set_target_speed(double speed);
  double get_target_speed();
  double get_current_speed(); // speed in m/s
  void set_pid(double Kp, double Ki, double Kd);
  void update_pid();
  void target_speed_incr(void);
  void target_speed_decr(void);
  double GetKp() { return pid.GetKp(); }
  double GetKi() { return pid.GetKi(); }
  double GetKd() { return pid.GetKd(); }
  bool verboseModePID = false;
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_CONSTSPEED_H
