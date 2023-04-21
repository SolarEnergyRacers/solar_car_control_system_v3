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

string SDCard::init() { return re_init(); }

string SDCard::re_init() {
  bool hasError = false;
  console << "[  ] Init '" << getName() << "'..." << NL;

  if (!mount())
    hasError = true;

  return fmt::format("[{}] SDCard           initialized.", hasError ? "--" : "ok");
}

// https://github.com/espressif/arduino-esp32/issues/5676
// https://github.com/espressif/arduino-esp32/blob/master/libraries/SPI/examples/SPI_Multiple_Buses/SPI_Multiple_Buses.ino

bool SDCard::mount() {
  if (isMounted()) {
    return true;
  }
  if (!carState.SdCardDetect) {
    carState.EngineerInfo = "No SD card detected!.";
    console << "     " << carState.EngineerInfo << NL;
    mounted = false;
    // xSemaphoreTakeT(spiBus.mutex);
    // SD.end();
    // xSemaphoreGive(spiBus.mutex);
    return false;
  }
  try {
    carState.EngineerInfo = "Mounting SD card ...";
    console << "     " << carState.EngineerInfo << NL;
    // xSemaphoreGive(spiBus.mutex);
    xSemaphoreTakeT(spiBus.mutex);
    // mounted = SD.begin(SPI_CS_SDCARD, spiBus.spi, 400000U, "/", 10); //fails!
    mounted = SD.begin(SPI_CS_SDCARD, spiBus.spi);
    xSemaphoreGive(spiBus.mutex);
    if (mounted) {
      carState.EngineerInfo = "SD card mounted";
      console << "     " << carState.EngineerInfo << NL;
      uint8_t cardType = SD.cardType();

      console << "     SD Card Type: ";
      switch (cardType) {
      case CARD_NONE: {
        console << "No SD card attached" << NL_ARGMAX;
        uint64_t cardSizeXX = SD.cardSize() / (1024 * 1024);
        console << "SD Card Size: " << cardSizeXX << "MB" << NL;
      }
        return true;
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
      console << "SD Card Size: " << cardSize << "MB" << NL;

      return true;
    }
  } catch (exception &ex) {
    xSemaphoreGive(spiBus.mutex);
    carState.EngineerInfo = "ERROR: Unable to mount sdCard";
    console << "     " << carState.EngineerInfo << ex.what() << NL;
  }
  carState.EngineerInfo = "ERROR: Unable to mount sdCard.";
  console << "     " << carState.EngineerInfo << NL;
  return false;
}

bool SDCard::open_log_file() {
  if (carState.LogFilename.length() < 1) {
    console << "ERROR empty open_log_file name." << NL;
    return false;
  }
  if (!isMounted()) {
    console << "ERROR at open_log_file '" << carState.LogFilename << "': SD card not mounted.\n";
    return false;
  }
  try {
    // xSemaphoreTakeT(spiBus.mutex);
    dataFile = SD.open(carState.LogFilename.c_str(), FILE_APPEND); // mode: APPEND: FILE_APPEND, OVERWRITE: FILE_WRITE
    // xSemaphoreGive(spiBus.mutex);
    if (dataFile != 0) {
      console << "     Log file opend for append.\n";
      return true;
    }
    console << "     ERROR opening '" << carState.LogFilename << "'\n";
  } catch (exception &ex) {
    // xSemaphoreGive(spiBus.mutex);
    console << "     ERROR opening '" << carState.LogFilename << "': " << ex.what() << NL;
  }
  return false;
}

bool SDCard::unmount() {
  if (!carState.SdCardDetect) {
    carState.EngineerInfo = "No SD card detected!.";
    console << "     " << carState.EngineerInfo << NL;
    mounted = false;
    return false;
  }
  if (isReadyForLog()) {
    try {
      // xSemaphoreTakeT(spiBus.mutex);
      dataFile.flush();
      dataFile.close();
      SD.end();
      // xSemaphoreGive(spiBus.mutex);
      carState.EngineerInfo = "Log file closed.";
      console << "     " << carState.EngineerInfo << NL;
    } catch (exception &ex) {
      // xSemaphoreGive(spiBus.mutex);
      carState.EngineerInfo = "ERROR closing log file";
      console << "     " << carState.EngineerInfo << ": " << ex.what() << NL;
    }
  }
  if (isMounted()) {
    try {
      // xSemaphoreTakeT(spiBus.mutex);
      SD.end();
      // xSemaphoreGive(spiBus.mutex);
      carState.EngineerInfo = "SD card unmounted.";
      console << "     " << carState.EngineerInfo << NL;
    } catch (exception &ex) {
      // xSemaphoreGive(spiBus.mutex);
      carState.EngineerInfo = "ERROR unmounting SD card: ";
      console << "     " << carState.EngineerInfo << ex.what() << NL;
    }
    mounted = false;
  } else {
    carState.EngineerInfo = "SD card already unmounted.";
    console << "     " << carState.EngineerInfo << NL;
  }
  return !mounted;
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

void SDCard::write(string msg) {
  if (!isReadyForLog()) {
    if (!isReadyForLog()) {
      // give it a shot
      open_log_file();
    }
  }
  if (isReadyForLog()) {
    try {
      xSemaphoreTakeT(spiBus.mutex);
      dataFile.print(msg.c_str());
      dataFile.flush();
      xSemaphoreGive(spiBus.mutex);
    } catch (exception &ex) {
      xSemaphoreGive(spiBus.mutex);
      mounted = false; // prepare for complete re_init
      carState.EngineerInfo = "ERROR writing SD card";
      console << "     " << carState.EngineerInfo << ": " << ex.what() << NL;
    }
  }
}
