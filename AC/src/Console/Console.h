
//
// Software Serial Console Streaming object
//
// https://bingmann.github.io/2007/0301-StdOStreamPrintable.html
// https://stackoverflow.com/questions/22714089/no-match-for-operator-operand-types-stdostream-c-oop-and-point
// std::cout << myPoint is just syntactic sugar for operator<<(std::cout, myPoint).

#ifndef SOLAR_CAR_CONTROL_SYSTEM_CONSOLE_H
#define SOLAR_CAR_CONTROL_SYSTEM_CONSOLE_H

// local definitions
#include <definitions.h>
// standard libraries
#include <Streaming.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
// Arduino
#include <Arduino.h>
#include <SoftwareSerial.h>

using namespace std;

class Console {
public:
  Console();

private:
  string buffer = "";

public:
  //------- OUT ---------
  // transformers
  friend Console &operator<<(Console &c, const bool &var);
  friend Console &operator<<(Console &c, const char &var);
  friend Console &operator<<(Console &c, const double &var);
  friend Console &operator<<(Console &c, const float &var);
  friend Console &operator<<(Console &c, const int &var);
  friend Console &operator<<(Console &c, const long &var);
  friend Console &operator<<(Console &c, const size_t &var);
  friend Console &operator<<(Console &c, const string &var);
  friend Console &operator<<(Console &c, const unsigned long &var);
  friend Console &operator<<(Console &c, const uint64_t &var);
  friend Console &operator<<(Console &c, const volatile int &var);
  // real output
  friend Console &operator<<(Console &c, const char *var);

  //-------- IN ---------
  friend string &operator>>(string &s, Console &c);
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_CONSOLE_H
