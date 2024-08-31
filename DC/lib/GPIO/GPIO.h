//
// General Purpose Input / Output
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_GPIO_H
#define SOLAR_CAR_CONTROL_SYSTEM_GPIO_H

#include <AbstractTask.h>
#include "../definitions.h"
#include <driver/gpio.h>

class GPInputOutput : public AbstractTask {
public:
  // RTOS task
  string getName(void) { return "GPIO"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void *pvParams);

  // Class functions and members
  void register_gpio_interrupt(void);
  static volatile int interrupt_counter;
  static portMUX_TYPE mutex;

  static void IRAM_ATTR handle_gpio_interrupt(void) {
    // portENTER_CRITICAL_ISR(&mutex);
    // count number of interrupts
    interrupt_counter++;
    // portEXIT_CRITICAL_ISR(&mutex);
  };
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_GPIO_H
