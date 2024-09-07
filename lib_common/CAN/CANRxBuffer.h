#ifndef SOLAR_CAR_CONTROL_SYSTEM_CANRXBUFFER_H
#define SOLAR_CAR_CONTROL_SYSTEM_CANRXBUFFER_H

#include <Arduino.h>
#include <vector>

#include <global_definitions.h>
#include <CANPacket.h>

class CANRxBuffer {
private:
  StaticQueue_t overhead;
  uint8_t buffer[CAN_RX_BUFFER_SIZE * sizeof(CANPacket)];
  QueueHandle_t queue;

public:
  CANRxBuffer();

  void push(const CANPacket& packet);
  CANPacket pop();
  bool isAvailable();
  uint16_t getSize();
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_CANRXBUFFER_H
