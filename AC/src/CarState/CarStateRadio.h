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
// #include <CANPacket.h>

using namespace std;

class CarStateRadio {
private:
  unsigned long lastSendMillis;

public:
  list<uint16_t> radio_packages = {
      // mandatory
      (uint16_t)DC_BASE0x00,   //
      (uint16_t)DC_BASE0x01,   //
      (uint16_t)AC_BASE0x00,   //
      (uint16_t)Mppt1Base0x01, //
      (uint16_t)Mppt2Base0x01, //
      (uint16_t)Mppt3Base0x01, //
      (uint16_t)BmsBase0x00,   //
      (uint16_t)BmsBase0xF7,   //
      (uint16_t)BmsBase0xF8,   //
      (uint16_t)BmsBase0xF9,   //
      (uint16_t)BmsBase0xFA,   //
      (uint16_t)BmsBase0xFD,   //
      // nice to have
      (uint16_t)Mppt1Base0x00, //
      (uint16_t)Mppt1Base0x02, //
      (uint16_t)Mppt2Base0x00, //
      (uint16_t)Mppt2Base0x02, //
      (uint16_t)Mppt3Base0x00, //
      (uint16_t)Mppt3Base0x02, //
      (uint16_t)BmsBase0x01,   //
      (uint16_t)BmsBase0x02    //
  };

  void init_values();
  void send_serial();
  void cache_filtered(uint16_t adr, CANPacket packet);
};

#endif // CARSTATERADIO_H