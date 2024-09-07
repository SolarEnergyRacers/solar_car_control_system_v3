//
// System Utility and Helper Functions
//

// standard libraries
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_spi_flash.h>
#include <esp_system.h>

#include <System.h>

#include <AbstractTask.h>
#include <RtcDS1307.h> // millis()
// #include <CANBus.h>
// extern CANBus canBus;



using namespace std;

void chip_info(void) {

  // print chip information
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ", chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
         (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

  printf("silicon revision %d, ", chip_info.revision);

  printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

// static overhead structure to track tasks and their timers
#define max_N_tasks 8
int do_report_task_stack;
int timer_values[max_N_tasks] = {0};
AbstractTask* task_list[8] = {NULL};

string memory_info(void) {
  // stringstream ss("");
  // ss << min_free_8bit_cap;
  // heap_caps_dump_all();
  // return ss.c_str();
  // return "Not implemented yet.";
   if (do_report_task_stack) {
    do_report_task_stack = 0;
    printf("stack report off\n");
  }
  else {
    for (int idx=0; idx<max_N_tasks; ++idx) timer_values[idx] = 0;
    do_report_task_stack = 1;
    printf("stack report on\n");
  }

  stringstream ss;
  return ss.str();
}

void report_task_stack(AbstractTask* task_ptr) {
  // todo: is task_ptr actually constant?
  // if not -> eventually runs out of max_N_tasks slots for timers

  // check if report is even requested
  if (!do_report_task_stack) return;

  // find timer id
  int timer_id = max_N_tasks + 1;
  for (int idx=0; idx<max_N_tasks; ++idx) {
    if (task_list[idx] == NULL) {
      // register new task
      task_list[idx] = task_ptr;
      timer_id = idx;
      break;
    }
    if (task_list[idx] == task_ptr) {
      // found known task
      timer_id = idx;
      break;
    }
  }
  // print stack usage
  if (timer_id < max_N_tasks) {
    if (millis() > timer_values[timer_id] + 2000) {
      timer_values[timer_id] = millis();
      UBaseType_t mark = uxTaskGetStackHighWaterMark(NULL); 
        // crashes when given a task id. Even if task id is from current task.
      printf("stack %s = %i\n", (*task_ptr).getName().c_str(), mark);
    }
  }
  else {
    printf("attempted reporting stack use for out of range id: %i\n", timer_id);
  }
}

void restart(void) {

  // count time down
  for (int i = 10; i >= 0; i--) {
    printf("Restarting in %d seconds...\n", i);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  // actual restart
  printf("Restarting now.\n");
  fflush(stdout);
  esp_restart();
}
