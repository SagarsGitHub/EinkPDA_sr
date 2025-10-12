#include <pocketmage.h>

static constexpr const char* TAG = "SD";

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
    ESP_LOGE(TAG, "MOUNT FAILED");

    OLED().oledWord("SD Card Not Detected!");
    delay(2000);
    if (ALLOW_NO_MICROSD) {
      OLED().oledWord("All Work Will Be Lost!");
      delay(5000);
      noSD = true;
    }
    else {
      OLED().oledWord("Insert SD Card and Reboot!");
      delay(5000);
      // Put OLED to sleep
      u8g2.setPowerSave(1);
      // Shut Down Jingle
      BZ().playJingle(Jingles::Shutdown);
      // Sleep
      esp_deep_sleep_start();
      return;
    }
  }
  wireSD();

  setCpuFrequencyMhz(240);
  // Create folders and files if needed
  if (!SD_MMC.exists("/sys"))                 SD_MMC.mkdir( "/sys"                );
  if (!SD_MMC.exists("/notes"))               SD_MMC.mkdir( "/notes"              );
  if (!SD_MMC.exists("/journal"))             SD_MMC.mkdir( "/journal"            );
  if (!SD_MMC.exists("/dict"))                SD_MMC.mkdir( "/dict"               );
  if (!SD_MMC.exists("/apps"))                SD_MMC.mkdir( "/apps"               );
  if (!SD_MMC.exists("/apps/temp"))           SD_MMC.mkdir( "/apps/temp"          );
  if (!SD_MMC.exists("/screensavers"))        SD_MMC.mkdir( "/screensavers"       );
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

// Wire function  for SD class
// add any global references here + add set function to class header file
void wireSD() {
    pm_sd.setFileSys(&SD_MMC);
    pm_sd.setOled(OLED());
    pm_sd.setEink(EINK());
    pm_sd.setEditingFile(&editingFile);
    pm_sd.setFilesList(filesList);
    pm_sd.setNoSD(&noSD);
    pm_sd.setNoTimeout(&noTimeout);
}

// Access for other apps
PocketmageSD& SD() { return pm_sd; }
