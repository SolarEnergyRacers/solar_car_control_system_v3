
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include <NTPClient.h>


#include "index.h"  //Web page header file

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>
#include <mySD.h>


//===============================================================
// Get NTP Time -----------------------------------------
//===============================================================
// Define NTP Client to get time
const long utcOffsetInSeconds = 7200;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//===============================================================
//Zeitmessung------------------------------------
//===============================================================
WebServer server(80);

#define xInEs1 26
const float fDistance = 4.0;

// Global Var
bool xEs1 = false;     //Lichtschranke
bool xEs1_first = false;
bool xEs1_old = false; 

int iRoundCounter = 0;  // Rundenzähler

unsigned long millis_old = 0;
unsigned long millis_start = 0;
unsigned long ulTimeLastRound = 0;     // Zeit letzte Runde
String strTimeLastRound  = "";   
String strTimestampLastRound = "";     // Uhrzeit letzte Runde
unsigned long ulTimeFastRound = 0;     // Zeit schnellste Runde
String strTimeFastRound  = "";   

String strActNTPTime = "";

String strIncomingString = "";
bool xWaitForInput = 0;

//Enter your SSID and PASSWORD
const char* ssid = "zuHause";
const char* password = "blabla";

/*
const char* ssid = "ESP";
const char* password = "12345678";

// Put IP Address details
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
*/

//===============================================================
//SD Karte------------------------------------------------
//===============================================================

const uint8_t Button1_pin = 27;  // connect a push button to switch display
const uint8_t Button2_pin = 33;  // mount/unmount sd card
const uint8_t Button3_pin = 32;  // connect a push button to print directory

// oled display pins
const uint8_t SCLK_OLED = 14;  // SCLK
const uint8_t MOSI_OLED = 13;  // MOSI (Master Output Slave Input)
const uint8_t CS_OLED = 15;    // OLED Card Chip Select
const uint8_t DC_OLED = 16;    // OLED DC(Data/Command)
const uint8_t RST_OLED = 4;    // OLED Reset

// sd card pins
const uint8_t CS_SDCARD = 5;     // SD Card Chip Select
const uint8_t MOSI_SDCARD = 23;  // MOSI (Master Output Slave Input)
const uint8_t MISO_SDCARD = 19;  // (Master Input Slave Output)
const uint8_t SCLK_SDCARD = 18;  // SCLK

// SSD1331 color definitions
const uint16_t OLED_Color_Black = 0x0000;
const uint16_t OLED_Color_Blue = 0x001F;
const uint16_t OLED_Color_Red = 0xF800;
const uint16_t OLED_Color_Green = 0x07E0;
const uint16_t OLED_Color_Cyan = 0x07FF;
const uint16_t OLED_Color_Magenta = 0xF81F;
const uint16_t OLED_Color_Yellow = 0xFFE0;
const uint16_t OLED_Color_White = 0xFFFF;

// The colors we actually want to use
uint16_t OLED_Text_Color = OLED_Color_Black;
uint16_t OLED_Backround_Color = OLED_Color_Blue;

// globals
volatile bool isButton1Pressed = false;  // the interrupt service routine affects this
volatile bool isButton2Pressed = false;  // the interrupt service routine affects this
volatile bool isButton3Pressed = false;  // the interrupt service routine affects this
Adafruit_SSD1331 oled = Adafruit_SSD1331(CS_OLED, DC_OLED, MOSI_OLED, SCLK_OLED, RST_OLED);
bool sd_inited = false;                 // not initited until mount
bool isDisplayVisible = false;          // assume the display is off until configured in setup()
const size_t MaxString = 16;            // declare size of working string buffers. Basic strlen("d hh:mm:ss") = 10
char oldTimeString[MaxString] = { 0 };  // the old string to keep cool if nothing has changed (initially empty)
char newTimeString[MaxString] = { 0 };  // the string being displayed and written (initially empty)
char newNTPTimeString[MaxString] = { 0 };  

void mount_sd_card();
// void printDirectory();
void writeToSDCard();
void write_mount_text();
void write_unmount_text();
void claer_info_line();
void displayUpTime();
char *getUpTime();
char *convertTime();
char *getNTPTime();


//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void ActTimeActRound() {
  server.send(200, "text/plane", convertTime(millis() - millis_old));
}

void RoundCounter() {
  server.send(200, "text/plane", String(iRoundCounter)); 
}

void Distance() {
 server.send(200, "text/plane", String((iRoundCounter * fDistance))); 
}

void TimeLastRound() {
  server.send(200, "text/plane", strTimeLastRound);
}
 
void TimeFastRound() {
  server.send(200, "text/plane", strTimeFastRound);
}

void Time() {
  server.send(200, "text/plane", getUpTime()); // String((millis()-millis_start)/1000));
}

