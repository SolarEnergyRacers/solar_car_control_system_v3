// (de-)activate functionality & devices
#ifndef LocalFunctionsAndDevices_H
#define LocalFunctionsAndDevices_H

#define CARSPEED_ON false     // car pid speed controller
#define DS_ON false           // 1-Wire Digital Thermometer (DS18B20)
#define INT_ON false          // interrupt for I/O Expander (MCP23014)
#define PWM_ON false          // 16-channel, 12-bit PWM Fm, I2C (PCA9685)
#define RTC_ON true           // real time clock, I2C
#define SERIAL_RADIO_ON false // additionally write/read to/from the Serial2, the radio console

#define WithTaskSuspend false // use of FreeRTOS task suspend functionality

#endif // LocalFunctionsAndDevices_H
