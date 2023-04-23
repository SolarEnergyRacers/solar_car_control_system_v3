//
// Car Speed PID Control
//

#include <definitions.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <inttypes.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

#include <CarControl.h>
#include <CarState.h>
#include <ConstSpeed.h>

#include <Console.h>
#include <DAC.h>
#include <Helper.h>
#include <PID_v1.h>

extern Console console;
extern PID pid;
extern ConstSpeed constSpeed;
extern CarState carState;
extern CarControl carControl;
extern bool SystemInited;
extern DAC dac;

// ------------------
// FreeRTOS functions

string ConstSpeed::re_init() {
  //pid = NULL;
  // pid.SetMode(MANUAL);
  // pid.SetMode(AUTOMATIC);
  return init();
}

string ConstSpeed::init() {
  bool hasError = false;
  console << "[  ] Init 'ConstSpeed'...\n";
  target_speed = 0;
  pid = PID(&input_value, &output_setpoint, &target_speed, carState.Kp, carState.Ki, carState.Kd, DIRECT);
  pid.SetMode(AUTOMATIC);
  pid.SetOutputLimits(-DAC_MAX, DAC_MAX);
  return fmt::format("[{}] ConstSpeed initialized.", hasError ? "--" : "ok");
}

void ConstSpeed::exit(void) { set_target_speed(0); }
// ------------------

void ConstSpeed::set_target_speed(double speed) { target_speed = speed; }

void ConstSpeed::target_speed_incr(void) { target_speed += speed_increment; }

void ConstSpeed::target_speed_decr(void) { target_speed -= speed_increment; }

double ConstSpeed::get_target_speed() { return target_speed; }

double ConstSpeed::get_current_speed() { return carState.Speed; }

void ConstSpeed::update_pid(double Kp, double Ki, double Kd) {
  carState.Kp = Kp;
  carState.Ki = Ki;
  carState.Kd = Kd;
  pid.SetTunings(carState.Kp, carState.Ki, carState.Kd);
}

void ConstSpeed::task(void *pvParams) {

  /*
   * How is accelration / decelartion handled in hardware?
   *
   * - acceleration: Digital to analog converter value representing the target speed -> 0V: stop, 5V -> max speed
   * - a separate I/O is used for forward/reverse switching
   * - deceleration/recuperation: Digital to analog converter value representing the recuperation amount: -> 0V: no rec, 5V: max recup
   * -> Note: In case of recup > 0, we should have acceleration 0
   *
   * TODO: ini file: recuperate on constant speed mode , or just let it roll (i.e. let it roll if the speed is too high is less convenient
   * for the driver, however, it conserves energy since we do not over-regulate) For the moment, we recuperate
   */

  while (1) {
    if (SystemInited && carState.ConstantModeOn && carState.ConstantMode == CONSTANT_MODE::SPEED) {
      // read target speed
      input_value = carState.Speed;
      target_speed = carState.TargetSpeed;

      if (verboseModePID) {
        console << "#- input_value=" << input_value << ", target_speed=" << target_speed << " =>";
      }
      // update pid controller
      bool hasNewValue = pid.Compute();
      if (!hasNewValue) {
        if (verboseModePID) {
          console << " cst=0" << carState.AccelerationDisplay << "(" << output_setpoint << ")\n";
        }
        return;
      }

      // set acceleration & deceleration
      uint8_t acc = 0;
      uint8_t dec = 0;
      if (output_setpoint > 0) {
        acc = round(output_setpoint);
        if (verboseModePID)
          console << "acc=" << acc;
      } else if (output_setpoint < 0) {
        dec = round(-output_setpoint);
        if (verboseModePID)
          console << "dec=" << dec;
      }
      carState.AccelerationDisplay = round((acc > 0 ? acc : -dec) * MAX_ACCELERATION_DISPLAY_VALUE / DAC_MAX);
      carControl.set_DAC();

      if (verboseModePID) {
        console << ", disp: " << carState.AccelerationDisplay << " (" << output_setpoint << ")\n";
      }
    }
    taskSuspend();
  }
}
