//
// CAN Bus
//
#include <fmt/core.h>

#include <global_definitions.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <Arduino.h>
#include <CAN.h>

#include <CANBus.h>
#include <CarState.h>
#include <Console.h>
#include <Helper.h>
#include <I2CBus.h>

#include <System.h>

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
  // if (xSemaphoreTakeFromISR(canBus.mutex_in, &xHigherPriorityTaskWoken) != pdTRUE) {
  //   canBus.counterI_notAvail++;
  //   return;
  // }

  uint16_t packetId = CAN.packetId();
  if (canBus.is_to_ignore_packet(packetId)) {
    // xSemaphoreGiveFromISR(canBus.mutex_in, &xHigherPriorityTaskWoken);
    return;
  }

  if (!CAN.available()) {
    // xSemaphoreGiveFromISR(canBus.mutex_in, &xHigherPriorityTaskWoken);
    // canBus.counterI_notAvail++;
    return;
  }
  for (int i = 0; i < packetSize && i < 8; i++) {
    rxData = rxData | (((uint64_t)CAN.read()) << (i * 8));
  }
  // xSemaphoreGiveFromISR(canBus.mutex_in, &xHigherPriorityTaskWoken);

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

  // mutex_out = xSemaphoreCreateBinary();
  // xSemaphoreGive(mutex_out);
  // mutex_in = xSemaphoreCreateBinary();
  CAN.setPins(CAN_RX, CAN_TX);
  if (!CAN.begin(CAN_SPEED)) {
    CAN.setTimeout(400);
    // xSemaphoreGive(mutex_in);
    hasError = true;
    console << fmt::format("     ERROR: CANBus with rx={}, tx={} NOT, speed={} inited.\n", CAN_RX, CAN_TX, CAN_SPEED);
  } else {
    CAN.onReceive(onReceive);
    // xSemaphoreGive(mutex_in);
    console << fmt::format("     CANBus with rx={}, tx={}, speed={} inited.\n", CAN_RX, CAN_TX, CAN_SPEED);
  }
  return fmt::format("[{}] CANBus initialized.", hasError ? "--" : "ok");
}

void CANBus::exit() {
  // Exit needs to be implemented for Task, here or in AbstractTask
  // xSemaphoreTakeT(mutex_in);
  CAN.end();
  // xSemaphoreGive(mutex_in);
}

