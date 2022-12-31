//
// This is the abstract task class, which has to be implemented by all other tasks
//

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

#include <Abstract_task.h>
#include <Console.h>

using namespace std;

extern Console console;

void Abstract_task::sleep() { vTaskDelay(sleep_polling_ms / portTICK_PERIOD_MS); };

string Abstract_task::create_task(int priority, uint32_t sleep_polling, int stack_size, const BaseType_t xCoreID) {
  sleep_polling_ms = sleep_polling;
  console << "[  ] Create " << getName() << " as prio-" << priority << "-task (sleep_polling=" << sleep_polling
          << "ms, stack=" << stack_size << ") try core " << xCoreID << " ...";

  // xTaskCreate((void (*)(void *)) & init_task, getInfo().c_str(), stack_size, (void *)this, priority, &taskHandle);
  // xTaskCreate((void (*)(void *)) & init_task, getInfo().c_str(), stack_size, (void *)this, priority, NULL);
  xTaskCreatePinnedToCore((void (*)(void *)) & init_task, getInfo().c_str(), stack_size, (void *)this, priority, &taskHandle, xCoreID);
  console << " done." << NL;
  return report_task_init(this);
};

string Abstract_task::re_init() {

#if WithTaskSuspend == true
  vTaskResume(taskHandle);
#endif
  return getName();
};

void Abstract_task::exit() {
#if WithTaskSuspend == true
  vTaskDelete(taskHandle);
#endif
};

string Abstract_task::getInfo() { return getName(); }

string Abstract_task::report_task_init() { return report_task_init(this); }
string Abstract_task::report_task_init(Abstract_task *task) {
  stringstream ss;
  ss << "[ok] " << task->getName() << " started (" << task->sleep_polling_ms << "ms), at core " << xPortGetCoreID() << ". ";
  return ss.str();
}
