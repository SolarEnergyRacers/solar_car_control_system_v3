//
// This is the abstract task class, which has to be implemented by all other tasks
//
//#include<../.pio/libdeps/esp32dev/fmt/include/fmt/core.h>
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

#include <Console.h>
#include <Abstract_task.h>

using namespace std;

extern Console console;

void Abstract_task::sleep() { vTaskDelay(sleep_polling_ms / portTICK_PERIOD_MS); };

string Abstract_task::create_task(int priority, uint32_t sleep_polling, int stack_size) {
  sleep_polling_ms = sleep_polling;
  console << "[  ] Create " << getName() << " as prio-" << priority << "-task (sleep_polling=" << sleep_polling
          << "ms, stack=" << stack_size << ") ...";
#if WithTaskSuspend == true
  xTaskCreate((void (*)(void *)) & init_task, getInfo().c_str(), stack_size, (void *)this, 1, &xHandle);
#else
  xTaskCreate((void (*)(void *)) & init_task, getInfo().c_str(), stack_size, (void *)this, priority, NULL);
#endif
  console << " done.\n";
  return report_task_init(this);
};

string Abstract_task::re_init() {

#if WithTaskSuspend == true
  vTaskResume(xHandle);
#endif
  return getName();
};

void Abstract_task::exit() {
#if WithTaskSuspend == true
  vTaskDelete(xHandle);
#endif
};

string Abstract_task::getInfo() { return getName(); }

string Abstract_task::report_task_init() { return report_task_init(this); }
string Abstract_task::report_task_init(Abstract_task *task) {
  stringstream ss;
  ss << "[ok] " << task->getName() << " started (" << task->sleep_polling_ms << "ms).";
  return ss.str();
}