void CANBus::init_ages() {
  // init max ages
  max_ages[BMS_BASE_ADDR] = MAXAGE_BMU_HEARTBEAT;
  max_ages[BMS_BASE_ADDR | 0x1] = MAXAGE_CMU_TEMP;     // CMU1
  max_ages[BMS_BASE_ADDR | 0x2] = MAXAGE_CMU_VOLTAGES; // CMU1
  max_ages[BMS_BASE_ADDR | 0x3] = MAXAGE_CMU_VOLTAGES; // CMU1
  max_ages[BMS_BASE_ADDR | 0x4] = MAXAGE_CMU_TEMP;     // CMU2
  max_ages[BMS_BASE_ADDR | 0x5] = MAXAGE_CMU_VOLTAGES; // CMU2
  max_ages[BMS_BASE_ADDR | 0x6] = MAXAGE_CMU_VOLTAGES; // CMU2
  max_ages[BMS_BASE_ADDR | 0x7] = MAXAGE_CMU_TEMP;     // CMU3
  max_ages[BMS_BASE_ADDR | 0x8] = MAXAGE_CMU_VOLTAGES; // CMU3
  max_ages[BMS_BASE_ADDR | 0x9] = MAXAGE_CMU_VOLTAGES; // CMU3
  max_ages[BMS_BASE_ADDR | 0xF4] = MAXAGE_PACK_SOC;
  max_ages[BMS_BASE_ADDR | 0xF5] = MAXAGE_BALANCE_SOC;
  max_ages[BMS_BASE_ADDR | 0xF6] = MAXAGE_CHARGER_CONTROL;
  max_ages[BMS_BASE_ADDR | 0xF7] = MAXAGE_PRECHARGE_STATUS;
  max_ages[BMS_BASE_ADDR | 0xF8] = MAXAGE_MIN_MAX_U_CELL;
  max_ages[BMS_BASE_ADDR | 0xF9] = MAXAGE_MIN_MAX_T_CELL;
  max_ages[BMS_BASE_ADDR | 0xFA] = MAXAGE_PACK_VOLTAGE;
  max_ages[BMS_BASE_ADDR | 0xFB] = MAXAGE_PACK_STATUS;
  max_ages[BMS_BASE_ADDR | 0xFC] = MAXAGE_PACK_FAN_STATUS;
  max_ages[BMS_BASE_ADDR | 0xFD] = MAXAGE_EXT_PACK_STATUS;

  max_ages[MPPT1_BASE_ADDR] = MAXAGE_MPPT_INPUT;
  max_ages[MPPT1_BASE_ADDR | 0x1] = MAXAGE_MPPT_OUTPUT;
  max_ages[MPPT1_BASE_ADDR | 0x2] = MAXAGE_MPPT_TEMP;
  max_ages[MPPT1_BASE_ADDR | 0x3] = MAXAGE_MPPT_AUX_POWER;
  max_ages[MPPT1_BASE_ADDR | 0x4] = MAXAGE_MPPT_LIMITS;
  max_ages[MPPT1_BASE_ADDR | 0x5] = MAXAGE_MPPT_STATUS;
  max_ages[MPPT1_BASE_ADDR | 0x6] = MAXAGE_MPPT_POWER_CONN;

  max_ages[MPPT2_BASE_ADDR] = MAXAGE_MPPT_INPUT;
  max_ages[MPPT2_BASE_ADDR | 0x1] = MAXAGE_MPPT_OUTPUT;
  max_ages[MPPT2_BASE_ADDR | 0x2] = MAXAGE_MPPT_TEMP;
  max_ages[MPPT2_BASE_ADDR | 0x3] = MAXAGE_MPPT_AUX_POWER;
  max_ages[MPPT2_BASE_ADDR | 0x4] = MAXAGE_MPPT_LIMITS;
  max_ages[MPPT2_BASE_ADDR | 0x5] = MAXAGE_MPPT_STATUS;
  max_ages[MPPT2_BASE_ADDR | 0x6] = MAXAGE_MPPT_POWER_CONN;

  max_ages[MPPT3_BASE_ADDR] = MAXAGE_MPPT_INPUT;
  max_ages[MPPT3_BASE_ADDR | 0x1] = MAXAGE_MPPT_OUTPUT;
  max_ages[MPPT3_BASE_ADDR | 0x2] = MAXAGE_MPPT_TEMP;
  max_ages[MPPT3_BASE_ADDR | 0x3] = MAXAGE_MPPT_AUX_POWER;
  max_ages[MPPT3_BASE_ADDR | 0x4] = MAXAGE_MPPT_LIMITS;
  max_ages[MPPT3_BASE_ADDR | 0x5] = MAXAGE_MPPT_STATUS;
  max_ages[MPPT3_BASE_ADDR | 0x6] = MAXAGE_MPPT_POWER_CONN;

  // init ages
  ages[BMS_BASE_ADDR] = INT32_MAX;
  ages[BMS_BASE_ADDR | 0x1] = INT32_MAX; // CMU1
  ages[BMS_BASE_ADDR | 0x2] = INT32_MAX; // CMU1
  ages[BMS_BASE_ADDR | 0x3] = INT32_MAX; // CMU1
  ages[BMS_BASE_ADDR | 0x4] = INT32_MAX; // CMU2
  ages[BMS_BASE_ADDR | 0x5] = INT32_MAX; // CMU2
  ages[BMS_BASE_ADDR | 0x6] = INT32_MAX; // CMU2
  ages[BMS_BASE_ADDR | 0x7] = INT32_MAX; // CMU3
  ages[BMS_BASE_ADDR | 0x8] = INT32_MAX; // CMU3
  ages[BMS_BASE_ADDR | 0x9] = INT32_MAX; // CMU3
  ages[BMS_BASE_ADDR | 0xF4] = INT32_MAX;
  ages[BMS_BASE_ADDR | 0xF5] = INT32_MAX;
  ages[BMS_BASE_ADDR | 0xF6] = INT32_MAX;
  ages[BMS_BASE_ADDR | 0xF7] = INT32_MAX;
  ages[BMS_BASE_ADDR | 0xF8] = INT32_MAX;
  ages[BMS_BASE_ADDR | 0xF9] = INT32_MAX;
  ages[BMS_BASE_ADDR | 0xFA] = INT32_MAX;
  ages[BMS_BASE_ADDR | 0xFB] = INT32_MAX;
  ages[BMS_BASE_ADDR | 0xFC] = INT32_MAX;
  ages[BMS_BASE_ADDR | 0xFD] = INT32_MAX;

  ages[MPPT1_BASE_ADDR] = INT32_MAX;
  ages[MPPT1_BASE_ADDR | 0x1] = INT32_MAX;
  ages[MPPT1_BASE_ADDR | 0x2] = INT32_MAX;
  ages[MPPT1_BASE_ADDR | 0x3] = INT32_MAX;
  ages[MPPT1_BASE_ADDR | 0x4] = INT32_MAX;
  ages[MPPT1_BASE_ADDR | 0x5] = INT32_MAX;
  ages[MPPT1_BASE_ADDR | 0x6] = INT32_MAX;

  ages[MPPT2_BASE_ADDR] = INT32_MAX;
  ages[MPPT2_BASE_ADDR | 0x1] = INT32_MAX;
  ages[MPPT2_BASE_ADDR | 0x2] = INT32_MAX;
  ages[MPPT2_BASE_ADDR | 0x3] = INT32_MAX;
  ages[MPPT2_BASE_ADDR | 0x4] = INT32_MAX;
  ages[MPPT2_BASE_ADDR | 0x5] = INT32_MAX;
  ages[MPPT2_BASE_ADDR | 0x6] = INT32_MAX;

  ages[MPPT3_BASE_ADDR] = INT32_MAX;
  ages[MPPT3_BASE_ADDR | 0x1] = INT32_MAX;
  ages[MPPT3_BASE_ADDR | 0x2] = INT32_MAX;
  ages[MPPT3_BASE_ADDR | 0x3] = INT32_MAX;
  ages[MPPT3_BASE_ADDR | 0x4] = INT32_MAX;
  ages[MPPT3_BASE_ADDR | 0x5] = INT32_MAX;
  ages[MPPT3_BASE_ADDR | 0x6] = INT32_MAX;
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

CANPacket CANBus::writePacket(uint16_t adr,
                              uint16_t data_u16_0, // LifeSign
                              uint8_t data_u8_2,   // Kp * 100
                              uint8_t data_u8_3,   // Ki * 100
                              uint8_t data_u8_4,   // Kd * 100
                              bool data_b_41,      // ConstantMode Speed/Power
                              bool force) {
  uint64_t data = 0UL;
  CANPacket packet = CANPacket(adr, data);
  packet.setData_u16(0, data_u16_0);
  packet.setData_u8(2, data_u8_2);
  packet.setData_u8(3, data_u8_3);
  packet.setData_u8(4, data_u8_4);
  packet.setData_b(41, data_b_41);
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

    spinlock_t foo;
    spinlock_initialize(&foo);
    taskENTER_CRITICAL(&foo);
    CAN.endPacket();
    taskEXIT_CRITICAL(&foo);
    yield(); // try to make up for busy wait in CRITICAL
    // xSemaphoreGive(mutex_out);
  } catch (exception &ex) {
    // xSemaphoreGive(mutex_out);
    console << "ERROR: Couldn not send uint64_t data to address " << adr << ", ex: " << ex.what() << NL;
  }
}

