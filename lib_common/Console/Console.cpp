#include <global_definitions.h>

#include <Streaming.h>

#include <iostream>
//#include <malloc.h>
//#include <stdio.h>
#include <string>

#include <Console.h>

using namespace std;

bool static extended_charset(char c) { return c < ' ' && c != '\t' && c != '\n' && c != '\r'; }

const char *strip_extended_chars(string str) {
  str.erase(remove_if(str.begin(), str.end(), extended_charset), str.end());
  return str.c_str();
}

Console::Console() { buffer = string(); }

//--Operator overloads-
Console &operator<<(Console &c, const bool &var) { return operator<<(c, to_string(var)); }
Console &operator<<(Console &c, const double &var) { return operator<<(c, to_string(var)); }
Console &operator<<(Console &c, const float &var) { return operator<<(c, to_string(var)); }
Console &operator<<(Console &c, const int &var) { return operator<<(c, to_string(var)); }
Console &operator<<(Console &c, const long &var) { return operator<<(c, to_string(var)); }
Console &operator<<(Console &c, const size_t &var) { return operator<<(c, to_string(var)); }
Console &operator<<(Console &c, const string &var) { return operator<<(c, var.c_str()); }
Console &operator<<(Console &c, const unsigned long &var) { return operator<<(c, to_string(var)); }
Console &operator<<(Console &c, const uint64_t &var) { return operator<<(c, to_string(var)); }
Console &operator<<(Console &c, const volatile int &var) { return operator<<(c, to_string(var)); }

//------- OUT ---------
Console &operator<<(Console &c, const char &chr) {
  cout << chr << flush;
  return c;
}

Console &operator<<(Console &c, const char *str) {
  cout << str << flush;
  return c;
}

//--------IN-----------
string &operator>>(string &s, Console &c) {
  cout << s;
  return c.buffer;
}
