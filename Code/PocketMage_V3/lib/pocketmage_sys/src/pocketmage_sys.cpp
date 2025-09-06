#include <pocketmage.h>
#include <pocketmage_sys.h>
#include <pocketmage_eink.h>
#include <pocketmage_oled.h>
#include <pocketmage_sd.h>
#include <pocketmage_kb.h>
#include <pocketmage_bz.h>
#include <pocketmage_touch.h>
#include <pocketmage_clock.h>
#include <config.h>
#include <RTClib.h>
#include <SD_MMC.h>
#include <Preferences.h>
#include <esp_log.h>
static constexpr const char* TAG = "SYSTEM";

static int countVisibleChars(String input) {
  int count = 0;

  for (size_t i = 0; i < input.length(); i++) {
    char c = input[i];
    // Check if the character is a visible character or space
    if (c >= 32 && c <= 126) {  // ASCII range for printable characters and space
      count++;
    }
  }

  return count;
}

// ===================== Pocketmage methods =====================

// --------- file helpers ---------
void Pocketmage::saveFile() {
    if (noSD_ && *noSD_) {
        OLED().oledWord("SAVE FAILED - No SD!");
        delay(5000);
        return;
    } else {
        if (SDActive_) *SDActive_ = true;
        setCpuFrequencyMhz(240);
        delay(50);

        String textToSave = vectorToString();
        ESP_LOGV(TAG, "Text to save: %s", textToSave.c_str());

        if (editingFile_ && (*editingFile_ == "" || *editingFile_ == "-"))
        *editingFile_ = "/temp.txt";
        if (keypad_) keypad_->disableInterrupts();
        if (editingFile_ && !editingFile_->startsWith("/"))
        *editingFile_ = "/" + *editingFile_;
        // OLED().oledWord("Saving File: "+ editingFile);
        SD().writeFile(SD_MMC, (editingFile_ ? editingFile_->c_str() : "/temp.txt"), textToSave.c_str());
        // OLED().oledWord("Saved: "+ editingFile);

        // Write MetaData
        if (editingFile_) writeMetadata(*editingFile_);

        // delay(1000);
        if (keypad_) keypad_->enableInterrupts();
        if (SAVE_POWER)
        setCpuFrequencyMhz(POWER_SAVE_FREQ);
        if (SDActive_) *SDActive_ = false;
    }
}

void Pocketmage::writeMetadata(const String& path) {
    File file = SD_MMC.open(path);
    if (!file || file.isDirectory()) {
        ESP_LOGE(TAG, "Invalid file for metadata: %s", file.path());
        return;
    }

    // Get file size
    size_t fileSizeBytes = file.size();
    file.close();

    // Format size string
    String fileSizeStr = String(fileSizeBytes) + " Bytes";

    // Get line and char counts
    int charCount = countVisibleChars(SD().readFileToString(SD_MMC, path.c_str()));

    String charStr = String(charCount) + " Char";

    // Get current time from RTC
    DateTime now = CLOCK().nowDT();
    char timestamp[20];
    sprintf(timestamp, "%04d%02d%02d-%02d%02d", now.year(), now.month(), now.day(), now.hour(),
            now.minute());

    // Compose new metadata line
    String newEntry = path + "|" + timestamp + "|" + fileSizeStr + "|" + charStr;

    const char* metaPath = SYS_METADATA_FILE;

    // Read existing entries and rebuild the file without duplicates
    File metaFile = SD_MMC.open(metaPath, FILE_READ);
    String updatedMeta = "";
    bool replaced = false;

    if (metaFile) {
        while (metaFile.available()) {
        String line = metaFile.readStringUntil('\n');
        if (line.startsWith(path + "|")) {
            updatedMeta += newEntry + "\n";
            replaced = true;
        } else if (line.length() > 1) {
            updatedMeta += line + "\n";
        }
        }
        metaFile.close();
    }

    if (!replaced) {
        updatedMeta += newEntry + "\n";
    }

    // Write back the updated metadata
    metaFile = SD_MMC.open(metaPath, FILE_WRITE);
    if (!metaFile) {
        ESP_LOGE(TAG, "Failed to open metadata file for writing: %s", metaPath);
        return;
    }
    metaFile.print(updatedMeta);
    metaFile.close();

    ESP_LOGI(TAG, "Metadata updated");
}

