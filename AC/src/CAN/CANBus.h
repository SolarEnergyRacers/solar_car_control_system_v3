//
// CAN Bus
//

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
  string print_raw_packet(CANPacket packet);

public:
  CANBus();

  void push(CANPacket packet);
  CANPacket pop() { return rxBuffer.pop(); }

  bool isPacketToRenew(uint16_t packetId);
  void setPacketTimeStamp(uint16_t packetId, int32_t millis);

  int availiblePackets() { return rxBuffer.getSize(); }
  int getMaxPacketsBufferUsage() { return packetsCountMax; };

  bool writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint8_t data3, uint8_t data4);
  bool writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint16_t data3);
  bool writePacket(uint16_t adr, uint32_t data0, uint32_t data1);
  bool writePacket(uint16_t adr, uint64_t data);

  SemaphoreHandle_t mutex;
  bool verboseModeCan = false;
  bool verboseModeCanIn = false;
  bool verboseModeCanOut = false;
  bool verboseModeCanDebug = false;
};
