//
// Car State for Radio
//

#ifndef CARSTATERADIO_H
#define CARSTATERADIO_H

#include <list>
#include <map>
#include <sstream>
#include <string>

#include <definitions.h>

#include <CANBus.h>
#include <CANPacket.h>

using namespace std;

class CarStateRadio {
private:
  unsigned long lastSendMillis;

public:
  list<int> radio_packages = {
      // mandatory
      Mppt1Base0x01, //
      Mppt2Base0x01, //
      Mppt3Base0x01, //
      BmsBase0x00,   //
      BmsBase0xF7,   //
      BmsBase0xF8,   //
      BmsBase0xF9,   //
      BmsBase0xFA,   //
      BmsBase0xFD,   //
      DC_BASE0x00,   //
      DC_BASE0x01,   //
      AC_BASE0x00,   //
      // nice to have
      Mppt1Base0x00, //
      Mppt1Base0x02, //
      Mppt2Base0x00, //
      Mppt2Base0x02, //
      Mppt3Base0x00, //
      Mppt3Base0x02, //
      BmsBase0x01,   //
      BmsBase0x02    //
  };

  void init_values();
};

#endif // CARSTATERADIO_H