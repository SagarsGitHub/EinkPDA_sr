#pragma once
#include <Arduino.h>
#include <vector>
#include <globals.h>

class String;

// ===================== SYSTEM CLASS =====================
class Pocketmage {
public:
  Pocketmage() = default;

  // Wiring for external flags/state used in ISRs and globals
  void setPwrBtnEventFlag(volatile bool* pwrBtnEvent)                                 { pwrBtnEvent_ = pwrBtnEvent; }
  void setNoSd(bool* noSD)                                                                          { noSD_ = noSD; }
  void setSdActive(volatile bool* SDActive)                                                 { SDActive_ = SDActive; }
  void setEditingFile(String* editingFile)                                            { editingFile_ = editingFile; }
  void setKeypad(class Adafruit_TCA8418* keypad)                                                { keypad_ = keypad; }
  void setPrefs(Preferences* prefs)                                                               { prefs_ = prefs; }
  void setTimeoutMillis(volatile int* timeoutMillis)                              { timeoutMillis_ = timeoutMillis; }
  void setPrevTimeMillis(volatile int* prevTimeMillis)                          { prevTimeMillis_ = prevTimeMillis; }
  void setDisableTimeout(volatile bool* disableTimeout)                         { disableTimeout_ = disableTimeout; }
  void setOledPowerSave(volatile bool* OLEDPowerSave)                             { OLEDPowerSave_ = OLEDPowerSave; }
  void setNewState(volatile bool* newState)                                                 { newState_ = newState; }
  void setCurrentAppState(AppState* CurrentAppState)                          { CurrentAppState_ = CurrentAppState; }
  void setCurrentHOMEState(HOMEState* CurrentHOMEState)                     { CurrentHOMEState_ = CurrentHOMEState; }
  void setCurrentKBState(KBState* CurrentKBState)                               { CurrentKBState_ = CurrentKBState; }
  void setDynamicScroll(volatile long* dynamicScroll)                             { dynamicScroll_ = dynamicScroll; }
  void setNewLineAdded(volatile bool* newLineAdded)                                 { newLineAdded_ = newLineAdded; }
  void setBattState(volatile int* battState)                                              { battState_ = battState; }
  void setPrevSec(uint8_t* prevSec)                                                           { prevSec_ = prevSec; }
  void setEinkTaskHandle(TaskHandle_t* einkHandlerTaskHandle)     { einkHandlerTaskHandle_ = einkHandlerTaskHandle; }

  // Main methods
  // File
  void saveFile();
  void writeMetadata(const String& path);
  void loadFile(bool showOLED = true);
  void delFile(String fileName);
  void deleteMetadata(String path);
  void renFile(String oldFile, String newFile);
  void renMetadata(String oldPath, String newPath);
  void copyFile(String oldFile, String newFile);
  void appendToFile(String path, String inText);

  // Time
  void setTimeFromString(String timeStr);
  void checkTimeout();
  void setCpuSpeed(int newFreq);

  // Power
  void deepSleep(bool alternateScreenSaver = false);
  void IRAM_ATTR PWR_BTN_irq();
  void updateBattState();
  void loadState(bool changeState = true);

  // Debug
  void printDebug();

private:
  volatile bool* pwrBtnEvent_           = nullptr;
  bool*          noSD_                  = nullptr;
  volatile bool* SDActive_              = nullptr;
  String*        editingFile_           = nullptr;
  class Adafruit_TCA8418* keypad_       = nullptr;
  Preferences*   prefs_                 = nullptr;
  volatile int*  timeoutMillis_         = nullptr;
  volatile int*  prevTimeMillis_        = nullptr;
  volatile bool* disableTimeout_        = nullptr;
  volatile bool* OLEDPowerSave_         = nullptr;
  volatile bool* newState_              = nullptr;
  AppState*      CurrentAppState_       = nullptr;
  HOMEState*     CurrentHOMEState_      = nullptr;
  KBState*       CurrentKBState_        = nullptr;
  volatile long* dynamicScroll_         = nullptr;
  volatile bool* newLineAdded_          = nullptr;
  volatile int*  battState_             = nullptr;
  uint8_t*       prevSec_               = nullptr;
  TaskHandle_t*  einkHandlerTaskHandle_ = nullptr;
};


void wirePocketmage();
Pocketmage& pocketmage();
void IRAM_ATTR PWR_BTN_irq_handler();

// ===================== SYSTEM SETUP =====================
void setupSystem();

// ===================== GLOBAL TEXT HELPERS =====================
String vectorToString();
void stringToVector(String inputText);
String removeChar(String str, char character);
int stringToInt(String str);
