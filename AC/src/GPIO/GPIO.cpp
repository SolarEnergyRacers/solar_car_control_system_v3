//
// General Purpose Input / Output
//

#include <definitions.h>

#include <fmt/core.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Console.h>
#include <GPIO.h>
#include <driver/gpio.h>

extern Console console;

string GPInputOutput::re_init() { return init(); }

string GPInputOutput::init() {
  bool hasError = false;
  // vPortCPUInitializeMutex(&mutex);
  // Init GPIO pins for CS of SD-card and TFT
  pinMode(ESP32_AC_SD_DETECT, INPUT);

  pinMode(SPI_CS_SDCARD, OUTPUT);
  digitalWrite(SPI_CS_SDCARD, HIGH);

  pinMode(SPI_CS_TFT, OUTPUT);
  digitalWrite(SPI_CS_TFT, HIGH);

  pinMode(SPI_CLK, OUTPUT);
  pinMode(SPI_MOSI, OUTPUT);
  pinMode(SPI_MISO, INPUT);
  pinMode(SPI_CS_TFT, OUTPUT);
  pinMode(SPI_RST, OUTPUT);

  pinMode(ESP32_AC_BUTTON_AC_NEXT, INPUT);
  pinMode(ESP32_AC_BUTTON_CONST_MODE, INPUT);

  // console << "Start test SPI_CS_SDCARD.\n";
  // for( int i = 0; i < 100; i++){
  //   digitalWrite(SPI_CS_SDCARD, LOW);
  //   console << "Low\n";
  //   delay(1500);
  //   digitalWrite(SPI_CS_SDCARD, HIGH);
  //   console << "Height\n";
  //   delay(1500);
  // }
  return fmt::format("[{}] Set SPI_CS_TFT={} and SPI_CS_SDCARD={}, GPIO initialized.", hasError ? "--" : "ok", SPI_CS_TFT, SPI_CS_SDCARD);
}

void GPInputOutput::exit() {
  // TODO
}

void GPInputOutput::register_gpio_interrupt() {

  // // report
  // console << "[HW Interrupt] Register GPIO interrupt pin " << GPIO_INTERRUPT_PIN << " (falling edge)\n";

  // // set operating mode of interrupt pin to pull-up (i.e. interrupt is generated
  // // if pin is getting grounded)
  // pinMode(GPIO_INTERRUPT_PIN, INPUT_PULLUP);
  // // register interrupt routine
  // attachInterrupt(digitalPinToInterrupt(GPIO_INTERRUPT_PIN), handle_gpio_interrupt, FALLING);
}

volatile int GPInputOutput::interrupt_counter = 0;
// portMUX_TYPE GPInputOutput::mutex = portMUX_INITIALIZER_UNLOCKED;

void GPInputOutput::task(void *pvParams) {

  // polling loop
  while (1) {

    // print the number of interrupts handled by the interrupt handler
    // handle_gpio_interrupt
    if (interrupt_counter > 0) {
      console << "[INT] Number of interrupts: " << interrupt_counter << "\n";
      interrupt_counter = 0;
    }
    taskSuspend();
  }
}