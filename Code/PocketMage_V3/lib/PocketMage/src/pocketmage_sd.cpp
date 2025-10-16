//  .d88888b  888888ba   //
//  88.    "' 88    `8b  //
//  `Y88888b. 88     88  //
//        `8b 88     88  //
//  d8'   .8P 88    .8P  //
//   Y88888P  8888888P   //

#include <pocketmage.h>
#include <config.h> // for FULL_REFRESH_AFTER
#include <SD_MMC.h>

extern bool SAVE_POWER;
static constexpr const char* tag = "SD";

// Initialization of sd class
static PocketmageSD pm_sd;

// Setup for SD Class
// @ dependencies:
//   - setupOled()
//   - setupBZ()
//   - setupEINK()
void setupSD() {
  SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0);
  if (!SD_MMC.begin("/sdcard", true) || SD_MMC.cardType() == CARD_NONE) {
    ESP_LOGE(tag, "MOUNT FAILED");

    OLED().oledWord("SD Card Not Detected!");
    delay(2000);
    if (ALLOW_NO_MICROSD) {
      OLED().oledWord("All Work Will Be Lost!");
      delay(5000);
      SD().setNoSD(true);
    }
    else {
      OLED().oledWord("Insert SD Card and Reboot!");
      delay(5000);
      // Put OLED to sleep
      OLED().setPowerSave(1);
      // Shut Down Jingle
      BZ().playJingle(Jingles::Shutdown);
      // Sleep
      esp_deep_sleep_start();
      return;
    }
  }

  setCpuFrequencyMhz(240);
  // Create folders and files if needed
  if (!SD_MMC.exists("/sys"))                 SD_MMC.mkdir( "/sys"                );
  if (!SD_MMC.exists("/notes"))               SD_MMC.mkdir( "/notes"              );
  if (!SD_MMC.exists("/journal"))             SD_MMC.mkdir( "/journal"            );
  if (!SD_MMC.exists("/dict"))                SD_MMC.mkdir( "/dict"               );
  if (!SD_MMC.exists("/apps"))                SD_MMC.mkdir( "/apps"               );
  if (!SD_MMC.exists("/apps/temp"))           SD_MMC.mkdir( "/apps/temp"          );
  if (!SD_MMC.exists("/notes"))               SD_MMC.mkdir( "/notes"              );
  if (!SD_MMC.exists("/assets"))              SD_MMC.mkdir( "/assets"             );
  if (!SD_MMC.exists("/assets/backgrounds"))  SD_MMC.mkdir( "/assets/backgrounds" );

  if (!SD_MMC.exists("/assets/backgrounds/HOWTOADDBACKGROUNDS.txt")) {
    File f = SD_MMC.open("/assets/backgrounds/HOWTOADDBACKGROUNDS.txt", FILE_WRITE);
    if (f) {
      f.print("How to add custom backgrounds:\n1. Make a background that is 1 bit (black OR white) and 320x240 pixels.\n2. Export your background as a .bmp file.\n3. Use image2cpp to convert your image to a .bin file. Use the settings: Invert Image Colors (TRUE), Swap Bits in Byte (FALSE). Select the \"Download as Binary File (.bin)\" button.\n4. Place the .bin file in this folder.\n5. Enjoy your new custom wallpapers!");
      f.close();
    }
  }
  
  if (!SD_MMC.exists("/sys/events.txt")) {
    File f = SD_MMC.open("/sys/events.txt", FILE_WRITE);
    if (f) f.close();
  }
  if (!SD_MMC.exists("/sys/tasks.txt")) {
    File f = SD_MMC.open("/sys/tasks.txt", FILE_WRITE);
    if (f) f.close();
  }
  if (!SD_MMC.exists("/sys/SDMMC_META.txt")) {
    File f = SD_MMC.open("/sys/SDMMC_META.txt", FILE_WRITE);
    if (f) f.close();
  }
}

// Access for other apps
PocketmageSD& SD() { return pm_sd; }

