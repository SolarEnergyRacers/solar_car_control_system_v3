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
  CANRxBuffer rxBuffer;
  std::map<uint16_t, int32_t> max_ages;
  std::map<uint16_t, int32_t> ages;
  int handle_rx_packet(CANPacket packet);
  int packetsCountMax;

public:
  CANBus();
  int availiblePackets();
  int getMaxPacketsBufferUsage() { return packetsCountMax; };

  void onReceive(int packetSize);

  bool writePacket(uint16_t adr, uint8_t data[]);
  bool writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint16_t data3);
  bool writePacket(uint16_t adr, uint32_t data0, uint32_t data1);
  bool writePacket(uint16_t adr, uint64_t data);
  bool writePacket(uint16_t adr, CANPacket packet);

  SemaphoreHandle_t mutex;
  bool verboseModeCan = false;
  bool verboseModeCanDebug = false;
};
