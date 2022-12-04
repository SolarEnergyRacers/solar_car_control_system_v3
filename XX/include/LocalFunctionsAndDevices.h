// (de-)activate functionality & devices
#ifndef LocalFunctionsAndDevices_H
#define LocalFunctionsAndDevices_H

#define CARCONTROL_ON true     // central car control task
#define CARSPEED_ON true       // car pid speed controller
#define COMMANDHANDLER_ON true // command input and deploy
#define DS_ON false            // 1-Wire Digital Thermometer (DS18B20)
#define INT_ON false           // interrupt for I/O Expander (MCP23014)
#define PWM_ON false           // 16-channel, 12-bit PWM Fm, I2C (PCA9685)
//#define RTC_ON true            // real time clock, I2C
#define SD_ON true            // SD card
#define SERIAL_RADIO_ON true // additionally write/read to/from the Serial2, the radio console

/*
 * The break relais only works with 12V. If you want without
 * external power supply that means with only 5V the logic must be inverted.
 */
#define VCC_12V true

#define WithTaskSuspend false // use of FreeRTOS task suspend functionality

#endif // LocalFunctionsAndDevices_H
