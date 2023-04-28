//
// DS1307
// 

#include "RTC_SER.h"

// std
#include <time.h>

// lib-deps
#include <RtcDS1307.h>

// project
#include <I2CBus.h>
#include <iostream>
#include <Console.h>


extern Console console;

/**
 * initialize GlobalTime and its RTC
 * returns sum of:
 * 0x0: ok.
 * 0x1: set_RTC_squarewave failed.
 * 0x2: set_RTC_running failed.
 * 0x4: get_RTC / set_RTC failed.
 * 0x8: set_RTC_running failed.
 */
int GlobalTime::init(DS1307SquareWaveOut sq_freq, bool do_run) {
	int err_code = 0;
	is_init = true;  // must be true before accessing I2CBus
	err_code += 0x1 * set_RTC_squarewave(sq_freq);
	err_code += 0x2 * set_RTC_running(do_run);
	if ( _datetime.TotalSeconds() == 0 ) {
		err_code += 0x4 * get_RTC();
	}
	else {
		err_code += 0x4 * set_RTC(_datetime);
	}
	err_code += 0x8 * set_RTC_running(1);
	return err_code;
}

/**
 * @brief update internal datetime from cpu clock or RTC
 */
void GlobalTime::update() {
	constexpr int min_wait = 60 * 1000;  // min. time before next RTC query
	auto lock = RAII_mux(mutex, portMAX_DELAY);
	if (!lock.ok()) return;
	int millisec = millis();
	if ((millisec - last_update) > min_wait || (millisec - last_update) < 0) {
		if (get_RTC()) return;
		else console << "ERROR: RTC update failed, using cpu clock instead.\n";
	}
	// if get_RTC() not needed or failed:
	uint32_t sec = _datetime.TotalSeconds();
	sec += (millisec - millisec0) / 1000;
	_datetime = RtcDateTime(sec);
	millisec0 = millisec;
	return;
}

/**
 * @brief get the time as an std:string object with the specified format
 * @param format
 * time format: 
 * http://www.cplusplus.com/reference/ctime/strftime/
 * TL;DR: '%H:%M:%S %Y-%m-%d (%b)' for '09:10:59 2023-03-28 (Tue)'
 */
std::string GlobalTime::strTime(const std::string& format){
	update();
	tm timeinfo;
	time_t now = _datetime.Unix32Time();
	gmtime_r(&now, &timeinfo);
	char s[128];
	const char* c = format.c_str();
	strftime(s, 127, c, &timeinfo);
	return std::string(s);
}

bool GlobalTime::set_RTC_running(bool do_run) {
	if (!is_init) {
		console << "tried to set RTC before GlobalTime was init()ed.\n";
		return 0;
	}
	auto lock = RAII_mux(i2cBus->mutex, portMAX_DELAY);
	if (!lock.ok()) {
		cout << "ERROR: set_RTC_running() didn't get mux lock.\n";
		return 0;
	}
	Rtc.SetIsRunning(do_run);
	return 1;
}

bool GlobalTime::get_RTC_running() {
	if (!is_init) {
		console << "tried to set RTC before GlobalTime was init()ed.\n";
		return 0;
	}
	auto lock = RAII_mux(i2cBus->mutex, portMAX_DELAY);
	if (!lock.ok()) {
		cout << "ERROR: get_RTC_running() didn't get mux lock.\n";
		return 0;
	}
	return Rtc.GetIsRunning();
}

bool GlobalTime::set_RTC_squarewave(DS1307SquareWaveOut sq_freq) {
	if (!is_init) {
		console << "tried to set RTC before GlobalTime was init()ed.\n";
		return 0;
	}
	auto lock = RAII_mux(i2cBus->mutex, portMAX_DELAY);
	if (!lock.ok()) {
		cout << "ERROR: set_RTC_squarewave() didn't get mux lock.\n";
		return 0;
	}
	Rtc.SetSquareWavePin(sq_freq);
	return 1;
}

/**
 * @brief force update internal datetime from RTC
 */
bool GlobalTime::get_RTC() {
	if (!is_init) {
		console << "tried to read RTC before GlobalTime was init()ed.\n";
		return 0;
	}
	auto lock = RAII_mux(i2cBus->mutex, portMAX_DELAY);
	if (lock.ok()){
		_datetime = Rtc.GetDateTime();
		last_update = millis();
		millisec0 = last_update;
		return 1;
	}
	else return 0;
}

/**
 * @brief set RTC time
 * @param secondsFrom2000: seconds since 2000-01-01
 */
bool GlobalTime::set_RTC(uint32_t secondsFrom2000) {
	if (!is_init) {
		console << "tried to set RTC before GlobalTime was init()ed.\n";
		return 0;
	}
	auto lock1 = RAII_mux(mutex, portMAX_DELAY);  // writing this->...
	if (!lock1.ok()) return 0;
	auto lock2 = RAII_mux(i2cBus->mutex, portMAX_DELAY);
	if (lock2.ok()){
		Rtc.SetDateTime(RtcDateTime(secondsFrom2000));
		_datetime = RtcDateTime(secondsFrom2000);  // update internal time
		millisec0 = millis();  // adjust for offline update
		return 1;
	}
	else return 0;
}

/**
 * @brief set RTC time
 * @param datetime: RtcDateTime object
 */
bool GlobalTime::set_RTC(const RtcDateTime& datetime) {
	if (!is_init) {
		console << "tried to set RTC before GlobalTime was init()ed.\n";
		return 0;
	}
	auto lock1 = RAII_mux(mutex, portMAX_DELAY);  // writing this->...
	if (!lock1.ok()) return 0;
	auto lock2 = RAII_mux(i2cBus->mutex, portMAX_DELAY);
	if (lock2.ok()){
		Rtc.SetDateTime(datetime);
		_datetime = datetime;  // update internal time
		millisec0 = millis();  // adjust for offline update
		return 1;
	}
	else return 0;
}