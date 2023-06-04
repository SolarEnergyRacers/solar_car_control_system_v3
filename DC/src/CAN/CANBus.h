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

#define Mppt1Base0x01 MPPT1_BASE_ADDR | 0x01
#define Mppt2Base0x01 MPPT2_BASE_ADDR | 0x01
#define Mppt3Base0x01 MPPT3_BASE_ADDR | 0x01
#define BmsBase0x00 BMS_BASE_ADDR | 0x00
#define BmsBase0xF7 BMS_BASE_ADDR | 0xF7
#define BmsBase0xF8 BMS_BASE_ADDR | 0xF8
#define BmsBase0xF9 BMS_BASE_ADDR | 0xF9
#define BmsBase0xFA BMS_BASE_ADDR | 0xFA
#define BmsBase0xFD BMS_BASE_ADDR | 0xFD
#define DC_BASE0x00 DC_BASE_ADDR | 0x00
#define DC_BASE0x01 DC_BASE_ADDR | 0x01
#define AC_BASE0x00 8
#define Mppt1Base0x00 MPPT1_BASE_ADDR | 0x00
#define Mppt1Base0x02 MPPT1_BASE_ADDR | 0x02
#define Mppt2Base0x00 MPPT2_BASE_ADDR | 0x00
#define Mppt2Base0x02 MPPT2_BASE_ADDR | 0x02
#define Mppt3Base0x00 MPPT3_BASE_ADDR | 0x00
#define Mppt3Base0x02 MPPT3_BASE_ADDR | 0x02
#define BmsBase0x01 15

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
  int counterMaxPacketsIn;
  int counterMaxPacketsOut;
  int counterR;
  int counterW;
  int counterR_notAvail;
  int counterW_notAvail;

  CANRxBuffer rxBufferIn;
  CANRxBuffer rxBufferOut;
  std::map<uint16_t, int32_t> max_ages;
  std::map<uint16_t, int32_t> ages;

  void init_ages();
  string print_raw_packet(string msg, CANPacket packet);

  // optimize with: https://learn.microsoft.com/en-us/cpp/cpp/functions-with-variable-argument-lists-cpp?view=msvc-170
  CANPacket popIn() { return rxBufferIn.pop(); }
  void handle_rx_packet(CANPacket packet);
  CANPacket popOut() { return rxBufferOut.pop(); }
  void write_rx_packet(CANPacket packet);

public:
  CANBus();

  int counterI;
  int counterI_notAvail;

  bool writePacket(uint16_t adr, uint16_t data_u16_0, uint16_t data_u16_1, int8_t data_i8_4, uint8_t data_u8_5, uint8_t data_u8_6,
                   bool b_56, bool b_57, bool b_58, bool b_59, bool b_60, bool b_61, bool b_62, bool b_63, bool force = false);
  bool writePacket(uint16_t adr, uint16_t data_u16_0, uint16_t data_u16_1, uint16_t data_u16_2, uint16_t data_u16_3, bool force = false);
  bool writePacket(uint16_t adr, CANPacket packet, bool force = false);

  void pushIn(CANPacket packet) {
    rxBufferIn.push(packet);
    counterMaxPacketsIn = max(counterMaxPacketsIn, availiblePacketsIn());
  }
  void pushOut(CANPacket packet) {
    rxBufferOut.push(packet);
    counterMaxPacketsOut = max(counterMaxPacketsOut, availiblePacketsOut());
  }

  bool isPacketToRenew(uint16_t packetId);
  void setPacketTimeStamp(uint16_t packetId, int32_t millis);

  int availiblePacketsIn() { return rxBufferIn.getSize(); }
  int availiblePacketsOut() { return rxBufferOut.getSize(); }
  int getMaxPacketsBufferInUsage() { return counterMaxPacketsIn; };
  int getMaxPacketsBufferOutUsage() { return counterMaxPacketsOut; };

  SemaphoreHandle_t mutex_in;
  SemaphoreHandle_t mutex_out;
  bool verboseModeCanIn = false;
  bool verboseModeCanInNative = false;
  bool verboseModeCanOut = false;
  bool verboseModeCanOutNative = false;
  bool verboseModeCanBusLoad = false;
};
#endif // SOLAR_CAR_CONTROL_SYSTEM_CANBUS_H
