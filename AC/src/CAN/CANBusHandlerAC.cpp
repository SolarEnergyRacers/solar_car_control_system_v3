//
// CAN Bus
//
#include <fmt/core.h>

#include <definitions.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <algorithm>
#include <iostream>
#include <list>

#include <Arduino.h>
#include <CAN.h>

#include <CANBus.h>
#include <CarState.h>
#include <CarStateRadio.h>
#include <Console.h>
#include <Helper.h>

extern CarState carState;
extern CarStateRadio carStateRadio;
extern Console console;
extern CANBus canBus;

bool CANBus::is_to_ignore_packet(uint16_t packetId) {
  return packetId != (DC_BASE0x00) && packetId != (DC_BASE0x01) && !canBus.isPacketToRenew(packetId);
}

void CANBus::init_ages() {
  // init max ages
  max_ages[BmsBase0x00] = MAXAGE_BMU_HEARTBEAT;
  max_ages[BmsBase0x01] = MAXAGE_CMU_TEMP;     // CMU1
  max_ages[BmsBase0x02] = MAXAGE_CMU_VOLTAGES; // CMU1
  max_ages[BmsBase0x03] = MAXAGE_CMU_VOLTAGES; // CMU1
  max_ages[BmsBase0x04] = MAXAGE_CMU_TEMP;     // CMU2
  max_ages[BmsBase0x05] = MAXAGE_CMU_VOLTAGES; // CMU2
  max_ages[BmsBase0x06] = MAXAGE_CMU_VOLTAGES; // CMU2
  max_ages[BmsBase0x07] = MAXAGE_CMU_TEMP;     // CMU3
  max_ages[BmsBase0x00] = MAXAGE_CMU_VOLTAGES; // CMU3
  max_ages[BmsBase0x09] = MAXAGE_CMU_VOLTAGES; // CMU3
  max_ages[BmsBase0xF4] = MAXAGE_PACK_SOC;
  max_ages[BmsBase0xF5] = MAXAGE_BALANCE_SOC;
  max_ages[BmsBase0xF6] = MAXAGE_CHARGER_CONTROL;
  max_ages[BmsBase0xF7] = MAXAGE_PRECHARGE_STATUS;
  max_ages[BmsBase0xF8] = MAXAGE_MIN_MAX_U_CELL;
  max_ages[BmsBase0xF9] = MAXAGE_MIN_MAX_T_CELL;
  max_ages[BmsBase0xFA] = MAXAGE_PACK_VOLTAGE;
  max_ages[BmsBase0xFB] = MAXAGE_PACK_STATUS;
  max_ages[BmsBase0xFC] = MAXAGE_PACK_FAN_STATUS;
  max_ages[BmsBase0xFD] = MAXAGE_EXT_PACK_STATUS;

  max_ages[Mppt1Base0x00] = MAXAGE_MPPT_INPUT;
  max_ages[Mppt1Base0x01] = MAXAGE_MPPT_OUTPUT;
  max_ages[Mppt1Base0x02] = MAXAGE_MPPT_TEMP;
  max_ages[Mppt1Base0x03] = MAXAGE_MPPT_AUX_POWER;
  max_ages[Mppt1Base0x04] = MAXAGE_MPPT_LIMITS;
  max_ages[Mppt1Base0x05] = MAXAGE_MPPT_STATUS;
  max_ages[Mppt1Base0x06] = MAXAGE_MPPT_POWER_CONN;

  max_ages[Mppt2Base0x00] = MAXAGE_MPPT_INPUT;
  max_ages[Mppt2Base0x01] = MAXAGE_MPPT_OUTPUT;
  max_ages[Mppt2Base0x02] = MAXAGE_MPPT_TEMP;
  max_ages[Mppt2Base0x03] = MAXAGE_MPPT_AUX_POWER;
  max_ages[Mppt2Base0x04] = MAXAGE_MPPT_LIMITS;
  max_ages[Mppt2Base0x05] = MAXAGE_MPPT_STATUS;
  max_ages[Mppt2Base0x06] = MAXAGE_MPPT_POWER_CONN;

  max_ages[Mppt3Base0x00] = MAXAGE_MPPT_INPUT;
  max_ages[Mppt3Base0x01] = MAXAGE_MPPT_OUTPUT;
  max_ages[Mppt3Base0x02] = MAXAGE_MPPT_TEMP;
  max_ages[Mppt3Base0x03] = MAXAGE_MPPT_AUX_POWER;
  max_ages[Mppt3Base0x04] = MAXAGE_MPPT_LIMITS;
  max_ages[Mppt3Base0x05] = MAXAGE_MPPT_STATUS;
  max_ages[Mppt3Base0x06] = MAXAGE_MPPT_POWER_CONN;

  // init ages
  ages[BmsBase0x00] = INT32_MAX;
  ages[BmsBase0x01] = INT32_MAX; // CMU1
  ages[BmsBase0x04] = INT32_MAX; // CMU2
  ages[BmsBase0x07] = INT32_MAX; // CMU3
  ages[BmsBase0x02] = INT32_MAX; // CMU1
  ages[BmsBase0x03] = INT32_MAX; // CMU1
  ages[BmsBase0x05] = INT32_MAX; // CMU2
  ages[BmsBase0x06] = INT32_MAX; // CMU2
  ages[BmsBase0x08] = INT32_MAX; // CMU3
  ages[BmsBase0x09] = INT32_MAX; // CMU3
  ages[BmsBase0xF4] = INT32_MAX;
  ages[BmsBase0xF5] = INT32_MAX;
  ages[BmsBase0xF6] = INT32_MAX;
  ages[BmsBase0xF7] = INT32_MAX;
  ages[BmsBase0xF8] = INT32_MAX;
  ages[BmsBase0xF9] = INT32_MAX;
  ages[BmsBase0xFA] = INT32_MAX;
  ages[BmsBase0xFB] = INT32_MAX;
  ages[BmsBase0xFC] = INT32_MAX;
  ages[BmsBase0xFD] = INT32_MAX;

  ages[Mppt1Base0x00] = INT32_MAX;
  ages[Mppt1Base0x01] = INT32_MAX;
  ages[Mppt1Base0x02] = INT32_MAX;
  ages[Mppt1Base0x03] = INT32_MAX;
  ages[Mppt1Base0x04] = INT32_MAX;
  ages[Mppt1Base0x05] = INT32_MAX;
  ages[Mppt1Base0x06] = INT32_MAX;

  ages[Mppt2Base0x00] = INT32_MAX;
  ages[Mppt2Base0x01] = INT32_MAX;
  ages[Mppt2Base0x02] = INT32_MAX;
  ages[Mppt2Base0x03] = INT32_MAX;
  ages[Mppt2Base0x04] = INT32_MAX;
  ages[Mppt2Base0x05] = INT32_MAX;
  ages[Mppt2Base0x06] = INT32_MAX;

  ages[Mppt3Base0x00] = INT32_MAX;
  ages[Mppt3Base0x01] = INT32_MAX;
  ages[Mppt3Base0x02] = INT32_MAX;
  ages[Mppt3Base0x03] = INT32_MAX;
  ages[Mppt3Base0x04] = INT32_MAX;
  ages[Mppt3Base0x05] = INT32_MAX;
  ages[Mppt3Base0x06] = INT32_MAX;
}

