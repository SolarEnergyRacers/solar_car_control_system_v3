//
// CAN Bus
// https://github.com/sandeepmistry/arduino-CAN
// --> https://github.com/SolarEnergyRacers/SER-arduino-CAN
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
  int handle_rx_packet(CANPacket packet);

  // Class functions and members
private:
  int counterMaxPackets;
  CANRxBuffer rxBuffer;
  std::map<uint16_t, int32_t> max_ages;
  std::map<uint16_t, int32_t> ages;

  void init_ages();
  string print_raw_packet(string msg, CANPacket packet);

public:
  CANBus();

  void push(CANPacket packet);
  CANPacket pop() { return rxBuffer.pop(); }

  bool isPacketToRenew(uint16_t packetId);
  void setPacketTimeStamp(uint16_t packetId, int32_t millis);

  int availiblePackets() { return rxBuffer.getSize(); }
  int getMaxPacketsBufferUsage() { return counterMaxPackets; };

  // optimize with: https://learn.microsoft.com/en-us/cpp/cpp/functions-with-variable-argument-lists-cpp?view=msvc-170
  bool writePacket(uint16_t adr, uint16_t data_u16_0, uint16_t data_u16_1, int8_t data_i8_4, uint8_t data_u8_5, uint8_t data_u8_6,
                   bool b_56, bool b_57, bool b_58, bool b_59, bool b_60, bool b_61, bool b_62, bool b_63, bool force = false);
  bool writePacket(uint16_t adr, uint16_t data_u16_0, uint16_t data_u16_1, uint16_t data_u16_2, uint16_t data_u16_3, bool force = false);
  bool writePacket(uint16_t adr, CANPacket packet, bool force = false);

  SemaphoreHandle_t mutex;
  bool verboseModeCanIn = false;
  bool verboseModeCanInNative = false;
  bool verboseModeCanOut = false;
  bool verboseModeCanOutNative = false;
  bool verboseModeCanBusLoad = false;
};
#endif // SOLAR_CAR_CONTROL_SYSTEM_CANBUS_H
