//
// Helper Functions
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_HELPER_H
#define SOLAR_CAR_CONTROL_SYSTEM_HELPER_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h> // semaphore
#include <string>

// #include <../.pio/libdeps/esp32dev/ESP32Time/ESP32Time.h>
// #include <../.pio/libdeps/esp32dev/RTCDS1307/RTCDS1307.h>

//#include <RTC/RTC.h>

using namespace std;

template <size_t N> int splitString(string (&arr)[N], string str) {
  int n = 0;
  istringstream iss(str);
  for (auto it = istream_iterator<string>(iss); it != istream_iterator<string>() && n < N; ++it, ++n)
    arr[n] = *it;
  return n;
}

char *fgets_stdio_blocking(char *str, int n);
void xSemaphoreTakeT(xQueueHandle mutex);
// string formatDateTime(RtcDateTime now);
string getDateTime();
string getTime();
string getTimeStamp();

#endif // SOLAR_CAR_CONTROL_SYSTEM_HELPER_H