void Pocketmage::loadFile(bool showOLED) {
    if (noSD_ && *noSD_) {
        OLED().oledWord("LOAD FAILED - No SD!");
        delay(5000);
        return;
    } else {
        if (SDActive_) *SDActive_ = true;
        setCpuFrequencyMhz(240);
        delay(50);

        if (keypad_) keypad_->disableInterrupts();
        if (showOLED)
        OLED().oledWord("Loading File");
        if (editingFile_ && !editingFile_->startsWith("/"))
        *editingFile_ = "/" + *editingFile_;
        String textToLoad = SD().readFileToString(SD_MMC, (editingFile_ ? editingFile_->c_str() : "/temp.txt"));
        ESP_LOGV(TAG, "Text to load: %s", textToLoad.c_str());

        stringToVector(textToLoad);
        if (keypad_) keypad_->enableInterrupts();
        if (showOLED) {
        OLED().oledWord("File Loaded");
        delay(200);
        }
        if (SAVE_POWER)
        setCpuFrequencyMhz(POWER_SAVE_FREQ);
        if (SDActive_) *SDActive_ = false;
    }
}

void Pocketmage::delFile(String fileName) {
    if (noSD_ && *noSD_) {
        OLED().oledWord("DELETE FAILED - No SD!");
        delay(5000);
        return;
    } else {
        if (SDActive_) *SDActive_ = true;
        setCpuFrequencyMhz(240);
        delay(50);

        if (keypad_) keypad_->disableInterrupts();
        // OLED().oledWord("Deleting File: "+ fileName);
        if (!fileName.startsWith("/"))
        fileName = "/" + fileName;
        SD().deleteFile(SD_MMC, fileName.c_str());
        // OLED().oledWord("Deleted: "+ fileName);

        // Delete MetaData
        deleteMetadata(fileName);

        delay(1000);
        if (keypad_) keypad_->enableInterrupts();
        if (SAVE_POWER)
        setCpuFrequencyMhz(POWER_SAVE_FREQ);
        if (SDActive_) *SDActive_ = false;
    }
}

void Pocketmage::deleteMetadata(String path) {
    const char* metaPath = SYS_METADATA_FILE;

    // Open metadata file for reading
    File metaFile = SD_MMC.open(metaPath, FILE_READ);
    if (!metaFile) {
        ESP_LOGE(TAG, "Metadata file not found: %s", metaPath);
        return;
    }

    // Store lines that don't match the given path
    std::vector<String> keptLines;
    while (metaFile.available()) {
        String line = metaFile.readStringUntil('\n');
        if (!line.startsWith(path + "|")) {
        keptLines.push_back(line);
        }
    }
    metaFile.close();

    // Delete the original metadata file
    SD_MMC.remove(metaPath);

    // Recreate the file and write back the kept lines
    File writeFile = SD_MMC.open(metaPath, FILE_WRITE);
    if (!writeFile) {
        ESP_LOGE(TAG, "Failed to recreate metadata file. %s", writeFile.path());
        return;
    }

    for (const String& line : keptLines) {
        writeFile.println(line);
    }

    writeFile.close();
    ESP_LOGI(TAG, "Metadata entry deleted (if it existed).");
}

void Pocketmage::renFile(String oldFile, String newFile) {
    if (noSD_ && *noSD_) {
        OLED().oledWord("RENAME FAILED - No SD!");
        delay(5000);
        return;
    } else {
        if (SDActive_) *SDActive_ = true;
        setCpuFrequencyMhz(240);
        delay(50);

        if (keypad_) keypad_->disableInterrupts();
        // OLED().oledWord("Renaming "+ oldFile + " to " + newFile);
        if (!oldFile.startsWith("/"))
        oldFile = "/" + oldFile;
        if (!newFile.startsWith("/"))
        newFile = "/" + newFile;
        SD().renameFile(SD_MMC, oldFile.c_str(), newFile.c_str());
        OLED().oledWord(oldFile + " -> " + newFile);
        delay(1000);

        // Update MetaData
        renMetadata(oldFile, newFile);

        if (keypad_) keypad_->enableInterrupts();
        if (SAVE_POWER)
        setCpuFrequencyMhz(POWER_SAVE_FREQ);
        if (SDActive_) *SDActive_ = false;
    }
}

