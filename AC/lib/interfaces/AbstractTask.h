//
// This is the abstract task class, which has to be implemented by all other tasks
//

#ifndef ABSTRACTTASK_H
#define ABSTRACTTASK_H

#include <global_definitions.h>
#include "../definitions.h"
#include <stdint.h>
#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

using namespace std;

class AbstractTask {
private:
  TaskHandle_t xHandle;

  uint32_t core_id;
  uint32_t priority;
  uint32_t stack_size;
  uint32_t sleep_polling;

public:
  void set_sleep_polling(uint32_t sleep_polling_ms) { sleep_polling = sleep_polling_ms / portTICK_PERIOD_MS; };
  string init_t(int core_id, int priority = 1, int stack_size = 10000, int sleep_polling_ms = 300);
  virtual string init(void) = 0;
  virtual string getName(void) = 0;
  virtual string re_init(void) = 0;
  virtual void exit(void) = 0;
  virtual void task(void *pvParams) = 0;

  uint32_t getCoreId() { return core_id; };
  uint32_t getPriority() { return priority; };
  uint32_t getStackSize() { return stack_size; };
  uint32_t getSleepPolling_ms() { return sleep_polling * portTICK_PERIOD_MS; };
  TaskHandle_t *getTaskHandle() { return &xHandle; };
  const char *getInfo(void) { return getName().c_str(); };

  void taskSuspend(void) { vTaskDelay(sleep_polling); };
  void taskSuspend(int sleep_ms) { vTaskDelay(sleep_ms / portTICK_PERIOD_MS); };
  string report_task_init();
  string report_task_init(AbstractTask *task);
};

#endif // ABSTRACTTASK_H
