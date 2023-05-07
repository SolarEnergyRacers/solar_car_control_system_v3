//
// Helper Functions
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_HELPER_H
#define SOLAR_CAR_CONTROL_SYSTEM_HELPER_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h> // semaphore
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <limits>
#include <Console.h>

extern Console console;

// #include <../.pio/libdeps/esp32dev/ESP32Time/ESP32Time.h>
// #include <../.pio/libdeps/esp32dev/RTCDS1307/RTCDS1307.h>

// #include <RTC/RTC.h>

using namespace std;

char *fgets_stdio_blocking(char *str, int n);
void xSemaphoreTakeT(xQueueHandle mutex);
// string formatDateTime(RtcDateTime now);
// string getDateTime();  // use globalTime.strTime("%F %R") instead
// string getTime();  // use globalTime.strTime("%R") instead
// string getTimeStamp();  // use globalTime.getUptime() instead
uint16_t normalize_0_UINT16(uint16_t minOriginValue, uint16_t maxOriginValue, uint16_t value);
int transformArea(int minViewValue, int maxViewValue, int minOriginValue, int maxOriginValue, int value);
void vTaskDelay(int delay_ms, string msg);

template <size_t N> int splitString(string (&arr)[N], string str) {
  int n = 0;
  istringstream iss(str);
  for (auto it = istream_iterator<string>(iss); it != istream_iterator<string>() && n < N; ++it, ++n)
    arr[n] = *it;
  return n;
}

template<typename any_in, typename any_out>
any_out normalize_0(any_in minOriginValue, any_in maxOriginValue, any_in value) {
  float k = static_cast<float>(std::numeric_limits<any_out>::max()) / (maxOriginValue - minOriginValue);
  value = value < minOriginValue ? minOriginValue : value;
  value = value > maxOriginValue ? maxOriginValue : value;// - 0.500001;
  return static_cast<any_out>( round((value - minOriginValue) * k) );
}


/** 
 * @brief try to take mux within timeout. 
 * @brief **Warning: needs this.ok() to check if successful**
 */

/**
 * @brief try to take mux within timeout. 
 * @throw runtime_error("mux") if fails to acquire within timeout
 */
class RAII_mux {
private:
  SemaphoreHandle_t mux;
  // bool _ok;
public:
  RAII_mux(SemaphoreHandle_t mux, TickType_t timeout)
  :mux(mux) {
    // _ok = xSemaphoreTake(mux, timeout);
    if (!xSemaphoreTake(mux, timeout)) {
      console << "mux '" << mux << "' not acquired within timeout\n";
      throw std::runtime_error("mux");
    }
  }
  ~RAII_mux() {xSemaphoreGive(mux);}
  // bool ok() {return _ok;}
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_HELPER_H
