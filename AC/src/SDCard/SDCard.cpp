//
// SD Card
//

#include <definitions.h>

// standard libraries
#include <fmt/core.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <FS.h>
#include <SD.h>
// #include <SD_MMC.h>
#include <SPI.h>

#include <ADS1X15.h>
#include <Adafruit_ILI9341.h>
#include <CarState.h>
#include <Console.h>
#include <Helper.h>
#include <SDCard.h>
#include <SPIBus.h>

extern Console console;
extern SPIBus spiBus;
extern SDCard sdCard;

string SDCard::re_init() { return init(); }

string SDCard::init() {
  bool hasError = true;
  console << "[  ] Init '" << getName() << "'..." << NL;
  if (update_sd_card_detect()) {
    carState.EngineerInfo = "SD card detected";
    console << "     " << carState.EngineerInfo << NL;
    hasError = !mount();
    // if (!hasError)
    //   hasError = !check_log_file();
  } else {
    carState.EngineerInfo = "No SD card detected during init!";
    console << "     " << carState.EngineerInfo << NL;
  }
  return fmt::format("[{}] SDCard           initialized.", hasError ? "--" : "ok");
}

// https://github.com/espressif/arduino-esp32/issues/5676
// https://github.com/espressif/arduino-esp32/blob/master/libraries/SPI/examples/SPI_Multiple_Buses/SPI_Multiple_Buses.ino

bool SDCard::update_sd_card_detect() {
  carState.SdCardDetect = digitalRead(ESP32_AC_SD_DETECT);
  return carState.SdCardDetect;
}

bool SDCard::mount() {
  if (isMounted()) {
    return true;
  }
  update_sd_card_detect();
  if (!carState.SdCardDetect) {
    carState.EngineerInfo = "No SD card detected!";
    console << "     " << carState.EngineerInfo << NL;
    mounted = false;
    return false;
  }
  try {
    carState.EngineerInfo = "Mounting SD card...";
    console << "     " << carState.EngineerInfo << NL;
    int attempts = 0;
    mounted = false;
    xSemaphoreTakeT(spiBus.mutex);
    // mounted = SD.begin(SPI_CS_SDCARD, spiBus.spi, 400000U, "/", 10); //fails!
    while (!mounted && attempts++ < 3) {
      mounted = SD.begin(SPI_CS_SDCARD, spiBus.spi);
      vTaskDelay(100);
    }
    xSemaphoreGive(spiBus.mutex);
    if (mounted) {
      carState.EngineerInfo = "SD card mounted";
      console << "     " << carState.EngineerInfo << ", " << attempts << " attempts" << NL;
      xSemaphoreTakeT(spiBus.mutex);
      uint8_t cardType = SD.cardType();
      xSemaphoreGive(spiBus.mutex);
      console << "     SD Card Type: ";
      switch (cardType) {
      case CARD_NONE:
        console << "CARD_NONE";
        break;
      case CARD_MMC:
        console << "MMC";
        break;
      case CARD_SD:
        console << "SDSC";
        break;
      case CARD_SDHC:
        console << "SDHC";
        break;
      default:
        console << "UNKNOWN";
      }
      uint64_t cardSize = SD.cardSize() / (1024 * 1024);
      console << NL << "     SD Card Size: " << cardSize << "MB" << NL;
      return true;
    }
  } catch (exception &ex) {
    xSemaphoreGive(spiBus.mutex);
    carState.EngineerInfo = "ERROR: Unable to mount sdCard";
    console << "     " << carState.EngineerInfo << ", exception: " << ex.what() << NL;
  }
  carState.EngineerInfo = "ERROR: Unable to mount sdCard";
  console << "     " << carState.EngineerInfo << NL;
  return false;
}

void SDCard::close_log_file() {
  xSemaphoreTakeT(spiBus.mutex);
  dataFile.flush();
  dataFile.close();
  xSemaphoreGive(spiBus.mutex);
}

bool SDCard::open_log_file() {
  xSemaphoreTakeT(spiBus.mutex);
  dataFile = SD.open(carState.LogFilename.c_str(), FILE_APPEND); // mode: APPEND: FILE_APPEND, OVERWRITE: FILE_WRITE
  xSemaphoreGive(spiBus.mutex);
  if (dataFile != 0)
    return true;
  return false;
}

bool SDCard::check_log_file() {
  if (carState.LogFilename.length() < 1) {
    carState.EngineerInfo = "ERROR empty open_log_file name";
    console << "     " << carState.EngineerInfo << NL;
    return false;
  }
  if (!mount()) {
    carState.EngineerInfo = "ERROR at open_log_file: SD card not mounted";
    console << "     " << carState.EngineerInfo << NL;
    return false;
  }
  try {
    bool isSuccess = open_log_file();
    close_log_file();
    if (isSuccess) {
      carState.EngineerInfo = "Log file opend for append";
      console << "     " << carState.EngineerInfo << NL;
      return true;
    }
    carState.EngineerInfo = "ERROR opening logfile";
    console << "     " << carState.EngineerInfo << ": " << carState.LogFilename << NL;
  } catch (exception &ex) {
    xSemaphoreGive(spiBus.mutex);
    carState.EngineerInfo = "ERROR opening logfile, exception";
    console << "     ERROR opening '" << carState.LogFilename << "': " << ex.what() << NL;
  }
  return false;
}

bool SDCard::unmount() {
  if (!carState.SdCardDetect) {
    carState.EngineerInfo = "No SD card detected to unmount!";
    console << "     " << carState.EngineerInfo << NL;
    mounted = false;
    return false;
  }
  if (isMounted()) {
    try {
      xSemaphoreTakeT(spiBus.mutex);
      SD.end();
      xSemaphoreGive(spiBus.mutex);
      carState.EngineerInfo = "SD card unmounted.";
      console << "     " << carState.EngineerInfo << NL;
      mounted = false;
    } catch (exception &ex) {
      xSemaphoreGive(spiBus.mutex);
      carState.EngineerInfo = "ERROR unmounting SD card: ";
      console << "     " << carState.EngineerInfo << ex.what() << NL;
      return false;
    }
  } else {
    carState.EngineerInfo = "SD card already unmounted.";
    console << "     " << carState.EngineerInfo << NL;
  }
  return true;
}

void SDCard::directory() {
  if (!isMounted()) {
    console << "SD card not mounted." << NL;
  }

  console << "SD-Card content:" << NL;
  File root = SD.open("/");
  printDirectory(root, 1);
  root.close();
  console << "~~~~~~~~~~~~~~~~" << NL;
}

void SDCard::printDirectory(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      console << "  "; // TAB size: 2
    }
    console << entry.name();
    if (entry.isDirectory()) {
      console << "/\n";
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      console << " [" << entry.size() << "]\n";
    }
    entry.close();
  }
}

void SDCard::write_log_line(string msg) { write_log(msg + NL); }

void SDCard::write_log(string msg) {
  if (isMounted()) {
    try {
      open_log_file();
      xSemaphoreTakeT(spiBus.mutex);
      dataFile.print(msg.c_str());
      xSemaphoreGive(spiBus.mutex);
      close_log_file();
    } catch (exception &ex) {
      xSemaphoreGive(spiBus.mutex);
      carState.EngineerInfo = "ERROR writing SD card";
      console << "     " << carState.EngineerInfo << ": " << ex.what() << NL;
    }
  }
}
