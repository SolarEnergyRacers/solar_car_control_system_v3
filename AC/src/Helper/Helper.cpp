//
// Helper Functions
//
#include <definitions.h>

#include <fmt/core.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <iostream>
#include <string>
// #include <../.pio/libdeps/esp32dev/ESP32Time/ESP32Time.h>
// #include <../.pio/libdeps/esp32dev/RTCDS1307/RTCDS1307.h>

#include <Console.h>
#include <Helper.h>
// #include <RTC.h>

extern Console console;
// extern RTC rtc;
// extern ESP32Time esp32time;

using namespace std;

char *fgets_stdio_blocking(char *str, int n) {

  char c;
  int i = 0;
  do {
    c = getchar();
    if (c == 255) { // no char available to consume
      // taskYIELD(); // does not seem to do the trick
      vTaskDelay(100 / portTICK_PERIOD_MS);
    } else { // store char
      str[i++] = c;
    }
    console << "[" << i << ":" << c << "]";
  } while (i < (n - 1) && c != '\n' && c != '\r');
  str[i] = 0; // add zero-termination

  // on success
  return str;
}

void xSemaphoreTakeT(xQueueHandle mutex) {
  if (!xSemaphoreTake(mutex, portMAX_DELAY)) {
    console << "ERROR: mutex ************************************ " << mutex << " ****************\n";
    throw runtime_error("ERROR: mutex");
  }
}

// https://github.com/fbiego/ESP32Time
// string getDateTime() { return "xx.xx.xxxx xx:xx"; } // esp32time.getTime("%Y-%m-%d,%H:%M:%S").c_str(); }
// string getTime() { return "xx:xx"; }                // esp32time.getTime("%H:%M:%S").c_str(); }

// string formatDateTime(RtcDateTime now) {
//   string static dateTimeString =
//       fmt::format("{:04d}-{:02d}-{:02d},{:02d}:{:02d}:{:02d}", now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(),
//       now.Second());
//   return dateTimeString;
// }

// string getTimeStamp() {
//   unsigned long seconds = millis() / 1000;
//   unsigned long secsRemaining = seconds % 3600;
//   int runHours = seconds / 3600;
//   int runMinutes = secsRemaining / 60;
//   int runSeconds = secsRemaining % 60;
//   return fmt::format("T{:02d}:{:02d}:{:02d}", runHours, runMinutes, runSeconds);
// }

uint16_t normalize_0_UINT16(uint16_t minOriginValue, uint16_t maxOriginValue, uint16_t value) {
  float k = (float)UINT16_MAX / (maxOriginValue - minOriginValue);
  value = value < minOriginValue ? minOriginValue : value;
  value = value > maxOriginValue ? maxOriginValue : value;
  return (uint16_t)round((value - minOriginValue) * k);
}

int transformArea(int minViewValue, int maxViewValue, int minOriginValue, int maxOriginValue, int value) {
  float k = (float)(maxViewValue - minViewValue) / (maxOriginValue - minOriginValue);
  value = value < minOriginValue ? minOriginValue : value;
  value = value > maxOriginValue ? maxOriginValue : value;
  value = (int)round((value - minOriginValue) * k);
  value = value < minViewValue ? minViewValue : value;
  return value;
}

void vTaskDelay(int delay_ms, string msg) {
  console << msg;
  vTaskDelay(10);
}
