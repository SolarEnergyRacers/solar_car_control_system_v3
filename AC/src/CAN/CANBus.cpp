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

void onReceive(int packetSize) {

  if (!SystemInited)
    return;

  int packetId = CAN.packetId();
  if (canBus.is_to_ignore_packet(packetId))
    return;

  canBus.setPacketTimeStamp(packetId, millis());

  if (CAN.available()) {
    uint64_t rxData = 0l;
    for (int i = 0; i < packetSize && i < 8; i++) {
      rxData = rxData | (((uint64_t)CAN.read()) << (i * 8));
    }
    canBus.push(CANPacket(packetId, rxData));
  }
  // xSemaphoreGive(canBus.mutex);
}

bool CANBus::isPacketToRenew(uint16_t packetId) {
  return max_ages[packetId] == 0 || (max_ages[packetId] != -1 && millis() - ages[packetId] > max_ages[packetId]);
}

void CANBus::setPacketTimeStamp(uint16_t packetId, int32_t millis) { ages[packetId] = millis; }

CANBus::CANBus() {
  packetsCountMax = 0;
  init_ages();
}

string CANBus::re_init() {
  CAN.end();
  return CANBus::init();
}

string CANBus::init() {
  bool hasError = false;
  console << "[  ] Init CANBus...\n";
  packetsCountMax = 0;
  mutex = xSemaphoreCreateBinary();
  CAN.setPins(CAN_RX, CAN_TX);
  if (!CAN.begin(CAN_SPEED)) {
    xSemaphoreGive(canBus.mutex);
    console << fmt::format("     ERROR: CANBus with rx={}, tx={} NOT, speed={} inited.\n", CAN_RX, CAN_TX, CAN_SPEED);
    hasError = true;
  } else {
    xSemaphoreGive(canBus.mutex);
    console << fmt::format("     CANBus with rx={}, tx={}, speed={} inited.\n", CAN_RX, CAN_TX, CAN_SPEED);
  }
  xSemaphoreTakeT(canBus.mutex);
  CAN.onReceive(onReceive);
  xSemaphoreGive(canBus.mutex);
  return fmt::format("[{}] CANBus initialized.", hasError ? "--" : "ok");
}

void CANBus::exit() {
  // Exit needs to be implemented for Task, here or in AbstractTask
  xSemaphoreTakeT(canBus.mutex);
  CAN.end();
  xSemaphoreGive(canBus.mutex);
}

void CANBus::push(CANPacket packet) {
  rxBuffer.push(packet);
  // register level of rxBuffer filling:
  if (packetsCountMax < availiblePackets())
    packetsCountMax = availiblePackets();
}

bool CANBus::writePacket(uint16_t adr, uint8_t data0, int8_t data1, bool b0, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6,
                         bool b7) {
  uint8_t boolByte = ((uint8_t)b7) << 7 | ((uint8_t)b6) << 6 | ((uint8_t)b5) << 5 | ((uint8_t)b4) << 4 | ((uint8_t)b3) << 3 |
                     ((uint8_t)b2) << 2 | ((uint8_t)b1) << 1 | ((uint8_t)b0) << 0;
  uint64_t data = ((uint64_t)boolByte) << 48 | ((uint64_t)data1) << 8 | ((int64_t)data0) << 0;
  return writePacket(adr, data);
}
bool CANBus::writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint8_t data3, int8_t data4) {
  uint64_t data =
      ((uint64_t)data0) << 48 | ((uint64_t)data1) << 32 | ((uint64_t)data2) << 16 | ((uint64_t)data3) << 8 | ((int64_t)data4) << 0;
  return writePacket(adr, data);
}
bool CANBus::writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint8_t data3, uint8_t data4) {
  uint64_t data =
      ((uint64_t)data0) << 48 | ((uint64_t)data1) << 32 | ((uint64_t)data2) << 16 | ((uint64_t)data3) << 8 | ((uint64_t)data4) << 0;
  return writePacket(adr, data);
}
bool CANBus::writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint16_t data3) {
  uint64_t data = ((uint64_t)data0) << 48 | ((uint64_t)data1) << 32 | ((uint64_t)data2) << 16 | ((uint64_t)data3) << 0;
  return writePacket(adr, data);
}

bool CANBus::writePacket(uint16_t adr, uint32_t data0, uint32_t data1) {
  uint64_t data = (uint64_t)data0 << 32 | (uint64_t)data1 << 0;
  return writePacket(adr, data);
}

bool CANBus::writePacket(uint16_t adr, uint64_t data) {
  CANPacket packet = CANPacket(adr, data);
  try {
    if (canBus.verboseModeCanOut)
      console << print_raw_packet("S", packet) << NL;
    xSemaphoreTakeT(mutex);
    CAN.beginPacket(adr);
    CAN.write(packet.getData_ui8(0));
    CAN.write(packet.getData_ui8(1));
    CAN.write(packet.getData_ui8(2));
    CAN.write(packet.getData_ui8(3));
    CAN.write(packet.getData_ui8(4));
    CAN.write(packet.getData_ui8(5));
    CAN.write(packet.getData_ui8(6));
    CAN.write(packet.getData_ui8(7));
    CAN.endPacket();
    xSemaphoreGive(mutex);
  } catch (exception &ex) {
    console << "ERROR: Couldn not send uint64_t data to address " << adr << NL;
    return false;
  }
  return true;
}

string CANBus::print_raw_packet(string msg, CANPacket packet) {
  return fmt::format("C{}-{}-[{:02d}|{:02d}] {} CAN.PacketId=0x{:03x}-data: {:016x} -- {:02x} - {:02x} - {:02x} - {:02x} - {:02x} - "
                     "{:02x} - {:02x} - {:02x}",
                     xPortGetCoreID(), esp_timer_get_time() / 1000000, availiblePackets(), getMaxPacketsBufferUsage(), msg, packet.getId(),
                     packet.getData_ui64(), packet.getData_ui8(7), packet.getData_ui8(6), packet.getData_ui8(5), packet.getData_ui8(4),
                     packet.getData_ui8(3), packet.getData_ui8(2), packet.getData_ui8(1), packet.getData_ui8(0));
}

void CANBus::task(void *pvParams) {
  while (1) {
    if (SystemInited) {
      // handle recieved message with CANBus
      xSemaphoreTakeT(canBus.mutex);
      while (rxBuffer.isAvailable()) {
        handle_rx_packet(rxBuffer.pop());
      }
      xSemaphoreGive(canBus.mutex);
    }
    taskSuspend();
  }
}
