//
// CAN Bus
//
#include <fmt/core.h>

#include <definitions.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

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

using namespace std;

BaseType_t xHigherPriorityTaskWoken = pdFALSE;
void onReceive(int packetSize) {

  if (!SystemInited)
    return;

  uint64_t rxData = 0l;
  if (xSemaphoreTakeFromISR(canBus.mutex_in, &xHigherPriorityTaskWoken) != pdTRUE) {
    canBus.counterI_notAvail++;
    return;
  }

  uint16_t packetId = CAN.packetId();
  if (canBus.is_to_ignore_packet(packetId)) {
    xSemaphoreGiveFromISR(canBus.mutex_in, &xHigherPriorityTaskWoken);
    return;
  }

  if (!CAN.available()) {
    xSemaphoreGiveFromISR(canBus.mutex_in, &xHigherPriorityTaskWoken);
    canBus.counterI_notAvail++;
    return;
  }
  for (int i = 0; i < packetSize && i < 8; i++) {
    rxData = rxData | (((uint64_t)CAN.read()) << (i * 8));
  }
  xSemaphoreGiveFromISR(canBus.mutex_in, &xHigherPriorityTaskWoken);

  canBus.pushIn(CANPacket(packetId, rxData));
  canBus.counterI++;
  canBus.counterI_notAvail = 0;
  canBus.setPacketTimeStamp(packetId, millis());
}

bool CANBus::isPacketToRenew(uint16_t packetId) {
  return max_ages[packetId] == 0 || (max_ages[packetId] != -1 && millis() - ages[packetId] > max_ages[packetId]);
}

void CANBus::setPacketTimeStamp(uint16_t packetId, int32_t millis) { ages[packetId] = millis; }

CANBus::CANBus() { init_ages(); }

string CANBus::re_init() { return CANBus::init(); }

string CANBus::init() {
  bool hasError = false;
  counterI = 0;
  counterR = 0;
  counterW = 0;
  counterI_notAvail = 0;
  counterR_notAvail = 0;
  counterW_notAvail = 0;

  counterMaxPacketsIn = 0;
  counterMaxPacketsOut = 0;

  mutex_out = xSemaphoreCreateBinary();
  xSemaphoreGive(mutex_out);
  mutex_in = xSemaphoreCreateBinary();
  CAN.setPins(CAN_RX, CAN_TX);
  if (!CAN.begin(CAN_SPEED)) {
    CAN.setTimeout(400);
    xSemaphoreGive(mutex_in);
    hasError = true;
    console << fmt::format("     ERROR: CANBus with rx={}, tx={} NOT, speed={} inited.\n", CAN_RX, CAN_TX, CAN_SPEED);
  } else {
    CAN.onReceive(onReceive);
    xSemaphoreGive(mutex_in);
    console << fmt::format("     CANBus with rx={}, tx={}, speed={} inited.\n", CAN_RX, CAN_TX, CAN_SPEED);
  }
  return fmt::format("[{}] CANBus initialized.", hasError ? "--" : "ok");
}

void CANBus::exit() {
  // Exit needs to be implemented for Task, here or in AbstractTask
  xSemaphoreTakeT(mutex_in);
  CAN.end();
  xSemaphoreGive(mutex_in);
}

CANPacket CANBus::writePacket(uint16_t adr,
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

CANPacket CANBus::writePacket(uint16_t adr,
                              uint16_t data_u16_0, // LifeSign
                              uint16_t data_u16_1, // Potentiometer value
                              uint16_t data_u16_2, // HAL-paddle Acceleration ADC value
                              uint16_t data_u16_3, // HAL-paddle Deceleration ADC value
                              bool force) {
  uint64_t data = 0UL;
  CANPacket packet = CANPacket(adr, data);
  packet.setData_u16(0, data_u16_0);
  packet.setData_u16(1, data_u16_1);
  packet.setData_u16(2, data_u16_2);
  packet.setData_u16(3, data_u16_3);
  return writePacket(adr, packet, force);
}

std::map<uint16_t, CANPacket> packetsLast;
CANPacket CANBus::writePacket(uint16_t adr, CANPacket packet, bool force) {
  if (force || packetsLast.find(adr) == packetsLast.end() || packetsLast[adr].getData_i64() != packet.getData_i64()) {
    packetsLast[adr] = packet;
    pushOut(packet);
  }
  return packet;
}

void CANBus::write_rx_packet(CANPacket packet) {
  uint16_t adr = 0;
  try {
    // if (xSemaphoreTake(mutex_out, (TickType_t)32) != pdTRUE) {
    //   console << fmt::format("\nFAIL on Package [{:x}] write, counterW_notAvail={}\n", adr, counterW_notAvail);
    //   counterW_notAvail++;
    //   return;
    // }
    adr = packet.getId();
    if (adr == 0) {
      // xSemaphoreGive(mutex_out);
      return;
    }

    counterW++;
    if (verboseModeCanOutNative)
      console << print_raw_packet("W", packet) << NL;
    counterW_notAvail = 0;
    CAN.beginPacket(packet.getId());
    CAN.write(packet.getData_i8(0));
    CAN.write(packet.getData_i8(1));
    CAN.write(packet.getData_i8(2));
    CAN.write(packet.getData_i8(3));
    CAN.write(packet.getData_i8(4));
    CAN.write(packet.getData_i8(5));
    CAN.write(packet.getData_i8(6));
    CAN.write(packet.getData_i8(7));
    CAN.endPacket();
    // xSemaphoreGive(mutex_out);
  } catch (exception &ex) {
    // xSemaphoreGive(mutex_out);
    console << "ERROR: Couldn not send uint64_t data to address " << adr << ", ex: " << ex.what() << NL;
  }
}

string CANBus::print_raw_packet(string msg, CANPacket packet) {
  return fmt::format(
      "C{}-{}-[I:{:02d}|{:02d},O:{:02d}|{:02d}]={}=Id=0x{:03x}-data: {:016x} -- {:02x} - {:02x} - {:02x} - {:02x} - {:02x} - "
      "{:02x} - {:02x} - {:02x}",
      xPortGetCoreID(), esp_timer_get_time() / 1000000, availiblePacketsIn(), getMaxPacketsBufferInUsage(), availiblePacketsOut(),
      getMaxPacketsBufferOutUsage(), msg, packet.getId(), packet.getData_u64(), packet.getData_u8(7), packet.getData_u8(6),
      packet.getData_u8(5), packet.getData_u8(4), packet.getData_u8(3), packet.getData_u8(2), packet.getData_u8(1), packet.getData_u8(0));
}

void CANBus::task(void *pvParams) {
  while (1) {
    if (SystemInited) {
      if (counterR_notAvail > 8 || counterI_notAvail > 8 || counterW_notAvail > 8) {
        console << NL
                << fmt::format("CANBus REINIT trigger: I{}|{}, R{}|{}, W{}|{}", counterI_notAvail, counterI, counterR_notAvail, counterR,
                               counterW_notAvail, counterW)
                << NL;
        canBus.re_init();
      }

      while (rxBufferIn.isAvailable()) {
        handle_rx_packet(rxBufferIn.pop());
      }

      while (rxBufferOut.isAvailable()) {
        write_rx_packet(rxBufferOut.pop());
      }
    }
    taskSuspend();
  }
}
