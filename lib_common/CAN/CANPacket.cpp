#include <Arduino.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <iostream>

#include <CANPacket.h>

CANPacket::CANPacket() {
  id = 0;
  data.data_u64 = 0ull;
}

CANPacket::CANPacket(uint16_t id, uint8_t data[]) {
  this->id = id;
  this->data.data_u64 = 0ull;
  for (int i = 0; i < 8; i++) {
    this->data.data_u8[i] = data[i];
  }
}

CANPacket::CANPacket(uint16_t id, uint64_t data) {
  this->id = id;
  this->data.data_u64 = data;
}

uint16_t CANPacket::getId() { return id; }

void CANPacket::setId(uint16_t id) { this->id = id; }

void CANPacket::setData(uint8_t data[]) {
  for (int i = 0; i < 8; i++) {
    this->data.data_u8[i] = data[i];
  }
}

void CANPacket::to_serial(uint16_t adr, std::array<uint8_t, BUFFER_SIZE> &buffer) {
  if (adr != id)
    cout << fmt::format("ERROR: adr={:04x} != packetId={:04x}\n");
  // uint16_t maskedId = id | 0b1111100000000000;
  uint8_t highByte = static_cast<uint8_t>((id & 0xFF00) >> 8);
  uint8_t lowByte = static_cast<uint8_t>(id & 0x00FF);
  highByte |= 0b11111000;
  uint16_t maskedId = lowByte << 8 | highByte;
  int pos = 0;
  memcpy(buffer.data() + pos, &maskedId, sizeof(maskedId));
  pos += sizeof(maskedId);
  memcpy(buffer.data() + pos, &data, sizeof(data));
  pos += sizeof(data);
  uint8_t new_line = '\n';
  memcpy(buffer.data() + pos, &new_line, sizeof(new_line));
  pos += sizeof(new_line);
}
