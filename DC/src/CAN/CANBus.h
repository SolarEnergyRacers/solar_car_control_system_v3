//
// CAN Bus
//
#ifndef SOLAR_CAR_CONTROL_SYSTEM_CANBUS_H
#define SOLAR_CAR_CONTROL_SYSTEM_CANBUS_H

#include <map>

#include <AbstractTask.h>
#include <CANRxBuffer.h>
#include <CarState.h>

class CANBus : public AbstractTask {

public:
  // RTOS task
  string getName(void) { return "CANBus"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void *pvParams);
  bool is_to_ignore_packet(int packetId);

  // Class functions and members
private:
  int packetsCountMax;
  CANRxBuffer rxBuffer;
  std::map<uint16_t, int32_t> max_ages;
  std::map<uint16_t, int32_t> ages;

  void init_ages();
  int handle_rx_packet(CANPacket packet);
  string print_raw_packet(string msg, CANPacket packet);

public:
  CANBus();

  void push(CANPacket packet);
  CANPacket pop() { return rxBuffer.pop(); }

  bool isPacketToRenew(uint16_t packetId);
  void setPacketTimeStamp(uint16_t packetId, int32_t millis);

  int availiblePackets() { return rxBuffer.getSize(); }
  int getMaxPacketsBufferUsage() { return packetsCountMax; };

  // optimize with: https://learn.microsoft.com/en-us/cpp/cpp/functions-with-variable-argument-lists-cpp?view=msvc-170
  bool writePacket(uint16_t adr, uint8_t data0, int8_t data1, bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7);
  bool writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint8_t data3, uint8_t data4);
  bool writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint8_t data3, int8_t data4);
  bool writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint16_t data3);
  bool writePacket(uint16_t adr, uint32_t data0, uint32_t data1);
  bool writePacket(uint16_t adr, uint64_t data);

  SemaphoreHandle_t mutex;
  bool verboseModeCanIn = false;
  bool verboseModeCanInNative = false;
  bool verboseModeCanOut = false;
  bool verboseModeCanOutNative = false;
};
#endif // SOLAR_CAR_CONTROL_SYSTEM_CANBUS_H
