//
// Display
//

#ifndef SER_DISPLAY_C_H
#define SER_DISPLAY_C_H

#define ILI9341 // (320x240)

// #include <ADS1X15.h>
#include <Adafruit_ILI9341.h> // placed here for display colors in other moduls

#include <AbstractTask.h>
#include <CarState.h>
#include <LocalFunctionsAndDevices.h>

// #include <freertos/FreeRTOS.h>
// #include <freertos/semphr.h>

extern CarState carState;

using namespace std;

template <typename Enumeration> auto as_integer(Enumeration const value) -> typename std::underlying_type<Enumeration>::type {
  return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

// class Display : public AbstractTask, public Adafruit_ILI9341(SPIClass *spiClass, int8_t dc, int8_t cs, int8_t rst) {}
// class Display : public AbstractTask, public Adafruit_ILI9341(SPIClass *spiClass, int8_t dc, int8_t cs, int8_t rst) {}
class Display {
public:
  // RTOS task
  string getName(void) { return "Display"; };
  string init(void);
  string init(Adafruit_ILI9341 *ili9341);
  string re_init(void);
  // void exit(void);
  // void task(void *pvParams);
  int bgColor = 0x0;
  int height;
  int width;

private:
  void setupScrollArea(uint16_t TFA, uint16_t BFA);
  void scrollAddress(uint16_t VSP);
  int scroll(int lines);
  string _setup(void);

public:
  Adafruit_ILI9341 *tft;

  ~Display(){};
  Display();
  Display(Adafruit_ILI9341 *disp);
  void lifeSign();
  void set_DisplayStatus(DISPLAY_STATUS theNewStatus) { carState.displayStatus = theNewStatus; };
  DISPLAY_STATUS get_DisplayStatus() { return carState.displayStatus; };
  char *get_DisplayStatus_text() { return (char *)DISPLAY_STATUS_str[(int)carState.displayStatus]; };

  void print(string msg);
  int write_ganz_99(int x, int y, int valueLast, int value, int textSize, bool justInited = false, int color = ILI9341_ORANGE,
                    int bgColor = ILI9341_BLACK);
  void getCursor(int &x, int &y);
  void setCursor(int x, int y);
  void clear_screen(int bgColor);
  int getPixelWidthOfText(int textSize, string t1);
  int getPixelWidthOfTexts(int textSize, string t1, string t2);

public:
  // workers
  float write_float(int x, int y, float valueLast, float value, int textSize, int color);
  int write_nat_999(int x, int y, int valueLast, int value, int textSize, int color);

  void drawCentreString(const string &buf, int x, int y);

private:
};
#endif // SER_DISPLAY_C_H
