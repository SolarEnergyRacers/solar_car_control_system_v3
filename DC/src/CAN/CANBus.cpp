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
extern CANBus canBus;
extern I2CBus i2cBus;

void onReceiveForwarder(int packetSize) { canBus.onReceive(packetSize); }

CANBus::CANBus() {
  packetsCountMax = 0;
  // init max ages
  max_ages[BMS_BASE_ADDR] = MAXAGE_BMU_HEARTBEAT;
  max_ages[BMS_BASE_ADDR | 0x1] = MAXAGE_CMU_TEMP;     // CMU1
  max_ages[BMS_BASE_ADDR | 0x4] = MAXAGE_CMU_TEMP;     // CMU2
  max_ages[BMS_BASE_ADDR | 0x7] = MAXAGE_CMU_TEMP;     // CMU3
  max_ages[BMS_BASE_ADDR | 0x2] = MAXAGE_CMU_VOLTAGES; // CMU1
  max_ages[BMS_BASE_ADDR | 0x3] = MAXAGE_CMU_VOLTAGES; // CMU1
  max_ages[BMS_BASE_ADDR | 0x5] = MAXAGE_CMU_VOLTAGES; // CMU2
  max_ages[BMS_BASE_ADDR | 0x6] = MAXAGE_CMU_VOLTAGES; // CMU2
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
  ages[BMS_BASE_ADDR | 0x4] = INT32_MAX; // CMU2
  ages[BMS_BASE_ADDR | 0x7] = INT32_MAX; // CMU3
  ages[BMS_BASE_ADDR | 0x2] = INT32_MAX; // CMU1
  ages[BMS_BASE_ADDR | 0x3] = INT32_MAX; // CMU1
  ages[BMS_BASE_ADDR | 0x5] = INT32_MAX; // CMU2
  ages[BMS_BASE_ADDR | 0x6] = INT32_MAX; // CMU2
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
    xSemaphoreGive(mutex);
    console << fmt::format("     ERROR: CANBus with rx={}, tx={} NOT, speed={} inited.\n", CAN_RX, CAN_TX, CAN_SPEED);
    hasError = true;
  } else {
    xSemaphoreGive(mutex);
    console << fmt::format("     CANBus with rx={}, tx={}, speed={} inited.\n", CAN_RX, CAN_TX, CAN_SPEED);
  }
  CAN.onReceive(onReceiveForwarder); // couldn't get it to work with method of object

  return fmt::format("[{}] CANBus initialized.", hasError ? "--" : "ok");
}

void CANBus::exit() {
  // Exit needs to be implemented for Task, here or in Abstract_task
  xSemaphoreTakeT(mutex);
  CAN.end();
  xSemaphoreGive(mutex);
}

int CANBus::availiblePackets() { return rxBuffer.getSize(); }

void CANBus::onReceive(int packetSize) {

  xSemaphoreTakeT(mutex);
  int packetId = CAN.packetId();
  xSemaphoreGive(mutex);

  if (packetId == DC_BASE_ADDR || packetId == AC_BASE_ADDR || max_ages[packetId] == 0 ||
      (max_ages[packetId] != -1 && millis() - ages[packetId] > max_ages[packetId])) {

    ages[packetId] = millis();

    CANPacket packet;
    uint64_t rxData = 0;
    xSemaphoreTakeT(mutex);
    for (int i = 0; i < packetSize; i++) {
      if (CAN.available()) {
        rxData = rxData | (((uint64_t)CAN.read()) << (i * 8));
      }
    }
    xSemaphoreGive(mutex);

    packet.setId(packetId);
    packet.setData(rxData);

    rxBuffer.push(packet);
    if (packetsCountMax < availiblePackets())
      packetsCountMax = availiblePackets();
  }
}

bool CANBus::writePacket(uint16_t adr, uint16_t data0, uint16_t data1, uint16_t data2, uint16_t data3) {
  try {
    CANPacket packet = CANPacket(adr, data0, data1, data2, data3);
    return writePacket(adr, packet);
  } catch (exception &ex) {
    console << "ERROR: Couldn not send uint64_t data to address " << adr << NL;
    return false;
  }
  return true;
}

bool CANBus::writePacket(uint16_t adr, uint32_t data0, uint32_t data1) {
  try {
    CANPacket packet = CANPacket(adr, data0, data1);
    return writePacket(adr, packet);
  } catch (exception &ex) {
    console << "ERROR: Couldn not send uint64_t data to address " << adr << NL;
    return false;
  }
  return true;
}

