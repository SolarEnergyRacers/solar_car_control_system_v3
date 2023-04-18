//
// SPI Bus
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_SPIBUS_H
#define SOLAR_CAR_CONTROL_SYSTEM_SPIBUS_H

#include <definitions.h>

#include <fmt/core.h>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <SPI.h>

#include <Console.h>
#include <SPIBus.h>

class SPIBus {
private:
public:
  SemaphoreHandle_t mutex = NULL;
  SPIClass spi = SPIClass(VSPI);
  string init();
  string re_init();
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_SPIBUS_H