string CANBus::print_raw_packet(const string msg, CANPacket packet) {
  return fmt::format(
      "C{}-{}-[I:{:02d}|{:02d},O:{:02d}|{:02d}]={}=Id=0x{:03x}-data: {:016x} -- {:02x} - {:02x} - {:02x} - {:02x} - {:02x} - "
      "{:02x} - {:02x} - {:02x}",
      xPortGetCoreID(), esp_timer_get_time() / 1000000, availiblePacketsIn(), getMaxPacketsBufferInUsage(), availiblePacketsOut(),
      getMaxPacketsBufferOutUsage(), msg, packet.getId(), packet.getData_u64(), packet.getData_u8(7), packet.getData_u8(6),
      packet.getData_u8(5), packet.getData_u8(4), packet.getData_u8(3), packet.getData_u8(2), packet.getData_u8(1), packet.getData_u8(0));
}

void CANBus::task(void *pvParams) {
  deadCounter = 0;

  // reset CAN controller
  *((volatile uint32_t*)(0x3ff6b000 + 0x00 * 4)) |= 0x01;
  *((volatile uint32_t*)(0x3ff6b000 + 0x00 * 4)) &= 0xFE;

  while (1) {
    report_task_stack(this);
    if (SystemInited) {
      if (deadCounter == 0){
        deadCounter = millis() + 15e3;  // on boot: do not terminate for some time
      }
      if (counterR_notAvail > 8 || counterI_notAvail > 8 || counterW_notAvail > 8) {
        console << NL
                << fmt::format("CANBus REINIT trigger: I{}|{}, R{}|{}, W{}|{}", counterI_notAvail, counterI, counterR_notAvail, counterR,
                               counterW_notAvail, counterW)
                << NL;
        canBus.re_init();
      }

      if(verboseModeCanOutNative) console << "CAN 1" << NL;

      while (rxBufferIn.isAvailable()) {
        handle_rx_packet(rxBufferIn.pop());
        deadCounter = millis();  
        // deadCounter = max((uint64_t)deadCounter, millis());  
        // // only ever increment (prevent override of grace period at startup)
      }

      if(verboseModeCanOutNative) console << "CAN 2" << NL;

      uint bench = esp_timer_get_time();  // this might be an unacceptably long time to disable all and any context switches / ISR? -> log time
      while (rxBufferOut.isAvailable()) {
        write_rx_packet(rxBufferOut.pop());
      }

      if(verboseModeCanOutNative) console << "CAN 3: " << (uint)(esp_timer_get_time() - bench) << NL;

      if (millis() > deadCounter + 2e3) {
        console << "CAN presumed dead. Rebooting..." << NL;
        ESP.restart();
      }
    }
    taskSuspend();
  }
}