void CANBus::handle_rx_packet(CANPacket packet) {
  uint16_t packetId = packet.getId();
  if (packetId == 0)
    return;
  counterR++;
  if (canBus.verboseModeCanInNative)
    console << print_raw_packet("R", packet) << NL;

  carStateRadio.push_if_radio_packet(packetId, packet);

  switch (packetId) {
  case DC_BASE0x00:
    carState.LifeSign = packet.getData_u16(0);
    carState.Potentiometer = packet.getData_u16(1);
    carState.Acceleration = packet.getData_u16(2);
    carState.Deceleration = packet.getData_u16(3);
    if (verboseModeCanIn)
      console << fmt::format("[{:02d}|{:02d}] CAN.PacketId=0x{:03x}-R-data:lifeSign={:4x}, speed={:4x}, decl={:4x}, accl={:4x}, poti={:4x}",
                             availiblePacketsIn(), getMaxPacketsBufferInUsage(), packetId | 0x00, carState.LifeSign, carState.Speed,
                             carState.Deceleration, carState.Acceleration, carState.Potentiometer)
              << NL;
    break;

  case DC_BASE0x01: {
    carState.TargetSpeed = (float)packet.getData_u16(0);
    carState.TargetPower = (float)packet.getData_u16(1) / 1000.;
    carState.AccelerationDisplay = packet.getData_i8(4);
    carState.Speed = packet.getData_u8(6);
    int driveDirection = packet.getData_b(56);
    carState.DriveDirection = driveDirection == 1 ? DRIVE_DIRECTION::FORWARD : DRIVE_DIRECTION::BACKWARD;
    carState.BreakPedal = packet.getData_b(57);
    carState.MotorOn = packet.getData_b(58);
    carState.ConstantModeOn = packet.getData_b(59);
  }
    if (verboseModeCanIn)
      console << fmt::format(
                     "[{:02d}|{:02d}] CAN.PacketId=0x{:03x}-R-data:targetSpeed={:3}, targetPower={:3}, speed={:3d}, "
                     "accelDispl={:3d}, constMode={:5s}({}), direction={}, breakPedal={}, MotorOn={}",
                     availiblePacketsIn(), getMaxPacketsBufferInUsage(), packetId | 0x01, carState.TargetSpeed, carState.TargetPower,
                     carState.Speed, carState.AccelerationDisplay, CONSTANT_MODE_str[(int)(carState.ConstantMode)], carState.ConstantModeOn,
                     DRIVE_DIRECTION_str[(int)(carState.DriveDirection)], carState.BreakPedal, carState.MotorOn, carState.ConstantModeOn)
              << NL;
    break;
  // case DC_BASE0x02:
  //   break;
  case BmsBase0x00:
    // heartbeat packet.getData_ui32(0)
    break;
  case BmsBase0xFA:
    carState.BatteryVoltage = (float)packet.getData_u32(0) / 1000.0;
    carState.Uavg = carState.BatteryVoltage / 28.; // 28 cells in serie
    carState.BatteryCurrent = (float)packet.getData_i32(1) / 1000.0;
    // Battery Voltage mV packet.getData_ui32(0)
    // Battery Current mA packet.getData_i32(1)
    if (verboseModeCanIn) {
      console << ", Uavg=" << carState.Uavg << ", BatVolt=" << carState.BatteryVoltage << ", BatCur=" << carState.BatteryCurrent
              << fmt::format(", raw: 0x{:08x}", packet.getData_u64()) << NL;
    }
    break;
  case BmsBase0xF8:
    carState.Umin = packet.getData_u16(0) / 1000.;
    carState.Umax = packet.getData_u16(1) / 1000.;
    // Battery min Cell Voltage mV packet.getData_u16(0)
    // Battery max Cell Voltage mV packet.getData_u16(1)
    // CMU number with min Cell Voltage packet.getData_u8(4)
    // Cell number with min Voltage packet.getData_u8(5)
    // CMU number with max Cell Voltage packet.getData_u8(6)
    // Cell number with max Voltage packet.getData_u8(7)
    if (verboseModeCanIn) {
      console << ", Umin=" << carState.Umin << ", Umax=" << carState.Umax << NL;
    }
    break;
  case BmsBase0xF7:
    // Contactor 1 driver error packet.getData_b(0)
    // Contactor 2 driver error packet.getData_b(1)
    // Contactor 1 driver output status packet.getData_b(2)
    // Contactor 2 driver output status packet.getData_b(3)
    // 12V contactor supply OK packet.getData_b(4)
    // Contactor 3 driver error packet.getData_b(5)
    // Contactor 3 driver output status packet.getData_b(6)

    // Precharge State packet.getData_u8(1)
    /*
        0=Error
        1=Idle
        2=Measure
        3=Precharge
        4=Run
        5=Enable Pack
    */
    switch (packet.getData_u8(1)) {
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
    if (verboseModeCanIn) {
      console << ", PrechargeState=" << PRECHARGE_STATE_str[(int)(carState.PrechargeState)] << NL;
    }

    // Precharge Timer info also available
    break;
  case BmsBase0xF9:
    carState.Tmin = packet.getData_u16(0) / 10.;
    carState.Tmax = packet.getData_u16(1) / 10.;
    if (verboseModeCanIn) {
      console << ", Bat Tmin=" << carState.Tmin << ", Bat Tmax=" << carState.Tmax << NL;
    }
    break;

  case BmsBase0xFD:
    carState.BatteryErrors.clear();
    if (packet.getData_u32(0) > 0) { // Saving CPU time in case there are no errors
      for (int i = 0; i < 13; i++) {
        if (packet.getData_b(i)) {
          carState.BatteryErrors.push_front(static_cast<BATTERY_ERROR>(i));
          if (verboseModeCanIn) {
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

  case Mppt1Base0x01:
    carState.Mppt1Current = packet.getData_f32(1);
    carState.PhotoVoltaicCurrent = carState.Mppt1Current + carState.Mppt2Current + carState.Mppt3Current;

    // MPPT1 Output Voltage V packet.getData_f32(0)
    // MPPT1 Output Current A packet.getData_f32(1)
    if (verboseModeCanIn) {
      console << ", Mppt1Cur=" << carState.Mppt1Current << NL;
    }

    break;
  case Mppt2Base0x01:
    carState.Mppt2Current = packet.getData_f32(1);
    carState.PhotoVoltaicCurrent = carState.Mppt1Current + carState.Mppt2Current + carState.Mppt3Current;

    // MPPT2 Output Voltage V packet.getData_f32(0)
    // MPPT2 Output Current A packet.getData_f32(1)
    if (verboseModeCanIn) {
      console << ", Mppt2Cur=" << carState.Mppt2Current << NL;
    }

    break;
  case Mppt3Base0x01:
    carState.Mppt3Current = packet.getData_f32(1);
    carState.PhotoVoltaicCurrent = carState.Mppt1Current + carState.Mppt2Current + carState.Mppt3Current;

    // MPPT3 Output Voltage V packet.getData_f32(0)
    // MPPT3 Output Current A packet.getData_f32(1)
    if (verboseModeCanIn) {
      console << ", Mppt3Cur=" << carState.Mppt3Current << NL;
    }
    break;
  case Mppt1Base0x02:
    carState.T1 = packet.getData_f32(0);
    if (verboseModeCanIn) {
      console << "T1=" << carState.T1 << NL;
    }
    break;
  case Mppt2Base0x02:
    carState.T2 = packet.getData_f32(0);
    if (verboseModeCanIn) {
      console << "T2=" << carState.T2 << NL;
    }
    break;
  case Mppt3Base0x02:
    carState.T3 = packet.getData_f32(0);
    if (verboseModeCanIn) {
      console << "T3=" << carState.T3 << NL;
    }
  }
}
