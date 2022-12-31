//
// CAN Bus
//

#include <map>

#include <Abstract_task.h>
#include <CAN/CANRxBuffer.h>
#include <CarState/CarState.h>

class CANBus : public Abstract_task {

public:
  // RTOS task
  string getName(void) { return "CANBus"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void);

  // Class functions and members
private:
  std::map<uint16_t, int32_t> max_ages;
  std::map<uint16_t, int32_t> ages;
  int handle_rx_packet(CANPacket packet);
  int packetsCountMax;
  CANRxBuffer rxBuffer;
  // void (CANBus::*receive_ptr) (int);

public:
  CANBus();

  void push(CANPacket packet);
  CANPacket pop();

  bool isPacketToRenew(uint16_t packetId);
  void setPacketTimeStamp(uint16_t packetId, int32_t millis);

  int availiblePackets();
  int getMaxPacketsBufferUsage() { return packetsCountMax; };

  // void onReceive(int packetSize);

  bool writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint16_t data3);
  bool writePacket(uint16_t adr, uint32_t data0, uint32_t data1);
  bool writePacket(uint16_t adr, uint64_t data);

  SemaphoreHandle_t mutex;
  bool verboseModeCan = false;
  bool verboseModeCanIn = false;
  bool verboseModeCanOut = false;
  bool verboseModeCanDebug = false;
};
