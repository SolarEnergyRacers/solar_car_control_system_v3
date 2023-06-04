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

void CarStateRadio::init_values() {
  for (int adr : radio_packages) {
    carState.packet_cache[adr] = CANPacket();
  }
}

void CarStateRadio::send_serial() {
  std::array<uint8_t, CANPacket::BUFFER_SIZE> buffer;
  for (const auto &kv : carState.packet_cache) {
    CANPacket packet = kv.second;
    packet.to_serial(kv.first, buffer);
    Serial2.write(buffer.data(), buffer.size());
    if (uart.verboseModeRadioSend) {
      cout << fmt::format("{:4x}:", packet.getId());
      for (uint8_t element : buffer) {
        cout << fmt::format(" {:02x}", element);
      }
      cout << NL;
    }
  }
  if (uart.verboseModeRadioSend)
    cout << "-----------" << NL;
}

void CarStateRadio::cache_filtered(uint16_t adr, CANPacket packet) {
  if (contains(radio_packages, adr))
    carState.packet_cache[adr] = packet;
}
