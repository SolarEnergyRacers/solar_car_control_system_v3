//
// CAN Bus
// https://github.com/sandeepmistry/arduino-CAN
// --> https://github.com/SolarEnergyRacers/SER-arduino-CAN
//
#ifndef SOLAR_CAR_CONTROL_SYSTEM_CANBUS_H
#define SOLAR_CAR_CONTROL_SYSTEM_CANBUS_H

#include <map>

#include <AbstractTask.h>
#include <CANRxBuffer.h>
// #include <CarState.h>

// all handled CAN addresses:
#define AC_BASE0x00 AC_BASE_ADDR | 0x00 // AC: LifeSign, switch constant mode Speed / Power, Kp,  Ki

#define BmsBase0x00 BMS_BASE_ADDR | 0x00 // BMU_HEARTBEAT
#define BmsBase0x01 BMS_BASE_ADDR | 0x01 // CMU_TEMP_CMU1
#define BmsBase0x02 BMS_BASE_ADDR | 0x02 // CMU_TEMP_CMU2
#define BmsBase0x03 BMS_BASE_ADDR | 0x03 // CMU_TEMP_CMU3
#define BmsBase0x04 BMS_BASE_ADDR | 0x04 // CMU_VOLTAGES_CMU1
#define BmsBase0x05 BMS_BASE_ADDR | 0x05 // CMU_VOLTAGES_CMU1
#define BmsBase0x06 BMS_BASE_ADDR | 0x06 // CMU_VOLTAGES_CMU2
#define BmsBase0x07 BMS_BASE_ADDR | 0x07 // CMU_VOLTAGES_CMU2
#define BmsBase0x08 BMS_BASE_ADDR | 0x08 // CMU_VOLTAGES_CMU3
#define BmsBase0x09 BMS_BASE_ADDR | 0x09 // CMU_VOLTAGES_CMU3
#define BmsBase0xF4 BMS_BASE_ADDR | 0xF4 // PACK_SOC
#define BmsBase0xF5 BMS_BASE_ADDR | 0xF5 // BALANCE_SOC
#define BmsBase0xF6 BMS_BASE_ADDR | 0xF6 // CHARGER_CONTROL
#define BmsBase0xF7 BMS_BASE_ADDR | 0xF7 // PRECHARGE_STATUS
#define BmsBase0xF8 BMS_BASE_ADDR | 0xF8 // MIN_MAX_U_CELL
#define BmsBase0xF9 BMS_BASE_ADDR | 0xF9 // MIN_MAX_T_CELL
#define BmsBase0xFA BMS_BASE_ADDR | 0xFA // PACK_VOLTAGE
#define BmsBase0xFB BMS_BASE_ADDR | 0xFB // PACK_STATUS
#define BmsBase0xFC BMS_BASE_ADDR | 0xFC // PACK_FAN_STATUS
#define BmsBase0xFD BMS_BASE_ADDR | 0xFD // EXT_PACK_STATUS

#define DC_BASE0x00 DC_BASE_ADDR | 0x00 // DC: LifeSign, Potentiometer, Acceleration, Deceleration
#define DC_BASE0x01                                                                                                                        \
  DC_BASE_ADDR | 0x01 // DC: TargetSpeed, TargetPower, AccelerationDisplay,
                      //     Speed, Direction, DriveDirection, BreakPedal,
                      //     MotorOn, ConstantModeOn

#define Mppt1Base0x00 MPPT1_BASE_ADDR | 0x00 // MPPT_INPUT
#define Mppt1Base0x01 MPPT1_BASE_ADDR | 0x01 // MPPT_OUTPUT
#define Mppt1Base0x02 MPPT1_BASE_ADDR | 0x02 // MPPT_TEMP
#define Mppt1Base0x03 MPPT1_BASE_ADDR | 0x03 // MPPT_AUX_POWER
#define Mppt1Base0x04 MPPT1_BASE_ADDR | 0x04 // MPPT_LIMITS
#define Mppt1Base0x05 MPPT1_BASE_ADDR | 0x05 // MPPT_STATUS
#define Mppt1Base0x06 MPPT1_BASE_ADDR | 0x06 // MPPT_POWER_CONN

#define Mppt2Base0x00 MPPT2_BASE_ADDR | 0x00 // MPPT_INPUT
#define Mppt2Base0x01 MPPT2_BASE_ADDR | 0x01 // MPPT_OUTPUT
#define Mppt2Base0x02 MPPT2_BASE_ADDR | 0x02 // MPPT_TEMP
#define Mppt2Base0x03 MPPT2_BASE_ADDR | 0x03 // MPPT_AUX_POWER
#define Mppt2Base0x04 MPPT2_BASE_ADDR | 0x04 // MPPT_LIMITS
#define Mppt2Base0x05 MPPT2_BASE_ADDR | 0x05 // MPPT_STATUS
#define Mppt2Base0x06 MPPT2_BASE_ADDR | 0x06 // MPPT_POWER_CONN