bool CANBus::writePacket(uint16_t adr, uint64_t data) {
  try {
    CANPacket packet = CANPacket(adr, data);
    return writePacket(adr, packet);
  } catch (exception &ex) {
    console << "ERROR: Couldn not send uint64_t data to address " << adr << NL;
    return false;
  }
}

bool CANBus::writePacket(uint16_t adr, CANPacket packet) {
  try {
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
    console << "ERROR: Couldn not send data to address " << adr << NL;
    return false;
  }
  return true;
}

void CANBus::task() {
  CANPacket packet;

  while (1) {
    // Handle recieved message with CANBus
    while (rxBuffer.isAvailable()) {
      packet = rxBuffer.pop();

      handle_rx_packet(packet);
    }

    vTaskDelay(sleep_polling_ms / portTICK_PERIOD_MS);
  }
}

int CANBus::handle_rx_packet(CANPacket packet) {
  int retValue = 0;
  int packetId = packet.getId();
  // Do something with packet
  switch (packetId) {
  case AC_BASE_ADDR:
    carState.Speed = (int)(packet.getData_ui64() / 10);
    if (canBus.verboseModeCan)
      console << fmt::format("[{:02d}|{:02d}] CAN.PacketId=0x{:03x}-data=0x{:x}", canBus.availiblePackets(),
                             canBus.getMaxPacketsBufferUsage(), packetId, packet.getData_ui64())
              << NL;
    break;
  case DC_BASE_ADDR:
    if (canBus.verboseModeCan)
      console << fmt::format("[{:02d}|{:02d}] CAN.PacketId=0x{:03x}-data=0x{:x}", canBus.availiblePackets(),
                             canBus.getMaxPacketsBufferUsage(), packetId, packet.getData_ui64())
              << NL;
    break;
  case BMS_BASE_ADDR:
    // heartbeat packet.getData_ui32(0)
    break;
  case BMS_BASE_ADDR | 0xFA:
    carState.BatteryVoltage = (float)packet.getData_ui32(0) / 1000.0;
    carState.Uavg = carState.BatteryVoltage / 28.; // 28 cells in serie
    carState.BatteryCurrent = (float)packet.getData_i32(1) / 1000.0;
    // Battery Voltage mV packet.getData_ui32(0)
    // Battery Current mA packet.getData_i32(1)
    if (verboseModeCan) {
      console << ", Uavg=" << carState.Uavg << ", BatVolt=" << carState.BatteryVoltage << ", BatCur=" << carState.BatteryCurrent
              << fmt::format(", raw: 0x{:08x}", packet.getData_ui64()) << NL;
    }
    break;
  case BMS_BASE_ADDR | 0xF8:
    carState.Umin = packet.getData_ui16(0) / 1000.;
    carState.Umax = packet.getData_ui16(1) / 1000.;
    // Battery min Cell Voltage mV packet.getData_ui16(0)
    // Battery max Cell Voltage mV packet.getData_ui16(1)
    // CMU number with min Cell Voltage packet.getData_ui8(4)
    // Cell number with min Voltage packet.getData_ui8(5)
    // CMU number with max Cell Voltage packet.getData_ui8(6)
    // Cell number with max Voltage packet.getData_ui8(7)
    if (verboseModeCan) {
      console << ", Umin=" << carState.Umin << ", Umax=" << carState.Umax << NL;
    }
    break;
  case BMS_BASE_ADDR | 0xF7:
    // Contactor 1 driver error packet.getData_b(0)
    // Contactor 2 driver error packet.getData_b(1)
    // Contactor 1 driver output status packet.getData_b(2)
    // Contactor 2 driver output status packet.getData_b(3)
    // 12V contactor supply OK packet.getData_b(4)
    // Contactor 3 driver error packet.getData_b(5)
    // Contactor 3 driver output status packet.getData_b(6)

    // Precharge State packet.getData_ui8(1)
    /*
        0=Error
        1=Idle
        2=Measure
        3=Precharge
        4=Run
        5=Enable Pack
    */
    switch (packet.getData_ui8(1)) {
    case 0:
      carState.PrechargeState = PRECHARGE_STATE::ERROR;
      break;
    case 1:
      carState.PrechargeState = PRECHARGE_STATE::IDLE;
      break;
    case 2:
      carState.PrechargeState = PRECHARGE_STATE::MEASURE;
      break;
    case 3:
      carState.PrechargeState = PRECHARGE_STATE::PRECHARGE;
      break;
    case 4:
      carState.PrechargeState = PRECHARGE_STATE::RUN;
      break;
    case 5:
      carState.PrechargeState = PRECHARGE_STATE::ENABLE_PACK;
      break;
    }
    if (verboseModeCan) {
      console << ", PrechargeState=" << PRECHARGE_STATE_str[(int)(carState.PrechargeState)] << NL;
    }

    // Precharge Timer info also available
    break;
  case BMS_BASE_ADDR | 0xF9:
    carState.Tmin = packet.getData_ui16(0) / 10.;
    carState.Tmax = packet.getData_ui16(1) / 10.;
    if (verboseModeCan) {
      console << ", Bat Tmin=" << carState.Tmin << ", Bat Tmax=" << carState.Tmax << NL;
    }
    break;

  case BMS_BASE_ADDR | 0xFD:

    carState.BatteryErrors.clear();

    if (packet.getData_ui32(0) > 0) { // Saving CPU time in case there are no errors
      for (int i = 0; i < 13; i++) {
        if (packet.getData_b(i)) {
          carState.BatteryErrors.push_front(static_cast<BATTERY_ERROR>(i));
          if (verboseModeCan) {
            console << ", BatErrors=" << carState.batteryErrorsAsString(true) << NL;
          }
        }
      }
    }

    // Cell over voltage packet.getData_b(0)
    // Cell under voltage packet.getData_b(1)
    // Cell over Temp packet.getData_b(2)
    // Measurement untrusted packet.getData_b(3)

    // CMU Comm Timeout packet.getData_b(4)
    // Vehicle Comm Timeout packet.getData_b(5)
    // BMU in Setup Mode packet.getData_b(6)
    // CMU CAN Power Status packet.getData_b(7)

    // Pack isolation test fail packet.getData_b(8)
    // SOC measurement invalid packet.getData_b(9)
    // CAN 12V in low, about to shutdown packet.getData_b(10)
    // Contactor Stuck/Not engaged packet.getData_b(11)

    // Extra Cell present packet.getData_b(12)
    break;

  case MPPT1_BASE_ADDR | 0x1:
    carState.Mppt1Current = packet.getData_f32(1);
    carState.PhotoVoltaicCurrent = carState.Mppt1Current + carState.Mppt2Current + carState.Mppt3Current;

    // MPPT1 Output Voltage V packet.getData_f32(0)
    // MPPT1 Output Current A packet.getData_f32(1)
    if (verboseModeCan) {
      console << ", Mppt1Cur=" << carState.Mppt1Current << NL;
    }

    break;
  case MPPT2_BASE_ADDR | 0x1:
    carState.Mppt2Current = packet.getData_f32(1);
    carState.PhotoVoltaicCurrent = carState.Mppt1Current + carState.Mppt2Current + carState.Mppt3Current;

    // MPPT2 Output Voltage V packet.getData_f32(0)
    // MPPT2 Output Current A packet.getData_f32(1)
    if (verboseModeCan) {
      console << ", Mppt2Cur=" << carState.Mppt2Current << NL;
    }

    break;
  case MPPT3_BASE_ADDR | 0x1:
    carState.Mppt3Current = packet.getData_f32(1);
    carState.PhotoVoltaicCurrent = carState.Mppt1Current + carState.Mppt2Current + carState.Mppt3Current;

    // MPPT3 Output Voltage V packet.getData_f32(0)
    // MPPT3 Output Current A packet.getData_f32(1)
    if (verboseModeCan) {
      console << ", Mppt3Cur=" << carState.Mppt3Current << NL;
    }
    break;
  case MPPT1_BASE_ADDR | 0x2:
    carState.T1 = packet.getData_f32(0);
    if (verboseModeCan) {
      console << "T1=" << carState.T1 << NL;
    }
    break;
  case MPPT2_BASE_ADDR | 0x2:
    carState.T2 = packet.getData_f32(0);
    if (verboseModeCan) {
      console << "T2=" << carState.T2 << NL;
    }
    break;
  case MPPT3_BASE_ADDR | 0x2:
    carState.T3 = packet.getData_f32(0);
    if (verboseModeCan) {
      console << "T3=" << carState.T3 << NL;
    }
  }
  return retValue;
}
