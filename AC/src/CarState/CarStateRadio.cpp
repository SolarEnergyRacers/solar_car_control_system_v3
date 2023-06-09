//
// Car State Radio
//
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

#include <definitions.h>

#include <CANPacket.h>
#include <CarState.h>
#include <CarStateRadio.h>
#include <ConfigFile.h>
#include <Console.h>
#include <Helper.h>
#include <Serial.h>

using namespace std;

extern CarStateRadio carStateRadio;
extern Console console;
extern CarState carState;
extern Uart uart;

void CarStateRadio::send() {
  if (mode == SEND_MODE::BINARY)
    send_binary();
  else
    send_ascii();
}

void CarStateRadio::send_binary() {
  std::array<uint8_t, CANPacket::BUFFER_SIZE> buffer;
  for (const auto &kv : packet_cache) {
    CANPacket packet = kv.second;
    packet.to_serial(kv.first, buffer);
    Serial2.write(buffer.data(), buffer.size());
    if (verboseModeRadioSend) {
      cout << fmt::format("{:4x}:b:", packet.getId());
      for (uint8_t element : buffer) {
        cout << fmt::format(" {:02x}", element);
      }
      cout << NL;
    }
  }
  if (verboseModeRadioSend)
    cout << "-----------" << NL;
}

void CarStateRadio::send_ascii() {
  std::array<uint8_t, CANPacket::BUFFER_SIZE> buffer;
  for (const auto &kv : packet_cache) {
    CANPacket packet = kv.second;
    packet.to_serial(kv.first, buffer);
    for (uint8_t element : buffer) {
      Serial2.print(fmt::format("{:02x}", element).c_str());
    }
    if (verboseModeRadioSend) {
      cout << fmt::format("{:04x}:a:", packet.getId());
      for (uint8_t element : buffer) {
        cout << fmt::format(" {:02x}", element);
      }
      cout << NL;
    }
  }
  if (verboseModeRadioSend)
    cout << "-----------" << NL;
}

void CarStateRadio::push_if_radio_packet(uint16_t adr, CANPacket packet) {
  if (contains(radio_packets, adr))
    packet_cache[adr] = packet;
}