void Pocketmage::renMetadata(String oldPath, String newPath) {
    setCpuFrequencyMhz(240);
    const char* metaPath = SYS_METADATA_FILE;

    // Open metadata file for reading
    File metaFile = SD_MMC.open(metaPath, FILE_READ);
    if (!metaFile) {
        ESP_LOGE(TAG, "Metadata file not found: %s", metaPath);
        return;
    }

    std::vector<String> updatedLines;

    while (metaFile.available()) {
        String line = metaFile.readStringUntil('\n');
        if (line.startsWith(oldPath + "|")) {
        // Replace old path with new path at the start of the line
        int separatorIndex = line.indexOf('|');
        if (separatorIndex != -1) {
            // Keep rest of line after '|'
            String rest = line.substring(separatorIndex);
            line = newPath + rest;
        } else {
            // Just replace whole line with new path if malformed
            line = newPath;
        }
        }
        updatedLines.push_back(line);
    }

    metaFile.close();

    // Delete old metadata file
    SD_MMC.remove(metaPath);

    // Recreate file and write updated lines
    File writeFile = SD_MMC.open(metaPath, FILE_WRITE);
    if (!writeFile) {
        ESP_LOGE(TAG, "Failed to recreate metadata file. %s", writeFile.path());
        return;
    }

    for (const String& l : updatedLines) {
        writeFile.println(l);
    }

    writeFile.close();
    ESP_LOGI(TAG, "Metadata updated for renamed file.");

    if (SAVE_POWER)
        setCpuFrequencyMhz(40);
}

void Pocketmage::copyFile(String oldFile, String newFile) {
    if (noSD_ && *noSD_) {
        OLED().oledWord("COPY FAILED - No SD!");
        delay(5000);
        return;
    } else {
        if (SDActive_) *SDActive_ = true;
        setCpuFrequencyMhz(240);
        delay(50);

        if (keypad_) keypad_->disableInterrupts();
        OLED().oledWord("Loading File");
        if (!oldFile.startsWith("/"))
        oldFile = "/" + oldFile;
        if (!newFile.startsWith("/"))
        newFile = "/" + newFile;
        String textToLoad = SD().readFileToString(SD_MMC, (oldFile).c_str());
        SD().writeFile(SD_MMC, (newFile).c_str(), textToLoad.c_str());
        OLED().oledWord("Saved: " + newFile);

        // Write MetaData
        writeMetadata(newFile);

        delay(1000);
        if (keypad_) keypad_->enableInterrupts();

        if (SAVE_POWER)
        setCpuFrequencyMhz(POWER_SAVE_FREQ);
        if (SDActive_) *SDActive_ = false;
    }
}

void Pocketmage::appendToFile(String path, String inText) {
    if (noSD_ && *noSD_) {
        OLED().oledWord("OP FAILED - No SD!");
        delay(5000);
        return;
    } else {
        if (SDActive_) *SDActive_ = true;
        setCpuFrequencyMhz(240);
        delay(50);

        if (keypad_) keypad_->disableInterrupts();
        SD().appendFile(SD_MMC, path.c_str(), inText.c_str());

        // Write MetaData
        writeMetadata(path);

        if (keypad_) keypad_->enableInterrupts();

        if (SAVE_POWER)
        setCpuFrequencyMhz(POWER_SAVE_FREQ);
        if (SDActive_) *SDActive_ = false;
    }
}

// --------- time/power helpers ---------
void Pocketmage::setTimeFromString(String timeStr) {
    if (timeStr.length() != 5 || timeStr[2] != ':') {
        ESP_LOGE(TAG, "Invalid format! Use HH:MM. Provided str: %s", timeStr.c_str());
        return;
    }

    int hours = timeStr.substring(0, 2).toInt();
    int minutes = timeStr.substring(3, 5).toInt();

    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
        OLED().oledWord("Invalid");
        delay(500);
        return;
    }

    DateTime now = CLOCK().nowDT();  // Get current date
    CLOCK().getRTC().adjust(DateTime(now.year(), now.month(), now.day(), hours, minutes, 0));

    ESP_LOGI(TAG, "Time updated!");
}