//===============================================================
// Setup
//===============================================================
void setup(void){
  //===============================================================
  //Lichtschranke-----------------------------------
  Serial.begin(115200);
  delay(400);
  Serial.println();
  Serial.println("Booting Sketch...");

  pinMode(xInEs1, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

/*
//ESP32 Accsess Point -----------------------------------
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
*/


//ESP32 connects to your wifi -----------------------------------
  WiFi.mode(WIFI_STA); //Connect to your wifi
  WiFi.begin(ssid, password);

  Serial.println("Connecting to ");
  Serial.print(ssid);

  //Wait for WiFi to connect
  while(WiFi.waitForConnectResult() != WL_CONNECTED){      
      Serial.print(".");
  }

    
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.println(ssid);
  Serial.print(" Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to ESP
  // Serial.println("192.168.1.1");
  //----------------------------------------------------------------
  //===============================================================
 
  server.on("/", handleRoot);       //This is display page
  server.on("/readActTime", ActTimeActRound);
  server.on("/readRoundCnt", RoundCounter);
  server.on("/readDistance", Distance);
  server.on("/readLastTime", TimeLastRound); 
  server.on("/readFastTime", TimeFastRound); 
  server.on("/readTime", Time);
   
  server.begin();                    //Start server
  Serial.println("HTTP server started");

  timeClient.begin();       // NTP
  
  //===============================================================
  //SD Karte------------------------------------------------
  //===============================================================
  printf("Initializing ...\n");
  // pinMode(LED_BUILTIN, OUTPUT);                                                       // set pinmode to output
  pinMode(Button1_pin, INPUT);                                                        //_PULLUP);                                                 // button press pulls pin LOW so configure HIGH
  attachInterrupt(digitalPinToInterrupt(Button1_pin), senseButton1Pressed, FALLING);  // use an interrupt to sense when the button is pressed
  isButton1Pressed = false;
  pinMode(Button2_pin, INPUT);                                                        //_PULLUP);                                                 // button press pulls pin LOW so configure HIGH
  attachInterrupt(digitalPinToInterrupt(Button2_pin), senseButton2Pressed, FALLING);  // use an interrupt to sense when the button is pressed
  isButton2Pressed = false;                                                           // ignore any power-on-reboot garbage
  pinMode(Button3_pin, INPUT);                                                        //_PULLUP);                                                 // button press pulls pin LOW so configure HIGH
  attachInterrupt(digitalPinToInterrupt(Button3_pin), senseButton3Pressed, FALLING);  // use an interrupt to sense when the button is pressed
  isButton3Pressed = false;

  // initialise the sd card
  printf("Try mount ...");
  mount_sd_card();
  printf("%s\n", sd_inited ? "ok." : "failed!");
  if (sd_inited) {
    write_mount_text();
    printDirectory();
  }

  // initialise the SSD1331
  printf("Init SSD1331 ...");
  oled.begin();
  oled.setFont();
  oled.fillScreen(OLED_Backround_Color);
  oled.setTextColor(OLED_Text_Color);
  oled.setTextSize(1);
  isDisplayVisible = true;  // the display is now on
  printf("Display now %s\n", (isDisplayVisible ? "ON" : "OFF"));

  write_help();
  // final init message
  printf("Initialization done.\n");

}

//===============================================================
// Loop ----------------------------------------------------
//===============================================================
void loop(void){
  server.handleClient();
  
  xEs1 = digitalRead(xInEs1);

  // Start (erste Betätigung) -------------------------
  if (xEs1 == HIGH & xEs1_first == LOW){
    
    strActNTPTime = getNTPTime();
    Serial.println("Start erste Runde 12h-Fahrt Uhrzeit: " + strActNTPTime);
    writeToSDCard("--------Start erste Runde 12h-Fahrt-------- Uhrzeit: " + strActNTPTime);
    
    millis_old = millis();
    millis_start = millis();
    
    xEs1_first = 1;
    
  // Zeitmessung --------------------------------------
  } else if (xEs1 != xEs1_old & xEs1 == HIGH ) {  
  
  // Rundenzähler
    iRoundCounter = iRoundCounter + 1;
    
    // Berechnung Rundenzeit
    ulTimeLastRound = (millis() - millis_old); 
    millis_old = millis();
  
    strTimeLastRound = convertTime(ulTimeLastRound);

    strTimestampLastRound = getNTPTime();

    writeToSDCard("Runde " + String(iRoundCounter) + ": " + strTimeLastRound + ", Timestamp " +  strTimestampLastRound);      // write on sd card
   
    // schnellste Runde
    if (ulTimeFastRound >= ulTimeLastRound || ulTimeFastRound == 0){ 
      ulTimeFastRound = ulTimeLastRound;
      strTimeFastRound = convertTime(ulTimeFastRound);
    }
      
    // Print serial
    Serial.print("Runde: ");
    Serial.print(iRoundCounter);
    Serial.print(", Zeit: ");
    Serial.print(strTimeLastRound);  
    Serial.print(", Uhrzeit: ");     
    Serial.println(strTimestampLastRound);

  }

  xEs1_old = xEs1;    // Old sensor status
  
  // Status Lichtschranke auf LED
  digitalWrite(LED_BUILTIN, digitalRead(xInEs1));
 
  //===============================================================
  // Serial Read
  if ((Serial.available() > 0) && (strIncomingString == "")) {
      strIncomingString = Serial.readString();
      
      Serial.print("Eingabe: ");
      Serial.println(strIncomingString);
  }

  if ((strIncomingString == "SetRound")) {
    
    if (xWaitForInput == 0) {
      xWaitForInput = 1;
      Serial.println("Type Round Number");
    }
  
    if (Serial.available() > 0) {
      iRoundCounter = (Serial.readString().toInt());
      Serial.print("Anzahl Runden gesetzt auf: ");
      Serial.println(iRoundCounter);
      strIncomingString = "";
      xWaitForInput = 0;
    }
    
  } else if  ((strIncomingString == "SetDriveTime")) {
    
    if (xWaitForInput == 0) {
      xWaitForInput = 1;
      Serial.println("Type Drive Time");
    }
  
    if (Serial.available() > 0) {
      String ReadSerialTime = Serial.readString();
    
      millis_start = (ReadSerialTime.toInt())*-1000;
    
      Serial.print("Zeit gesetzt auf: ");
      Serial.println(millis_start);
      
    strIncomingString = "";
      xWaitForInput = 0;
    }   

    
  } else if (strIncomingString != "") {
    Serial.println("Ungültige Eingabe");
    strIncomingString = "";
  }


  //===============================================================
  //SD Karte-------------------------------------------------
  char *newTimeString = getUpTime();
  
  if (isButton1Pressed) {                  // has the button been pressed?
    
    Serial.println(newTimeString);
  
    isDisplayVisible = !isDisplayVisible;  // yes! toggle display visibility
    printf("button pressed, display now %s\n", (isDisplayVisible ? "ON" : "OFF"));
    delay(300);
    isButton1Pressed = false;
  }
  oled.enableDisplay(isDisplayVisible);  // apply

  // char *newTimeString = getUpTime();
  if (strcmp(newTimeString, oldTimeString) != 0) {  // has the time string changed since the last oled update?
    displayUpTime(newTimeString);                   // unconditional display, regardless of whether display is visible
    // writeToSDCard(newTimeString);                     // write on sd card
    strcpy(oldTimeString, newTimeString);           // and remember the new value
  }

  if (isButton2Pressed) {  // has the button been pressed?
    printDirectory();
    delay(300);
    isButton2Pressed = false;
  }

  if (isButton3Pressed) {  // has the button been pressed?
    if (sd_inited) {
      SD.end();
      sd_inited = false;
    }
    write_unmount_text();
    delay(4000);      // wait 2s to remove sd card
    isButton3Pressed = false;
  }

}



//===============================================================
// SD Card Functions
//===============================================================

char unmount_text[] = { "sdcard unmounted" };
char mount_text[] = { "sdcard mounted" };
void clear_info_line() {
  int x = 16;
  oled.setTextColor(OLED_Backround_Color);
  oled.setCursor(0, x);
  oled.print(unmount_text);
  oled.setCursor(0, x);
  oled.print(mount_text);
}
void write_unmount_text() {
  int x = 16;
  clear_info_line();
  oled.setTextColor(OLED_Text_Color);
  oled.setCursor(0, x);
  oled.print(unmount_text);
}
void write_mount_text() {
  int x = 16;
  clear_info_line();
  oled.setTextColor(OLED_Text_Color);
  oled.setCursor(0, x);
  oled.print(mount_text);
}

void write_help() {
  // int x = 30;
  int x = 21;
  oled.setTextColor(OLED_Text_Color);
  oled.setCursor(0, x);
  oled.print("________________");
  oled.setCursor(0, x + 9);
  oled.print("P26-Lichtschr.");
  oled.setCursor(0, x + 18);
  oled.print("P27-displ.on/off");
  oled.setCursor(0, x + 27);
  oled.print("P32-print dir");
  oled.setCursor(0, x + 36);
  oled.print("P33-unmount");
}

// initialize SD library with SPI pins
void mount_sd_card() {
  // uint8_t csPin, int8_t mosi, int8_t miso, int8_t sck
  if (!SD.begin(CS_SDCARD, MOSI_SDCARD, MISO_SDCARD, SCLK_SDCARD)) {
    sd_inited = false;
    write_unmount_text();
    return;
  }
  write_mount_text();
  sd_inited = true;
}

// interrupt service routines
void senseButton1Pressed() {
  if (!isButton1Pressed) isButton1Pressed = true;
}
void senseButton2Pressed() {
  if (!isButton2Pressed) isButton2Pressed = true;
}
void senseButton3Pressed() {
  if (!isButton3Pressed) isButton3Pressed = true;
}

char *getUpTime() {
  unsigned long upSeconds = (millis()-millis_start) / 1000;  // calculate seconds, truncated to the nearest whole second
  // unsigned long days = upSeconds / 86400;     // calculate days, truncated to nearest whole day
  // upSeconds = upSeconds % 86400;              // the remaining hhmmss are
  unsigned long hours = upSeconds / 3600;     // calculate hours, truncated to the nearest whole hour
  upSeconds = upSeconds % 3600;               // the remaining mmss are
  unsigned long minutes = upSeconds / 60;     // calculate minutes, truncated to the nearest whole minute
  upSeconds = upSeconds % 60;                 // the remaining ss are

  // construct the string representation
  sprintf(newTimeString, "%02luh %02lumin %us", hours, minutes, upSeconds);
  return newTimeString;
}

char *getNTPTime() {
  
  timeClient.update();
  
  unsigned long NTPHours = timeClient.getHours(); 
  unsigned long NTPMinutes = timeClient.getMinutes();
  unsigned long NTPSeconds = timeClient.getSeconds();
  
  // construct the string representation
  sprintf(newNTPTimeString, "%02lu:%02lu:%u", NTPHours, NTPMinutes, NTPSeconds);
  return newNTPTimeString;
}

char *convertTime(unsigned long TimeToConvert) {
 
  unsigned long MilliSeconds = TimeToConvert;
  unsigned long hours = MilliSeconds / 3600000;     // calculate hours, truncated to the nearest whole hour
  MilliSeconds = MilliSeconds % 3600000;         // the remaining ms are
  unsigned long minutes = MilliSeconds / 60000;  // calculate minutes, truncated to the nearest whole minute
  MilliSeconds = MilliSeconds % 60000;           // the remaining ms are
  unsigned long seconds = MilliSeconds / 1000;  // calculate seconds, truncated to the nearest whole second
  MilliSeconds = MilliSeconds % 1000;           // the remaining ms are
  
  // construct the string representation
  sprintf(newTimeString, "%02luh %02lumin %02lus %ums", hours, minutes, seconds, MilliSeconds);
  return newTimeString;
  
}

void displayUpTime(const char *newTimeString) {
  oled.setCursor(0, 0);                     // yes! home the cursor
  oled.setTextColor(OLED_Backround_Color);  // change the text color to the background color
  oled.print(oldTimeString);                // redraw the old value to erase
  oled.setCursor(0, 0);                     // home the cursor
  oled.setTextColor(OLED_Text_Color);       // change the text color to foreground color
  oled.print(newTimeString);                // draw the new time value
}

void writeToSDCard(const String newTimeString) {  // char *newTimeString) {
  if (!sd_inited) {
    printf("Try mount ...");
    mount_sd_card();
    printf("%s\n", sd_inited ? "ok." : "failed!");
    if (sd_inited) write_mount_text();
    else write_unmount_text();
  }
  if (!sd_inited) return;

  ext::File f = SD.open("uptime.txt", FILE_WRITE);
  if (f == NULL) return;

  f.println(newTimeString);
  f.flush();
  f.close();  // close the file

  // delay(100);
  // // after writing then reopen the file and read it
  // f = SD.open("uptime.txt");
  // if (f) {
  //   // read from the file until there's nothing else in it
  //   while (f.available()) {
  //     // read the file and print to Terminal
  //     Serial.write(f.read());
  //   }
  //   f.close();
  // } else {
  //   Serial.println("error opening uptime.txt");
  // }
}

void printDirectoryR(ext::File dir, int numTabs) {
  while (true) {
    ext::File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      printf("  ");  // we'll have a nice indentation
    }
    // Print the name
    printf(entry.name());
    /* Recurse for directories, otherwise print the file size */
    if (entry.isDirectory()) {
      printf("/\n");
      printDirectoryR(entry, numTabs + 1);
    } else {
      /* files have sizes, directories do not */
      printf("  ");
      printf("%d\n", entry.size());
    }
    entry.close();
  }
}

void printDirectory() {
  if (sd_inited) SD.end();
  mount_sd_card();
  if (!sd_inited) return;
  printf("__Dictionary:______________________________\n");
  ext::File root = SD.open("/");
  if (root) {
    printDirectoryR(root, 0);
    root.close();
  }
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}
