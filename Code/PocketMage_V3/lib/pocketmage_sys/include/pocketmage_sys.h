#pragma once
#include <Arduino.h>
#include <vector>

class String;

// ===================== Pocketmage CLASS =====================
class Pocketmage{
public:
    // file
    void saveFile();
    void writeMetadata(const String& path);
    void loadFile(bool showOLED = true);
    void delFile(String fileName);
    void deleteMetadata(String path);
    void renFile(String oldFile, String newFile);
    void renMetadata(String oldPath, String newPath);
    void copyFile(String oldFile, String newFile);
    void appendToFile(String path, String inText);
  
  // time
    void setTimeFromString(String timeStr);
    void checkTimeout();
    void setCpuSpeed(int newFreq);
  
  // power
    void deepSleep(bool alternateScreenSaver = false);
    //void IRAM_ATTR PWR_BTN_irq();
    void updateBattState();
    void loadState(bool changeState = true);
  
  // debug
    void printDebug();

  // text
  String vectorToString();
  void stringToVector(String inputText);
  String removeChar(String str, char character);
  int stringToInt(String str);  

};

// Interrupt handler stored in IRAM for fast interrupt response
void setupSystem();
void IRAM_ATTR PWR_BTN_irq_handler();
Pocketmage& pocketmage();