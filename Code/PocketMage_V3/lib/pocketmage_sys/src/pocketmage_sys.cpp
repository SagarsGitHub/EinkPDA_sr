// Pocketmage system compatibility layer (class-style) for SPIFFS device
// - Keeps the refactor's Pocketmage class API
// - Delegates to existing global functions implemented in this codebase

#include <pocketmage.h>
#include <pocketmage_sys.h>
#include <pocketmage_eink.h>
#include <pocketmage_oled.h>
#include <pocketmage_sd.h>
#include <pocketmage_kb.h>

#include <pocketmage_touch.h>

#include <config.h>
#include <SPIFFS.h>
#include <SD_MMC.h>
#include <Preferences.h>
#include <esp_log.h>


// ===================== Pocketmage class methods =====================
void Pocketmage::saveFile()                         { ::saveFile(); }
void Pocketmage::writeMetadata(const String& path)  { ::writeMetadata(path); }
void Pocketmage::loadFile(bool showOLED)            { ::loadFile(showOLED); }
void Pocketmage::delFile(String fileName)           { ::delFile(fileName); }
void Pocketmage::deleteMetadata(String path)        { ::deleteMetadata(path); }
void Pocketmage::renFile(String oldFile, String newFile) { ::renFile(oldFile, newFile); }
void Pocketmage::renMetadata(String oldPath, String newPath) { ::renMetadata(oldPath, newPath); }
void Pocketmage::copyFile(String oldFile, String newFile)    { ::copyFile(oldFile, newFile); }
void Pocketmage::appendToFile(String path, String inText)    { ::appendToFile(path, inText); }

void Pocketmage::setTimeFromString(String timeStr)  { ::setTimeFromString(timeStr); }
void Pocketmage::checkTimeout()                     { ::checkTimeout(); }
void Pocketmage::setCpuSpeed(int newFreq)           { ::setCpuSpeed(newFreq); }

void Pocketmage::deepSleep(bool alternate)          { ::deepSleep(alternate); }
void IRAM_ATTR Pocketmage::PWR_BTN_irq()            { ::PWR_BTN_irq(); }
void Pocketmage::updateBattState()                  { ::updateBattState(); }
void Pocketmage::loadState(bool changeState)        { ::loadState(changeState); }

void Pocketmage::printDebug()                       { ::printDebug(); }

// ===================== Singleton + wiring helpers =====================

