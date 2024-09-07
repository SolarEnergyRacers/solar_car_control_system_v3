/*
 * Global definitions (pinout, device settings, ..)
 */
#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

//#define VERSION "$GIT_BRANCH_$GIT_COMMIT_HASH"
#define VERSION "5.0.0"

#define NL "\n"

#define CAN_SPEED 125E3 // CAN Speed 125 kb/s
//#define CAN_SPEED 500E3 // CAN Speed 500 kb/s
#define CAN_RX_BUFFER_SIZE 200
#define CAN_TASK_WAIT 50

#define MC_BASE_ADDR 0x500
#define BMS_BASE_ADDR 0x700
#define MPPT1_BASE_ADDR 0x600
#define MPPT2_BASE_ADDR 0x610
#define MPPT3_BASE_ADDR 0x620
#define AC_BASE_ADDR 0x630
#define DC_BASE_ADDR 0x660

/* CAN Frame max ages
 *  ------------------
 *  in ms
 *  0 = update every time
 *  -1 = ignore frames
 *  BMS
 */
#define MAXAGE_BMU_HEARTBEAT 0
#define MAXAGE_CMU_TEMP 0
#define MAXAGE_CMU_VOLTAGES 0
#define MAXAGE_PACK_SOC 0
#define MAXAGE_BALANCE_SOC 5000
#define MAXAGE_CHARGER_CONTROL 0
#define MAXAGE_PRECHARGE_STATUS 0
#define MAXAGE_MIN_MAX_U_CELL 1000
#define MAXAGE_MIN_MAX_T_CELL 0
#define MAXAGE_PACK_VOLTAGE 500
#define MAXAGE_PACK_STATUS 0
#define MAXAGE_PACK_FAN_STATUS 0
#define MAXAGE_EXT_PACK_STATUS 0

/*
 *  MPPTs
 */
#define MAXAGE_MPPT_INPUT 0
#define MAXAGE_MPPT_OUTPUT 0
#define MAXAGE_MPPT_TEMP 5000
#define MAXAGE_MPPT_AUX_POWER 0
#define MAXAGE_MPPT_LIMITS 0
#define MAXAGE_MPPT_STATUS 0
#define MAXAGE_MPPT_POWER_CONN 5000

#define MAXAGE_MC_S 0
#define MAXAGE_MC_S2 1000
#define MAXAGE_MC_S3 1000
#define MAXAGE_MC_S4 0
#define MAXAGE_MC_S5 0


const int MAX_ACCELERATION_DISPLAY_VALUE = 97; // absolute of minimal or maximal value for acceleration scaling

/*
 * Define wich version of printed board layouts are used
 * (possibility to use older test boards too)
 */
#define STEERINGWHEEL_BOARD_V1 false
#define STEERINGWHEEL_BOARD_V2 true

#endif // GLOBAL_DEFINITIONS_H
