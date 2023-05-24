#include <Arduino.h>

#include <CANPacket.h>

CANPacket::CANPacket() {
  id = 0;
  data.data_u64 = 0;
}

CANPacket::CANPacket(uint16_t id, uint8_t data[]) {
  this->id = id;
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

void CANPacket::to_serial(std::array<uint8_t, 10> &buffer) {
  buffer[0] = id;
  buffer[1] = 1;
  for (auto idx = 0; idx < 8; idx++) {
    buffer[idx + 2] = getData_b(idx);
  }
}