void Pocketmage::checkTimeout() {
    int randomScreenSaver = 0;
    if (timeoutMillis_) *timeoutMillis_ = millis();

    // Trigger timeout deep sleep
    if (!(disableTimeout_ && *disableTimeout_)) {
        if ((timeoutMillis_ && prevTimeMillis_ && TIMEOUT) && ((*timeoutMillis_ - *prevTimeMillis_) >= (TIMEOUT * 1000))) {
        ESP_LOGW(TAG, "Device idle... Deep sleeping");

        // Give a chance to keep device awake
        OLED().oledWord("  Going to sleep!  ");
        int i = millis();
        int j = millis();
        while ((j - i) <= 4000) {  // 4 sec
            j = millis();
            if (digitalRead(KB_IRQ) == 0) {
            OLED().oledWord("Good Save!");
            delay(500);
            if (prevTimeMillis_) *prevTimeMillis_ = millis();
            if (keypad_) keypad_->flush();
            return;
            }
        }

        // Save current work:
        saveFile();

        switch (CurrentAppState_ ? *CurrentAppState_ : HOME) {
            case TXT:
            if (SLEEPMODE == "TEXT" && editingFile_ && *editingFile_ != "") {
                EINK().setFullRefreshAfter(FULL_REFRESH_AFTER + 1);
                display.setFullWindow();
                EINK().einkTextDynamic(true, true);

                display.setFont(&FreeMonoBold9pt7b);

                display.fillRect(0, display.height() - 26, display.width(), 26, GxEPD_WHITE);
                display.drawRect(0, display.height() - 20, display.width(), 20, GxEPD_BLACK);
                display.setCursor(4, display.height() - 6);
                //display.drawBitmap(display.width() - 30, display.height() - 20, KBStatusallArray[6], 30,
                //                20, GxEPD_BLACK);
                EINK().statusBar(editingFile_ ? *editingFile_ : String(""), true);

                display.fillRect(320 - 86, 240 - 52, 87, 52, GxEPD_WHITE);
                display.drawBitmap(320 - 86, 240 - 52, sleep1, 87, 52, GxEPD_BLACK);

                // Put device to sleep with alternate sleep screen
            deepSleep(true);
        } else
                deepSleep();
            break;

            default:
            deepSleep();
            break;
        }

        display.nextPage();
        display.hibernate();

        // Sleep the device
        BZ().playJingle(Jingles::Shutdown);
        esp_deep_sleep_start();
        }
    } else {
        if (prevTimeMillis_) *prevTimeMillis_ = millis();
    }

    // Power Button Event sleep
    if (pwrBtnEvent_ && *pwrBtnEvent_ && CurrentHOMEState_ && (*CurrentHOMEState_ != NOWLATER)) {
        *pwrBtnEvent_ = false;

        // Save current work:
        OLED().oledWord("Saving Work");
        saveFile();

        if (digitalRead(CHRG_SENS) == HIGH) {
        // Save last state
        if (prefs_) {
          prefs_->begin("PocketMage", false);
          if (CurrentAppState_) prefs_->putInt("CurrentAppState", static_cast<int>(*CurrentAppState_));
          if (editingFile_)     prefs_->putString("editingFile", *editingFile_);
          prefs_->end();
        }

        if (CurrentAppState_) *CurrentAppState_ = HOME;
        if (CurrentHOMEState_) *CurrentHOMEState_ = NOWLATER;
        updateTaskArray();
        sortTasksByDueDate(tasks);

        u8g2.setPowerSave(1);
        if (OLEDPowerSave_)  *OLEDPowerSave_ = true;
        if (disableTimeout_) *disableTimeout_ = true;
        if (newState_)       *newState_ = true;

        // Shutdown Jingle
        BZ().playJingle(Jingles::Shutdown);

        // Clear screen
        display.setFullWindow();
        display.fillScreen(GxEPD_WHITE);

        } else {
        switch (CurrentAppState_ ? *CurrentAppState_ : HOME) {
            case TXT:
            if (SLEEPMODE == "TEXT" && editingFile_ && *editingFile_ != "") {
                EINK().setFullRefreshAfter(FULL_REFRESH_AFTER + 1);
                display.setFullWindow();
                EINK().einkTextDynamic(true, true);
                display.setFont(&FreeMonoBold9pt7b);

                display.fillRect(0, display.height() - 26, display.width(), 26, GxEPD_WHITE);
                display.drawRect(0, display.height() - 20, display.width(), 20, GxEPD_BLACK);
                display.setCursor(4, display.height() - 6);
                //display.drawBitmap(display.width() - 30, display.height() - 20, KBStatusallArray[6], 30,
                //                20, GxEPD_BLACK);
                EINK().statusBar(editingFile_ ? *editingFile_ : String(""), true);

                display.fillRect(320 - 86, 240 - 52, 87, 52, GxEPD_WHITE);
                display.drawBitmap(320 - 86, 240 - 52, sleep1, 87, 52, GxEPD_BLACK);

                deepSleep(true);
            }
            // Sleep device normally
            else
                deepSleep();
            break;
            default:
            deepSleep();
            break;
        }
        }

    } else if (pwrBtnEvent_ && *pwrBtnEvent_ && CurrentHOMEState_ && (*CurrentHOMEState_ == NOWLATER)) {
        // Load last state
        /*prefs.begin("PocketMage", true);
        editingFile = prefs.getString("editingFile", "");
        if (HOME_ON_BOOT) CurrentAppState = HOME;
        else CurrentAppState = static_cast<AppState>(prefs.getInt("CurrentAppState", HOME));
        prefs.end();*/
        loadState();
        if (keypad_) keypad_->flush();

        if (CurrentHOMEState_) *CurrentHOMEState_ = HOME_HOME;
        if (pwrBtnEvent_) *pwrBtnEvent_ = false;
        if (OLEDPowerSave_ && *OLEDPowerSave_) {
        u8g2.setPowerSave(0);
        *OLEDPowerSave_ = false;
        }
        display.fillScreen(GxEPD_WHITE);
        EINK().forceSlowFullUpdate(true);

        // Play startup jingle
        BZ().playJingle(Jingles::Startup);

        EINK().refresh();
        delay(200);
        if (newState_) *newState_ = true;
    }
    }

