//
// SD Card
//

#ifndef SOLAR_CAR_CONTROL_SYSTEM_SDCARD_H
#define SOLAR_CAR_CONTROL_SYSTEM_SDCARD_H

#include <CarState.h>
#include <SD.h>

extern CarState carState;

class SDCard {
private:
  bool mounted = false;
  File dataFile;

  void printDirectory(File dir, int numTabs);

public:
  string getName() { return "SDCard"; };
  string init();
  string re_init();
  bool isMounted() { return carState.SdCardDetect && mounted; }
  // write a string into the dataFile
  void write_log(const string msg);
  void write_log_line(const string msg);
  // prints the directory tree of the card
  void directory();
  // read hardware detect of sd card
  bool update_sd_card_detect();
  // mount the card
  bool mount();
  // prepare log
  bool check_log_file();
  bool open_log_file();
  void close_log_file();
  // close log file, unmount the card, end the SD and disable logging
  bool unmount();

  bool verboseModeSdCard = false;
};
#endif // SOLAR_CAR_CONTROL_SYSTEM_SDCARD_H
