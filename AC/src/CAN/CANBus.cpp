//
// CAN Bus
//
#include <fmt/core.h>

#include <definitions.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include <Arduino.h>
#include <CAN.h>

#include <CANBus.h>
#include <CarState.h>
#include <Console.h>
#include <Helper.h>
#include <I2CBus.h>

extern CarState carState;
extern Console console;
extern I2CBus i2cBus;
extern CANBus canBus;
extern bool SystemInited;

bool canBusReinitRequestR = false;
bool canBusReinitRequestI = false;
bool canBusReinitRequestW = false;
int counterI = 0;
int counterR = 0;
int counterI_notAvail = 0;
int counterR_notAvail = 0;
int counterW_notAvail = 0;

using namespace std;

uint32_t timeout_ms = 1000;
int timerID = 3;
int packetSize;

BaseType_t xHigherPriorityTaskWoken = pdFALSE;
void onReceive_worker(TimerHandle_t pxTimer) {
  if (xSemaphoreTakeFromISR(canBus.mutex, &xHigherPriorityTaskWoken) == pdTRUE) {
    int packetId = CAN.packetId();
    if (canBus.is_to_ignore_packet(packetId))
      return;

    counterI++;
    counterI_notAvail = 0;
    canBus.setPacketTimeStamp(packetId, millis());

    if (CAN.available()) {
      uint64_t rxData = 0l;
      for (int i = 0; i < packetSize && i < 8; i++) {
        rxData = rxData | (((uint64_t)CAN.read()) << (i * 8));
      }
      canBus.push(CANPacket(packetId, rxData));
    }
    xSemaphoreGiveFromISR(canBus.mutex, &xHigherPriorityTaskWoken);
  } else {
    if (counterI_notAvail++ > 8)
      canBusReinitRequestI = true;
  }
}

void onReceive(int _packetSize) {
  if (!SystemInited)
    return;

  packetSize = _packetSize;
  onReceive_worker(NULL);
  return;
  // TimerHandle_t timer = xTimerCreate("onReceive_timer", pdMS_TO_TICKS(timeout_ms), pdFALSE, 0, onReceive_worker);

  // xTimerStart(timer, 0);

  // // Wait for timer to expire or function to complete
  // TickType_t start_time = xTaskGetTickCount();
  // while (xTimerIsTimerActive(timer)) {
  //   // Check if timeout has elapsed
  //   if (xTaskGetTickCount() - start_time >= pdMS_TO_TICKS(timeout_ms)) {
  //     console << "onReceive TIMEOUT" << NL;
  //     break;
  //   }
  //   vTaskDelay(pdMS_TO_TICKS(10));
  // }

  // xTimerStop(timer, 0);
  // xTimerDelete(timer, 0);

  TimerHandle_t xTimer = xTimerCreate("onReceive_timer",         // Just a text name, not used by the kernel.
                                      pdMS_TO_TICKS(timeout_ms), // The timer period in ticks.
                                      pdFALSE,                   // one shoot timer.
                                      (void *)timerID,           // Assign each timer a unique id equal to its array index.
                                      onReceive_worker           // Each timer calls the same callback when it expires.
  );
  if (xTimer == NULL) { // The timer was not created.
  } else {
    // Start the timer.  No block time is specified, and even if one was
    // it would be ignored because the scheduler has not yet been
    // started.
    if (xTimerStart(xTimer, timerID) != pdPASS) { // The timer could not be set into the Active state.
    }
  }

  // ...
  // Create tasks here.
  // ...
  // Starting the scheduler will start the timers running as they have already
  // been set into the active state.
  vTaskStartScheduler();
  // Should not reach here.
  for (;;)
    ;
}

//-------------------------------------------------------

bool CANBus::isPacketToRenew(uint16_t packetId) {
  return max_ages[packetId] == 0 || (max_ages[packetId] != -1 && millis() - ages[packetId] > max_ages[packetId]);
}

void CANBus::setPacketTimeStamp(uint16_t packetId, int32_t millis) { ages[packetId] = millis; }

