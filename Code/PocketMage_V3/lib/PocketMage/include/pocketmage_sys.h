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
#include <vector>

class String;

extern bool mscEnabled;
extern bool sinkEnabled;
extern volatile bool SDActive;

extern bool rebootToPocketMage();
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