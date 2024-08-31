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
  string commands = "REDPSVJMUFBICOTKi:!sc?";
  string helpText = "Available commands (" + commands +
                    "):\n"
                    "\t-------- SYSTEM COMMANDS -------------------------\n"
                    "\tR _ _ _ _ _ _ _ _ _ _ reset and reinit driver display\n"
                    "\tE _ _ _ _ _ _ _ _ _ _ switch to engineer screen\n"
                    "\tD _ _ _ _ _ _ _ _ _ _ switch to driver display\n"
                    "\tP _ _ _ _ _ _ _ _ _ _ print directory of sdcard\n"
                    "\tS _ _ _ _ _ _ _ _ _ _ print status\n"
                    "\t  [a]                   - print status and all values\n"
                    "\tV [|+]  _ _ _ _ _ _ _ write_log to sdcard | with header (+)\n"
                    "\tM _ _ _ _ _ _ _ _ _ _ mount sdcard and eneable logging\n"
                    "\tU _ _ _ _ _ _ _ _ _ _ unmount sdcard and disable logging\n"
                    "\tH _ _ _ _ _ _ _ _ _ _ memory_info\n"
                    "\tB [v|m|<rate>]  _ _ _ _ show or set baud rate of Serial2, v-verbose mode\n"
                    "\tF _ _ _ _ _ _ _ _ _ _ Re-read configuration\n"
                    "\tI _ _ _ _ _ _ _ _ _ _ read and show IOs\n"
                    "\t  [s]                   - scan and show I2C devices\n"
                    // "\t  [i|o]                 - show in|out continuously\n"
                    // "\t  [a|d]                 - show ADC|DAC continuously\n"
                    // "\t  [c]                   - show controls acc/dec continuously\n"
                    "\t  [R]                   - reinit\n"
                    "\tC _ _ _ _ _ _ _ _ _ _ set CAN verbose mode\n"
                    "\t  [i]                   - verbose CAN in\n"
                    "\t  [I]                   - verbose CAN in native packages\n"
                    "\t  [o]                   - verbose CAN out\n"
                    "\t  [O]                   - verbose CAN out native packages\n"
                    "\t  [S]                   - verbose sd card\n"
                    "\t  motC batV             - inject motor current [A] and battery voltage [V]\n"
                    "\tO _ _ _ _ _ _ _ _ _ _ set CarControl verbose mode\n"
                    "\t  [o]                   - verbose\n"
                    "\t  [O]                   - verbose debug\n"
                    "\tT _ _ _ _ _ _ _ _ _ _ get system time\n"
                    "\t  [yyyy mm dd hh MM ss] - set RTC date and time\n"
                    "\tK [|kp ki kd] _ _ _ _ show / update PID constants\n"
                    "\ti _ _ _ _ _ _ _ _ _ _ minimal drive to console\n"
                    "\t-------- DRIVER INFO COMMANDS --------------------\n"
                    "\t:<text> _ _ _ _ _ _ _ display driver info text\n"
                    "\t!<text> _ _ _ _ _ _ _ display driver warn text\n"
                    "\ts [|+|-]  _ _ _ _ _ _ speed arrow off, green up (+), red down (-)\n"
                    "\t-------- Driver SUPPORT COMMANDS -----------------\n"
                    "\tc [-|+|s|p] _ _ _ _ _ _ constant mode on (-:off|+:on|s:speed|p:power)\n";

  string printSystemValues(void);
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_CMDHANDLER_H
