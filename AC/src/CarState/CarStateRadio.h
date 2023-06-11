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

enum class SEND_MODE { BINARY, ASCII };
static const char *SEND_MODE_str[] = {"BINARY", "ASCII"};

class CarStateRadio {
private:
  unsigned long lastSendMillis;
  void send_ascii();
  void send_binary();

  std::map<uint16_t, CANPacket> packet_cache;

  list<uint16_t> radio_packets = {
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
      (uint16_t)McBase0x00,    //
      (uint16_t)McBase0x0e,    //
      (uint16_t)McBase0x0f,    //
      (uint16_t)McBase0x10,    //
      (uint16_t)McBase0x1b,    //
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

public:
  CarStateRadio() {
    // BEGIN prevent stupid compiler warnings "defined but not used"
    (void)SEND_MODE_str;
    // BEGIN prevent stupid compiler warnings "defined but not used"
  }
  SEND_MODE mode = SEND_MODE::BINARY;
  void send();
  void push_if_radio_packet(uint16_t adr, CANPacket packet);
  bool verboseModeRadioSend = false;
};

#endif // CARSTATERADIO_H