//
// Command Receiver and Handler
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_CMDHANDLER_H
#define SOLAR_CAR_CONTROL_SYSTEM_CMDHANDLER_H

#include <AbstractTask.h>

class CmdHandler : public AbstractTask {
public:
  // RTOS task
  string getName(void) { return "CmdHandler"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void *pvParams);

  // Class functions and members
  string commands = "SVMUICOTK:!sc?";
  string helpText = "Available commands (" + commands +
                    "):\n"
                    "\t-------- SYSTEM COMMANDS -------------------------\n"
                    "\tS _ _ _ _ _ _ _ _ _ _ print status\n"
                    "\t  [a]                 - print status and all values\n"
                    "\tH _ _ _ _ _ _ _ _ _ _ memory_info\n"
                    "\tI _ _ _ _ _ _ _ _ _ _ read and show IOs\n"
                    "\t  [s]                 - scan and show I2C devices\n"
                    "\t  [i|o]               - show in|out continuously\n"
                    "\t  [h]                 - show in handler results\n"
                    "\t  [a|d]               - show ADC|DAC continuously\n"
                    "\t  [c]                 - show controls acc/dec continuously\n"
                    "\t  [R]                 - reinit\n"
                    "\tC _ _ _ _ _ _ _ _ _ _ set CAN verbose mode\n"
                    "\t  [i]                 - verbose CAN in\n"
                    "\t  [I]                 - verbose CAN in native packages\n"
                    "\t  [o]                 - verbose CAN out\n"
                    "\t  [O]                 - verbose CAN out native packages\n"
                    "\tO _ _ _ _ _ _ _ _ _ _ set CarControl verbose mode\n"
                    "\t  [o]                 - verbose\n"
                    "\t  [O]                 - verbose debug\n"
                    "\tT [yyyy mm dd hh MM]  get/set RTC date and time\n"
                    "\tK [|kp ki kd] _ _ _ _ show / update PID constants\n"
                    "\t-------- DRIVER INFO COMMANDS --------------------\n"
                    "\t:<text> _ _ _ _ _ _ _ display driver info text\n"
                    "\t!<text> _ _ _ _ _ _ _ display driver warn text\n"
                    "\ts [|+|-]  _ _ _ _ _ _ speed arrow off, green up (+), red down (-)\n"
                    "\t-------- Driver SUPPORT COMMANDS -----------------\n"
                    "\tc [-|s|p] _ _ _ _ _ _ constant mode on (-:off|s:speed|p:power)\n";

  string printSystemValues(void);
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_CMDHANDLER_H
