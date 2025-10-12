//  .d88888b  888888ba   //
//  88.    "' 88    `8b  //
//  `Y88888b. 88     88  //
//        `8b 88     88  //
//  d8'   .8P 88    .8P  //
//   Y88888P  8888888P   //


#pragma once
#include <Arduino.h>
#include <FS.h>

// forward-declaration to avoid including U8g2lib.h, GxEPD2_BW.h, pocketmage_oled.h, and pocketmage_eink.h
class PocketmageOled;
class PocketmageEink;

// ===================== SD CLASS =====================
class PocketmageSD {
public:
  explicit PocketmageSD() {}

  // Wire up external buffers/state used to read from globals
  void setFileSys(fs::FS* fileSys)                 { fileSys_ = fileSys;}  // reference to fs::FS*
  void setOled(PocketmageOled* oled)                     { oled_ = oled;}  // overloaded reference to pocketmage oled object (remove)
  void setOled(PocketmageOled& oled)                    { oled_ = &oled;}  // reference reference to pocketmage oled object
  void setEink(PocketmageEink* eink)                     { eink_ = eink;}  // overloaded reference to pocketmage eink object (remove)
  void setEink(PocketmageEink& eink)                    { eink_ = &eink;}  // reference to pocketmage eink object
  void setFilesList(String* filesList)          {filesList_ = filesList;}  // reference to filesList

  // Main methods  To Do: remove arguments for fs::FS &fs and reference internal fs::FS* instead
  void listDir(fs::FS &fs, const char *dirname);
  void readFile(fs::FS &fs, const char *path);
  String readFileToString(fs::FS &fs, const char *path);
  void writeFile(fs::FS &fs, const char *path, const char *message);
  void appendFile(fs::FS &fs, const char *path, const char *message);
  void renameFile(fs::FS &fs, const char *path1, const char *path2);
  void deleteFile(fs::FS &fs, const char *path);
  void setNoSD(bool noSD) { noSD_ = noSD; };  // reference to noSD flag
  bool getNoSD() { return noSD_; };
  String getEditingFile() { return editingFile_; };
  void setEditingFile(const String& file) { editingFile_ = file; };

private:
  static constexpr const char*  tag               = "MAGE_SD";
  fs::FS*                       fileSys_          = nullptr;    // class reference to sd file system object

  PocketmageOled*               oled_             = nullptr;
  PocketmageEink*               eink_             = nullptr;

  uint8_t                       fileIndex_        = 0;
  String                        excludedFiles_[3] = { "/temp.txt", "/settings.txt", "/tasks.txt" };

  // App state
  String                       editingFile_      = "";
  String*                       filesList_        = nullptr;  // size MAX_FILES
  bool noSD_ = false;                    // No SD card present
};

void wireSD();
void setupSD();
PocketmageSD& SD();
