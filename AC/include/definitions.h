/*
 * Global definitions (pinout, device settings, ..)
 */
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define SER4TYPE "AC"
//#define VERSION "$GIT_BRANCH_$GIT_COMMIT_HASH"
#define VERSION "3.1.0"

#include <LocalFunctionsAndDevices.h>

#define FILENAME_SER4CONFIG "SER4CNFG.INI"

#define NL "\n"

/*
 * ESP32 IOs.
 */
#define ESP32_AC_BUTTON_AC_NEXT 27    // Next Screen
#define ESP32_AC_BUTTON_CONST_MODE 33 // switch constant mode (Speed, Power)
#define ESP32_AC_SD_DETECT 35         // SD card detect

/* Non free selectable addresses:
 *
 * 6-axis inertial sensor, gyro:
 * #define BMI088_ACC_ADDRESS 0x19
 * #define BMI088_GYRO_ADDRESS 0x69
 *
 * RTC
 * const uint8_t DS1307_ADDRESS = 0x68;
 */
#define I2C_ADDRESS_DS1307 0x68

/*
 *  GPInputOutput
 */
//#define GPIO_INTERRUPT_PIN 33

/*
 * OneWire
 */
#define ONEWIRE_PIN 12

/*
 * I2C
 */
#define I2C_SDA 23
#define I2C_SCL 22
// #define I2C_FREQ 200000 // 200kHz
#define I2C_FREQ 100000 // 100kHz  (DS1307 limit)
//#define I2C_FREQ 50000 // 50kHz

// analog digital coder
#define I2C_ADDRESS_ADS1x15 0x48
#define ADC_MAX 65535

// Puls width modifier
#define PWM_NUM_PORTS 16
#define PWM_MAX_VALUE 4096
#define I2C_ADDRESS_PCA9685 0x42

// Extended digital IOs
#define MCP23017_NUM_DEVICES 2
#define MCP23017_NUM_PORTS 16
#define IOExtPINCOUNT (MCP23017_NUM_DEVICES * MCP23017_NUM_PORTS)
#define I2C_ADDRESS_MCP23017_IOExt0 0x20
#define I2C_ADDRESS_MCP23017_IOExt1 0x21
#define I2C_INTERRUPT 33

// digital potentiometer
// address = b0101{DS1803_ADDR2, DS1803_ADDR1, DS1803_ADDR0}
#define DS1803_BASE_ADDR 0x28
#define DS1803_ADDR0 0 // pulled down to ground
#define DS1803_ADDR1 0 // pulled down to ground
#define DS1803_ADDR2 0 // pulled down to ground
#define I2C_ADDRESS_DS1803 (DS1803_BASE_ADDR | (DS1803_ADDR2 << 2) | (DS1803_ADDR1 << 1) | DS1803_ADDR0)

/*
 * SERIAL, SERIAL2
 *
 * RX and TX are defined in pins_arduino.h, all others here
 */
#define SERIAL2_RX 16
#define SERIAL2_TX 17

/*
 * CAN Bus
 *
 *  GPIO25  TX
 *  GPIO26  RX
 */
#define CAN_TX (gpio_num_t)25
#define CAN_RX (gpio_num_t)26
#define CAN_SPEED 125E3 // CAN Speed 125 kb/s
//#define CAN_SPEED 500E3 // CAN Speed 500 kb/s
#define CAN_RX_BUFFER_SIZE 20
#define CAN_TASK_WAIT 50

// #define AC_BASE_ADDR 0x400
// #define DC_BASE_ADDR 0x410
// #define BMS_BASE_ADDR 0x500 // to config
// #define MPPT1_BASE_ADDR 0x600
// #define MPPT2_BASE_ADDR 0x610
// #define MPPT3_BASE_ADDR 0x620
// #define MC_BASE_ADDR 0x500
// #define CAN_RX_BUFFER_SIZE 20
// #define CAN_TASK_WAIT 50

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

/*
 *  SPI
 *
 *  ESP32  - SPI PIN
 *  --------------
 *  VSPI
 *  GPIO19   MOSI
 *  GPIO18   MISO
 *  GPIO5    CLK
 *  GPIO21   CS (first spi device)
 *
 */
#define SPI_MOSI 18
#define SPI_MISO 19
#define SPI_CLK 5

#define SPI_DC 4
#define SPI_RST 21

#define SPI_CS_TFT 32
#define SPI_CS_SDCARD 14

/*
 * ESP32 JTAG Debug Probe Wiring
 *
 *  ESP32  - Probe
 *  --------------
 *  GPIO12 - TDI
 *  GPIO15 - TDO
 *  GPIO13 - TCK
 *  GPIO14 - TMS
 *
 *  Tutorial:
 * https://medium.com/@manuel.bl/low-cost-esp32-in-circuit-debugging-dbbee39e508b
 *
 *  General ESP32 Pinout:
 * https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
 */

/*
 * Define wich version of printed board layouts are used
 * (possibility to use older test boards too)
 */
#define STEERINGWHEEL_BOARD_V1 false
#define STEERINGWHEEL_BOARD_V2 true

#endif // DEFINITIONS_H