#define Mppt3Base0x00 MPPT3_BASE_ADDR | 0x00 // MPPT_INPUT
#define Mppt3Base0x01 MPPT3_BASE_ADDR | 0x01 // MPPT_OUTPUT
#define Mppt3Base0x02 MPPT3_BASE_ADDR | 0x02 // MPPT_TEMP
#define Mppt3Base0x03 MPPT3_BASE_ADDR | 0x03 // MPPT_AUX_POWER
#define Mppt3Base0x04 MPPT3_BASE_ADDR | 0x04 // MPPT_LIMITS
#define Mppt3Base0x05 MPPT3_BASE_ADDR | 0x05 // MPPT_STATUS
#define Mppt3Base0x06 MPPT3_BASE_ADDR | 0x06 // MPPT_POWER_CONN

#define McBase0x00 MC_BASE_ADDR | 0x09 // ERPM, Current, Duty Cycle
#define McBase0x0e MC_BASE_ADDR | 0x0e // Ah Used, Ah Charged
#define McBase0x0f MC_BASE_ADDR | 0x0f // Wh Used, Wh Charged
#define McBase0x10 MC_BASE_ADDR | 0x10 // Temp Fet, Temp Motor, Current In, PID position
#define McBase0x1b MC_BASE_ADDR | 0x1b // Tachometer, Voltage In


// init
class CANBus : public AbstractTask {

public:
  // RTOS task
  string getName(void) { return "CANBus"; };
  string init(void);
  string re_init(void);
  void exit(void);
  void task(void *pvParams);
  bool is_to_ignore_packet(uint16_t packetId);

  // Class functions and members
private:
  int counterMaxPacketsIn;
  int counterMaxPacketsOut;
  int counterR;
  int counterW;
  int counterR_notAvail;
  int counterW_notAvail;

  CANRxBuffer rxBufferIn;
  CANRxBuffer rxBufferOut;
  std::map<uint16_t, int32_t> max_ages;
  std::map<uint16_t, int32_t> ages;

  void init_ages();
  string print_raw_packet(string msg, CANPacket packet);

  // optimize with: https://learn.microsoft.com/en-us/cpp/cpp/functions-with-variable-argument-lists-cpp?view=msvc-170
  CANPacket popIn() { return rxBufferIn.pop(); }
  void handle_rx_packet(CANPacket packet);
  CANPacket popOut() { return rxBufferOut.pop(); }
  void write_rx_packet(CANPacket packet);

public:
  CANBus();

  int counterI;
  int counterI_notAvail;

  CANPacket writePacket(uint16_t adr, uint16_t data_u16_0, uint16_t data_u16_1, int8_t data_i8_4, uint8_t data_u8_5, uint8_t data_u8_6,
                        bool b_56, bool b_57, bool b_58, bool b_59, bool b_60, bool b_61, bool b_62, bool b_63, bool force = false);
  CANPacket writePacket(uint16_t adr, uint16_t data_u16_0, uint16_t data_u16_1, uint16_t data_u16_2, uint16_t data_u16_3,
                        bool force = false);
  CANPacket writePacket(uint16_t adr, CANPacket packet, bool force = false);

  void pushIn(CANPacket packet) {
    rxBufferIn.push(packet);
    counterMaxPacketsIn = max(counterMaxPacketsIn, availiblePacketsIn());
  }
  void pushOut(CANPacket packet) {
    rxBufferOut.push(packet);
    counterMaxPacketsOut = max(counterMaxPacketsOut, availiblePacketsOut());
  }

  bool isPacketToRenew(uint16_t packetId);
  void setPacketTimeStamp(uint16_t packetId, int32_t millis);

  int availiblePacketsIn() { return rxBufferIn.getSize(); }
  int availiblePacketsOut() { return rxBufferOut.getSize(); }
  int getMaxPacketsBufferInUsage() { return counterMaxPacketsIn; };
  int getMaxPacketsBufferOutUsage() { return counterMaxPacketsOut; };

  SemaphoreHandle_t mutex_in;
  SemaphoreHandle_t mutex_out;
  bool verboseModeCanIn = false;
  bool verboseModeCanInNative = false;
  bool verboseModeCanOut = false;
  bool verboseModeCanOutNative = false;
  bool verboseModeCanBusLoad = false;
};
#endif // SOLAR_CAR_CONTROL_SYSTEM_CANBUS_H