void Pocketmage::setCpuSpeed(int newFreq) {
    // Return early if the frequency is already set
    if (getCpuFrequencyMhz() == newFreq)
        return;

    int validFreqs[] = {240, 160, 80, 40, 20, 10};
    bool isValid = false;

    for (int i = 0; i < sizeof(validFreqs) / sizeof(validFreqs[0]); i++) {
        if (newFreq == validFreqs[i]) {
        isValid = true;
        break;
        }
    }

    if (isValid) {
        setCpuFrequencyMhz(newFreq);
        ESP_LOGI(TAG, "CPU Speed changed to: %d MHz", newFreq);
    }
}

// --------- power helpers ---------
void Pocketmage::deepSleep(bool alternateScreenSaver) {
    // Put OLED to sleep
    u8g2.setPowerSave(1);

    // Stop the einkHandler task
    if (einkHandlerTaskHandle_ && *einkHandlerTaskHandle_ != NULL) {
        vTaskDelete(*einkHandlerTaskHandle_);
        *einkHandlerTaskHandle_ = NULL;
    }

    // Shutdown Jingle
    BZ().playJingle(Jingles::Shutdown);

    if (alternateScreenSaver == false) {
        int numScreensavers = sizeof(ScreenSaver_allArray) / sizeof(ScreenSaver_allArray[0]);
        int randomScreenSaver_ = esp_random() % numScreensavers;

        // display.setPartialWindow(0, 0, 320, 60);
        display.setFullWindow();
        display.drawBitmap(0, 0, ScreenSaver_allArray[randomScreenSaver_], 320, 240, GxEPD_BLACK);
        EINK().multiPassRefresh(2);
    } else {
        // Display alternate screensaver
        EINK().forceSlowFullUpdate(true);
        EINK().refresh();
        delay(100);
    }

    // Put E-Ink to sleep
    display.hibernate();

    // Save last state
    if (prefs_) {
      prefs_->begin("PocketMage", false);
      if (CurrentAppState_) prefs_->putInt("CurrentAppState", static_cast<int>(*CurrentAppState_));
      if (editingFile_)     prefs_->putString("editingFile", *editingFile_);
      prefs_->end();
    }

    // Sleep the ESP32
    esp_deep_sleep_start();
}

