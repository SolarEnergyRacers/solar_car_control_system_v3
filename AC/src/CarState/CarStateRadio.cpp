//
// Car State Radio
//
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

#include <CANBus.h>
#include <CarState.h>
#include <CarStateRadio.h>
#include <ConfigFile.h>
#include <Console.h>

#include <CarState.h>
#include <Helper.h>
#include <RTC_SER.h>
#include <SDCard.h>
#include <definitions.h>

using namespace std;

extern CarStateRadio carStateRadio;
extern Console console;
extern GlobalTime globalTime;
extern SDCard sdCard;
extern CarState carState;

void CarStateRadio::init_values() {
  for (int adr : radio_packages) {
    carState.packet_cache[adr] = CANPacket();
  }
}