CANBus::CANBus() {
  packetsCountMax = 0;
  init_ages();
}

string CANBus::re_init() { return CANBus::init(); }

string CANBus::init() {
  bool hasError = false;
  canBusReinitRequestR = false;
  canBusReinitRequestI = false;
  canBusReinitRequestW = false;
  counterI = 0;
  counterR = 0;
  counterI_notAvail = 0;
  counterR_notAvail = 0;
  counterW_notAvail = 0;
  packetsCountMax = 0;
  mutex = xSemaphoreCreateBinary();
  CAN.setPins(CAN_RX, CAN_TX);
  if (!CAN.begin(CAN_SPEED)) {
    CAN.setTimeout(400);
    xSemaphoreGive(mutex);
    hasError = true;
    console << fmt::format("     ERROR: CANBus with rx={}, tx={} NOT, speed={} inited.\n", CAN_RX, CAN_TX, CAN_SPEED);
  } else {
    CAN.onReceive(onReceive);
    xSemaphoreGive(mutex);
    console << fmt::format("     CANBus with rx={}, tx={}, speed={} inited.\n", CAN_RX, CAN_TX, CAN_SPEED);
  }
  return fmt::format("[{}] CANBus initialized.", hasError ? "--" : "ok");
}

void CANBus::exit() {
  // Exit needs to be implemented for Task, here or in AbstractTask
  xSemaphoreTakeT(mutex);
  CAN.end();
  xSemaphoreGive(mutex);
}

void CANBus::push(CANPacket packet) {
  rxBuffer.push(packet);
  // register level of rxBuffer filling:
  if (packetsCountMax < availiblePackets())
    packetsCountMax = availiblePackets();
}

// bool CANBus::writePacket(uint16_t adr, uint8_t data0, int8_t data1, bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6,
//                          bool b7, bool force) {
//   uint8_t boolByte = ((uint8_t)b7) << 7 | ((uint8_t)b6) << 6 | ((uint8_t)b5) << 5 | ((uint8_t)b4) << 4 | ((uint8_t)b3) << 3 |
//                      ((uint8_t)b2) << 2 | ((uint8_t)b1) << 1 | ((uint8_t)b0) << 0;
//   uint64_t data = ((uint64_t)boolByte) << 48 | ((uint64_t)data0) << 8 | ((int64_t)data1 & 0x00000000000000ff) << 0;
//   return writePacket(adr, data, force);
// }

bool CANBus::writePacket(uint16_t adr,
                         uint16_t data_u16_0, // Target Speed [float as value\*1000]
                         uint16_t data_u16_1, // Target Power [float as value\*1000]
                         int8_t data_i8_4,    // Display Acceleration
                         uint8_t data_u8_5,   // empty
                         uint8_t data_u8_6,   // Display Speed
                         bool b_56,           // Fwd [1] / Bwd [0]
                         bool b_57,           // Button Lvl Brake Pedal
                         bool b_58,           // MC Off [0] / On [1]
                         bool b_59,           // Constant Mode Off [false], On [true]
                         bool b_60,           // empty
                         bool b_61,           // empty
                         bool b_62,           // empty
                         bool b_63,           // empty
                         bool force) {
  uint64_t data = 0;
  CANPacket packet = CANPacket(adr, data);
  packet.setData_u16(0, data_u16_0);
  packet.setData_u16(1, data_u16_1);
  packet.setData_i8(4, data_i8_4);
  packet.setData_u8(5, data_u8_5);
  packet.setData_u8(6, data_u8_6);
  packet.setData_b(56, b_56);
  packet.setData_b(57, b_57);
  packet.setData_b(58, b_58);
  packet.setData_b(59, b_59);
  packet.setData_b(60, b_60);
  packet.setData_b(61, b_61);
  packet.setData_b(62, b_62);
  packet.setData_b(63, b_63);
  return writePacket(adr, packet, force);
}

