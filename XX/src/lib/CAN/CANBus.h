//
// CAN Bus
//

#include <map>

#include <Abstract_task.h>
#include <CAN/CANRxBuffer.h>
#include <CarState/CarState.h>

void read_can_demo_task(void *pvParameter);

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

public:
  CANBus();

  void onReceive(int packetSize);

  SemaphoreHandle_t mutex;
  bool verboseModeCan = false;
};
