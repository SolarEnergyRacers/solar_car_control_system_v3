//
// This is the abstract task class, which has to be implemented by all other tasks
//

// standard libraries
#include <fmt/core.h>
#include <fmt/format.h>
#include <inttypes.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <AbstractTask.h>
#include <Console.h>

using namespace std;

extern Console console;

// string AbstractTask::create_task(TaskFunction_t workerTask, int priority, uint32_t sleep_polling, int stack_size, const BaseType_t
// xCoreID) {
//   // console << "[  ] Create " << getName() << " as prio-" << priority << "-task (sleep_polling=" << sleep_polling
//   //         << "ms, stack=" << stack_size << ") try core " << xCoreID << " ...";
//   // sleep_polling_ms = sleep_polling;
//   // // xTaskCreatePinnedToCore((void (*)(void *)) & init_task, /* task function. */
//   // //                         getInfo(),              /* name of task. */
//   // //                         stack_size,                     /* stack size of task */
//   // //                         NULL,                           /* parameter of the task */
//   // //                         priority,                       /* priority of the task */
//   // //                         getTaskHandle(),                    /* task handle to keep track of created task */
//   // //                         xCoreID);                       /* pin task to core id */
//   // console << " done." << NL;
//   return report_task_init(this);
// }

string AbstractTask::init_t(int core_id, int priority, int stack_size, int sleep_polling_ms) {
  this->core_id = core_id;
  this->priority = priority;
  this->stack_size = stack_size;
  set_sleep_polling(sleep_polling_ms);
  return init();
}

string AbstractTask::re_init() {
  vTaskResume(xHandle);
  return getName();
}

void AbstractTask::exit() { vTaskDelete(xHandle); }

string AbstractTask::report_task_init() { return report_task_init(this); };

string AbstractTask::report_task_init(AbstractTask *task) {
  stringstream ss;
  ss << "[ok] " << task->getName() << " started as prio-" << task->priority
     << "-task (taskSuspend=" << task->sleep_polling * portTICK_PERIOD_MS << "ms, stack=" << task->stack_size << ") try core "
     << task->core_id << " real core" << xPortGetCoreID();
  return ss.str();
}