void IRAM_ATTR Pocketmage::PWR_BTN_irq() {
  if (pwrBtnEvent_) *pwrBtnEvent_ = true;
}

void Pocketmage::updateBattState() {
    // Read and scale voltage (add calibration offset if needed)
    float rawVoltage = (analogRead(BAT_SENS) * (3.3 / 4095.0) * 2) + 0.2;

    // Moving average smoothing (adjust alpha for responsiveness)
    static float filteredVoltage = rawVoltage;
    const float alpha = 0.1;  // Low-pass filter constant (lower = smoother, slower response)
    filteredVoltage = alpha * rawVoltage + (1.0 - alpha) * filteredVoltage;

    static float prevVoltage = 0.0;
    static int prevBattState = -1;  // Ensure valid initial state
    const float threshold = 0.05;   // Hysteresis threshold

    int newState = battState_ ? *battState_ : 0;

    // Charging state overrides everything
    if (digitalRead(CHRG_SENS) == 1) {
        newState = 5;
    } else {
        // Normal battery voltage thresholds with hysteresis
        if (filteredVoltage > 4.1 || (prevBattState == 4 && filteredVoltage > 4.1 - threshold)) {
        newState = 4;
        } else if (filteredVoltage > 3.9 || (prevBattState == 3 && filteredVoltage > 3.9 - threshold)) {
        newState = 3;
        } else if (filteredVoltage > 3.8 || (prevBattState == 2 && filteredVoltage > 3.8 - threshold)) {
        newState = 2;
        } else if (filteredVoltage > 3.7 || (prevBattState == 1 && filteredVoltage > 3.7 - threshold)) {
        newState = 1;
        } else if (filteredVoltage <= 3.7) {
        newState = 0;
        }
    }

    if (battState_ && newState != *battState_) {
        *battState_ = newState;
        prevBattState = newState;
        // newState = true;
    }

    prevVoltage = filteredVoltage;
}

void Pocketmage::loadState(bool changeState) {
    // LOAD PREFERENCES
    if (!prefs_) return;
    prefs_->begin("PocketMage", true);  // Read-Only
    // Misc
    TIMEOUT = prefs_->getInt ("TIMEOUT", 120);
    DEBUG_VERBOSE = prefs_->getBool("DEBUG_VERBOSE", true);
    SYSTEM_CLOCK = prefs_->getBool("SYSTEM_CLOCK", true);
    SHOW_YEAR = prefs_->getBool("SHOW_YEAR", true);
    SAVE_POWER = prefs_->getBool("SAVE_POWER", true);
    ALLOW_NO_MICROSD = prefs_->getBool("ALLOW_NO_SD", false);
    editingFile = prefs_->getString("editingFile", "");
    HOME_ON_BOOT = prefs_->getBool("HOME_ON_BOOT", false);
    OLED_BRIGHTNESS = prefs_->getInt ("OLED_BRIGHTNESS", 255);
    OLED_MAX_FPS = prefs_->getInt ("OLED_MAX_FPS", 30);

    // Update State (if needed)
    if (changeState) {
        u8g2.setContrast(OLED_BRIGHTNESS);

        if (HOME_ON_BOOT)
          { if (CurrentAppState_) *CurrentAppState_ = HOME; }
        else {
          if (CurrentAppState_) *CurrentAppState_ = static_cast<AppState>(prefs_->getInt("CurrentAppState", HOME));
        }

        if (keypad_) keypad_->flush();

        // Initialize boot app if needed
        switch (CurrentAppState_ ? *CurrentAppState_ : HOME) {
        case HOME:
            if (newState_) *newState_ = true;
            break;
        case TXT:
            if (editingFile_ && *editingFile_ != "")
            loadFile(false);
            else {
            stringToVector("");
            }
            if (CurrentKBState_) *CurrentKBState_ = NORMAL;
            if (dynamicScroll_)  *dynamicScroll_ = 0;
            if (newLineAdded_)   *newLineAdded_ = true;
            if (newState_)       *newState_ = false;
            break;
        case SETTINGS:
            if (newState_) *newState_ = true;
            break;
        case TASKS:
            TASKS_INIT();
            break;
        case USB_APP:
            if (CurrentAppState_) *CurrentAppState_ = HOME;
            if (CurrentKBState_)  *CurrentKBState_ = NORMAL;
            if (newState_)        *newState_ = true;
            break;
        case CALENDAR:
            CALENDAR_INIT();
            break;
        case LEXICON:
            LEXICON_INIT();
            break;
        case JOURNAL:
            JOURNAL_INIT();
            break;
        }
    }
    prefs_->end();
}

