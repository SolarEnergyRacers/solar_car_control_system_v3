#ifndef SOLAR_CAR_CONTROL_SYSTEM_CANRXBUFFER_H
#define SOLAR_CAR_CONTROL_SYSTEM_CANRXBUFFER_H

#include <Arduino.h>
#include <vector>

#include <CANPacket.h>
#include <definitions.h>

class CANRxBuffer {
private:
  volatile uint8_t head;
  volatile uint8_t tail;
  volatile uint16_t items;
  CANPacket rxBuffer[CAN_RX_BUFFER_SIZE];

public:
  CANRxBuffer();

  void push(CANPacket packet);
  CANPacket pop();
  bool isAvailable();
  uint16_t getSize();
};

#endif // SOLAR_CAR_CONTROL_SYSTEM_CANRXBUFFER_H
