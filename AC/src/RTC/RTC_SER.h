//
// DS1307
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_RTC_SER_H
#define SOLAR_CAR_CONTROL_SYSTEM_RTC_SER_H

// std
#include <limits>
#include <stdint.h>
#include <time.h>

// lib-deps
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Wire.h>
#include <RtcDS1307.h>

// project
#include <I2CBus.h>


class GlobalTime {
private:
	bool is_init = false;
	unsigned int sec0 = 0;  // dt since last update (sec)
	int last_update = std::numeric_limits<int>::max();  // actual RTC sync (sec)

	I2CBus* i2cBus;

	RtcDateTime _datetime;

	RtcDS1307<TwoWire> Rtc = RtcDS1307<TwoWire>(Wire);

	void update();

	// thread safety. **MUST BE LOCKED BEFORE I2CBUS MUX**
	SemaphoreHandle_t mutex = NULL;  

public:
	/**
	 * @param p_i2cBus: pointer to I2CBus with relevant mux
	 * @param datetime: datetime to write to RTC. 
	 * nullptr (default arg.) to not overwrite RTC time.
	 * datetime cannot be 2000-01-01 00:00:00 (used to mark "don't set RTC")
	 */
	GlobalTime(I2CBus* p_i2cBus, const RtcDateTime* datetime = nullptr) 
	: i2cBus(p_i2cBus) {
		this->_datetime = datetime ? *datetime : RtcDateTime(0);
		mutex = xSemaphoreCreateMutex();
	}

	int init(DS1307SquareWaveOut sq_freq, bool do_run);
	int re_init(DS1307SquareWaveOut sq_freq, bool do_run) {
		return init(sq_freq, do_run);
	}

	int msec() {return millis()%1000;}
	int sec()  {update(); return _datetime.Second();}
	int min()  {update(); return _datetime.Minute();}
	int hour() {update(); return _datetime.Hour();}

	int day()     {update(); return _datetime.Day();}
	int month()   {update(); return _datetime.Month();}
	int year()    {update(); return _datetime.Year();}
	int weekday() {update(); return _datetime.DayOfWeek();}

	RtcDateTime datetime() {update(); return _datetime;}
	std::string strTime(const std::string& format);
	std::string strUptime(bool with_ms = false);

	bool get_RTC();
	bool set_RTC(uint32_t secondsFrom2000);
	bool set_RTC(const RtcDateTime& datetime);

	bool set_RTC_running(bool do_run);
	bool get_RTC_running();
	bool set_RTC_squarewave(DS1307SquareWaveOut sq_freq);
};

#endif  // SOLAR_CAR_CONTROL_SYSTEM_RTC_SER_H