// --------- debug ---------
void Pocketmage::printDebug() {
    DateTime now = CLOCK().nowDT();
    if (prevSec_ && (now.second() != *prevSec_)) {
    *prevSec_ = now.second();
    float batteryVoltage = (analogRead(BAT_SENS) * (3.3 / 4095.0) * 2) + 0.2;

    // Display GPIO states and system info
    ESP_LOGD(
        TAG, "PWR_BTN: %d, KB_INT: %d, CHRG: %d, RTC_INT: %d, BAT: %.2f, CPU_FRQ: %.1f, FFU: %d",
        digitalRead(PWR_BTN), digitalRead(KB_IRQ), digitalRead(CHRG_SENS), digitalRead(RTC_INT),
        batteryVoltage, (float)getCpuFrequencyMhz(), (int)GxEPD2_310_GDEQ031T10::useFastFullUpdate);

    // Display system time
    ESP_LOGD(TAG, "SYSTEM_CLOCK: %d/%d/%d (%s) %d:%d:%d", now.month(), now.day(), now.year(),
        daysOfTheWeek[now.dayOfTheWeek()], now.hour(), now.minute(), now.second());
    }
}

// ===================== GLOBAL TEXT HELPERS =====================
String vectorToString() {
String result;
EINK().setTXTFont(EINK().getCurrentFont());

for (size_t i = 0; i < allLines.size(); i++) {
    result += allLines[i];

    int16_t x1, y1;
    uint16_t charWidth, charHeight;
    display.getTextBounds(allLines[i], 0, 0, &x1, &y1, &charWidth, &charHeight);

    // Add newline only if the line doesn't fully use the available space
    if (charWidth < display.width() && i < allLines.size() - 1) {
    result += '\n';
    }
}

return result;
}

void stringToVector(String inputText) {
EINK().setTXTFont(EINK().getCurrentFont());
allLines.clear();
String currentLine_;

for (size_t i = 0; i < inputText.length(); i++) {
    char c = inputText[i];

    int16_t x1, y1;
    uint16_t charWidth, charHeight;
    display.getTextBounds(currentLine_, 0, 0, &x1, &y1, &charWidth, &charHeight);

    // Check if new line needed
    if ((c == '\n' || charWidth >= display.width() - 5) && !currentLine_.isEmpty()) {
    if (currentLine_.endsWith(" ")) {
        allLines.push_back(currentLine_);
        currentLine_ = "";
    } else {
        int lastSpace = currentLine_.lastIndexOf(' ');
        if (lastSpace != -1) {
        // Split line at last space
        String partialWord = currentLine_.substring(lastSpace + 1);
        currentLine_ = currentLine_.substring(0, lastSpace);
        allLines.push_back(currentLine_);
        currentLine_ = partialWord;  // Start new line with partial word
        } else {
        // No spaces, whole line is a single word
        allLines.push_back(currentLine_);
        currentLine_ = "";
        }
    }
    }

    if (c != '\n') {
    currentLine_ += c;
    }
}

// Push last line if not empty
if (!currentLine_.isEmpty()) {
    allLines.push_back(currentLine_);
}
}

String removeChar(String str, char character) {
String result = "";
for (size_t i = 0; i < str.length(); i++) {
    if (str[i] != character) {
    result += str[i];
    }
}
return result;
}

int stringToInt(String str) {
str.trim();  // Remove leading/trailing whitespace

if (str.length() == 0)
    return -1;

for (int i = 0; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) {
    return -1;  // Invalid character found
    }
}

return str.toInt();  // Safe to convert
}
