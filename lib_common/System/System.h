//
// System Utility and Helper Functions
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_SYSTEM_H
#define SOLAR_CAR_CONTROL_SYSTEM_SYSTEM_H

// standard libraries
#include <stdio.h>
#include <string>

class AbstractTask; 
// no need to mess with include hierarchy for 1 simple class reference

using namespace std;

void chip_info(void);
void restart(void);
string memory_info(void);
void report_task_stack(AbstractTask* task_ptr);

#endif // SOLAR_CAR_CONTROL_SYSTEM_SYSTEM_H
