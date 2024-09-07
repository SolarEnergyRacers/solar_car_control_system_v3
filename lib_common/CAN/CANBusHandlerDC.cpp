//
// CAN Bus
//
#ifndef DCMODE
#pragma message "DC-Mode not compiled" 
#else
#pragma message "Compiling for DC-Mode" 

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

extern CarState carState;
extern Console console;
extern CANBus canBus;

bool CANBus::is_to_ignore_packet(uint16_t packetId) {
  return packetId != (DC_BASE_ADDR | 0x00) && packetId != (DC_BASE_ADDR | 0x01) && !canBus.isPacketToRenew(packetId);
}

void CANBus::handle_rx_packet(CANPacket packet) {
  int packetId = packet.getId();
  if (packetId == 0)
    return;
  counterR++;
  
  if (canBus.verboseModeCanInNative)
    console << print_raw_packet("R", packet) << NL;
  
  switch (packetId) {
  case AC_BASE_ADDR | 0x00:
    carState.LifeSign = packet.getData_u16(0);
    carState.Kp = (double)packet.getData_u8(2) / 10.;
    carState.Ki = (double)packet.getData_u8(3) / 10.;
    carState.Kd = (double)packet.getData_u8(4) / 10.;
    carState.ConstantMode = packet.getData_b(41) ? CONSTANT_MODE::SPEED : CONSTANT_MODE::POWER;
    carState.ConfirmDriverInfo = packet.getData_b(42);
    if (canBus.verboseModeCanIn)
      console << fmt::format("LifeSign= {:4x}, Kp={5.2f}, Ki={5.2f}, Kd={5.2f} carState.ConstantMode={}\n", carState.LifeSign, carState.Kp,
                             carState.Ki, carState.Kd, CONSTANT_MODE_str[(int)(carState.ConstantMode)]);
    break;

  case DC_BASE_ADDR:
    break;
  case BMS_BASE_ADDR:
    // heartbeat packet.getData_ui32(0)
    break;
  case BMS_BASE_ADDR | 0xFA:
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
  case BMS_BASE_ADDR | 0xF8:
    carState.Umin = packet.getData_u16(0) / 1000.;
    carState.Umax = packet.getData_u16(1) / 1000.;
    // Battery min Cell Voltage mV packet.getData_u16(0)
    // Battery max Cell Voltage mV packet.getData_u16(1)
    // CMU number with min Cell Voltage packet.getData_ui8(4)
    // Cell number with min Voltage packet.getData_ui8(5)
    // CMU number with max Cell Voltage packet.getData_ui8(6)
    // Cell number with max Voltage packet.getData_ui8(7)
    if (verboseModeCanIn) {
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
  case BMS_BASE_ADDR | 0xF9:
    carState.Tmin = packet.getData_u16(0) / 10.;
    carState.Tmax = packet.getData_u16(1) / 10.;
    if (verboseModeCanIn) {
      console << ", Bat Tmin=" << carState.Tmin << ", Bat Tmax=" << carState.Tmax << NL;
    }
    break;

  case BMS_BASE_ADDR | 0xFD:
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

  case MPPT1_BASE_ADDR | 0x1:
    carState.Mppt1Current = packet.getData_f32(1);
    carState.PhotoVoltaicCurrent = carState.Mppt1Current + carState.Mppt2Current + carState.Mppt3Current;

    // MPPT1 Output Voltage V packet.getData_f32(0)
    // MPPT1 Output Current A packet.getData_f32(1)
    if (verboseModeCanIn) {
      console << ", Mppt1Cur=" << carState.Mppt1Current << NL;
    }

    break;
  case MPPT2_BASE_ADDR | 0x1:
    carState.Mppt2Current = packet.getData_f32(1);
    carState.PhotoVoltaicCurrent = carState.Mppt1Current + carState.Mppt2Current + carState.Mppt3Current;

    // MPPT2 Output Voltage V packet.getData_f32(0)
    // MPPT2 Output Current A packet.getData_f32(1)
    if (verboseModeCanIn) {
      console << ", Mppt2Cur=" << carState.Mppt2Current << NL;
    }

    break;
  case MPPT3_BASE_ADDR | 0x1:
    carState.Mppt3Current = packet.getData_f32(1);
    carState.PhotoVoltaicCurrent = carState.Mppt1Current + carState.Mppt2Current + carState.Mppt3Current;

    // MPPT3 Output Voltage V packet.getData_f32(0)
    // MPPT3 Output Current A packet.getData_f32(1)
    if (verboseModeCanIn) {
      console << ", Mppt3Cur=" << carState.Mppt3Current << NL;
    }
    break;
  case MPPT1_BASE_ADDR | 0x2:
    carState.T1 = packet.getData_f32(0);
    if (verboseModeCanIn) {
      console << "T1=" << carState.T1 << NL;
    }
    break;
  case MPPT2_BASE_ADDR | 0x2:
    carState.T2 = packet.getData_f32(0);
    if (verboseModeCanIn) {
      console << "T2=" << carState.T2 << NL;
    }
    break;
  case MPPT3_BASE_ADDR | 0x2:
    carState.T3 = packet.getData_f32(0);
    if (verboseModeCanIn) {
      console << "T3=" << carState.T3 << NL;
    }
  }
}

#endif