bool CANBus::writePacket(uint16_t adr,
                         uint16_t data_u16_0, // LifeSign
                         uint16_t data_u16_1, // Potentiometer value
                         uint16_t data_u16_2, // HAL-paddle Acceleration ADC value
                         uint16_t data_u16_3, // HAL-paddle Deceleration ADC value
                         bool force) {
  uint64_t data = 0;
  CANPacket packet = CANPacket(adr, data);
  packet.setData_u16(0, data_u16_0);
  packet.setData_u16(1, data_u16_1);
  packet.setData_u16(2, data_u16_2);
  packet.setData_u16(3, data_u16_3);
  return writePacket(adr, packet, force);
}

std::map<uint16_t, CANPacket> packetsLast;
bool CANBus::writePacket(uint16_t adr, CANPacket packet, bool force) {

  if (force || packetsLast.find(adr) == packetsLast.end() || packetsLast[adr].getData_i64() != packet.getData_i64()) {
    if (canBus.verboseModeCanOutNative)
      console << print_raw_packet("S", packet) << NL;
    try {
      packetsLast[adr] = packet;
      if (xSemaphoreTake(mutex, (TickType_t)11) == pdTRUE) {
        counterW_notAvail = 0;
        canBusReinitRequestW = false;
        CAN.beginPacket(adr);
        CAN.write(packet.getData_i8(0));
        CAN.write(packet.getData_i8(1));
        CAN.write(packet.getData_i8(2));
        CAN.write(packet.getData_i8(3));
        CAN.write(packet.getData_i8(4));
        CAN.write(packet.getData_i8(5));
        CAN.write(packet.getData_i8(6));
        CAN.write(packet.getData_i8(7));
        CAN.endPacket();
        xSemaphoreGive(mutex);
      } else {
        console << fmt::format(" W[{:x}]FAIL ", adr);
        if (counterW_notAvail++ > 8)
          canBusReinitRequestW = true;
        return false;
      }
    } catch (exception &ex) {
      xSemaphoreGive(mutex);
      console << "ERROR: Couldn not send uint64_t data to address " << adr << NL;
    }
  }
  return true;
}

string CANBus::print_raw_packet(string msg, CANPacket packet) {
  return fmt::format("C{}-{}-[{:02d}|{:02d}]={}=CAN.PacketId=0x{:03x}-data: {:016x} -- {:02x} - {:02x} - {:02x} - {:02x} - {:02x} - "
                     "{:02x} - {:02x} - {:02x}",
                     xPortGetCoreID(), esp_timer_get_time() / 1000000, availiblePackets(), getMaxPacketsBufferUsage(), msg, packet.getId(),
                     packet.getData_u64(), packet.getData_u8(7), packet.getData_u8(6), packet.getData_u8(5), packet.getData_u8(4),
                     packet.getData_u8(3), packet.getData_u8(2), packet.getData_u8(1), packet.getData_u8(0));
}

void CANBus::task(void *pvParams) {
  while (1) {
    if (SystemInited) {
      // console << fmt::format("({}_{}_{})", counterI_notAvail, counterR_notAvail, counterW_notAvail);

      if (canBusReinitRequestR || canBusReinitRequestI || canBusReinitRequestW) {
        console << NL
                << fmt::format("CANBus REINIT, trigger: I{} | R{} | W{}: {:4d} | {:4d} ERR: {}_{}_{}", canBusReinitRequestI,
                               canBusReinitRequestR, canBusReinitRequestW, counterI, counterR, counterI_notAvail, counterR_notAvail,
                               counterW_notAvail)
                << NL;
        vTaskDelay(10, "i-");
        canBus.re_init();
        vTaskDelay(10, "j-");
      }
      if (xSemaphoreTake(mutex, (TickType_t)1300) == pdTRUE) {
        counterR++;
        counterR_notAvail = 0;
        while (rxBuffer.isAvailable()) {
          handle_rx_packet(rxBuffer.pop());
        }
        xSemaphoreGive(mutex);
      } else {
        if (counterR_notAvail++ > 8)
          canBusReinitRequestR = true;
      }
    }
    taskSuspend();
  }
}
