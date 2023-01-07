#ifndef SOLAR_CAR_CONTROL_SYSTEM_CANPACKET_H
#define SOLAR_CAR_CONTROL_SYSTEM_CANPACKET_H

#include <Arduino.h>

typedef union {
  bool data_b[64];
  uint8_t data_u8[8];
  int8_t data_i8[8];
  uint16_t data_u16[4];
  int16_t data_i16[4];
  uint32_t data_u32[2];
  int32_t data_i32[2];
  float data_f32[2];
  uint64_t data_u64;
  int64_t data_i64;
} group_64;

class CANPacket {
private:
  uint16_t id;
  group_64 data;

public:
  CANPacket();
  CANPacket(uint16_t id, uint8_t data[]);
  CANPacket(uint16_t id, uint64_t data);

  CANPacket &operator=(const CANPacket &that) {
    if (this != &that) {
      this->id = that.id;
      this->data = that.data;
    }

    return *this;
  };

  void setId(uint16_t id);
  uint16_t getId();

  void setData(uint8_t data[]);
  void setData(uint64_t data) { this->data.data_u64 = data; };

  void setData_u64(uint8_t index, uint64_t value) { this->data.data_u64 = value; }
  uint64_t getData_u64() { return this->data.data_u64; }
  void setData_i64(uint8_t index, int64_t value) { this->data.data_i64 = value; }
  int64_t getData_i64() { return this->data.data_i64; }

  void setData_u32(uint8_t index, uint32_t value) { this->data.data_u32[index] = value; }
  uint32_t getData_u32(uint8_t index) { return this->data.data_u32[index]; }
  void setData_i32(uint8_t index, int32_t value) { this->data.data_i32[index] = value; }
  int32_t getData_i32(uint8_t index) { return this->data.data_i32[index]; }
  void setData_f32(uint8_t index, float value) { this->data.data_f32[index] = value; }
  float getData_f32(uint8_t index) { return this->data.data_f32[index]; }

  void setData_u16(uint8_t index, uint16_t value) { this->data.data_u16[index] = value; }
  uint16_t getData_u16(uint8_t index) { return this->data.data_u16[index]; }
  void setData_i16(uint8_t index, int16_t value) { this->data.data_i16[index] = value; }
  int16_t getData_i16(uint8_t index) { return this->data.data_i16[index]; }

  void setData_u8(uint8_t index, uint8_t value) { this->data.data_u8[index] = value; }
  uint8_t getData_u8(uint8_t index) { return this->data.data_u8[index]; }
  void setData_i8(uint8_t index, int8_t value) { this->data.data_i8[index] = value; }
  int8_t getData_i8(uint8_t index) { return this->data.data_i8[index]; }

  void setData_b(uint8_t index, bool value) { this->data.data_b[index] = value; }
  bool getData_b(uint8_t index) { return this->data.data_b[index]; }
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_CANPACKET_H
