//  888888ba                    dP                  dP                                          //
//  88    `8b                   88                  88                                          //
// a88aaaa8P' .d8888b. .d8888b. 88  .dP  .d8888b. d8888P 88d8b.d8b. .d8888b. .d8888b. .d8888b.  //
//  88        88'  `88 88'  `"" 88888"   88ooood8   88   88'`88'`88 88'  `88 88'  `88 88ooood8  //
//  88        88.  .88 88.  ... 88  `8b. 88.  ...   88   88  88  88 88.  .88 88.  .88 88.  ...  //
//  dP        `88888P' `88888P' dP   `YP `88888P'   dP   dP  dP  dP `88888P8 `8888P88 `88888P'  //
//                                                                                .88           //
//                                                                            d8888P            //
                                                     
#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Preferences.h>
#include <USB.h>
#include <USBMSC.h>
#include <SD_MMC.h>
#include <vector>

class String;

// ===================== SYSTEM STATE =====================

extern TaskHandle_t einkHandlerTaskHandle; // E-Ink handler task

// ===================== USB & STORAGE =====================
// USB mass storage controller
extern USBMSC msc;           // USB MSC object
extern sdmmc_card_t* card;   // SD card pointer

enum KBState { NORMAL, SHIFT, FUNC };    // Keyboard state

// ===================== TASKS APP =====================
extern std::vector<std::vector<String>> tasks; // Task list

// ===================== HOME APP =====================
enum HOMEState { HOME_HOME, NOWLATER };       // Home app states
extern HOMEState CurrentHOMEState;            // Current home state

extern Preferences prefs;        // NVS preferences -> move to pocketmage_file? sd, prefs, usb, sd_mmc 
extern int OLEDFPSMillis;            // Last OLED FPS update time
extern int KBBounceMillis;           // Last keyboard debounce time
extern volatile bool PWR_BTN_event;  // Power button event
extern volatile bool newState;       // App state changed
extern volatile bool OLEDPowerSave;  // OLED power save mode
extern volatile bool disableTimeout; // Disable timeout globally
extern bool fileLoaded;
extern bool mscEnabled;
extern bool sinkEnabled;
extern volatile bool SDActive;
extern volatile int battState;       // Battery state
extern uint8_t prevSec;              // Previous seconds

extern String OTA1_APP;
extern String OTA2_APP;
extern String OTA3_APP;
extern String OTA4_APP;


extern const String appStateNames[];     // App state names
enum AppState { HOME, TXT, FILEWIZ, USB_APP, BT, SETTINGS, TASKS, CALENDAR, JOURNAL, LEXICON, APPLOADER };
extern AppState CurrentAppState;         // Current app state

// System tags
extern bool mscEnabled;      // Is USB MSC active?
extern bool sinkEnabled;     // Is USB in sink mode?

extern bool rebootToPocketMage();
// <PocketMage>
void einkHandler(void *parameter); // moved from EinkFunc.cpp
void applicationEinkHandler();
void processKB();
// To Do: remove nested namespaces and default to pocketmage::method()
// maybe: refactor pocketmage to be consistent with hardware classes in library (Pocketmage().method())
namespace pocketmage{
  namespace file{
    void saveFile();
    void writeMetadata(const String& path);
    void loadFile(bool showOLED = true);
    void delFile(String fileName);
    void deleteMetadata(String path);
    void renFile(String oldFile, String newFile);
    void renMetadata(String oldPath, String newPath);
    void copyFile(String oldFile, String newFile);
    void appendToFile(String path, String inText);
  }

  namespace time{
    void setTimeFromString(String timeStr);
    void checkTimeout();
    void setCpuSpeed(int newFreq);
  }
  namespace power{
    void deepSleep(bool alternateScreenSaver = false);
    void IRAM_ATTR PWR_BTN_irq();
    void updateBattState();
    void loadState(bool changeState = true);
  }
  namespace debug{
    void printDebug();
  }
}

// ===================== SYSTEM SETUP =====================
void PocketMage_INIT();
// ===================== GLOBAL TEXT HELPERS =====================
String vectorToString();
void stringToVector(String inputText);
String removeChar(String str, char character);
int stringToInt(String str);
extern volatile bool newLineAdded;           // New line added in TXT
extern std::vector<String> allLines;                // All lines in TXT
extern bool noTimeout;               // Disable timeout