//
// Car State Pins
//

#ifndef CARSTATEPIN_H
#define CARSTATEPIN_H

#include <global_definitions.h>

using namespace std;

class CarStatePin {
public:
  int port; // high nibble: device number, low nibble: portNr (pin)
  int mask;
  int mode;
  int value;
  int oldValue;
  bool inited;
  unsigned long timestamp;
  string name;
  void (*handlerFunction)();
  bool continouseMode;
  unsigned long debounceTime_ms; // in ms
};

#endif // CARSTATEPIN_H