// ===================== main functions =====================
// Low-Level SDMMC Operations switch to using internal fs::FS*
void PocketmageSD::listDir(fs::FS &fs, const char *dirname) {
  if (noSD_) {
    OLED().oledWord("OP FAILED - No SD!");
    delay(5000);
    return;
  }
  else {
    setCpuFrequencyMhz(240);
    delay(50);
    noTimeout = true;
    ESP_LOGI(tag, "Listing directory %s\r\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
      noTimeout = false;
      ESP_LOGE(tag, "Failed to open directory: %s", root.path());
      return;
    }
    if (!root.isDirectory()) {
      noTimeout = false;
      ESP_LOGE(tag, "Not a directory: %s", root.path());
      
      return;
    }

    // Reset fileIndex and initialize filesList with "-"
    fileIndex_ = 0; // Reset fileIndex
    for (int i = 0; i < MAX_FILES; i++) {
      filesList_[i] = "-";
    }

    File file = root.openNextFile();
    while (file && fileIndex_ < MAX_FILES) {
      if (!file.isDirectory()) {
        String fileName = String(file.name());
        
        // Check if file is in the exclusion list
        bool excluded = false;
        for (const String &excludedFile : excludedFiles_) {
          if (fileName.equals(excludedFile) || ("/"+fileName).equals(excludedFile)) {
            excluded = true;
            break;
          }
        }

        if (!excluded) {
          filesList_[fileIndex_++] = fileName; // Store file name if not excluded
        }
      }
      file = root.openNextFile();
    }

    // for (int i = 0; i < fileIndex_; i++) { // Only print valid entries
    //   Serial.println(filesList_[i]);       // NOTE: This prints out valid files
    // }

    noTimeout = false;
    if (SAVE_POWER) setCpuFrequencyMhz(POWER_SAVE_FREQ);
  }
}
void PocketmageSD::readFile(fs::FS &fs, const char *path) {
  if (noSD_) {
    OLED().oledWord("OP FAILED - No SD!");
    delay(5000);
    return;
  }
  else {
    setCpuFrequencyMhz(240);
    delay(50);
    noTimeout = true;
    ESP_LOGI(tag, "Reading file %s\r\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory()) {
      noTimeout = false;
      ESP_LOGE(tag, "Failed to open file for reading: %s", file.path());
      return;
    }

    file.close();
    noTimeout = false;
    if (SAVE_POWER) setCpuFrequencyMhz(POWER_SAVE_FREQ);
  }
}
String PocketmageSD::readFileToString(fs::FS &fs, const char *path) {
  if (noSD_) {
    OLED().oledWord("OP FAILED - No SD!");
    delay(5000);
    return "";
  }
  else { 
    setCpuFrequencyMhz(240);
    delay(50);

    noTimeout = true;
    ESP_LOGI(tag, "Reading file: %s\r\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory()) {
      noTimeout = false;
      ESP_LOGE(tag, "Failed to open file for reading: %s", path);
      OLED().oledWord("Load Failed");
      delay(500);
      return "";  // Return an empty string on failure
    }

    // TODO: Can't we use readString()?
    // Serial.println("- reading from file:");
    ESP_LOGI(tag, "Reading from file: %s", file.path());
    String content = file.readString();

    file.close();
    EINK().setFullRefreshAfter(FULL_REFRESH_AFTER); //Force a full refresh
    noTimeout = false;
    return content;  // Return the complete String
  }
}
void PocketmageSD::writeFile(fs::FS &fs, const char *path, const char *message) {
  if (noSD_) {
    OLED().oledWord("OP FAILED - No SD!");
    delay(5000);
    return;
  }
  else {
    setCpuFrequencyMhz(240);
    delay(50);
    noTimeout = true;
    ESP_LOGI(tag, "Writing file: %s\r\n", path);
    delay(200);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
      noTimeout = false;
      ESP_LOGE(tag, "Failed to open %s for writing", path);
      return;
    }
    if (file.print(message)) {
      ESP_LOGV(tag, "File written %s", path);
    } 
    else {
      ESP_LOGE(tag, "Write failed for %s", path);
    }
    file.close();
    noTimeout = false;
    if (SAVE_POWER) setCpuFrequencyMhz(POWER_SAVE_FREQ);
  }
}
void PocketmageSD::appendFile(fs::FS &fs, const char *path, const char *message) {
  if (noSD_) {
    OLED().oledWord("OP FAILED - No SD!");
    delay(5000);
    return;
  }
  else {
    setCpuFrequencyMhz(240);
    delay(50);
    noTimeout = true;
    ESP_LOGI(tag, "Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
      noTimeout = false;
      ESP_LOGE(tag, "Failed to open for appending: %s", path);
      return;
    }
    if (file.println(message)) {
      ESP_LOGV(tag, "Message appended to %s", path);
    } 
    else {
      ESP_LOGE(tag, "Append failed: %s", path);
    }
    file.close();
    noTimeout = false;
    if (SAVE_POWER) setCpuFrequencyMhz(POWER_SAVE_FREQ);
  }
}
void PocketmageSD::renameFile(fs::FS &fs, const char *path1, const char *path2) {
  if (noSD_) {
    OLED().oledWord("OP FAILED - No SD!");
    delay(5000);
    return;
  }
  else {
    setCpuFrequencyMhz(240);
    delay(50);
    noTimeout = true;
    ESP_LOGI(tag, "Renaming file %s to %s\r\n", path1, path2);

    if (fs.rename(path1, path2)) {
      ESP_LOGV(tag, "Renamed %s to %s\r\n", path1, path2);
    } 
    else {
      ESP_LOGE(tag, "Rename failed: %s to %s", path1, path2);
    }
    noTimeout = false;
    if (SAVE_POWER) setCpuFrequencyMhz(POWER_SAVE_FREQ);
  }
}
void PocketmageSD::deleteFile(fs::FS &fs, const char *path) {
  if (noSD_) {
    OLED().oledWord("OP FAILED - No SD!");
    delay(5000);
    return;
  }
  else {
    setCpuFrequencyMhz(240);
    delay(50);
    noTimeout = true;
    ESP_LOGI(tag, "Deleting file: %s\r\n", path);
    if (fs.remove(path)) {
      ESP_LOGV(tag, "File deleted: %s", path);
    } 
    else {
      ESP_LOGE(tag, "Delete failed for %s", path);
    }
    noTimeout = false;
    if (SAVE_POWER) setCpuFrequencyMhz(POWER_SAVE_FREQ);
  }
}
